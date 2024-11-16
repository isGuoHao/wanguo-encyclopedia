#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include "bsp_cpld.h"

static struct i2c_client *cpld_i2c_client;
static struct cpld_device cpld_device;

int __init bsp_cpld_i2c_init(void);
void __exit bsp_cpld_i2c_exit(void);

static int cpld_i2c_read_register(int reg, int *value) {
    u8 data[2];
    struct i2c_msg msg[2];
    int ret;

    // Write the register address
    data[0] = reg;
    msg[0].addr = cpld_i2c_client->addr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = data;

    // Read the register value
    msg[1].addr = cpld_i2c_client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 2;
    msg[1].buf = data;

    ret = i2c_transfer(cpld_i2c_client->adapter, msg, 2);
    if (ret != 2) {
        pr_err("I2C transfer failed: %d\n", ret);
        return -EIO;
    }

    *value = (data[0] << 8) | data[1];
    return 0;
}

static int cpld_i2c_write_register(int reg, int value) {
    u8 data[3];
    struct i2c_msg msg;
    int ret;

    // Prepare the data buffer
    data[0] = reg;
    data[1] = (value >> 8) & 0xFF;
    data[2] = value & 0xFF;

    // Set up the message
    msg.addr = cpld_i2c_client->addr;
    msg.flags = 0;
    msg.len = 3;
    msg.buf = data;

    ret = i2c_transfer(cpld_i2c_client->adapter, &msg, 1);
    if (ret != 1) {
        pr_err("I2C transfer failed: %d\n", ret);
        return -EIO;
    }

    return 0;
}

static int cpld_i2c_init(void) {
    // 初始化CPLD
    pr_info("CPLD I2C initialized\n");
    return 0;
}

static int cpld_i2c_deinit(void) {
    // 释放CPLD资源
    pr_info("CPLD I2C deinitialized\n");
    return 0;
}

static struct device_ops cpld_i2c_device_ops = {
    .init = cpld_i2c_init,
    .deinit = cpld_i2c_deinit,
};

static struct cpld_ops cpld_i2c_ops = {
    .ops = &cpld_i2c_device_ops,
    .read_register = cpld_i2c_read_register,
    .write_register = cpld_i2c_write_register,
};

#if 0
static int cpld_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) {
#else
static int cpld_i2c_probe(struct i2c_client *client) {
#endif
    cpld_i2c_client = client;
    pr_info("CPLD I2C client probed at address 0x%02x\n", client->addr);

    cpld_device.ops = &cpld_i2c_ops;
    cpld_device.dev = &client->dev;
    cpld_device.initialized = false;  // 初始状态未初始化

    int ret = cpld_register(&cpld_device, "cpld_i2c");
    if (ret) {
        pr_err("Failed to register CPLD I2C device\n");
        return ret;
    }

    return 0;
}

#if 0
static int cpld_i2c_remove(struct i2c_client *client) {
#else
static void cpld_i2c_remove(struct i2c_client *client) {
#endif
    pr_info("CPLD I2C client removed at address 0x%02x\n", client->addr);
    cpld_unregister(&cpld_device);
#if 0
    return 0;
#else
    return;
#endif
}

static const struct i2c_device_id cpld_i2c_id[] = {
    { "my-cpld-i2c", 0 },
    { }
};

static const struct of_device_id cpld_i2c_of_match[] = {
    { .compatible = "my-cpld-i2c" },
    { }
};

static struct i2c_driver cpld_i2c_driver = {
    .driver = {
        .name = "my-cpld-i2c",
        .of_match_table = cpld_i2c_of_match,
    },
    .probe = cpld_i2c_probe,
    .remove = cpld_i2c_remove,
    .id_table = cpld_i2c_id,
};

int __init bsp_cpld_i2c_init(void) {
    return i2c_add_driver(&cpld_i2c_driver);
}

void __exit bsp_cpld_i2c_exit(void) {
    i2c_del_driver(&cpld_i2c_driver);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CPLD I2C Driver");
MODULE_AUTHOR("Your Name");
