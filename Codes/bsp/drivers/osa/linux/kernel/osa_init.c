/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include "osa.h"

extern void osa_proc_init(void);
extern void osa_proc_exit(void);
extern int media_init(void);
extern void media_exit(void);
extern int media_mem_init(void);
extern void media_mem_exit(void);
extern void osa_device_init(void);

static int __init osa_init(void)
{
    osa_device_init();
    osa_proc_init();
    media_init();
    media_mem_init();
    osa_printk("osa %s init success!\n", OSA_VERSION);
    return 0;
}

static void __exit osa_exit(void)
{
    media_exit();
    media_mem_exit();
    osa_proc_exit();
    osa_printk("osa v1.0 exit!\n");
}



#ifdef MODULE

module_init(osa_init);
module_exit(osa_exit);
MODULE_LICENSE("GPL");

#else

int __init osa_driver_init(void)
{
    return osa_init();
}

#endif


