/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>
#include "osa_mmz.h"

void osa_flush_cache_all(void)
{
    flush_cache_all();
}
EXPORT_SYMBOL(osa_flush_cache_all);

void osa_cpuc_flush_dcache_area(void *addr, int size)
{
#ifdef CONFIG_64BIT
    __flush_dcache_area(addr, size);
#else
    __cpuc_flush_dcache_area(addr, size);
#endif
}
EXPORT_SYMBOL(osa_cpuc_flush_dcache_area);

void osa_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long length)
{
    mmz_mmb_flush_dcache_byaddr(kvirt, phys_addr, length);
}
EXPORT_SYMBOL(osa_flush_dcache_area);

int osa_flush_dcache_all(void)
{
#ifdef CONFIG_64BIT

    flush_cache_all();

#else

#ifdef CONFIG_SMP
    on_each_cpu((smp_call_func_t)__cpuc_flush_kern_all, NULL, 1);
#else
    __cpuc_flush_kern_all();
#endif

    outer_flush_all();

#endif

    return 0;
}
EXPORT_SYMBOL(osa_flush_dcache_all);

