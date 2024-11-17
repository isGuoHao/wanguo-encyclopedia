#ifndef __OSA_TASK_H__
#define __OSA_TASK_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

typedef struct osa_task {
    void *task_struct;
} osa_task_t;

typedef int (*threadfn_t)(void *data);

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

/**
 * osa_kthread_create - 创建一个新的内核线程
 * @thread: 线程函数指针
 * @data: 传递给线程函数的数据
 * @name: 线程名称
 * 返回: 新创建的线程结构指针
 */
extern osa_task_t *osa_kthread_create(threadfn_t thread, void *data, const char *name);

/**
 * osa_kthread_destory - 销毁一个内核线程
 * @task: 要销毁的线程结构指针
 * @stop_flag: 停止标志
 */
extern void osa_kthread_destory(osa_task_t *task, unsigned int stop_flag);

/**
 * osa_kthread_should_stop - 检查线程是否应该停止
 * 返回: 如果线程应该停止则返回非零值，否则返回0
 */
extern int osa_kthread_should_stop(void);

#endif // __OSA_TASK_H__
