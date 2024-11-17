#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/kmod.h>

#include "osa.h"
#include "pdc.h"
/*****************************************************************************/
/*                              pdc bus methods                              */
/*****************************************************************************/

static dev_t pdc_dev_num;

static void pdc_bus_release(struct device *dev)
{
    return;
}

struct device pdc_bus_dev = {
    .init_name = "pdc",
    .release = pdc_bus_release
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
/* sysfs modalias store/show */
static ssize_t modalias_show(struct device *dev, struct device_attribute *a,
                             char *buf)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    int len = snprintf(buf, PAGE_SIZE, "media:%s\n", pdev->devfs_name);

    return (len >= PAGE_SIZE) ? (PAGE_SIZE - 1) : len;
}

static struct device_attribute pdc_dev_attrs[] = {
    __ATTR_RO(modalias),
    __ATTR_NULL,
};
#endif

/* bus match & uevent */
static int pdc_match(struct device *dev, struct device_driver *drv)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    return (strncmp(pdev->devfs_name, drv->name, sizeof(pdev->devfs_name)) == 0);
}

static int pdc_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    add_uevent_var(env, "MODALIAS=media:%s", pdev->devfs_name);
    return 0;
}


/*****************************************************************************/
/*                              pdc pm methods                               */
/*****************************************************************************/

static int pdc_pm_prepare(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_prepare == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_prepare(pdev);
}

static void pdc_pm_complete(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_complete == NULL)) {
        return;
    }

    pdrv->ops->pm_complete(pdev);
}

static int pdc_pm_suspend(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_suspend == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_suspend(pdev);
}

static int pdc_pm_resume(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_resume == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_resume(pdev);
}

static int pdc_pm_freeze(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_freeze == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_freeze(pdev);
}

static int pdc_pm_thaw(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_thaw == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_thaw(pdev);
}

static int pdc_pm_poweroff(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_poweroff == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_poweroff(pdev);
}

static int pdc_pm_restore(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_restore == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_restore(pdev);
}
static int pdc_pm_suspend_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_suspend_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_suspend_noirq(pdev);
}

static int pdc_pm_resume_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_resume_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_resume_noirq(pdev);
}

static int pdc_pm_freeze_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_freeze_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_freeze_noirq(pdev);
}

static int pdc_pm_thaw_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_thaw_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_thaw_noirq(pdev);
}

static int pdc_pm_poweroff_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_poweroff_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_poweroff_noirq(pdev);
}

static int pdc_pm_restore_noirq(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->pm_restore_noirq == NULL)) {
        return 0;
    }

    return pdrv->ops->pm_restore_noirq(pdev);
}


static struct dev_pm_ops pdc_bus_pm_ops = {
    .prepare = pdc_pm_prepare,
    .complete = pdc_pm_complete,

    // with irq
    .suspend = pdc_pm_suspend,
    .resume = pdc_pm_resume,

    .freeze = pdc_pm_freeze,
    .thaw = pdc_pm_thaw,
    .poweroff = pdc_pm_poweroff,
    .restore = pdc_pm_restore,

    // with noirq
    .suspend_noirq = pdc_pm_suspend_noirq,
    .resume_noirq = pdc_pm_resume_noirq,
    .freeze_noirq = pdc_pm_freeze_noirq,
    .thaw_noirq = pdc_pm_thaw_noirq,
    .poweroff_noirq = pdc_pm_poweroff_noirq,
    .restore_noirq = pdc_pm_restore_noirq,
};

/*****************************************************************************/
/*                          pdc devfreq api                                  */
/*****************************************************************************/

int pdc_devfreq_target(struct device *dev, unsigned long *freq, unsigned int flags)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_target == NULL)) {
        return 0;
    }

    return pdrv->ops->devfreq_target(pdev, freq, flags);
}

int pdc_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);
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

int pdc_devfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_get_cur_freq == NULL)) {
        return 0;
    }

    return pdrv->ops->devfreq_get_cur_freq(pdev, freq);
}

void pdc_devfreq_exit(struct device *dev)
{
    struct pdc_device *pdev = to_pdc_device(dev);
    struct pdc_driver *pdrv = to_pdc_driver(dev->driver);

    if ((pdrv->ops == NULL) || (pdrv->ops->devfreq_exit == NULL)) {
        return;
    }

    return pdrv->ops->devfreq_exit(pdev);
}

int pdc_devfreq_register(struct pdc_device *media, osa_devfreq_para_t *devfreq_para)
{
    struct device *dev = (struct device *)&media->device;
    struct devfreq_dev_profile *profile = &(media->profile);

    profile->initial_freq = devfreq_para->initial_freq;
    profile->polling_ms = devfreq_para->polling_ms;
    profile->target = pdc_devfreq_target;
    profile->get_dev_status = pdc_devfreq_get_dev_status;
    profile->get_cur_freq = pdc_devfreq_get_cur_freq;
    profile->exit = pdc_devfreq_exit;
    profile->freq_table = devfreq_para->freq_table;
    profile->max_state = devfreq_para->max_state;

    media->devfreq = devm_devfreq_add_device(dev, profile, "userspace", NULL);
    if (IS_ERR(media->devfreq)) {
        printk("failed to add devfreq device\n");
        return -1;
    }

    return 0;
}

void pdc_devfreq_unregister(struct pdc_device *media)
{
    struct device *dev = (struct device *)&media->device;

    devm_devfreq_remove_device(dev, media->devfreq);

    return;
}

/*****************************************************************************/
/*                              pdc master api                               */
/*****************************************************************************/
static OSA_LIST_HEAD(pdc_list);
static DEFINE_MUTEX(pdc_sem);

/*
 * Assigned numbers, used for dynamic minors
 */
#define DYNAMIC_MINORS 64 /* like dynamic majors */
static unsigned char pdc_minors[DYNAMIC_MINORS / 8];

/**
 *    pdc_register - register a media device
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

int pdc_master_register(struct pdc_device *media)
{
    struct pdc_device *ptmp = NULL;
    struct pdc_driver *pdrv = NULL;

    int err = 0;

    mutex_lock(&pdc_sem);

    /* check if registered */
    osa_list_for_each_entry(ptmp, &pdc_list, list) {
        if (ptmp->minor == media->minor) {
            mutex_unlock(&pdc_sem);
            return -EBUSY;
        }
    }

    /* check minor */
    if (media->minor == PDC_DYNAMIC_MINOR) {
        int i = DYNAMIC_MINORS;

        while (--i >= 0)
            if ((pdc_minors[(unsigned int)i >> 3] & (1 << ((unsigned int)i & 7))) == 0) {
                break;
            }
        if (i < 0) {
            mutex_unlock(&pdc_sem);
            return -EBUSY;
        }

        media->minor = i;
    }

    if (media->minor < DYNAMIC_MINORS) {
        pdc_minors[media->minor >> 3] |= 1 << (media->minor & 7);
    }

    /* device register */
    err = pdc_device_register(media);
    if (err < 0) {
        pdc_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));
        goto out;
    }

    /* driver register */
    pdrv = pdc_driver_register(media->devfs_name, media->owner, media->drvops);
    if (IS_ERR(pdrv)) {
        pdc_device_unregister(media);

        pdc_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));

        err = PTR_ERR(pdrv);
        goto out;
    }

    media->driver = pdrv;

    /*
     * Add it to the front, so that later devices can "override"
     * earlier defaults
     */
    osa_list_add(&media->list, &pdc_list);

out:
    mutex_unlock(&pdc_sem);
    return err;
}
EXPORT_SYMBOL(pdc_master_register);

/**
 *    pdc_unregister - unregister a media device
 *    @media: device to unregister
 *
 *    Unregister a media device that was previously
 *    successfully registered with pdc_register(). Success
 *    is indicated by a zero return, a negative errno code
 *    indicates an error.
 */

int pdc_master_unregister(struct pdc_device *media)
{
    struct pdc_device *ptmp = NULL, *_ptmp = NULL;

    if (osa_list_empty(&media->list)) {
        return -EINVAL;
    }

    mutex_lock(&pdc_sem);

    osa_list_for_each_entry_safe(ptmp, _ptmp, &pdc_list, list) {
        /* if found, unregister device & driver */
        if (ptmp->minor == media->minor) {
            osa_list_del(&media->list);

            pdc_driver_unregister(media->driver);

            media->driver = NULL;

            pdc_device_unregister(media);

            pdc_minors[media->minor >> 3] &= ~(1 << (media->minor & 7));

            break;
        }
    }

    mutex_unlock(&pdc_sem);

    return 0;
}
EXPORT_SYMBOL(pdc_master_unregister);


/*****************************************************************************/
/*                              pdc bus api                                  */
/*****************************************************************************/

struct bus_type pdc_bus_type = {
    .name = "pdc",
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    .dev_attrs = pdc_dev_attrs,
#endif
    .match = pdc_match,
    .uevent = pdc_uevent,
    .pm = &pdc_bus_pm_ops,
};


static void pdc_device_release(struct device *dev)
{
    return;
}

int pdc_device_register(struct pdc_device *pdev)
{
    dev_set_name(&pdev->device, "%s", pdev->devfs_name);

    pdev->device.devt = MKDEV(pdc_dev_num, pdev->minor);
    pdev->device.release = pdc_device_release;
    pdev->device.bus = &pdc_bus_type;

    return device_register(&pdev->device);
}

void pdc_device_unregister(struct pdc_device *pdev)
{
    device_unregister(&pdev->device);
}

struct pdc_driver *pdc_driver_register(const char *name, struct module *owner, struct pdc_ops *ops)
{
    int ret;
    struct pdc_driver *pdrv = NULL;
#ifdef MODULE
    if ((name == NULL) || (owner == NULL)) {
#else
    if (name == NULL){
#endif
        return ERR_PTR(-EINVAL);
    }

    pdrv = kzalloc(sizeof(struct pdc_driver) + strnlen(name, MEDIA_MAX_DEV_NAME_LEN), GFP_KERNEL);
    if (pdrv == NULL) {
        return ERR_PTR(-ENOMEM);
    }

    /* init driver object */
    strncpy(pdrv->name, name, strnlen(name, MEDIA_MAX_DEV_NAME_LEN));

    pdrv->ops = ops;

    pdrv->driver.name = pdrv->name;
    pdrv->driver.owner = owner;
    pdrv->driver.bus = &pdc_bus_type;

    ret = driver_register(&pdrv->driver);
    if (ret) {
        kfree(pdrv);
        return ERR_PTR(ret);
    }

    return pdrv;
}

void pdc_driver_unregister(struct pdc_driver *pdrv)
{
    if (pdrv != NULL) {
        driver_unregister(&pdrv->driver);
        kfree(pdrv);
    }
}

/*****************************************************************************/
/*                                pdc init                                   */
/*****************************************************************************/
static int __init pdc_init(void)
{
    int iRet;

    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    iRet = device_register(&pdc_bus_dev);
    if (iRet) {
        return iRet;
    }

    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    iRet = bus_register(&pdc_bus_type);
    if (iRet){
        goto cleanup_device;
    }

    return 0;

cleanup_device:
    device_unregister(&pdc_bus_dev);
    return iRet;

}

static void __exit pdc_exit(void)
{
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    bus_unregister(&pdc_bus_type);
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    device_unregister(&pdc_bus_dev);
}

module_init(pdc_init);
module_exit(pdc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC Driver Module.");
