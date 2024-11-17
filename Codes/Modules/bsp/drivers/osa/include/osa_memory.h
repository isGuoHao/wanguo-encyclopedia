#ifndef __OSA_MEMORY_H__
#define __OSA_MEMORY_H__

#include <linux/slab.h>
#include <linux/vmalloc.h>

// 内存分配标志
#define osa_gfp_kernel    0
#define osa_gfp_atomic    1

// 分配和释放虚拟内存
extern void *osa_vmalloc(unsigned long size);
extern void osa_vfree(const void *addr);

// 分配和释放内核内存
extern void *osa_kmalloc(unsigned long size, unsigned int osa_gfp_flag);
extern void osa_kfree(const void *addr);

#endif // __OSA_MEMORY_H__
