#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/semaphore.h>
#include <linux/slab.h>

int osa_sema_init(osa_semaphore_t *sem, int val)
{
    struct semaphore *p = NULL;
    if (sem == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    if (p == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    sema_init(p, val);
    sem->sem = p;
    return 0;
}
EXPORT_SYMBOL(osa_sema_init);
int osa_down(osa_semaphore_t *sem)
{
    struct semaphore *p = NULL;
    if (sem == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    down(p);
    return 0;
}
EXPORT_SYMBOL(osa_down);
int osa_down_interruptible(osa_semaphore_t *sem)
{
    struct semaphore *p = NULL;
    if (sem == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    return down_interruptible(p);
}
EXPORT_SYMBOL(osa_down_interruptible);
int osa_down_trylock(osa_semaphore_t *sem)
{
    struct semaphore *p = NULL;
    if (sem == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    return down_trylock(p);
}
EXPORT_SYMBOL(osa_down_trylock);
void osa_up(osa_semaphore_t *sem)
{
    struct semaphore *p = NULL;
    p = (struct semaphore *)(sem->sem);
    up(p);
}
EXPORT_SYMBOL(osa_up);
void osa_sema_destory(osa_semaphore_t *sem)
{
    struct semaphore *p = NULL;
    p = (struct semaphore *)(sem->sem);
    kfree(p);
    sem->sem = NULL;
}
EXPORT_SYMBOL(osa_sema_destory);
