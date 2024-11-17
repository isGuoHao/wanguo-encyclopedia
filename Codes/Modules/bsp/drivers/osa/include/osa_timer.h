#ifndef __OSA_TIMER_H__
#define __OSA_TIMER_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

typedef enum OSA_HRTIMER_RESTART_E {
    OSA_HRTIMER_NORESTART, /* < The timer will not be restarted. */
    OSA_HRTIMER_RESTART /* < The timer must be restarted. */
} OSA_HRTIMER_RESTART_E;

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

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

typedef struct osa_hrtimer {
    void *timer;
    OSA_HRTIMER_RESTART_E (*function)(void *timer);
    unsigned long interval; /* Unit ms */
} osa_hrtimer_t;

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

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

#endif // __OSA_TIMER_H__
