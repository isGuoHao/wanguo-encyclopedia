#ifndef __OSA_SEMAPHORE_H__
#define __OSA_SEMAPHORE_H__

#include <linux/semaphore.h>

// 信号量错误码
#define EINTR              4

// 信号量类型
typedef struct osa_semaphore {
    void *sem;
} osa_semaphore_t;

// 信号量操作函数声明
extern int osa_sema_init(osa_semaphore_t *sem, int val);
extern int osa_down(osa_semaphore_t *sem);
extern int osa_down_interruptible(osa_semaphore_t *sem);
extern int osa_down_trylock(osa_semaphore_t *sem);
extern void osa_up(osa_semaphore_t *sem);
extern void osa_sema_destory(osa_semaphore_t *sem);

#endif // __OSA_SEMAPHORE_H__
