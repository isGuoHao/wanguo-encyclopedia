#ifndef _CPLD_PRIVATE_H
#define _CPLD_PRIVATE_H

#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>

enum cpld_type {
    CPLD_TYPE_SPI,
    CPLD_TYPE_I2C,
    CPLD_TYPE_I3C
};

union cpld_client {
    struct spi_device *spi;
    struct i2c_client *i2c;
    struct i3c_device *i3c;
};

struct cpld_device {
    struct device *dev;
    int device_id;
    enum cpld_type type; // 设备类型，SPI 或 I2C
    union cpld_client client;
    const struct cpld_ops *ops;
    struct list_head list;
};

struct cpld_ops {
    int (*read)(union cpld_client client, u8 reg, unsigned char *value);
    int (*write)(union cpld_client client, u8 reg, unsigned char value);
};

void cpld_register_new_device(struct cpld_device *cpld_dev);
void cpld_unregister_device(struct cpld_device *cpld_dev);

int  cpld_spi_init(void);
void cpld_spi_exit(void);

int  cpld_i2c_init(void);
void cpld_i2c_exit(void);

int  cpld_i3c_init(void);
void cpld_i3c_exit(void);

#endif /* _CPLD_PRIVATE_H */
