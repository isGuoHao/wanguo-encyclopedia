/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/interrupt.h>

int osa_request_irq(unsigned int irq, osa_irq_handler_t handler, osa_irq_handler_t thread_fn, const char *name,
                     void *dev)
{
    unsigned long flags = IRQF_SHARED;

    return request_threaded_irq(irq, (irq_handler_t)handler, (irq_handler_t)thread_fn, flags, name, dev);
}
EXPORT_SYMBOL(osa_request_irq);
void osa_free_irq(unsigned int irq, void *dev)
{
    free_irq(irq, dev);
}
EXPORT_SYMBOL(osa_free_irq);

int osa_in_interrupt(void)
{
    return in_interrupt();
}
EXPORT_SYMBOL(osa_in_interrupt);

#if 0
#ifdef CONFIG_ARM_GIC

#define DIS_IRQ_CNT 2
struct gic_sgi_handle {
    unsigned int irq;
    void (*handle)(unsigned int cpu_intrf,
                   unsigned int irq_num,
                   struct pt_regs *regs);
};
int osa_register_gic_handle(unsigned int index, unsigned int irq,
    osa_gic_handle_t handle, const char *name, void *dev)
{
    extern struct gic_sgi_handle dis_irq_handle[DIS_IRQ_CNT];


    if (index >= DIS_IRQ_CNT) {
        printk("index(%d) is error.\n", index);
        return -1;
    }
    dis_irq_handle[index].irq = irq;
    dis_irq_handle[index].handle = (void (*)(unsigned int, unsigned int, struct pt_regs *))(handle);

    return 0;
}
EXPORT_SYMBOL(osa_register_gic_handle);

int osa_unregister_gic_handle(unsigned int index, unsigned int irq, void *dev)
{
    extern struct gic_sgi_handle dis_irq_handle[DIS_IRQ_CNT];

    if (index >= DIS_IRQ_CNT) {
        printk("index(%d) is error.\n", index);
        return -1;
    }
    dis_irq_handle[index].irq = 0xF;
    dis_irq_handle[index].handle = NULL;

    return 0;
}
EXPORT_SYMBOL(osa_unregister_gic_handle);
#endif
#endif
