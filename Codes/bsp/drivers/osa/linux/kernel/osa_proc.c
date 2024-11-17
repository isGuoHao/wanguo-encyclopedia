/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/version.h>

#include "osa.h"

#define OSA_PROC_DEBUG 0

static struct osa_list_head list;
static osa_proc_entry_t *proc_entry = NULL;

osa_proc_entry_t *osa_create_proc(const char *name, osa_proc_entry_t *parent);
void osa_remove_proc(const char *name, osa_proc_entry_t *parent);
void osa_remove_proc_root(const char *name, osa_proc_entry_t *parent);
void osa_proc_init(void);
void osa_proc_exit(void);

static int osa_seq_show(struct seq_file *s, void *p)
{
    osa_proc_entry_t *oldsentry = s->private;
    osa_proc_entry_t sentry;
    if (oldsentry == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    memset(&sentry, 0, sizeof(osa_proc_entry_t));
    /* only these two parameters are used */
    sentry.seqfile = s;
    sentry.private = oldsentry->private;
    oldsentry->read(&sentry);
    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
static ssize_t osa_procwrite(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    osa_proc_entry_t *sentry = ((struct seq_file *)(file->private_data))->private;
    return sentry->write(sentry, (char *)buf, count, (long long *)ppos);
}
#else
static ssize_t osa_procwrite(struct file *file, const char __user *buf,
                              size_t count, loff_t *ppos)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
    osa_proc_entry_t *item = PDE_DATA(file_inode(file));
#else
    osa_proc_entry_t *item = pde_data(file_inode(file));
#endif

    if ((item != NULL) && (item->write != NULL)) {
        return item->write(item, buf, count, (long long *)ppos);
    }

    return -ENOSYS;
}
#endif

static int osa_procopen(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    osa_proc_entry_t *sentry = PDE(inode)->data;
#elif LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
    osa_proc_entry_t *item = PDE_DATA(file_inode(file));
#else
    osa_proc_entry_t *sentry = pde_data(inode);
#endif
    if ((sentry != NULL) && (sentry->open != NULL)) {
        sentry->open(sentry);
    }
    return single_open(file, osa_seq_show, sentry);
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
static struct file_operations osa_proc_ops = {
    .owner = THIS_MODULE,
    .open = osa_procopen,
    .read = seq_read,
    .write = osa_procwrite,
    .llseek = seq_lseek,
    .release = single_release
};
#else
struct proc_ops osa_proc_ops = {
    .proc_open = osa_procopen,
    .proc_read = seq_read,
    .proc_write = osa_procwrite,
    .proc_lseek = seq_lseek,
    .proc_release = single_release
};
#endif

osa_proc_entry_t *osa_create_proc(const char *name, osa_proc_entry_t *parent)
{
    struct proc_dir_entry *entry = NULL;
    osa_proc_entry_t *sentry = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    if (parent == NULL) {
        osa_printk("%s - parent is NULL!\n", __FUNCTION__);
        entry = create_proc_entry(name, 0, NULL);
    } else {
        osa_printk("%s - parent is not NULL! parent=%pK\n", __FUNCTION__, parent->proc_dir_entry);
        entry = create_proc_entry(name, 0, parent->proc_dir_entry);
    }
    if (entry == NULL) {
        osa_printk("%s - create_proc_entry failed!\n", __FUNCTION__);
        return NULL;
    }
    sentry = kmalloc(sizeof(struct osa_proc_dir_entry), GFP_KERNEL);
    if (sentry == NULL) {
        if (parent != NULL) {
            remove_proc_entry(name, parent->proc_dir_entry);
        } else {
            remove_proc_entry(name, NULL);
        }
        osa_printk("%s - kmalloc failed!\n", __FUNCTION__);
        return NULL;
    }

    osa_memset(sentry, 0, sizeof(struct osa_proc_dir_entry));

    osa_strncpy(sentry->name, name, sizeof(sentry->name) - 1);
    sentry->proc_dir_entry = entry;
    sentry->open = NULL;
    entry->proc_fops = &osa_proc_ops;
    entry->data = sentry;
#else
    sentry = kmalloc(sizeof(struct osa_proc_dir_entry), GFP_KERNEL);
    if (sentry == NULL) {
        osa_printk("%s - kmalloc failed!\n", __FUNCTION__);
        return NULL;
    }

    osa_memset(sentry, 0, sizeof(struct osa_proc_dir_entry));

    osa_strncpy(sentry->name, name, sizeof(sentry->name) - 1);

    if (parent == NULL) {
        entry = proc_create_data(name, 0, NULL, &osa_proc_ops, sentry);
    } else {
        entry = proc_create_data(name, 0, parent->proc_dir_entry, &osa_proc_ops, sentry);
    }
    if (entry == NULL) {
        osa_printk("%s - create_proc_entry failed!\n", __FUNCTION__);
        kfree(sentry);
        sentry = NULL;
        return NULL;
    }
    sentry->proc_dir_entry = entry;
    sentry->open = NULL;
#endif
    osa_list_add_tail(&(sentry->node), &list);
    return sentry;
}

void osa_remove_proc(const char *name, osa_proc_entry_t *parent)
{
    struct osa_proc_dir_entry *sproc = NULL;

    if (name == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    if (parent != NULL) {
        remove_proc_entry(name, parent->proc_dir_entry);
    } else {
        remove_proc_entry(name, NULL);
    }
    osa_list_for_each_entry(sproc, &list, node) {
        if (osa_strncmp(sproc->name, name, sizeof(sproc->name)) == 0) {
            osa_list_del(&(sproc->node));
            break;
        }
    }
    if (sproc != NULL) {
        kfree(sproc);
    }
}

osa_proc_entry_t *osa_create_proc_entry(const char *name, osa_proc_entry_t *parent)
{
    parent = proc_entry;

    return osa_create_proc(name, parent);
}
EXPORT_SYMBOL(osa_create_proc_entry);

void osa_remove_proc_entry(const char *name, osa_proc_entry_t *parent)
{
    parent = proc_entry;
    osa_remove_proc(name, parent);
    return;
}
EXPORT_SYMBOL(osa_remove_proc_entry);

osa_proc_entry_t *osa_proc_mkdir(const char *name, osa_proc_entry_t *parent)
{
    struct proc_dir_entry *proc = NULL;
    struct osa_proc_dir_entry *sproc = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    if (parent != NULL) {
        proc = proc_mkdir(name, parent->proc_dir_entry);
        osa_printk("%s - parent is not NULL!\n", __FUNCTION__);
        // proc = create_proc_entry(name, S_IFDIR | S_IRUGO | S_IXUGO, parent->proc_dir_entry);
    } else {
        proc = proc_mkdir(name, NULL);
        osa_printk("%s - parent is NULL! proc=%pK \n", __FUNCTION__, proc);
        // proc = create_proc_entry(name, S_IFDIR | S_IRUGO | S_IXUGO, NULL);
    }
    if (proc == NULL) {
        osa_printk("%s - proc_mkdir failed!\n", __FUNCTION__);
        return NULL;
    }
    // osa_memset(proc, 0x00, sizeof(proc));
    sproc = kmalloc(sizeof(struct osa_proc_dir_entry), GFP_KERNEL);
    if (sproc == NULL) {
        if (parent != NULL) {
            remove_proc_entry(name, parent->proc_dir_entry);
        } else {
            remove_proc_entry(name, NULL);
        }
        osa_printk("%s - kmalloc failed!\n", __FUNCTION__);
        return NULL;
    }

    osa_memset(sproc, 0, sizeof(struct osa_proc_dir_entry));

    osa_strncpy(sproc->name, name, sizeof(sproc->name) - 1);
    sproc->proc_dir_entry = proc;
    // sproc->read = NULL;
    // sproc->write = NULL;
    // proc->proc_fops = NULL;
    proc->data = sproc;
#else
    sproc = kmalloc(sizeof(struct osa_proc_dir_entry), GFP_KERNEL);
    if (sproc == NULL) {
        osa_printk("%s - kmalloc failed!\n", __FUNCTION__);
        return NULL;
    }

    osa_memset(sproc, 0, sizeof(struct osa_proc_dir_entry));

    osa_strncpy(sproc->name, name, sizeof(sproc->name) - 1);

    if (parent != NULL) {
        proc = proc_mkdir_data(name, 0, parent->proc_dir_entry, sproc);
    } else {
        proc = proc_mkdir_data(name, 0, NULL, sproc);
    }
    if (proc == NULL) {
        osa_printk("%s - proc_mkdir failed!\n", __FUNCTION__);
        kfree(sproc);
        sproc = NULL;
        return NULL;
    }
    sproc->proc_dir_entry = proc;
#endif
    osa_list_add_tail(&(sproc->node), &list);
    return sproc;
}
EXPORT_SYMBOL(osa_proc_mkdir);

void osa_remove_proc_root(const char *name, osa_proc_entry_t *parent)
{
    struct osa_proc_dir_entry *sproc = NULL;
    if (name == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    if (parent != NULL) {
        remove_proc_entry(name, parent->proc_dir_entry);
    } else {
        remove_proc_entry(name, NULL);
    }
    osa_list_for_each_entry(sproc, &list, node) {
        if (osa_strncmp(sproc->name, name, sizeof(sproc->name)) == 0) {
            osa_list_del(&(sproc->node));
            break;
        }
    }
    if (sproc != NULL) {
        kfree(sproc);
    }
}

void osa_seq_printf(osa_proc_entry_t *entry, const char *fmt, ...)
{
    struct seq_file *s = (struct seq_file *)(entry->seqfile);
    va_list args;

    va_start(args, fmt);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 3, 0)
    (void)seq_vprintf(s, fmt, args);
#else
    seq_vprintf(s, fmt, args);
#endif
    va_end(args);
}
EXPORT_SYMBOL(osa_seq_printf);

void osa_proc_init(void)
{
    OSA_INIT_LIST_HEAD(&list);
    proc_entry = osa_proc_mkdir("umap", OSA_NULL);
    if (proc_entry == OSA_NULL) {
        osa_printk("test init, proc mkdir error!\n");
    }
}
void osa_proc_exit(void)
{
    osa_remove_proc_root("umap", OSA_NULL);
}

