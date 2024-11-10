#include <linux/i2c.h>
#include <linux/string.h>
#include "cpld_private.h"

static int cpld_i2c_read(union cpld_client client, u8 reg, unsigned char *value)
{
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    return 0;

    struct i2c_client *i2c = client.i2c;
    s32 ret;

    ret = i2c_smbus_read_byte_data(i2c, reg);
    if (ret < 0) {
        dev_err(&i2c->dev, "I2C read error at reg 0x%02x\n", reg);
        return -EIO;
    }

    *value = (unsigned char)ret;
    return 0;
}

static int cpld_i2c_write(union cpld_client client, u8 reg, unsigned char value)
{
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    return 0;

    struct i2c_client *i2c = client.i2c;
    s32 ret;

    ret = i2c_smbus_write_byte_data(i2c, reg, value);
    if (ret < 0) {
        dev_err(&i2c->dev, "I2C write error at reg 0x%02x\n", reg);
        return -EIO;
    }

    return 0;
}

const struct cpld_ops cpld_i2c_ops = {
    .read = cpld_i2c_read,
    .write = cpld_i2c_write,
};

static int cpld_i2c_probe(struct i2c_client *client)
{
    struct device_node *np = client->dev.of_node;
    struct cpld_device *cpld_dev;
    int ret, device_id;

    ret = of_property_read_u32(np, "cpld-device-id", &device_id);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to read device ID\n");
        return -EINVAL;
    }

    pr_info("Probing CPLD I2C Device, ID: %d\n", device_id);

    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev) {
        return -ENOMEM;
    }

    cpld_dev->dev = &client->dev;
    cpld_dev->device_id = device_id;
    cpld_dev->client.i2c = client;
    cpld_dev->ops = &cpld_i2c_ops;
    cpld_dev->type = CPLD_TYPE_I2C; // 设置为 SPI 类型

    cpld_register_new_device(cpld_dev);

    dev_set_drvdata(&client->dev, cpld_dev);

    pr_info("CPLD I2C Device Probed\n");

    return 0;
}

static void cpld_i2c_remove(struct i2c_client *client)
{
    struct cpld_device *cpld_dev = dev_get_drvdata(&client->dev);

    if (!cpld_dev) {
        dev_err(&client->dev, "Device data is NULL\n");
        return;
    }

    cpld_unregister_device(cpld_dev);

    kfree(cpld_dev);

    return;
}

static const struct of_device_id cpld_i2c_of_match[] =
{
    { .compatible = "example,cpld-i2c" },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, cpld_i2c_of_match);

static struct i2c_driver cpld_i2c_driver =
{
    .driver = {
        .name = "cpld_i2c",
        .of_match_table = cpld_i2c_of_match,
    },
    .probe = cpld_i2c_probe,
    .remove = cpld_i2c_remove,
};

int cpld_i2c_init(void)
{
    return i2c_add_driver(&cpld_i2c_driver);
}

void cpld_i2c_exit(void)
{
    i2c_del_driver(&cpld_i2c_driver);
}
