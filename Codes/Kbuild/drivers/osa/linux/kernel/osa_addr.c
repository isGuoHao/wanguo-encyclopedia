/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/uaccess.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)

#ifndef CONFIG_64BIT
#include <mach/io.h>
#endif

#endif

void *osa_ioremap(unsigned long phys_addr, unsigned long size)
{
    return ioremap(phys_addr, size);
}
EXPORT_SYMBOL(osa_ioremap);

void *osa_ioremap_nocache(unsigned long phys_addr, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    return ioremap_nocache(phys_addr, size);
#else
    return ioremap(phys_addr, size);
#endif
}
EXPORT_SYMBOL(osa_ioremap_nocache);

void *osa_ioremap_cached(unsigned long phys_addr, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    return ioremap_cached(phys_addr, size);
#else
    return ioremap_cache(phys_addr, size);
#endif
}

void *osa_ioremap_wc(unsigned long phys_addr, unsigned long size)
{
    return ioremap_wc(phys_addr, size);
}
EXPORT_SYMBOL(osa_ioremap_wc);


EXPORT_SYMBOL(osa_ioremap_cached);

void osa_iounmap(void *addr)
{
    iounmap(addr);
}
EXPORT_SYMBOL(osa_iounmap);

unsigned long osa_copy_from_user(void *to, const void *from, unsigned long n)
{
    return copy_from_user(to, from, n);
}
EXPORT_SYMBOL(osa_copy_from_user);

unsigned long osa_copy_to_user(void *to, const void *from, unsigned long n)
{
    return copy_to_user(to, from, n);
}
EXPORT_SYMBOL(osa_copy_to_user);

int osa_access_ok(int type, const void *addr, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    return access_ok(type, addr, size);
#else
    return access_ok(addr, size);
#endif
}
EXPORT_SYMBOL(osa_access_ok);


