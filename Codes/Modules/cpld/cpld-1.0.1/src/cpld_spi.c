#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include "cpld_private.h"

static int cpld_spi_read(union cpld_client client, u8 reg, unsigned char *value)
{
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    return 0;

    struct spi_device *spi = client.spi;
    u8 tx[2] = { reg, 0 };
    u8 rx[2];
    struct spi_message m;
    struct spi_transfer t = {
        .tx_buf = tx,
        .rx_buf = rx,
        .len = 2,
    };

    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    if (spi_sync(spi, &m) < 0) {
        dev_err(&spi->dev, "SPI read error at reg 0x%02x\n", reg);
        return -EIO;
    }

    *value = rx[1];
    return 0;
}

static int cpld_spi_write(union cpld_client client, u8 reg, unsigned char value)
{
    printk(KERN_ERR "[WANGUO] (%s:%d) \n", __func__, __LINE__);
    return 0;

    struct spi_device *spi = client.spi;
    u8 tx[2] = { reg, value };
    struct spi_message m;
    struct spi_transfer t = {
        .tx_buf = tx,
        .len = 2,
    };

    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    if (spi_sync(spi, &m) < 0) {
        dev_err(&spi->dev, "SPI write error at reg 0x%02x\n", reg);
        return -EIO;
    }

    return 0;
}

const struct cpld_ops cpld_spi_ops =
{
    .read = cpld_spi_read,
    .write = cpld_spi_write,
};

static int cpld_spi_probe(struct spi_device *spi)
{
    struct device_node *np = spi->dev.of_node;
    struct cpld_device *cpld_dev;
    int ret, device_id;

    ret = of_property_read_u32(np, "cpld-device-id", &device_id);
    if (ret < 0) {
        dev_err(&spi->dev, "Failed to read device ID\n");
        return -EINVAL;
    }

    pr_info("Probing CPLD SPI Device, ID: %d\n", device_id);

    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev) {
        return -ENOMEM;
    }

    cpld_dev->dev = &spi->dev;
    cpld_dev->device_id = device_id;
    cpld_dev->client.spi = spi;
    cpld_dev->ops = &cpld_spi_ops;
    cpld_dev->type = CPLD_TYPE_SPI; // 设置为 SPI 类型

    cpld_register_new_device(cpld_dev);

    dev_set_drvdata(&spi->dev, cpld_dev);

    pr_info("CPLD SPI Device Probed\n");

    return 0;
}

static void cpld_spi_remove(struct spi_device *spi)
{
    struct cpld_device *cpld_dev = dev_get_drvdata(&spi->dev);

    if (!cpld_dev) {
        dev_err(&spi->dev, "Device data is NULL\n");
        return;
    }

    cpld_unregister_device(cpld_dev);

    kfree(cpld_dev);

    return;
}

static const struct of_device_id cpld_spi_of_match[] =
{
    { .compatible = "example,cpld-spi" },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, cpld_spi_of_match);

static struct spi_driver cpld_spi_driver =
{
    .driver = {
        .name = "cpld_spi",
        .of_match_table = cpld_spi_of_match,
    },
    .probe = cpld_spi_probe,
    .remove = cpld_spi_remove,
};

int cpld_spi_init(void)
{
    return spi_register_driver(&cpld_spi_driver);
}

void cpld_spi_exit(void)
{
    spi_unregister_driver(&cpld_spi_driver);
}
