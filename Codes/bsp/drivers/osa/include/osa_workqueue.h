#ifndef __OSA_WORKQUEUE_H__
#define __OSA_WORKQUEUE_H__

#include <linux/workqueue.h>

// 工作队列类型
typedef struct osa_work_struct {
    void *work;
    void (*func)(struct osa_work_struct *work);
} osa_work_struct_t;

// 工作队列函数类型
typedef void (*osa_work_func_t)(struct osa_work_struct *work);

// 初始化工作队列
extern int osa_init_work(struct osa_work_struct *work, osa_work_func_t func);

// 初始化工作队列的宏定义
#define OSA_INIT_WORK(_work, _func)      \
    do {                                  \
        osa_init_work((_work), (_func)); \
    } while (0)

// 调度工作队列
extern int osa_schedule_work(struct osa_work_struct *work);

// 销毁工作队列
extern void osa_destroy_work(struct osa_work_struct *work);

#endif // __OSA_WORKQUEUE_H__
