#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/devfreq.h>

#include "base.h"
#include "osa_devfreq.h"

static OSA_LIST_HEAD(media_list);
static DEFINE_MUTEX(media_sem);

int media_devfreq_target(struct device *dev, unsigned long *freq, unsigned int flags);
int media_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat);
int media_devfreq_get_cur_freq(struct device *dev, unsigned long *freq);
void media_devfreq_exit(struct device *dev);
int media_init(void);
void media_exit(void);


/*
 * Assigned numbers, used for dynamic minors
 */
#define DYNAMIC_MINORS 64 /* like dynamic majors */
static unsigned char media_minors[DYNAMIC_MINORS / 8];

int media_devfreq_target(struct device *dev, unsigned long *freq, unsigned int flags)
{
    struct media_device *pdev = to_media_device(dev);
    struct media_driver *pdrv = to_media_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_target == NULL)) {
        return 0;
    }

    return pdrv->ops->devfreq_target(pdev, freq, flags);
}

int media_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat)
{
    struct media_device *pdev = to_media_device(dev);
    struct media_driver *pdrv = to_media_driver(dev->driver);
    osa_devfreq_dev_status_t pstat;

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_get_dev_status == NULL)) {
        return 0;
    }
    pstat.total_time = stat->total_time;
    pstat.busy_time = stat->busy_time;
    pstat.current_frequency = stat->current_frequency;
    pstat.private_data = stat->private_data;

    return pdrv->ops->devfreq_get_dev_status(pdev, &pstat);
}

int media_devfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
    struct media_device *pdev = to_media_device(dev);
    struct media_driver *pdrv = to_media_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_get_cur_freq == NULL)) {
        return 0;
    }

    return pdrv->ops->devfreq_get_cur_freq(pdev, freq);
}

void media_devfreq_exit(struct device *dev)
{
    struct media_device *pdev = to_media_device(dev);
    struct media_driver *pdrv = to_media_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_exit == NULL)) {
        return;
    }

    return pdrv->ops->devfreq_exit(pdev);
}

int media_devfreq_register(struct media_device *media, osa_devfreq_para_t *devfreq_para)
{
    struct device *dev = (struct device *)&media->device;
    struct devfreq_dev_profile *profile = &(media->profile);

    profile->initial_freq = devfreq_para->initial_freq;
    profile->polling_ms = devfreq_para->polling_ms;
    profile->target = media_devfreq_target;
    profile->get_dev_status = media_devfreq_get_dev_status;
    profile->get_cur_freq = media_devfreq_get_cur_freq;
    profile->exit = media_devfreq_exit;
    profile->freq_table = devfreq_para->freq_table;
    profile->max_state = devfreq_para->max_state;

    media->devfreq = devm_devfreq_add_device(dev, profile, "userspace", NULL);
    if (IS_ERR(media->devfreq)) {
        printk("failed to add devfreq device\n");
        return -1;
    }

    return 0;
}

void media_devfreq_unregister(struct media_device *media)
{
    struct device *dev = (struct device *)&media->device;

    devm_devfreq_remove_device(dev, media->devfreq);

    return;
}

static int media_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);
    struct media_device *c = NULL;
    int err = -ENODEV;
    const struct file_operations *old_fops, *new_fops = NULL;

    mutex_lock(&media_sem);

    osa_list_for_each_entry(c, &media_list, list) {
        if (c->minor == minor) {
            new_fops = fops_get(c->fops);
            break;
        }
    }

    if (new_fops == NULL) {
        mutex_unlock(&media_sem);
        request_module("char-major-%d-%d", MEDIA_DEVICE_MAJOR, minor);
        mutex_lock(&media_sem);

        osa_list_for_each_entry(c, &media_list, list) {
            if (c->minor == minor) {
                new_fops = fops_get(c->fops);
                break;
            }
        }

        if (new_fops == NULL) {
            goto fail;
        }
    }

    err = 0;

    old_fops = file->f_op;
    file->f_op = new_fops;
    if (file->f_op->open) {
        file->private_data = c;
        err = file->f_op->open(inode, file);
        if (err) {
            fops_put(file->f_op);
            file->private_data = NULL;
            file->f_op = fops_get(old_fops);
        }
    }

    fops_put(old_fops);
fail:
    mutex_unlock(&media_sem);
    return err;
}

static struct file_operations media_fops = {
    .owner = THIS_MODULE,
    .open = media_open,
};

/**
 *    media_register - register a media device
 *    @media: device structure
 *
 *    Register a media device with the kernel. If the minor
 *    number is set to %MEDIA_DYNAMIC_MINOR a minor number is assigned
 *    and placed in the minor field of the structure. For other cases
 *    the minor number requested is used.
 *
 *    The structure passed is linked into the kernel and may not be
 *    destroyed until it has been unregistered.
 *
 *    A zero is returned on success and a negative errno code for
 *    failure.
 */

int media_register(struct media_device *media)
{
    struct media_device *ptmp = NULL;
    struct media_driver *pdrv = NULL;

    int err = 0;

    mutex_lock(&media_sem);

    /* check if registered */
    osa_list_for_each_entry(ptmp, &media_list, list) {
        if (ptmp->minor == media->minor) {
            mutex_unlock(&media_sem);
            return -EBUSY;
        }
    }

    /* check minor */
    if (media->minor == MEDIA_DYNAMIC_MINOR) {
        int i = DYNAMIC_MINORS;

        while (--i >= 0)
            if ((media_minors[(unsigned int)i >> 3] & (1 << ((unsigned int)i & 7))) == 0) {
                break;
            }
        if (i < 0) {
            mutex_unlock(&media_sem);
            return -EBUSY;
        }

        media->minor = i;
    }

    if (media->minor < DYNAMIC_MINORS) {
        media_minors[media->minor >> 3] |= 1 << (media->minor & 7);
    }

    /* device register */
    err = media_device_register(media);
    if (err < 0) {
        media_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));
        goto out;
    }

    /* driver register */
    pdrv = media_driver_register(media->devfs_name, media->owner, media->drvops);
    if (IS_ERR(pdrv)) {
        media_device_unregister(media);

        media_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));

        err = PTR_ERR(pdrv);
        goto out;
    }

    media->driver = pdrv;

    /*
     * Add it to the front, so that later devices can "override"
     * earlier defaults
     */
    osa_list_add(&media->list, &media_list);

out:
    mutex_unlock(&media_sem);
    return err;
}
EXPORT_SYMBOL(media_register);

/**
 *    media_unregister - unregister a media device
 *    @media: device to unregister
 *
 *    Unregister a media device that was previously
 *    successfully registered with media_register(). Success
 *    is indicated by a zero return, a negative errno code
 *    indicates an error.
 */

int media_unregister(struct media_device *media)
{
    struct media_device *ptmp = NULL, *_ptmp = NULL;

    if (osa_list_empty(&media->list)) {
        return -EINVAL;
    }

    mutex_lock(&media_sem);

    osa_list_for_each_entry_safe(ptmp, _ptmp, &media_list, list) {
        /* if found, unregister device & driver */
        if (ptmp->minor == media->minor) {
            osa_list_del(&media->list);

            media_driver_unregister(media->driver);

            media->driver = NULL;

            media_device_unregister(media);

            media_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));

            break;
        }
    }

    mutex_unlock(&media_sem);

    return 0;
}
EXPORT_SYMBOL(media_unregister);

int media_init(void)
{
    int ret;
    // 1
    ret = media_bus_init();
    if (ret) {
        goto err0;
    }

    // 2
    ret = -EIO;

    if (register_chrdev(MEDIA_DEVICE_MAJOR, "media", &media_fops)) {
        goto err1;
    }

    printk("Module media: init ok\n");

    return 0;

    // 3
err1:
    media_bus_exit();
err0:
    return ret;
}

void media_exit(void)
{
    // 0
    if (!osa_list_empty(&media_list)) {
        printk("!!! Module media: sub module in list\n");
        return;
    }

    unregister_chrdev(MEDIA_DEVICE_MAJOR, "media");

    media_bus_exit();

    printk("!!! Module media: exit ok\n");
}
