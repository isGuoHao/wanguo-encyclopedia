#ifndef __OSA_SPINLOCK_H__
#define __OSA_SPINLOCK_H__

#include <linux/spinlock.h>

// 自旋锁类型
typedef struct osa_spinlock {
    void *lock;
} osa_spinlock_t;

// 自旋锁操作函数声明
extern int osa_spin_lock_init(osa_spinlock_t *lock);
extern void osa_spin_lock(osa_spinlock_t *lock);
extern int osa_spin_trylock(osa_spinlock_t *lock);
extern void osa_spin_unlock(osa_spinlock_t *lock);
extern void osa_spin_lock_irqsave(osa_spinlock_t *lock, unsigned long *flags);
extern void osa_spin_unlock_irqrestore(osa_spinlock_t *lock, unsigned long *flags);
// notice:must be called when kmod exit, other wise will lead to memory leak;
extern void osa_spin_lock_destory(osa_spinlock_t *lock);

#endif // __OSA_SPINLOCK_H__
