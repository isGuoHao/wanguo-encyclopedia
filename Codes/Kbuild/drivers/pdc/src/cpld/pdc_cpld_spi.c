#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include "bsp_cpld.h"

static struct spi_device *cpld_spi_client;
static struct cpld_device cpld_device;

int __init bsp_cpld_spi_init(void);
void __exit bsp_cpld_spi_exit(void);

static int cpld_spi_read_register(int reg, int *value) {
    // 实现SPI读取寄存器
    return 0;
}

static int cpld_spi_write_register(int reg, int value) {
    // 实现SPI写入寄存器
    return 0;
}

static int cpld_spi_init(void) {
    // 初始化CPLD
    return 0;
}

static int cpld_spi_deinit(void) {
    // 释放CPLD资源
    return 0;
}

static struct device_ops cpld_spi_device_ops = {
    .init = cpld_spi_init,
    .deinit = cpld_spi_deinit,
};

static struct cpld_ops cpld_spi_ops = {
    .ops = &cpld_spi_device_ops,
    .read_register = cpld_spi_read_register,
    .write_register = cpld_spi_write_register,
};

static int cpld_spi_probe(struct spi_device *spi) {
    cpld_spi_client = spi;
    // pr_info("CPLD SPI client probed at address 0x%02x\n", spi->chip_select);

    cpld_device.ops = &cpld_spi_ops;
    cpld_device.dev = &spi->dev;
    cpld_device.initialized = false;  // 初始状态未初始化

    int ret = cpld_register(&cpld_device, "cpld_spi");
    if (ret) {
        pr_err("Failed to register CPLD SPI device\n");
        return ret;
    }

    return 0;
}

#if 0
static int cpld_spi_remove(struct spi_device *spi) {
#else
static void cpld_spi_remove(struct spi_device *spi) {
#endif
    // pr_info("CPLD SPI client removed at address 0x%02x\n", spi->chip_select);
    cpld_unregister(&cpld_device);
#if 0
    return 0;
#else
    return;
#endif
}

static const struct spi_device_id cpld_spi_id[] = {
    { "my-cpld-spi", 0 },
    { }
};

static const struct of_device_id cpld_spi_of_match[] = {
    { .compatible = "my-cpld-spi" },
    { }
};

static struct spi_driver cpld_spi_driver = {
    .driver = {
        .name = "my-cpld-spi",
        .of_match_table = cpld_spi_of_match,
    },
    .probe = cpld_spi_probe,
    .remove = cpld_spi_remove,
    .id_table = cpld_spi_id,
};

int __init bsp_cpld_spi_init(void) {
    return spi_register_driver(&cpld_spi_driver);
}

void __exit bsp_cpld_spi_exit(void) {
    spi_unregister_driver(&cpld_spi_driver);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CPLD SPI Driver");
MODULE_AUTHOR("Your Name");
