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
#include <linux/list.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_osa.h"

#include "pdc_cpld.h"

// 定义一个私有数据结构体
struct pdc_private_data {
    struct class *pdc_class;
    struct proc_dir_entry *proc_pdc_dir;
    struct list_head driver_list;           // 保存已经注册的驱动信息
};



// 私有数据指针
static struct pdc_private_data *pdc_private_data_instance;

static struct pdc_driver drivers[] = {
    { .init = pdc_cpld_init,        .exit = pdc_cpld_exit },
    {}
    // { .init = pdc_cpld_i2c_init,    .exit = pdc_cpld_i2c_exit },
    // { .init = pdc_cpld_spi_init,    .exit = pdc_cpld_spi_exit },
    // { .init = pdc_lcd_init,      .exit = pdc_lcd_exit },
    // { .init = pdc_mcu_init,      .exit = pdc_mcu_exit },
    // { .init = pdc_key_init,      .exit = pdc_key_exit },
    // { .init = pdc_led_init,      .exit = pdc_led_exit },
};

// 注册驱动
static int pdc_register_driver(struct pdc_driver *driver) {
    int ret;
    if (driver->init){
        ret = driver->init();
        if (ret) {
            return ret;
        }
    }

    list_add_tail(&driver->list, &pdc_private_data_instance->driver_list);
    return 0;
}

// 注销所有驱动
static void pdc_unregister_drivers(void) {
    struct pdc_driver *driver;

    list_for_each_entry_reverse(driver, &pdc_private_data_instance->driver_list, list) {
        if (driver->exit)
        {
            driver->exit();
        }
        list_del(&driver->list);
    }
}


// 模块初始化
static int __init pdc_driver_init(void) {
    int i, ret;

    // 分配私有数据结构体
    pdc_private_data_instance = kzalloc(sizeof(*pdc_private_data_instance), GFP_KERNEL);
    if (!pdc_private_data_instance) {
        pr_err("Failed to allocate pdc_private_data_instance\n");
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&pdc_private_data_instance->driver_list);

    // 创建设备类
    pdc_private_data_instance->pdc_class = osa_class_create("pdc");
    if (IS_ERR(pdc_private_data_instance->pdc_class)) {
        pr_err("Failed to create class\n");
        kfree(pdc_private_data_instance);
        return PTR_ERR(pdc_private_data_instance->pdc_class);
    }

    // 创建proc目录
    pdc_private_data_instance->proc_pdc_dir = proc_mkdir("pdc", NULL);
    if (!pdc_private_data_instance->proc_pdc_dir) {
        pr_err("Failed to create proc directory\n");
        class_destroy(pdc_private_data_instance->pdc_class);
        kfree(pdc_private_data_instance);
        return -ENOMEM;
    }


    for (i = 0; i < ARRAY_SIZE(drivers); i++) {
        ret = pdc_register_driver(&drivers[i]);
        if (ret) {
            pr_err("Failed to register driver %d\n", i);
            pdc_unregister_drivers();
            proc_remove(pdc_private_data_instance->proc_pdc_dir);
            class_destroy(pdc_private_data_instance->pdc_class);
            kfree(pdc_private_data_instance);
            return ret;
        }
    }

    pr_info("BSP driver registered\n");
    return 0;
}

// 模块退出
static void __exit pdc_driver_exit(void) {
    pdc_unregister_drivers();
    proc_remove(pdc_private_data_instance->proc_pdc_dir);
    class_destroy(pdc_private_data_instance->pdc_class);
    kfree(pdc_private_data_instance);
    pr_info("BSP driver unregistered\n");
}

module_init(pdc_driver_init);
module_exit(pdc_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BSP Driver with Class and Proc Support");
MODULE_AUTHOR("Your Name");
