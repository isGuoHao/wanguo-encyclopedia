#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

int osa_spin_lock_init(osa_spinlock_t *lock)
{
    spinlock_t *p = NULL;
    if (lock == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (spinlock_t *)kmalloc(sizeof(spinlock_t), GFP_KERNEL);
    if (p == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    spin_lock_init(p);
    lock->lock = p;
    return 0;
}
EXPORT_SYMBOL(osa_spin_lock_init);
void osa_spin_lock(osa_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    p = (spinlock_t *)(lock->lock);
    spin_lock(p);
}
EXPORT_SYMBOL(osa_spin_lock);
int osa_spin_trylock(osa_spinlock_t *lock)
{
    spinlock_t *p = NULL;
    if (lock == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (spinlock_t *)(lock->lock);
    return spin_trylock(p);
}
EXPORT_SYMBOL(osa_spin_trylock);
void osa_spin_unlock(osa_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    p = (spinlock_t *)(lock->lock);
    spin_unlock(p);
}
EXPORT_SYMBOL(osa_spin_unlock);
void osa_spin_lock_irqsave(osa_spinlock_t *lock, unsigned long *flags)
{
    spinlock_t *p = NULL;
    unsigned long f;

    p = (spinlock_t *)(lock->lock);
    spin_lock_irqsave(p, f);
    *flags = f;
}
EXPORT_SYMBOL(osa_spin_lock_irqsave);
void osa_spin_unlock_irqrestore(osa_spinlock_t *lock, unsigned long *flags)
{
    spinlock_t *p = NULL;
    unsigned long f;

    p = (spinlock_t *)(lock->lock);
    f = *flags;
    spin_unlock_irqrestore(p, f);
}
EXPORT_SYMBOL(osa_spin_unlock_irqrestore);
void osa_spin_lock_destory(osa_spinlock_t *lock)
{
    spinlock_t *p = NULL;
    p = (spinlock_t *)(lock->lock);
    kfree(p);
    lock->lock = NULL;
}
EXPORT_SYMBOL(osa_spin_lock_destory);
