/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>

#include "osa.h"

void osa_mb(void)
{
    mb();
}
EXPORT_SYMBOL(osa_mb);
void osa_rmb(void)
{
    rmb();
}
EXPORT_SYMBOL(osa_rmb);
void osa_wmb(void)
{
    wmb();
}
EXPORT_SYMBOL(osa_wmb);
void osa_smp_mb(void)
{
    smp_mb();
}
EXPORT_SYMBOL(osa_smp_mb);
void osa_smp_rmb(void)
{
    smp_rmb();
}
EXPORT_SYMBOL(osa_smp_rmb);
void osa_smp_wmb(void)
{
    smp_wmb();
}
EXPORT_SYMBOL(osa_smp_wmb);
void osa_isb(void)
{
    isb();
}
EXPORT_SYMBOL(osa_isb);
void osa_dsb(void)
{
#ifdef CONFIG_64BIT
    dsb(sy);
#else
    dsb();
#endif
}
EXPORT_SYMBOL(osa_dsb);
void osa_dmb(void)
{
#ifdef CONFIG_64BIT
    dmb(sy);
#else
    dmb();
#endif
}
EXPORT_SYMBOL(osa_dmb);
