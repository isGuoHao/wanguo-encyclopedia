/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/atomic.h>
#include <linux/printk.h>
#include <linux/slab.h>

int osa_atomic_init(osa_atomic_t *atomic)
{
    atomic_t *p = NULL;
    if (atomic == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (atomic_t *)kmalloc(sizeof(atomic_t), GFP_KERNEL);
    if (p == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    memset(p, 0, sizeof(atomic_t));
    atomic->atomic = p;
    return 0;
}
EXPORT_SYMBOL(osa_atomic_init);
void osa_atomic_destory(osa_atomic_t *atomic)
{
    kfree(atomic->atomic);
    atomic->atomic = NULL;
}
EXPORT_SYMBOL(osa_atomic_destory);
int osa_atomic_read(osa_atomic_t *atomic)
{
    atomic_t *p = NULL;
    if (atomic == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (atomic_t *)(atomic->atomic);
    return atomic_read(p);
}
EXPORT_SYMBOL(osa_atomic_read);
void osa_atomic_set(osa_atomic_t *atomic, int i)
{
    atomic_t *p = NULL;
    p = (atomic_t *)(atomic->atomic);
    atomic_set(p, i);
}
EXPORT_SYMBOL(osa_atomic_set);
int osa_atomic_inc_return(osa_atomic_t *atomic)
{
    atomic_t *p = NULL;
    if (atomic == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (atomic_t *)(atomic->atomic);
    return atomic_inc_return(p);
}
EXPORT_SYMBOL(osa_atomic_inc_return);
int osa_atomic_dec_return(osa_atomic_t *atomic)
{
    atomic_t *p = NULL;
    if (atomic == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (atomic_t *)(atomic->atomic);
    return atomic_dec_return(p);
}
EXPORT_SYMBOL(osa_atomic_dec_return);
