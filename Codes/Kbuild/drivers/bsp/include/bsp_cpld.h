#ifndef _BSP_CPLD_H_
#define _BSP_CPLD_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "bsp.h"
#include "bsp_osa.h"

// 定义CPLD设备结构体
struct cpld_device {
    struct list_head list;
    struct device *dev;
    struct cdev cdev;
    dev_t devno;
    struct class *class;
    struct device *device;
    struct proc_dir_entry *proc_entry;
    bool initialized;
    struct cpld_ops *ops;
};

// 定义CPLD操作结构体
struct cpld_ops {
	struct device_ops *ops;
    int (*read_register)(int reg, int *value);
    int (*write_register)(int reg, int value);
};

// 注册和注销CPLD设备的函数声明
int cpld_register(struct cpld_device *cpld_dev, const char *name);
void cpld_unregister(struct cpld_device *cpld_dev);

// 模块初始化和退出函数声明
int bsp_cpld_init(void);
void bsp_cpld_exit(void);

#endif // _BSP_CPLD_H_
