/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/kthread.h>
#include <linux/slab.h>

osa_task_t *osa_kthread_create(threadfn_t thread, void *data, const char *name)
{
    struct task_struct *k = NULL;
    osa_task_t *p = (osa_task_t *)kmalloc(sizeof(osa_task_t), GFP_KERNEL);
    if (p == NULL) {
        osa_printk("%s - kmalloc error!\n", __FUNCTION__);
        return NULL;
    }
    memset(p, 0, sizeof(osa_task_t));

    k = kthread_run(thread, data, name);
    if (IS_ERR(k)) {
        osa_printk("%s - kthread create error!\n", __FUNCTION__);
        kfree(p);
        return NULL;
    }
    p->task_struct = k;
    return p;
}
EXPORT_SYMBOL(osa_kthread_create);

void osa_kthread_destory(osa_task_t *task, unsigned int stop_flag)
{
    if (task == NULL) {
        osa_printk("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    /* note: When you call the Kthread_stop function, the thread function cannot be finished, otherwise it will oops. */
    if (stop_flag != 0) {
        kthread_stop ((struct task_struct *)(task->task_struct));
    }
    task->task_struct = NULL;
    kfree(task);
}
EXPORT_SYMBOL(osa_kthread_destory);

int osa_kthread_should_stop(void)
{
    return kthread_should_stop();
}
EXPORT_SYMBOL(osa_kthread_should_stop);
