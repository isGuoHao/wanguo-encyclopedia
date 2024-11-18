#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_cpld.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
static int pdc_cpld_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) {
#else
static int pdc_cpld_i2c_probe(struct i2c_client *client) {
#endif
    struct pdc_cpld_device *cpld_dev;
    int ret;

    printk(KERN_INFO "CPLD I2C Device probed\n");

    ret = pdc_cpld_device_alloc(&cpld_dev);
    if (ret) {
        return ret;
    }

    cpld_dev->i2c_client = client;

    // Initialize any CPLD-specific data here

    // Register the device
    ret = pdc_cpld_device_register(cpld_dev);
    if (ret) {
        pdc_cpld_device_free(cpld_dev);
        return ret;
    }

    i2c_set_clientdata(client, cpld_dev);

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
static int pdc_cpld_i2c_remove(struct i2c_client *client) {
#else
static void pdc_cpld_i2c_remove(struct i2c_client *client) {
#endif
    struct pdc_cpld_device *cpld_dev = i2c_get_clientdata(client);

    printk(KERN_INFO "CPLD I2C Device removed\n");

    pdc_cpld_device_unregister(cpld_dev);
    pdc_cpld_device_free(cpld_dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
    return 0;
#endif
}


static const struct i2c_device_id pdc_cpld_i2c_id[] = {
    { "pdc_cpld", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, pdc_cpld_i2c_id);

static struct i2c_driver pdc_cpld_i2c_driver = {
    .driver = {
        .name = "pdc_cpld",
        .owner = THIS_MODULE,
    },
    .probe = pdc_cpld_i2c_probe,
    .remove = pdc_cpld_i2c_remove,
    .id_table = pdc_cpld_i2c_id,
};

int pdc_cpld_i2c_driver_init(void) {
    int ret;

    printk(KERN_INFO "CPLD I2C Driver initialized\n");

    ret = i2c_add_driver(&pdc_cpld_i2c_driver);
    if (ret) {
        printk(KERN_ERR "Failed to register CPLD I2C driver\n");
    }

    return ret;
}

void pdc_cpld_i2c_driver_exit(void) {
    printk(KERN_INFO "CPLD I2C Driver exited\n");

    i2c_del_driver(&pdc_cpld_i2c_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC CPLD I2C Driver.");
