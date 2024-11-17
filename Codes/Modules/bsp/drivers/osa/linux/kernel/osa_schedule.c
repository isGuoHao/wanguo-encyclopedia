#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

void osa_yield(void)
{
    // yield();
    cond_resched();
}
EXPORT_SYMBOL(osa_yield);
