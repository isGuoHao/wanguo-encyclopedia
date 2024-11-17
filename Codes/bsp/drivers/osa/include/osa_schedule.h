#ifndef __OSA_SCHEDULE_H__
#define __OSA_SCHEDULE_H__

#include <linux/sched.h>

// 让出CPU
extern void osa_yield(void);

#endif // __OSA_SCHEDULE_H__
