#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i3c/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "cpld_core.h"

// I3C 读写
static int cpld_i3c_read(struct cpld_device *cpld_dev, u8 reg, u8 *data)
{
    struct i3c_priv_xfer xfer[2];
    u8 buf[1] = {reg};

    xfer[0].len = 1;
    xfer[0].type = I3C_PRIV_XFER_WRITE;
    xfer[0].data.out = buf;

    xfer[1].len = 1;
    xfer[1].type = I3C_PRIV_XFER_READ;
    xfer[1].data.in = data;

    return i3c_device_do_priv_xfers(cpld_dev->i3c_dev, xfer, ARRAY_SIZE(xfer));
}

static int cpld_i3c_write(struct cpld_device *cpld_dev, u8 reg, u8 data)
{
    struct i3c_priv_xfer xfer;
    u8 buf[2] = {reg, data};

    xfer.len = 2;
    xfer.type = I3C_PRIV_XFER_WRITE;
    xfer.data.out = buf;

    return i3c_device_do_priv_xfers(cpld_dev->i3c_dev, &xfer, 1);
}

// I3C 探测函数
static int cpld_i3c_probe(struct i3c_device *dev)
{
    struct cpld_device *cpld_dev;
    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev)
        return -ENOMEM;

    cpld_dev->i3c_dev = dev;
    cpld_dev->read = cpld_i3c_read;
    cpld_dev->write = cpld_i3c_write;

    if (cpld_register_device(cpld_dev) < 0) {
        kfree(cpld_dev);
        return -ENODEV;
    }

    pr_info("CPLD I3C device probed\n");
    return 0;
}

// I3C 移除函数
static void cpld_i3c_remove(struct i3c_device *dev)
{
    struct cpld_device *cpld_dev, *tmp;

    list_for_each_entry_safe(cpld_dev, tmp, &cpld_driver_instance->devices, list) {
        if (cpld_dev->i3c_dev == dev) {
            cpld_unregister_device(cpld_dev);
            pr_info("CPLD I3C device removed\n");
            break;
        }
    }
}

static const struct of_device_id cpld_i3c_of_match[] = {
    { .compatible = "wanguo,cpld-i3c" },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, cpld_i3c_of_match);

struct i3c_driver cpld_i3c_driver = {
    .probe = cpld_i3c_probe,
    .remove = cpld_i3c_remove,
    .of_match_table = cpld_i3c_of_match,
};

static int __init cpld_i3c_init(void)
{
    return i3c_add_driver(&cpld_i3c_driver);
}

static void __exit cpld_i3c_exit(void)
{
    i3c_del_driver(&cpld_i3c_driver);
}

module_init(cpld_i3c_init);
module_exit(cpld_i3c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("CPLD I3C Driver");
