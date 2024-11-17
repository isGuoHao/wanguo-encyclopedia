#ifndef __OSA_INTERRUPT_H__
#define __OSA_INTERRUPT_H__

#include <linux/interrupt.h>

// 中断返回值枚举
enum osa_irqreturn {
    OSA_IRQ_NONE = (0 << 0),
    OSA_IRQ_HANDLED = (1 << 0),
    OSA_IRQ_WAKE_THREAD = (1 << 1),
};

// 中断处理函数类型
typedef int (*osa_irq_handler_t)(int, void *);

// 请求中断
extern int osa_request_irq(unsigned int irq, osa_irq_handler_t handler, osa_irq_handler_t thread_fn,
                            const char *name, void *dev);

// 释放中断
extern void osa_free_irq(unsigned int irq, void *dev);

// 检查是否在中断上下文中
extern int osa_in_interrupt(void);

// GIC中断处理
#define OSA_DIS_IRQ_CNT   2
typedef void (*osa_gic_handle_t)(unsigned int, unsigned int, void *);

// 注册GIC中断处理函数
extern int osa_register_gic_handle(unsigned int index, unsigned int irq, osa_gic_handle_t handle, const char *name,
                                    void *dev);

// 注销GIC中断处理函数
extern int osa_unregister_gic_handle(unsigned int index, unsigned int irq, void *dev);

#endif // __OSA_INTERRUPT_H__
