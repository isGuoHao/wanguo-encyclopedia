#include <linux/module.h>
#include <linux/i3c/master.h>
#include <linux/i3c/device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/list.h>

#include "cpld_private.h"
#include "cpld_ioctl.h"

// I3C 操作函数
static int cpld_i3c_read(union cpld_client client, u8 addr, u8 *value)
{
    struct i3c_device *i3cdev = client.i3c;
    struct i3c_priv_xfer xfer;
    u8 buffer[2]; // 1 byte for address, 1 byte for data
    int ret;

    buffer[0] = addr;
    xfer.rnw = true;
    xfer.len = 2; // 1 byte for address, 1 byte for data
    xfer.data.in = buffer;

    ret = i3c_device_do_priv_xfers(i3cdev, &xfer, 1);
    if (ret < 0) {
        pr_err("I3C read error: %d\n", ret);
        return ret;
    }

    *value = buffer[1]; // Extract the data from the buffer

    return 0;
}

static int cpld_i3c_write(union cpld_client client, u8 addr, u8 value)
{
    struct i3c_device *i3cdev = client.i3c;
    struct i3c_priv_xfer xfer;
    u8 buffer[2]; // 1 byte for address, 1 byte for data
    int ret;

    buffer[0] = addr;
    buffer[1] = value;
    xfer.rnw = false;
    xfer.len = 2; // 1 byte for address, 1 byte for data
    xfer.data.out = buffer;

    ret = i3c_device_do_priv_xfers(i3cdev, &xfer, 1);
    if (ret < 0) {
        pr_err("I3C write error: %d\n", ret);
        return ret;
    }

    return 0;
}

static const struct cpld_ops cpld_i3c_ops = {
    .read = cpld_i3c_read,
    .write = cpld_i3c_write,
};

static int cpld_i3c_probe(struct i3c_device *i3cdev)
{
    struct cpld_device *cpld_dev;

    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev)
        return -ENOMEM;

    cpld_dev->device_id = i3cdev->desc->info.dyn_addr;
    cpld_dev->client.i3c = i3cdev;
    cpld_dev->ops = &cpld_i3c_ops;
    cpld_dev->type = CPLD_TYPE_I3C; // 设置为 I3C 类型

    cpld_register_new_device(cpld_dev);

    pr_info("I3C Device %d probed\n", cpld_dev->device_id);

    return 0;
}

static void cpld_i3c_remove(struct i3c_device *i3cdev)
{
    struct cpld_device *cpld_dev = dev_get_drvdata(&i3cdev->dev);

    if (!cpld_dev) {
        dev_err(&i3cdev->dev, "Device data is NULL\n");
        return;
    }

    cpld_unregister_device(cpld_dev);

    kfree(cpld_dev);

    pr_info("I3C Device %d removed\n", i3cdev->desc->info.dyn_addr);
}


static struct i3c_driver cpld_i3c_driver = {
    .probe = cpld_i3c_probe,
    .remove = cpld_i3c_remove,
    .driver.name = "cpld_i3c",
};

int cpld_i3c_init(void)
{
    return i3c_driver_register(&cpld_i3c_driver);
}

void cpld_i3c_exit(void)
{
    i3c_driver_unregister(&cpld_i3c_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("I3C driver for CPLD");
