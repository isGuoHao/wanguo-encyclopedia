/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#ifndef __OSA__
#define __OSA__




// wait api
typedef int (*osa_wait_cond_func_t)(const void *param);

typedef struct osa_wait {
    void *wait;
} osa_wait_t;
#define ERESTARTSYS        512

extern unsigned long osa_msecs_to_jiffies(const unsigned int m);
extern int osa_wait_init(osa_wait_t *wait);
extern int osa_wait_interruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param);
extern int osa_wait_uninterruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param);
extern int osa_wait_timeout_interruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param,
                                           unsigned long ms);
extern int osa_wait_timeout_uninterruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param,
                                             unsigned long ms);

#define osa_wait_event_interruptible(wait, func, param)                       \
    ({                                                                         \
        int __ret = 0;                                                         \
                                                                               \
        for (;;) {                                                             \
            if (func(param)) {                                                 \
                __ret = 0;                                                     \
                break;                                                         \
            }                                                                  \
            __ret = osa_wait_timeout_interruptible(wait, (func), param, 100); \
            if (__ret < 0)                                                     \
                break;                                                         \
        }                                                                      \
        __ret;                                                                 \
    })

#define osa_wait_event_uninterruptible(wait, func, param)          \
    ({                                                              \
        int __ret = 0;                                              \
                                                                    \
        for (;;) {                                                  \
            if (func(param)) {                                      \
                __ret = 0;                                          \
                break;                                              \
            }                                                       \
            __ret = osa_wait_uninterruptible(wait, (func), param); \
            if (__ret < 0)                                          \
                break;                                              \
        }                                                           \
        __ret;                                                      \
    })

#define osa_wait_event_timeout_interruptible(wait, func, param, timeout)        \
    ({                                                                           \
        int __ret = timeout;                                                     \
                                                                                 \
        if ((func(param)) && !timeout) {                                         \
            __ret = 1;                                                           \
        }                                                                        \
                                                                                 \
        for (;;) {                                                               \
            if (func(param)) {                                                   \
                __ret = osa_msecs_to_jiffies(__ret);                            \
                break;                                                           \
            }                                                                    \
            __ret = osa_wait_timeout_interruptible(wait, (func), param, __ret); \
            if (!__ret || __ret == -ERESTARTSYS)                                 \
                break;                                                           \
        }                                                                        \
        __ret;                                                                   \
    })

#define osa_wait_event_timeout_uninterruptible(wait, func, param, timeout)        \
    ({                                                                             \
        int __ret = timeout;                                                       \
                                                                                   \
        if ((func(param)) && !timeout) {                                           \
            __ret = 1;                                                             \
        }                                                                          \
                                                                                   \
        for (;;) {                                                                 \
            if (func(param)) {                                                     \
                __ret = osa_msecs_to_jiffies(__ret);                              \
                break;                                                             \
            }                                                                      \
            __ret = osa_wait_timeout_uninterruptible(wait, (func), param, __ret); \
            if (!__ret || __ret == -ERESTARTSYS)                                   \
                break;                                                             \
        }                                                                          \
        __ret;                                                                     \
    })

extern void osa_wakeup(osa_wait_t *wait);  // same as wake_up_all
extern void osa_wait_destory(osa_wait_t *wait);

// workqueue api
typedef struct osa_work_struct {
    void *work;
    void (*func)(struct osa_work_struct *work);
} osa_work_struct_t;
typedef void (*osa_work_func_t)(struct osa_work_struct *work);

extern int osa_init_work(struct osa_work_struct *work, osa_work_func_t func);

#define OSA_INIT_WORK(_work, _func)      \
    do {                                  \
        osa_init_work((_work), (_func)); \
    } while (0)

extern int osa_schedule_work(struct osa_work_struct *work);
extern void osa_destroy_work(struct osa_work_struct *work);

// shedule
extern void osa_yield(void);

// interrupt api
enum osa_irqreturn {
    OSA_IRQ_NONE = (0 << 0),
    OSA_IRQ_HANDLED = (1 << 0),
    OSA_IRQ_WAKE_THREAD = (1 << 1),
};

typedef int (*osa_irq_handler_t)(int, void *);
extern int osa_request_irq(unsigned int irq, osa_irq_handler_t handler, osa_irq_handler_t thread_fn,
                            const char *name, void *dev);
extern void osa_free_irq(unsigned int irq, void *dev);
extern int osa_in_interrupt(void);

#define OSA_DIS_IRQ_CNT   2
typedef void (*osa_gic_handle_t)(unsigned int, unsigned int, void *);
extern int osa_register_gic_handle(unsigned int index, unsigned int irq, osa_gic_handle_t handle, const char *name,
                                    void *dev);
extern int osa_unregister_gic_handle(unsigned int index, unsigned int irq, void *dev);

// task api
typedef struct osa_task {
    void *task_struct;
} osa_task_t;
typedef int (*threadfn_t)(void *data);
extern osa_task_t *osa_kthread_create(threadfn_t thread, void *data, const char *name);
extern void osa_kthread_destory(osa_task_t *task, unsigned int stop_flag);
int osa_kthread_should_stop(void);

// string api
extern char *osa_strcpy(char *s1, const char *s2);
extern char *osa_strncpy(char *s1, const char *s2, int size);
extern int osa_strlcpy(char *s1, const char *s2, int size);
extern char *osa_strcat(char *s1, const char *s2);
extern char *osa_strncat(char *s1, const char *s2, int size);
extern int osa_strlcat(char *s1, const char *s2, int size);
extern int osa_strcmp(const char *s1, const char *s2);
extern int osa_strncmp(const char *s1, const char *s2, int size);
extern int osa_strnicmp(const char *s1, const char *s2, int size);
extern int osa_strcasecmp(const char *s1, const char *s2);
extern int osa_strncasecmp(const char *s1, const char *s2, int n);
extern char *osa_strchr(const char *s, int n);
extern char *osa_strnchr(const char *s, int count, int c);
extern char *osa_strrchr(const char *s, int c);
extern char *osa_strstr(const char *s1, const char *s2);
extern char *osa_strnstr(const char *s1, const char *s2, int n);
extern int osa_strlen(const char *s);
extern int osa_strnlen(const char *s, int size);
extern char *osa_strpbrk(const char *s1, const char *s2);
extern char *osa_strsep(char **s, const char *ct);
extern int osa_strspn(const char *s, const char *accept);
extern int osa_strcspn(const char *s, const char *reject);
extern void *osa_memset(void *str, int c, int count);
extern void *osa_memcpy(void *s1, const void *s2, int count);
extern void *osa_memmove(void *s1, const void *s2, int count);
extern void *osa_memscan(void *addr, int c, int size);
extern int osa_memcmp(const void *cs, const void *ct, int count);
extern void *osa_memchr(const void *s, int c, int n);
extern void *osa_memchr_inv(const void *s, int c, int n);

extern unsigned long long osa_strtoull(const char *cp, char **endp, unsigned int base);
extern unsigned long osa_strtoul(const char *cp, char **endp, unsigned int base);
extern long osa_strtol(const char *cp, char **endp, unsigned int base);
extern long long osa_strtoll(const char *cp, char **endp, unsigned int base);
extern int osa_snprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osa_scnprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osa_sprintf(char *buf, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
extern int osa_sscanf(const char *buf, const char *fmt, ...);

// addr translate
extern void *osa_ioremap(unsigned long phys_addr, unsigned long size);
extern void *osa_ioremap_nocache(unsigned long phys_addr, unsigned long size);
extern void *osa_ioremap_cached(unsigned long phys_addr, unsigned long size);
extern void *osa_ioremap_wc(unsigned long phys_addr, unsigned long size);
extern void osa_iounmap(void *addr);

#define osa_readl(x) (*((volatile int *)(x)))
#define osa_writel(v, x) (*((volatile int *)(x)) = (v))

extern unsigned long osa_copy_from_user(void *to, const void *from, unsigned long n);
extern unsigned long osa_copy_to_user(void *to, const void *from, unsigned long n);

#define OSA_VERIFY_READ   0
#define OSA_VERIFY_WRITE  1
extern int osa_access_ok(int type, const void *addr, unsigned long size);

// cache api
extern void osa_flush_cache_all(void);
extern void osa_cpuc_flush_dcache_area(void *addr, int size);

extern void osa_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long length);
extern int osa_flush_dcache_all(void);

// math
extern unsigned long long osa_div_u64(unsigned long long dividend, unsigned int divisor);
extern long long osa_div_s64(long long dividend, int divisor);
extern unsigned long long osa_div64_u64(unsigned long long dividend, unsigned long long divisor);
extern long long osa_div64_s64(long long dividend, long long divisor);
extern unsigned long long osa_div_u64_rem(unsigned long long dividend, unsigned int divisor);
extern long long osa_div_s64_rem(long long dividend, int divisor);
extern unsigned long long osa_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);
extern unsigned int osa_random(void);

#define osa_max(x, y) ({                            \
        __typeof__(x) _max1 = (x);                  \
        __typeof__(y) _max2 = (y);                  \
        (void) (&_max1 == &_max2);              \
        _max1 > _max2 ? _max1 : _max2; })

#define osa_min(x, y) ({                \
    __typeof__(x) _min1 = (x);          \
     __typeof__(y) _min2 = (y);          \
     (void) (&_min1 == &_min2);      \
     _min1 < _min2 ? _min1 : _min2; })

#define osa_abs(x) ({                \
    long ret;                         \
    if (sizeof(x) == sizeof(long)) {  \
        long __x = (x);               \
        ret = (__x < 0) ? -__x : __x; \
    } else {                          \
        int __x = (x);                \
        ret = (__x < 0) ? -__x : __x; \
    }                                 \
    ret;                              \
})

// barrier
extern void osa_mb(void);
extern void osa_rmb(void);
extern void osa_wmb(void);
extern void osa_smp_mb(void);
extern void osa_smp_rmb(void);
extern void osa_smp_wmb(void);
extern void osa_isb(void);
extern void osa_dsb(void);
extern void osa_dmb(void);

// debug
extern int osa_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
extern void osa_panic(const char *fmt, const char *fun, int line, const char *);
#define OSA_BUG() \
    do {           \
    } while (1)

#define OSA_ASSERT(expr)                       \
    do {                                        \
        if (!(expr)) {                          \
            osa_printk("\nASSERT failed at:\n" \
                        "  >Condition: %s\n",   \
                #expr);                         \
            OSA_BUG();                         \
        }                                       \
    } while (0)

#define OSA_BUG_ON(expr)                                                               \
    do {                                                                                \
        if (expr) {                                                                     \
            osa_printk("BUG: failure at %d/%s()!\n", __LINE__, __func__); \
            OSA_BUG();                                                                 \
        }                                                                               \
    } while (0)

// proc
typedef struct osa_proc_dir_entry {
    char name[50];
    void *proc_dir_entry;
    int (*open)(struct osa_proc_dir_entry *entry);
    int (*read)(struct osa_proc_dir_entry *entry);
    int (*write)(struct osa_proc_dir_entry *entry, const char *buf, int count, long long *);
    void *private;
    void *seqfile;
    struct osa_list_head node;
} osa_proc_entry_t;
extern osa_proc_entry_t *osa_create_proc_entry(const char *name, osa_proc_entry_t *parent);
extern osa_proc_entry_t *osa_proc_mkdir(const char *name, osa_proc_entry_t *parent);
extern void osa_remove_proc_entry(const char *name, osa_proc_entry_t *parent);
extern void osa_seq_printf(osa_proc_entry_t *entry, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

// device api
#ifndef _IOC_TYPECHECK
#ifdef __KERNEL__
#include "osa_ioctl.h"
#endif
#endif
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

#define OSA_POLLIN        0x0001
#define OSA_POLLPRI       0x0002
#define OSA_POLLOUT       0x0004
#define OSA_POLLERR       0x0008
#define OSA_POLLHUP       0x0010
#define OSA_POLLNVAL      0x0020
#define OSA_POLLRDNORM    0x0040
#define OSA_POLLRDBAND    0x0080
#define OSA_POLLWRNORM    0x0100

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

#define OSA_SEEK_SET      0
#define OSA_SEEK_CUR      1
#define OSA_SEEK_END      2

// #define PAGE_SHIFT         12

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


// timer
typedef struct osa_timer {
    void *timer;
    void (*function)(unsigned long);
    unsigned long data;
} osa_timer_t;

typedef struct osa_timeval {
    long tv_sec;
    long tv_usec;
} osa_timeval_t;

typedef struct osa_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} osa_rtc_time_t;

/* Return values for the timer callback function */
typedef enum OSA_HRTIMER_RESTART_E {
    OSA_HRTIMER_NORESTART, /* < The timer will not be restarted. */
    OSA_HRTIMER_RESTART /* < The timer must be restarted. */
} OSA_HRTIMER_RESTART_E;

/* hrtimer struct */
typedef struct osa_hrtimer {
    void *timer;
    OSA_HRTIMER_RESTART_E (*function)(void *timer);
    unsigned long interval; /* Unit ms */
} osa_hrtimer_t;

extern int osa_hrtimer_create(osa_hrtimer_t *phrtimer);
extern int osa_hrtimer_start(osa_hrtimer_t *phrtimer);
extern int osa_hrtimer_destory(osa_hrtimer_t *phrtimer);

extern int osa_timer_init(osa_timer_t *timer);
extern int osa_set_timer(osa_timer_t *timer, unsigned long interval);  /* ms */
extern unsigned long osa_timer_get_private_data(void *data);
extern int osa_del_timer(osa_timer_t *timer);
extern int osa_timer_destory(osa_timer_t *timer);

extern unsigned long osa_msleep(unsigned int msecs);
extern void osa_udelay(unsigned int usecs);
extern void osa_mdelay(unsigned int msecs);

extern unsigned int osa_get_tickcount(void);
extern unsigned long long osa_sched_clock(void);
extern void osa_gettimeofday(osa_timeval_t *tv);
extern void osa_rtc_time_to_tm(unsigned long time, osa_rtc_time_t *tm);
extern void osa_rtc_tm_to_time(osa_rtc_time_t *tm, unsigned long *time);
extern int osa_rtc_valid_tm(struct osa_rtc_time *tm);
extern void osa_getjiffies(unsigned long long *pjiffies);

#define OSA_O_ACCMODE     00000003
#define OSA_O_RDONLY      00000000
#define OSA_O_WRONLY      00000001
#define OSA_O_RDWR        00000002
#define OSA_O_CREAT       00000100

extern void *osa_klib_fopen(const char *filename, int flags, int mode);
extern void osa_klib_fclose(void *filp);
extern int osa_klib_fwrite(const char *buf, int len, void *filp);
extern int osa_klib_fread(char *buf, unsigned int len, void *filp);

// reboot
struct osa_notifier_block {
    int (*notifier_call)(struct osa_notifier_block *nb, unsigned long action, void *data);
    void *notifier_block;
};
typedef int (*osa_notifier_fn_t)(struct osa_notifier_block *nb, unsigned long action, void *data);

extern int osa_register_reboot_notifier(struct osa_notifier_block *nb);
extern int osa_unregister_reboot_notifier(struct osa_notifier_block *nb);

#include <linux/stdarg.h>

#ifndef _OSA_VA_LIST

#define _OSA_VA_LIST
#define osa_va_list       va_list
#define osa_va_arg(ap, T) va_arg(ap, T)
#define osa_va_end(ap) va_end(ap)
#define osa_va_start(ap, A) va_start(ap, A)

#endif /* va_arg */

#define NULL_STRING        "NULL"

extern void osa_vprintk(const char *fmt, osa_va_list args);
extern int osa_vsnprintf(char *str, int size, const char *fmt, osa_va_list args);

#ifdef CONFIG_SNAPSHOT_BOOT

#ifndef OSA_UMH_WAIT_PROC
#define OSA_UMH_WAIT_PROC 2 /* wait for the process to complete */
#endif

extern int osa_call_usermodehelper_force(char *path, char **argv, char **envp, int wait);
#endif

int osa_platform_driver_register(void *drv);
void osa_platform_driver_unregister(void *drv);
void *osa_platform_get_resource_byname(void *dev, unsigned int type,
                                        const char *name);
void *osa_platform_get_resource(void *dev, unsigned int type,
                                 unsigned int num);
int osa_platform_get_irq(void *dev, unsigned int num);
int osa_platform_get_irq_byname(void *dev, const char *name);

#define osa_module_driver(osa_driver, osa_register, osa_unregister, ...) \
    static int __init osa_driver##_init(void)                               \
    {                                                                        \
        return osa_register(&(osa_driver));                                \
    }                                                                        \
    module_init(osa_driver##_init);                                         \
    static void __exit osa_driver##_exit(void)                              \
    {                                                                        \
        osa_unregister(&(osa_driver));                                     \
    }                                                                        \
    module_exit(osa_driver##_exit);

#define osa_module_platform_driver(platform_driver)                   \
    osa_module_driver(platform_driver, osa_platform_driver_register, \
        osa_platform_driver_unregister)

#endif
