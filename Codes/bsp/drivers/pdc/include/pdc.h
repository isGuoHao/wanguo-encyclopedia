#ifndef _BSP_H_
#define _BSP_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

struct device_ops {
    int (*init)(void);       // 初始化函数指针
    int (*deinit)(void);    // 反初始化函数指针
};

struct pdc_driver {
    int (*init)(void);
    void (*exit)(void);
    struct list_head list;           // 保存已经注册的驱动信息
};

#endif /* _BSP_H_ */

