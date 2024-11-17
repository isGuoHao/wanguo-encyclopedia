/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/kmod.h>
#include <linux/devfreq.h>

#include "osa.h"
#include "pdc.h"

#include "osa_devfreq.h"

#define DRVAL_DEBUG    0

#define GET_FILE(file)            \
    do {                          \
        if (__get_file(file) < 0) \
            return -1;            \
    } while (0)

#define PUT_FILE(file)            \
    do {                          \
        if (__put_file(file) < 0) \
            return -1;            \
    } while (0)

typedef struct osa_coat_dev {
    struct osa_dev osa_dev;
    struct pdc_device pdc_dev;
} osa_coat_dev_t;

struct osa_private_data {
    struct osa_dev *dev;
    void *data;
    struct osa_poll table;
    int f_ref_cnt;
};

spinlock_t f_lock;


void pdc_master_init(void)
{
    spin_lock_init(&f_lock);
}

static int __get_file(struct file *file)
{
    struct osa_private_data *pdata = NULL;

    spin_lock(&f_lock);
    pdata = file->private_data;
    if (pdata == NULL) {
        spin_unlock(&f_lock);
        return -1;
    }

    pdata->f_ref_cnt++;
    spin_unlock(&f_lock);

    return 0;
}

static int __put_file(struct file *file)
{
    struct osa_private_data *pdata = NULL;

    spin_lock(&f_lock);
    pdata = file->private_data;
    if (pdata == NULL) {
        spin_unlock(&f_lock);
        return -1;
    }

    pdata->f_ref_cnt--;
    spin_unlock(&f_lock);

    return 0;
}

static int osa_open(struct inode *inode, struct file *file)
{
    struct pdc_device *media = NULL;
    osa_coat_dev_t *coat_dev = NULL;
    struct osa_private_data *pdata = NULL;

    if (!capable(CAP_SYS_RAWIO) || !capable(CAP_SYS_ADMIN)) {
        return -EPERM;
    }

    // media = getmedia(inode);
    media = (struct pdc_device *)file->private_data;
    if (media == NULL) {
        osa_printk("%s - get media device error!\n", __FUNCTION__);
        return -1;
    }
    coat_dev = osa_container_of(media, struct osa_coat_dev, pdc_dev);
    pdata = (struct osa_private_data *)kmalloc(sizeof(struct osa_private_data), GFP_KERNEL);
    if (pdata == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    if (DRVAL_DEBUG) {
        osa_printk("%s - file->private_data=%pK!\n", __FUNCTION__, pdata);
    }

    memset(pdata, 0, sizeof(struct osa_private_data));

    file->private_data = pdata;
    pdata->dev = &(coat_dev->osa_dev);
    if (coat_dev->osa_dev.fops->open != NULL) {
        return coat_dev->osa_dev.fops->open((void *)&(pdata->data));
    }
    return 0;
}

static ssize_t osa_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    struct osa_private_data *pdata = file->private_data;
    int ret = 0;

    GET_FILE(file);

    if (pdata->dev->fops->read != NULL) {
        ret = pdata->dev->fops->read(buf, (int)size, (long *)offset, (void *)&(pdata->data));
    }

    PUT_FILE(file);
    return ret;
}

static ssize_t osa_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    struct osa_private_data *pdata = file->private_data;
    int ret = 0;

    GET_FILE(file);
    if (pdata->dev->fops->write != NULL) {
        ret = pdata->dev->fops->write(buf, (int)size, (long *)offset, (void *)&(pdata->data));
    }
    PUT_FILE(file);
    return ret;
}

static loff_t osa_llseek(struct file *file, loff_t offset, int whence)
{
    struct osa_private_data *pdata = file->private_data;
    int ret = 0;

    GET_FILE(file);
    if (DRVAL_DEBUG) {
        osa_printk("%s - file->private_data=%pK!\n", __FUNCTION__, pdata);
    }

    if (whence == SEEK_SET) {
        if (pdata->dev->fops->llseek != NULL) {
            ret = pdata->dev->fops->llseek((long)offset, OSA_SEEK_SET, (void *)&(pdata->data));
        }
    } else if (whence == SEEK_CUR) {
        if (pdata->dev->fops->llseek != NULL) {
            ret = pdata->dev->fops->llseek((long)offset, OSA_SEEK_CUR, (void *)&(pdata->data));
        }
    } else if (whence == SEEK_END) {
        if (pdata->dev->fops->llseek != NULL) {
            ret = pdata->dev->fops->llseek((long)offset, OSA_SEEK_END, (void *)&(pdata->data));
        }
    }

    PUT_FILE(file);
    return (loff_t)ret;
}

static int osa_release(struct inode *inode, struct file *file)
{
    int ret = 0;
    struct osa_private_data *pdata = file->private_data;

    GET_FILE(file);

    if (DRVAL_DEBUG) {
        osa_printk("%s - file->private_data=%pK!\n", __FUNCTION__, pdata);
    }

    if (pdata->dev->fops->release != NULL) {
        ret = pdata->dev->fops->release((void *)&(pdata->data));
    }
    if (ret != 0) {
        PUT_FILE(file);
        osa_printk("%s - release failed!\n", __FUNCTION__);
        return ret;
    }

    PUT_FILE(file);
    spin_lock(&f_lock);
    if (pdata->f_ref_cnt != 0) {
        osa_printk("%s - release failed!\n", __FUNCTION__);
        spin_unlock(&f_lock);
        return -1;
    }
    kfree(file->private_data);
    file->private_data = NULL;
    spin_unlock(&f_lock);

    return 0;
}

static long __osa_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = -1;
    struct osa_private_data *pdata = file->private_data;
    char *kbuf = NULL;

    if (DRVAL_DEBUG) {
        osa_printk("%s - file->private_data=%pK!\n", __FUNCTION__, pdata);
    }

    if (((_IOC_SIZE(cmd) == 0) && (_IOC_DIR(cmd) != _IOC_NONE))) {
        return -1;
    }

    if ((_IOC_DIR(cmd) != _IOC_NONE) && (((char *)(uintptr_t)arg) == NULL)) {
        osa_printk("%s - Input param err,it is null!\n", __FUNCTION__);
        return -1;
    }

    if (_IOC_DIR(cmd) == _IOC_NONE) {
        if (pdata->dev->fops->unlocked_ioctl == NULL) {
            return -1;
        } else {
            ret = pdata->dev->fops->unlocked_ioctl(cmd, arg, (void *)&(pdata->data));
        }
    } else if (_IOC_DIR(cmd) == _IOC_WRITE) {
        kbuf = (char *)vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }

        if (copy_from_user(kbuf, (char *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            vfree(kbuf);
            return -1;
        }

        if (pdata->dev->fops->unlocked_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->unlocked_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
        }
    } else if (_IOC_DIR(cmd) == _IOC_READ) {
        kbuf = vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }
        if (pdata->dev->fops->unlocked_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->unlocked_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
            if (ret == 0) {
                if (copy_to_user((char *)(uintptr_t)arg, kbuf, _IOC_SIZE(cmd))) {
                    vfree(kbuf);
                    return -1;
                }
            }
        }
    } else if (_IOC_DIR(cmd) == (_IOC_READ + _IOC_WRITE)) {
        kbuf = vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }

        if (copy_from_user(kbuf, (char *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            vfree(kbuf);
            return -1;
        }

        if (pdata->dev->fops->unlocked_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->unlocked_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
            if (ret == 0) {
                if (copy_to_user((char *)(uintptr_t)arg, kbuf, _IOC_SIZE(cmd))) {
                    vfree(kbuf);
                    return -1;
                }
            }
        }
    }

    if (kbuf != NULL) {
        vfree(kbuf);
    }

    return ret;
}

static long osa_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    GET_FILE(file);

    ret = __osa_unlocked_ioctl(file, cmd, arg);
    PUT_FILE(file);

    return ret;
}

#ifdef CONFIG_COMPAT
static long __osa_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = -1;
    struct osa_private_data *pdata = file->private_data;
    char *kbuf = NULL;

    if (DRVAL_DEBUG) {
        osa_printk("%s - file->private_data=%pK!\n", __FUNCTION__, pdata);
    }

    if (((_IOC_SIZE(cmd) == 0) && (_IOC_DIR(cmd) != _IOC_NONE))) {
        return -1;
    }

    if ((_IOC_DIR(cmd) != _IOC_NONE) && (((char *)(uintptr_t)arg) == NULL)) {
        osa_printk("%s - Input param err,it is null!\n", __FUNCTION__);
        return -1;
    }

    if (_IOC_DIR(cmd) == _IOC_NONE) {
        if (pdata->dev->fops->compat_ioctl == NULL) {
            return -1;
        } else {
            ret = pdata->dev->fops->compat_ioctl(cmd, arg, (void *)&(pdata->data));
        }
    } else if (_IOC_DIR(cmd) == _IOC_WRITE) {
        kbuf = (char *)vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }

        if (copy_from_user(kbuf, (char *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            vfree(kbuf);
            return -1;
        }

        if (pdata->dev->fops->compat_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->compat_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
        }
    } else if (_IOC_DIR(cmd) == _IOC_READ) {
        kbuf = vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }
        if (pdata->dev->fops->compat_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->compat_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
            if (ret == 0) {
                if (copy_to_user((char *)(uintptr_t)arg, kbuf, _IOC_SIZE(cmd))) {
                    vfree(kbuf);
                    return -1;
                }
            }
        }
    } else if (_IOC_DIR(cmd) == (_IOC_READ + _IOC_WRITE)) {
        kbuf = vmalloc(_IOC_SIZE(cmd));
        if (kbuf == NULL) {
            osa_printk("%s - vmalloc failed!\n", __FUNCTION__);
            return -1;
        }

        if (copy_from_user(kbuf, (char *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            vfree(kbuf);
            return -1;
        }

        if (pdata->dev->fops->compat_ioctl == NULL) {
            vfree(kbuf);
            return -1;
        } else {
            ret = pdata->dev->fops->compat_ioctl (cmd, (unsigned long)(uintptr_t)kbuf, (void *)&(pdata->data));
            if (ret == 0) {
                if (copy_to_user((char *)(uintptr_t)arg, kbuf, _IOC_SIZE(cmd))) {
                    vfree(kbuf);
                    return -1;
                }
            }
        }
    }

    if (kbuf != NULL) {
        vfree(kbuf);
    }

    return ret;
}

static long osa_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    GET_FILE(file);

    ret = __osa_compat_ioctl(file, cmd, arg);
    PUT_FILE(file);

    return ret;
}

#endif

static unsigned int osa_poll(struct file *file, struct poll_table_struct *table)
{
    struct osa_private_data *pdata = file->private_data;
    struct osa_poll t;
    unsigned int ret = 0;

    GET_FILE(file);

    if (DRVAL_DEBUG) {
        osa_printk("%s - table=%pK, file=%pK!\n", __FUNCTION__, table, file);
    }
    t.poll_table = table;
    t.data = file;
    if (pdata->dev->fops->poll != NULL) {
        ret = pdata->dev->fops->poll(&t, (void *)&(pdata->data));
    }

    PUT_FILE(file);
    return ret;
}

static int __osa_valid_mmap_phys_addr_range(unsigned long pfn, size_t size)
{
    /* check physical addr greater than the max addr supported by the system */
    if ((pfn + (size >> PAGE_SHIFT)) > (1 + ((~0UL) >> PAGE_SHIFT))) {
        osa_printk("--%s - %d--!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /* CMA need to alloc ram region, so annotate the code as follows */
#if 0
    /* check physical addr is ram region */
    if (pfn_valid(pfn) || pfn_valid(pfn + (size >> PAGE_SHIFT))) {
        osa_printk("--%s - %d--!\n", __FUNCTION__, __LINE__);
        return 0;
    }
#endif

    /* It's necessary for the variable "size" to align 4k(page_size). */
#define PAGE_SIZE_MASK 0xfffffffffffff000ULL
    if ((unsigned long)size & (~PAGE_SIZE_MASK)) {
        osa_printk("--%s - %d--!\n", __FUNCTION__, __LINE__);
        return 0;
    }
#undef PAGE_SIZE_MASK

    return 1;
}

static int osa_mmap(struct file *file, struct vm_area_struct *vm)
{
    struct osa_vm osa_vm;
    struct osa_private_data *pdata = file->private_data;
    osa_vm.vm = vm;

    if (!__osa_valid_mmap_phys_addr_range(vm->vm_pgoff, vm->vm_end - vm->vm_start)) {
        osa_printk("\n%s - invalid argument   size=%ld!!!\n", __FUNCTION__, vm->vm_end - vm->vm_start);
        return -EINVAL;
    }

    if (DRVAL_DEBUG) {
        osa_printk("%s - start=%lx, end=%lx!, off=%lx\n", __FUNCTION__, vm->vm_start, vm->vm_end, vm->vm_pgoff);
    }
    if (pdata->dev->fops->mmap != NULL) {
        return pdata->dev->fops->mmap(&osa_vm, vm->vm_start, vm->vm_end, vm->vm_pgoff, (void *)&(pdata->data));
    }
    return 0;
}

static struct file_operations s_osa_fops = {
    .owner = THIS_MODULE,
    .open = osa_open,
    .read = osa_read,
    .write = osa_write,
    .llseek = osa_llseek,
    .unlocked_ioctl = osa_unlocked_ioctl,
    .release = osa_release,
    .poll = osa_poll,
    .mmap = osa_mmap,
#ifdef CONFIG_COMPAT
    .compat_ioctl = osa_compat_ioctl,
#endif
};

static int pdc_pm_prepare(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_prepare) {
        return coat_dev->osa_dev.osa_pmops->pm_prepare(&(coat_dev->osa_dev));
    }
    return 0;
}

static void pdc_pm_complete(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_complete) {
        coat_dev->osa_dev.osa_pmops->pm_complete(&(coat_dev->osa_dev));
    }
}

static int pdc_pm_suspend(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_resume(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume) {
        return coat_dev->osa_dev.osa_pmops->pm_resume(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_freeze(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_thaw(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_poweroff(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_restore(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore) {
        return coat_dev->osa_dev.osa_pmops->pm_restore(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_suspend_late(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend_late) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_resume_early(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume_early) {
        return coat_dev->osa_dev.osa_pmops->pm_resume_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_freeze_late(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze_late) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_thaw_early(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw_early) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_poweroff_late(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff_late) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_restore_early(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore_early) {
        return coat_dev->osa_dev.osa_pmops->pm_restore_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_suspend_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_resume_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_resume_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_freeze_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_thaw_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_poweroff_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int pdc_pm_restore_noirq(struct pdc_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, pdc_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_restore_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static struct pdc_ops s_osa_pmops = {
    .pm_prepare = pdc_pm_prepare,
    .pm_complete = pdc_pm_complete,
    .pm_suspend = pdc_pm_suspend,
    .pm_resume = pdc_pm_resume,
    .pm_freeze = pdc_pm_freeze,
    .pm_thaw = pdc_pm_thaw,
    .pm_poweroff = pdc_pm_poweroff,
    .pm_restore = pdc_pm_restore,
    .pm_suspend_late = pdc_pm_suspend_late,
    .pm_resume_early = pdc_pm_resume_early,
    .pm_freeze_late = pdc_pm_freeze_late,
    .pm_thaw_early = pdc_pm_thaw_early,
    .pm_poweroff_late = pdc_pm_poweroff_late,
    .pm_restore_early = pdc_pm_restore_early,
    .pm_suspend_noirq = pdc_pm_suspend_noirq,
    .pm_resume_noirq = pdc_pm_resume_noirq,
    .pm_freeze_noirq = pdc_pm_freeze_noirq,
    .pm_thaw_noirq = pdc_pm_thaw_noirq,
    .pm_poweroff_noirq = pdc_pm_poweroff_noirq,
    .pm_restore_noirq = pdc_pm_restore_noirq,

#if 0
    .devfreq_target = pdc_devfreq_target,
    .devfreq_get_dev_status = pdc_devfreq_get_dev_status,
    .devfreq_get_cur_freq = pdc_devfreq_get_cur_freq,
    .devfreq_exit = pdc_devfreq_exit,
#endif
};

osa_dev_t *pdc_createdev(const char *name)
{
    osa_coat_dev_t *pdev = NULL;
    if (name == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return NULL;
    }
    pdev = (osa_coat_dev_t *)kmalloc(sizeof(osa_coat_dev_t), GFP_KERNEL);
    if (pdev == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return NULL;
    }
    memset(pdev, 0, sizeof(osa_coat_dev_t));
    osa_strncpy(pdev->osa_dev.name, name, sizeof(pdev->osa_dev.name) - 1);
    pdev->osa_dev.dev = pdev;
    return &(pdev->osa_dev);
}
EXPORT_SYMBOL(pdc_createdev);

int pdc_destroydev(osa_dev_t *osa_dev)
{
    osa_coat_dev_t *pdev = NULL;
    if (osa_dev == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    pdev = osa_dev->dev;
    if (pdev == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    kfree(pdev);
    return 0;
}
EXPORT_SYMBOL(pdc_destroydev);

int pdc_registerdevice(osa_dev_t *osa_dev)
{
    struct pdc_device *media = NULL;
    if ((osa_dev == NULL) || (osa_dev->fops == NULL)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    media = &(((osa_coat_dev_t *)(osa_dev->dev))->pdc_dev);
    if (osa_dev->minor != 0) {
        media->minor = osa_dev->minor;
    } else {
        media->minor = PDC_DYNAMIC_MINOR;
    }
    media->owner = THIS_MODULE;
    media->fops = &s_osa_fops;
    media->drvops = &s_osa_pmops;
    osa_strncpy(media->devfs_name, osa_dev->name, sizeof(media->devfs_name) - 1);
    return pdc_master_register(media);
}
EXPORT_SYMBOL(pdc_registerdevice);


void pdc_deregisterdevice(osa_dev_t *pdev)
{
    if (pdev == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    pdc_master_unregister((struct pdc_device *)&(((osa_coat_dev_t *)(pdev->dev))->pdc_dev));
}
EXPORT_SYMBOL(pdc_deregisterdevice);

