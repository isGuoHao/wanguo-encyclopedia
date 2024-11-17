#ifndef __OSA_DEVICE_H__
#define __OSA_DEVICE_H__


#ifndef _IOC_TYPECHECK
#ifdef __KERNEL__
#include "osa_ioctl.h"
#endif
#endif

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

#define OSA_POLLIN        0x0001
#define OSA_POLLPRI       0x0002
#define OSA_POLLOUT       0x0004
#define OSA_POLLERR       0x0008
#define OSA_POLLHUP       0x0010
#define OSA_POLLNVAL      0x0020
#define OSA_POLLRDNORM    0x0040
#define OSA_POLLRDBAND    0x0080
#define OSA_POLLWRNORM    0x0100

#define OSA_SEEK_SET      0
#define OSA_SEEK_CUR      1
#define OSA_SEEK_END      2

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

typedef struct osa_dev {
    char name[48];
    void *dev;
    int minor;
    struct osa_fileops *fops;
    struct osa_pmops *osa_pmops;
    struct osa_devfreq_dev_profile *osa_profile;
} osa_dev_t;

typedef struct osa_devfreq_dev_profile {
    unsigned long initial_freq;
    unsigned int polling_ms;

    int (*target)(osa_dev_t *dev, unsigned long *freq, unsigned int flags);
    int (*get_dev_status)(osa_dev_t *dev, struct osa_devfreq_dev_status *stat);
    int (*get_cur_freq)(osa_dev_t *dev, unsigned long *freq);
    void (*exit)(osa_dev_t *dev);

    unsigned long *freq_table;
    unsigned int max_state;
} osa_devfreq_dev_profile_t;

typedef struct osa_vm {
    void *vm;
} osa_vm_t;

typedef struct osa_poll {
    void *poll_table;
    void *data;
} osa_poll_t;

typedef struct osa_fileops {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    long (*unlocked_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
    unsigned int (*poll)(osa_poll_t *osa_poll, void *private_data);
    int (*mmap)(osa_vm_t *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
#ifdef CONFIG_COMPAT
    long (*compat_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
#endif
} osa_fileops_t;

typedef struct osa_pmops {
    int (*pm_prepare)(osa_dev_t *dev);
    void (*pm_complete)(osa_dev_t *dev);
    int (*pm_suspend)(osa_dev_t *dev);
    int (*pm_resume)(osa_dev_t *dev);
    int (*pm_freeze)(osa_dev_t *dev);
    int (*pm_thaw)(osa_dev_t *dev);
    int (*pm_poweroff)(osa_dev_t *dev);
    int (*pm_restore)(osa_dev_t *dev);
    int (*pm_suspend_late)(osa_dev_t *dev);
    int (*pm_resume_early)(osa_dev_t *dev);
    int (*pm_freeze_late)(osa_dev_t *dev);
    int (*pm_thaw_early)(osa_dev_t *dev);
    int (*pm_poweroff_late)(osa_dev_t *dev);
    int (*pm_restore_early)(osa_dev_t *dev);
    int (*pm_suspend_noirq)(osa_dev_t *dev);
    int (*pm_resume_noirq)(osa_dev_t *dev);
    int (*pm_freeze_noirq)(osa_dev_t *dev);
    int (*pm_thaw_noirq)(osa_dev_t *dev);
    int (*pm_poweroff_noirq)(osa_dev_t *dev);
    int (*pm_restore_noirq)(osa_dev_t *dev);
} osa_pmops_t;

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

extern osa_dev_t *osa_createdev(const char *name);
extern int osa_destroydev(osa_dev_t *pdev);
extern int osa_registerdevice(osa_dev_t *pdev);
extern void osa_deregisterdevice(osa_dev_t *pdev);
extern void osa_poll_wait(osa_poll_t *table, osa_wait_t *wait);
extern void osa_pgprot_noncached(osa_vm_t *vm);
extern void osa_pgprot_cached(osa_vm_t *vm);
extern void osa_pgprot_writecombine(osa_vm_t *vm);
extern void osa_pgprot_stronglyordered(osa_vm_t *vm);
extern int osa_remap_pfn_range(osa_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size);
extern int osa_io_remap_pfn_range(osa_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size);
extern int osa_devfreq_register(osa_dev_t *osa_dev, struct osa_devfreq_dev_profile *osa_profile,
    const char *governor, void *data);
extern void osa_devfreq_unregister(osa_dev_t *osa_dev);

#endif // __OSA_DEVICE_H__
