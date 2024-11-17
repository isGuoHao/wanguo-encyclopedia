/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

int osa_printk(const char *fmt, ...)
{
    va_list args;
    int r;

    va_start(args, fmt);
    r = vprintk(fmt, args);
    va_end(args);

    return r;
}
EXPORT_SYMBOL(osa_printk);

void osa_panic(const char *fmt, const char *fun, int line, const char *cond)
{
    panic(fmt, fun, line, cond);
}
EXPORT_SYMBOL(osa_panic);
