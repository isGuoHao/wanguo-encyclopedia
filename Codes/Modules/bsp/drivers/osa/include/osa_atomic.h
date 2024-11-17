#ifndef __OSA_ATOMIC_H__
#define __OSA_ATOMIC_H__

#include <linux/atomic.h>

// 原子操作类型
// atomic api
typedef struct {
    void *atomic;
} osa_atomic_t;


// 初始化原子变量
#define OSA_ATOMIC_INIT(i)  { ATOMIC_INIT(i) }

// 原子操作函数声明
extern int osa_atomic_init(osa_atomic_t *atomic);
extern void osa_atomic_destory(osa_atomic_t *atomic);
extern int osa_atomic_read(osa_atomic_t *v);
extern void osa_atomic_set(osa_atomic_t *v, int i);
extern int osa_atomic_inc_return(osa_atomic_t *v);
extern int osa_atomic_dec_return(osa_atomic_t *v);

#endif // __OSA_ATOMIC_H__
