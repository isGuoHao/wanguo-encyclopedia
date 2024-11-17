/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/rtc.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 19, 0)
#include <linux/sched/clock.h>
#endif
#include "osa.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 19, 0)
struct timer_list_info {
    struct timer_list time_list;
    unsigned long data;
};
typedef void (*timer_callback_fun)(struct timer_list * data);
#endif

int osa_hrtimer_create(osa_hrtimer_t *phrtimer)
{
    return -1;
}
int osa_hrtimer_start(osa_hrtimer_t *phrtimer)
{
    return -1;
}
int osa_hrtimer_destory(osa_hrtimer_t *phrtimer)
{
    return -1;
}

unsigned long osa_timer_get_private_data(void *data)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 19, 0)
    struct timer_list_info *list_info = osa_container_of(data, struct timer_list_info, time_list);

    return list_info->data;
#else
    return (unsigned long)data;
#endif
}
EXPORT_SYMBOL(osa_timer_get_private_data);

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 19, 0)
int osa_timer_init(osa_timer_t *timer)
{
    struct timer_list_info *t = NULL;

    if (timer == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = (struct timer_list_info *)kmalloc(sizeof(struct timer_list_info), GFP_KERNEL);
    if (t == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    t->data = timer->data;
    #if 0 // TODO: 临时注释，规避编译报错
    timer_setup(&t->time_list, (timer_callback_fun)timer->function, 0);
    #endif
    timer->timer = t;
    return 0;
}
EXPORT_SYMBOL(osa_timer_init);

int osa_set_timer(osa_timer_t *timer, unsigned long interval)
{
    struct timer_list_info *list_info = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->function == NULL) || (interval == 0)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    list_info = (struct timer_list_info *)timer->timer;
    list_info->data = timer->data;
    #if 0 // TODO: 临时注释，规避编译报错
    list_info->time_list.function = (timer_callback_fun)timer->function;
    timer->timer = t;
    #endif
    return mod_timer(&list_info->time_list, jiffies + msecs_to_jiffies(interval) - 1);
}
EXPORT_SYMBOL(osa_set_timer);
#else
int osa_timer_init(osa_timer_t *timer)
{
    struct timer_list *t = NULL;

    if (timer == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = (struct timer_list *)kmalloc(sizeof(struct timer_list), GFP_KERNEL);
    if (t == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    init_timer(t);
    timer->timer = t;
    return 0;
}
EXPORT_SYMBOL(osa_timer_init);

int osa_set_timer(osa_timer_t *timer, unsigned long interval)
{
    struct timer_list *t = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->function == NULL) || (interval == 0)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    t = timer->timer;
    t->function = timer->function;
    t->data = timer->data;
    return mod_timer(t, jiffies + msecs_to_jiffies(interval) - 1);
}
EXPORT_SYMBOL(osa_set_timer);
#endif

int osa_del_timer(osa_timer_t *timer)
{
    struct timer_list *t = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->function == NULL)) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    t = timer->timer;
    return del_timer(t);
}
EXPORT_SYMBOL(osa_del_timer);

int osa_timer_destory(osa_timer_t *timer)
{
    struct timer_list *t = timer->timer;
    del_timer(t);
    kfree(t);
    timer->timer = NULL;
    return 0;
}
EXPORT_SYMBOL(osa_timer_destory);

unsigned long osa_msleep(unsigned int msecs)
{
    return msleep_interruptible(msecs);
}
EXPORT_SYMBOL(osa_msleep);

void osa_udelay(unsigned int usecs)
{
    udelay(usecs);
}
EXPORT_SYMBOL(osa_udelay);

void osa_mdelay(unsigned int msecs)
{
    mdelay(msecs);
}
EXPORT_SYMBOL(osa_mdelay);

unsigned int osa_get_tickcount()
{
    return jiffies_to_msecs(jiffies);
}
EXPORT_SYMBOL(osa_get_tickcount);

unsigned long long osa_sched_clock()
{
    return sched_clock();
}
EXPORT_SYMBOL(osa_sched_clock);

void osa_gettimeofday(osa_timeval_t *tv)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    struct timeval t;
    if (tv == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    do_gettimeofday(&t);

    tv->tv_sec = t.tv_sec;
    tv->tv_usec = t.tv_usec;
#else
    struct timespec64 t;
    if (tv == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    ktime_get_real_ts64(&t);

    tv->tv_sec = (long)(t.tv_sec);
    tv->tv_usec = t.tv_nsec / 1000;
#endif
}
EXPORT_SYMBOL(osa_gettimeofday);

void osa_rtc_time_to_tm(unsigned long time, osa_rtc_time_t *tm)
{
    struct rtc_time _tm = { 0 };
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    rtc_time_to_tm(time, &_tm);
#else
    rtc_time64_to_tm((time64_t)time, &_tm);
#endif
    tm->tm_sec = _tm.tm_sec;
    tm->tm_min = _tm.tm_min;
    tm->tm_hour = _tm.tm_hour;
    tm->tm_mday = _tm.tm_mday;
    tm->tm_mon = _tm.tm_mon;
    tm->tm_year = _tm.tm_year;
    tm->tm_wday = _tm.tm_wday;
    tm->tm_yday = _tm.tm_yday;
    tm->tm_isdst = _tm.tm_isdst;
}
EXPORT_SYMBOL(osa_rtc_time_to_tm);

void osa_rtc_tm_to_time(osa_rtc_time_t *tm, unsigned long *time)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    rtc_tm_to_time(&_tm, time);
#else
    *time = (unsigned long)rtc_tm_to_time64(&_tm);
#endif
}
EXPORT_SYMBOL(osa_rtc_tm_to_time);

void osa_getjiffies(unsigned long long *pjiffies)
{
    *pjiffies = jiffies;
}
EXPORT_SYMBOL(osa_getjiffies);

int osa_rtc_valid_tm(struct osa_rtc_time *tm)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;

    return rtc_valid_tm(&_tm);
}
EXPORT_SYMBOL(osa_rtc_valid_tm);


