#ifndef __OSA_MMAP_H__
#define __OSA_MMAP_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

/**
 * osa_ioremap - 映射物理地址到虚拟地址
 * @phys_addr: 物理地址
 * @size: 映射的大小
 * 返回: 映射后的虚拟地址
 */
extern void *osa_ioremap(unsigned long phys_addr, unsigned long size);

/**
 * osa_ioremap_nocache - 映射物理地址到不可缓存的虚拟地址
 * @phys_addr: 物理地址
 * @size: 映射的大小
 * 返回: 映射后的虚拟地址
 */
extern void *osa_ioremap_nocache(unsigned long phys_addr, unsigned long size);

/**
 * osa_ioremap_cached - 映射物理地址到缓存的虚拟地址
 * @phys_addr: 物理地址
 * @size: 映射的大小
 * 返回: 映射后的虚拟地址
 */
extern void *osa_ioremap_cached(unsigned long phys_addr, unsigned long size);

/**
 * osa_ioremap_wc - 映射物理地址到写合并的虚拟地址
 * @phys_addr: 物理地址
 * @size: 映射的大小
 * 返回: 映射后的虚拟地址
 */
extern void *osa_ioremap_wc(unsigned long phys_addr, unsigned long size);

/**
 * osa_iounmap - 取消内存映射
 * @addr: 要取消映射的虚拟地址
 */
extern void osa_iounmap(void *addr);

#define osa_readl(x) (*((volatile int *)(x)))
#define osa_writel(v, x) (*((volatile int *)(x)) = (v))


extern unsigned long osa_copy_from_user(void *to, const void *from, unsigned long n);
extern unsigned long osa_copy_to_user(void *to, const void *from, unsigned long n);

#define OSA_VERIFY_READ   0
#define OSA_VERIFY_WRITE  1
extern int osa_access_ok(int type, const void *addr, unsigned long size);

#endif // __OSA_MMAP_H__
