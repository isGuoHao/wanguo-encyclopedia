#ifndef _LINUX_MEDIA_DEVICE_H_
#define _LINUX_MEDIA_DEVICE_H_

#include <linux/module.h>
#include <linux/major.h>
#include <linux/device.h>
#include <linux/devfreq.h>

#include "osa_list.h"
#include "osa_devfreq.h"

#define PDC_DEVICE_NAME "pdc"

// #define PDC_DEVICE_MAJOR        251        // 设置为0自动分配
#define PDC_DYNAMIC_MINOR       255

struct pdc_device;

struct pdc_ops {
    // pm methos
    int (*pm_prepare)(struct pdc_device *);
    void (*pm_complete)(struct pdc_device *);

    int (*pm_suspend)(struct pdc_device *);
    int (*pm_resume)(struct pdc_device *);

    int (*pm_freeze)(struct pdc_device *);
    int (*pm_thaw)(struct pdc_device *);
    int (*pm_poweroff)(struct pdc_device *);
    int (*pm_restore)(struct pdc_device *);

    int (*pm_suspend_late)(struct pdc_device *);
    int (*pm_resume_early)(struct pdc_device *);
    int (*pm_freeze_late)(struct pdc_device *);
    int (*pm_thaw_early)(struct pdc_device *);
    int (*pm_poweroff_late)(struct pdc_device *);
    int (*pm_restore_early)(struct pdc_device *);

    int (*pm_suspend_noirq)(struct pdc_device *);
    int (*pm_resume_noirq)(struct pdc_device *);

    int (*pm_freeze_noirq)(struct pdc_device *);
    int (*pm_thaw_noirq)(struct pdc_device *);
    int (*pm_poweroff_noirq)(struct pdc_device *);
    int (*pm_restore_noirq)(struct pdc_device *);

    /* devfreq */
    int (*devfreq_target)(struct pdc_device *, unsigned long *, unsigned int);
    int (*devfreq_get_dev_status)(struct pdc_device *, struct osa_devfreq_dev_status *);
    int (*devfreq_get_cur_freq)(struct pdc_device *, unsigned long *);
    void (*devfreq_exit)(struct pdc_device *);
};

#define MEDIA_MAX_DEV_NAME_LEN 32

struct pdc_driver {
    struct device_driver driver;
    struct pdc_ops *ops;
    char name[1];
};

#define to_pdc_driver(drv) \
    container_of((drv), struct pdc_driver, driver)

struct pdc_device {
    struct osa_list_head list;

    char devfs_name[MEDIA_MAX_DEV_NAME_LEN];

    unsigned int minor;

    struct device device;

    struct module *owner;

    const struct file_operations *fops;

    struct pdc_ops *drvops;

    /* for internal use */
    struct pdc_driver *driver;

    struct devfreq *devfreq;
    struct devfreq_dev_profile profile;
};


#define to_pdc_device(dev) \
    container_of((dev), struct pdc_device, device)


// pdc devfreq
int pdc_devfreq_target(struct device *dev, unsigned long *freq, unsigned int flags);
int pdc_devfreq_get_dev_status(struct device *dev, struct devfreq_dev_status *stat);
int pdc_devfreq_get_cur_freq(struct device *dev, unsigned long *freq);
void pdc_devfreq_exit(struct device *dev);
int pdc_devfreq_register(struct pdc_device *media, osa_devfreq_para_t *devfreq_para);
void pdc_devfreq_unregister(struct pdc_device *media);


// pdc device
int pdc_device_register(struct pdc_device *pdev);
void pdc_device_unregister(struct pdc_device *pdev);

// pdc driver
struct pdc_driver *pdc_driver_register(const char *name, struct module *owner, struct pdc_ops *ops);
void pdc_driver_unregister(struct pdc_driver *pdrv);


// pdc master
void pdc_master_init(void);
int pdc_master_unregister(struct pdc_device *media);
    int pdc_master_register(struct pdc_device *media);

// pdc module alias
#define MODULE_ALIAS_MEDIA(minor) \
    MODULE_ALIAS("media-char-major-" __stringify(MEDIA_DEVICE_MAJOR) "-" __stringify(minor))


// pdc api
osa_dev_t *pdc_createdev(const char *name);
int pdc_destroydev(osa_dev_t *osa_dev);
int pdc_registerdevice(osa_dev_t *osa_dev);
void pdc_deregisterdevice(osa_dev_t *pdev);


#endif /* _LINUX_MEDIA_DEVICE_H_ */
