/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include <linux/slab.h>

int osa_mutex_init(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    if (mutex == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = kmalloc(sizeof(struct mutex), GFP_KERNEL);
    if (p == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    mutex_init(p);
    mutex->mutex = p;
    return 0;
}
EXPORT_SYMBOL(osa_mutex_init);

int osa_mutex_lock(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    if (mutex == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct mutex *)(mutex->mutex);
    mutex_lock(p);
    return 0;
}
EXPORT_SYMBOL(osa_mutex_lock);

int osa_mutex_lock_interruptible(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    if (mutex == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct mutex *)(mutex->mutex);
    return mutex_lock_interruptible(p);
}
EXPORT_SYMBOL(osa_mutex_lock_interruptible);

int osa_mutex_trylock(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    if (mutex == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct mutex *)(mutex->mutex);

    return mutex_trylock(p);
}
EXPORT_SYMBOL(osa_mutex_trylock);

void osa_mutex_unlock(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    p = (struct mutex *)(mutex->mutex);

    mutex_unlock(p);
}
EXPORT_SYMBOL(osa_mutex_unlock);

void osa_mutex_destory(osa_mutex_t *mutex)
{
    struct mutex *p = NULL;
    p = (struct mutex *)(mutex->mutex);
    kfree(p);
    mutex->mutex = NULL;
}
EXPORT_SYMBOL(osa_mutex_destory);
