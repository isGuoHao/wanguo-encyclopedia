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
#include "media.h"

#include "osa.h"


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
    struct media_device media_dev;
} osa_coat_dev_t;

struct osa_private_data {
    struct osa_dev *dev;
    void *data;
    struct osa_poll table;
    int f_ref_cnt;
};

spinlock_t f_lock;

extern void osa_device_init(void);

void osa_device_init(void)
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
    struct media_device *media = NULL;
    osa_coat_dev_t *coat_dev = NULL;
    struct osa_private_data *pdata = NULL;

    if (!capable(CAP_SYS_RAWIO) || !capable(CAP_SYS_ADMIN)) {
        return -EPERM;
    }

    // media = getmedia(inode);
    media = (struct media_device *)file->private_data;
    if (media == NULL) {
        osa_printk("%s - get media device error!\n", __FUNCTION__);
        return -1;
    }
    coat_dev = osa_container_of(media, struct osa_coat_dev, media_dev);
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

static int osa_pm_prepare(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_prepare) {
        return coat_dev->osa_dev.osa_pmops->pm_prepare(&(coat_dev->osa_dev));
    }
    return 0;
}

static void osa_pm_complete(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_complete) {
        coat_dev->osa_dev.osa_pmops->pm_complete(&(coat_dev->osa_dev));
    }
}

static int osa_pm_suspend(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_resume(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume) {
        return coat_dev->osa_dev.osa_pmops->pm_resume(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_freeze(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_thaw(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_poweroff(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_restore(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore) {
        return coat_dev->osa_dev.osa_pmops->pm_restore(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_suspend_late(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend_late) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_resume_early(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume_early) {
        return coat_dev->osa_dev.osa_pmops->pm_resume_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_freeze_late(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze_late) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_thaw_early(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw_early) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_poweroff_late(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff_late) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff_late(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_restore_early(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore_early) {
        return coat_dev->osa_dev.osa_pmops->pm_restore_early(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_suspend_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_suspend_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_suspend_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_resume_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_resume_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_resume_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_freeze_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_freeze_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_freeze_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_thaw_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_thaw_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_thaw_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_poweroff_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_poweroff_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_poweroff_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_pm_restore_noirq(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_pmops && coat_dev->osa_dev.osa_pmops->pm_restore_noirq) {
        return coat_dev->osa_dev.osa_pmops->pm_restore_noirq(&(coat_dev->osa_dev));
    }
    return 0;
}

static int osa_devfreq_target(struct media_device *media, unsigned long *freq, unsigned int flags)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_profile->target) {
        return coat_dev->osa_dev.osa_profile->target(&(coat_dev->osa_dev), freq, flags);
    }
    return 0;
}

static int osa_devfreq_get_dev_status(struct media_device *media, struct osa_devfreq_dev_status *stat)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_profile->get_dev_status) {
        return coat_dev->osa_dev.osa_profile->get_dev_status(&(coat_dev->osa_dev), stat);
    }
    return 0;
}

static int osa_devfreq_get_cur_freq(struct media_device *media, unsigned long *freq)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);
    if (coat_dev->osa_dev.osa_profile->get_cur_freq) {
        return coat_dev->osa_dev.osa_profile->get_cur_freq(&(coat_dev->osa_dev), freq);
    }
    return 0;
}

static void osa_devfreq_exit(struct media_device *media)
{
    osa_coat_dev_t *coat_dev = container_of(media, struct osa_coat_dev, media_dev);

    if (coat_dev->osa_dev.osa_profile->exit) {
        return coat_dev->osa_dev.osa_profile->exit(&(coat_dev->osa_dev));
    }
}

int osa_devfreq_register(osa_dev_t *osa_dev, struct osa_devfreq_dev_profile *osa_profile,
    const char *governor, void *data)
{
    struct media_device *media = NULL;
    osa_devfreq_para_t devfreq_para = { 0 };

    if ((osa_dev == NULL) || (osa_profile == NULL)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    media = &(((osa_coat_dev_t *)(osa_dev->dev))->media_dev);
    if ((governor != NULL) && (osa_strcmp(governor, "userspace") == 0)) {
        osa_printk("Not supported governor\r\n");
        return -1;
    }

    osa_dev->osa_profile = osa_profile;

    devfreq_para.initial_freq = osa_dev->osa_profile->initial_freq;
    devfreq_para.polling_ms = osa_dev->osa_profile->polling_ms;
    devfreq_para.freq_table = osa_dev->osa_profile->freq_table;
    devfreq_para.max_state = osa_dev->osa_profile->max_state;

    return media_devfreq_register(media, &devfreq_para);
}
EXPORT_SYMBOL(osa_devfreq_register);

void osa_devfreq_unregister(osa_dev_t *osa_dev)
{
    struct media_device *media = NULL;

    if ((osa_dev == NULL) || (osa_dev->osa_profile == NULL)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    media = &(((osa_coat_dev_t *)(osa_dev->dev))->media_dev);
    media_devfreq_unregister(media);
}
EXPORT_SYMBOL(osa_devfreq_unregister);

static struct media_ops s_osa_pmops = {
    .pm_prepare = osa_pm_prepare,
    .pm_complete = osa_pm_complete,
    .pm_suspend = osa_pm_suspend,
    .pm_resume = osa_pm_resume,
    .pm_freeze = osa_pm_freeze,
    .pm_thaw = osa_pm_thaw,
    .pm_poweroff = osa_pm_poweroff,
    .pm_restore = osa_pm_restore,
    .pm_suspend_late = osa_pm_suspend_late,
    .pm_resume_early = osa_pm_resume_early,
    .pm_freeze_late = osa_pm_freeze_late,
    .pm_thaw_early = osa_pm_thaw_early,
    .pm_poweroff_late = osa_pm_poweroff_late,
    .pm_restore_early = osa_pm_restore_early,
    .pm_suspend_noirq = osa_pm_suspend_noirq,
    .pm_resume_noirq = osa_pm_resume_noirq,
    .pm_freeze_noirq = osa_pm_freeze_noirq,
    .pm_thaw_noirq = osa_pm_thaw_noirq,
    .pm_poweroff_noirq = osa_pm_poweroff_noirq,
    .pm_restore_noirq = osa_pm_restore_noirq,

    .devfreq_target = osa_devfreq_target,
    .devfreq_get_dev_status = osa_devfreq_get_dev_status,
    .devfreq_get_cur_freq = osa_devfreq_get_cur_freq,
    .devfreq_exit = osa_devfreq_exit,
};

osa_dev_t *osa_createdev(const char *name)
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
EXPORT_SYMBOL(osa_createdev);

int osa_destroydev(osa_dev_t *osa_dev)
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
EXPORT_SYMBOL(osa_destroydev);

int osa_registerdevice(osa_dev_t *osa_dev)
{
    struct media_device *media = NULL;
    if ((osa_dev == NULL) || (osa_dev->fops == NULL)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    media = &(((osa_coat_dev_t *)(osa_dev->dev))->media_dev);
    if (osa_dev->minor != 0) {
        media->minor = osa_dev->minor;
    } else {
        media->minor = MEDIA_DYNAMIC_MINOR;
    }
    media->owner = THIS_MODULE;
    media->fops = &s_osa_fops;
    media->drvops = &s_osa_pmops;
    osa_strncpy(media->devfs_name, osa_dev->name, sizeof(media->devfs_name) - 1);
    return media_register(media);
}
EXPORT_SYMBOL(osa_registerdevice);

void osa_deregisterdevice(osa_dev_t *pdev)
{
    if (pdev == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    media_unregister((struct media_device *)&(((osa_coat_dev_t *)(pdev->dev))->media_dev));
}
EXPORT_SYMBOL(osa_deregisterdevice);

void osa_poll_wait(osa_poll_t *table, osa_wait_t *wait)
{
    if (DRVAL_DEBUG) {
        osa_printk("%s - call poll_wait +!, table=%pK, file=%pK\n", __FUNCTION__, table->poll_table, table->data);
    }

    poll_wait ((struct file *)table->data, (wait_queue_head_t *)(wait->wait), table->poll_table);

    if (DRVAL_DEBUG) {
        osa_printk("%s - call poll_wait -!\n", __FUNCTION__);
    }
}
EXPORT_SYMBOL(osa_poll_wait);

#if 0
void osa_pgprot_noncached(osa_vm_t *vm) {
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);
    v->vm_page_prot = pgprot_noncached(v->vm_page_prot);
}
#else
void osa_pgprot_noncached(osa_vm_t *vm)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);
    v->vm_page_prot = pgprot_writecombine(v->vm_page_prot);
}

#endif
EXPORT_SYMBOL(osa_pgprot_noncached);

void osa_pgprot_cached(osa_vm_t *vm)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);

#ifdef CONFIG_64BIT
    v->vm_page_prot = __pgprot(pgprot_val(v->vm_page_prot)
                               | PTE_VALID | PTE_DIRTY | PTE_AF);
#else

    v->vm_page_prot = __pgprot(pgprot_val(v->vm_page_prot) | L_PTE_PRESENT
                               | L_PTE_YOUNG | L_PTE_DIRTY | L_PTE_MT_DEV_CACHED);
#endif
}
EXPORT_SYMBOL(osa_pgprot_cached);

void osa_pgprot_writecombine(osa_vm_t *vm)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);
    v->vm_page_prot = pgprot_writecombine(v->vm_page_prot);
}
EXPORT_SYMBOL(osa_pgprot_writecombine);

void osa_pgprot_stronglyordered(osa_vm_t *vm)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);

#ifdef CONFIG_64BIT
    v->vm_page_prot = pgprot_device(v->vm_page_prot);
#else
    v->vm_page_prot = pgprot_stronglyordered(v->vm_page_prot);
#endif
}
EXPORT_SYMBOL(osa_pgprot_stronglyordered);

int osa_remap_pfn_range(osa_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);
    if (size == 0) {
        return -EPERM;
    }
    return remap_pfn_range(v, addr, pfn, size, v->vm_page_prot);
}
EXPORT_SYMBOL(osa_remap_pfn_range);

int osa_io_remap_pfn_range(osa_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size)
{
    struct vm_area_struct *v = (struct vm_area_struct *)(vm->vm);
# if 0 // TODO: vm_flags changed to const type
    v->vm_flags |= VM_IO;
#endif
    if (size == 0) {
        return -EPERM;
    }
    return io_remap_pfn_range(v, addr, pfn, size, v->vm_page_prot);
}
EXPORT_SYMBOL(osa_io_remap_pfn_range);


#ifdef CONFIG_SNAPSHOT_BOOT
int osa_call_usermodehelper_force(char *path, char **argv, char **envp, int wait)
{
    return call_usermodehelper_force(path, argv, envp, wait);
}
EXPORT_SYMBOL(osa_call_usermodehelper_force);
#endif
