#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include "osa.h"

void *osa_vmalloc(unsigned long size)
{
    return vmalloc(size);
}
EXPORT_SYMBOL(osa_vmalloc);

void osa_vfree(const void *addr)
{
    vfree(addr);
}
EXPORT_SYMBOL(osa_vfree);

void *osa_kmalloc(unsigned long size, unsigned int osa_gfp_flag)
{
    if (osa_gfp_flag == osa_gfp_kernel) {
        return kmalloc(size, GFP_KERNEL);
    } else if (osa_gfp_flag == osa_gfp_atomic) {
        return kmalloc(size, GFP_ATOMIC);
    } else {
        return NULL;
    }
}
EXPORT_SYMBOL(osa_kmalloc);

void osa_kfree(const void *addr)
{
    kfree(addr);
}

EXPORT_SYMBOL(osa_kfree);


