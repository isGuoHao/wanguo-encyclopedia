#ifndef __OSA_MUTEX_H__
#define __OSA_MUTEX_H__

#include <linux/mutex.h>

// 互斥锁类型
typedef struct osa_mutex {
    void *mutex;
} osa_mutex_t;

// 互斥锁操作函数声明
extern int osa_mutex_init(osa_mutex_t *mutex);
extern int osa_mutex_lock(osa_mutex_t *mutex);
extern int osa_mutex_lock_interruptible(osa_mutex_t *mutex);
extern int osa_mutex_trylock(osa_mutex_t *mutex);
extern void osa_mutex_unlock(osa_mutex_t *mutex);
// notice:must be called when kmod exit, other wise will lead to memory leak;
extern void osa_mutex_destory(osa_mutex_t *mutex);

#endif // __OSA_MUTEX_H__
