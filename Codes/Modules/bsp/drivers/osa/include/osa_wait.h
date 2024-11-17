#ifndef __OSA_WAIT_H__
#define __OSA_WAIT_H__

#include <linux/wait.h>

// 等待条件函数类型
typedef int (*osa_wait_cond_func_t)(const void *param);

// 等待队列类型
typedef struct osa_wait {
    void *wait;
} osa_wait_t;

// 等待队列错误码
#define ERESTARTSYS        512

// 等待队列操作函数声明
extern unsigned long osa_msecs_to_jiffies(const unsigned int m);
extern int osa_wait_init(osa_wait_t *wait);
extern int osa_wait_interruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param);
extern int osa_wait_uninterruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param);
extern int osa_wait_timeout_interruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param,
                                           unsigned long ms);
extern int osa_wait_timeout_uninterruptible(osa_wait_t *wait, osa_wait_cond_func_t func, void *param,
                                             unsigned long ms);

// 等待事件宏定义
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

// 唤醒等待队列中的所有进程
extern void osa_wakeup(osa_wait_t *wait);

// 销毁等待队列
extern void osa_wait_destory(osa_wait_t *wait);

#endif // __OSA_WAIT_H__
