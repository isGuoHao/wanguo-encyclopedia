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

#include "bsp.h"
#include "bsp_osa.h"

#include "bsp_cpld.h"

// 定义一个私有数据结构体
struct bsp_private_data {
    struct class *bsp_class;
    struct proc_dir_entry *proc_bsp_dir;
    struct list_head driver_list;           // 保存已经注册的驱动信息
};



// 私有数据指针
static struct bsp_private_data *bsp_private_data_instance;

static struct bsp_driver drivers[] = {
    { .init = bsp_cpld_init,        .exit = bsp_cpld_exit },
    {}
    // { .init = bsp_cpld_i2c_init,    .exit = bsp_cpld_i2c_exit },
    // { .init = bsp_cpld_spi_init,    .exit = bsp_cpld_spi_exit },
    // { .init = bsp_lcd_init,      .exit = bsp_lcd_exit },
    // { .init = bsp_mcu_init,      .exit = bsp_mcu_exit },
    // { .init = bsp_key_init,      .exit = bsp_key_exit },
    // { .init = bsp_led_init,      .exit = bsp_led_exit },
};

// 注册驱动
static int bsp_register_driver(struct bsp_driver *driver) {
    int ret;
    if (driver->init){
        ret = driver->init();
        if (ret) {
            return ret;
        }
    }

    list_add_tail(&driver->list, &bsp_private_data_instance->driver_list);
    return 0;
}

// 注销所有驱动
static void bsp_unregister_drivers(void) {
    struct bsp_driver *driver;

    list_for_each_entry_reverse(driver, &bsp_private_data_instance->driver_list, list) {
        if (driver->exit)
        {
            driver->exit();
        }
        list_del(&driver->list);
    }
}


// 模块初始化
static int __init bsp_driver_init(void) {
    int i, ret;

    // 分配私有数据结构体
    bsp_private_data_instance = kzalloc(sizeof(*bsp_private_data_instance), GFP_KERNEL);
    if (!bsp_private_data_instance) {
        pr_err("Failed to allocate bsp_private_data_instance\n");
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&bsp_private_data_instance->driver_list);

    // 创建设备类
    bsp_private_data_instance->bsp_class = osa_class_create("bsp");
    if (IS_ERR(bsp_private_data_instance->bsp_class)) {
        pr_err("Failed to create class\n");
        kfree(bsp_private_data_instance);
        return PTR_ERR(bsp_private_data_instance->bsp_class);
    }

    // 创建proc目录
    bsp_private_data_instance->proc_bsp_dir = proc_mkdir("bsp", NULL);
    if (!bsp_private_data_instance->proc_bsp_dir) {
        pr_err("Failed to create proc directory\n");
        class_destroy(bsp_private_data_instance->bsp_class);
        kfree(bsp_private_data_instance);
        return -ENOMEM;
    }


    for (i = 0; i < ARRAY_SIZE(drivers); i++) {
        ret = bsp_register_driver(&drivers[i]);
        if (ret) {
            pr_err("Failed to register driver %d\n", i);
            bsp_unregister_drivers();
            proc_remove(bsp_private_data_instance->proc_bsp_dir);
            class_destroy(bsp_private_data_instance->bsp_class);
            kfree(bsp_private_data_instance);
            return ret;
        }
    }

    pr_info("BSP driver registered\n");
    return 0;
}

// 模块退出
static void __exit bsp_driver_exit(void) {
    bsp_unregister_drivers();
    proc_remove(bsp_private_data_instance->proc_bsp_dir);
    class_destroy(bsp_private_data_instance->bsp_class);
    kfree(bsp_private_data_instance);
    pr_info("BSP driver unregistered\n");
}

module_init(bsp_driver_init);
module_exit(bsp_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BSP Driver with Class and Proc Support");
MODULE_AUTHOR("Your Name");
