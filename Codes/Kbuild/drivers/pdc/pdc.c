#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init pdc_init(void)
{
    printk(KERN_INFO "PDC driver loaded.\n");
    return 0;
}

static void __exit pdc_exit(void)
{
    printk(KERN_INFO "PDC driver unloaded.\n");
}

module_init(pdc_init);
module_exit(pdc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PDC Driver Module.");
