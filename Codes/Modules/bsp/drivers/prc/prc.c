#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init prc_init(void)
{
    printk(KERN_INFO "PRC driver loaded.\n");
    return 0;
}

static void __exit prc_exit(void)
{
    printk(KERN_INFO "PRC driver unloaded.\n");
}

module_init(prc_init);
module_exit(prc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PRC Driver Module.");
