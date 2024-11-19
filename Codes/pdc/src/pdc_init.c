#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/list.h>

#include "pdc.h"
#include "pdc_cpld.h"
#include "pdc_lcd.h"
#include "pdc_led.h"

// 定义一个私有数据结构体
struct pdc_private_data {
    struct class *pdc_class;
    struct proc_dir_entry *proc_pdc_dir;
    struct list_head driver_list;           // 保存已经注册的驱动信息
};

// 私有数据指针
static struct pdc_private_data *pdc_private_data_instance;

#define DEBUG_SUB_DRIVER_SWITCH 0	// 是否开启子模块初始化

static struct pdc_subdriver sub_drivers[] = {
#if DEBUG_SUB_DRIVER_SWITCH
	/* CPLD master and device driver */
    { .init = pdc_cpld_master_init, .exit = pdc_cpld_master_exit },
    { .init = pdc_cpld_i2c_driver_init, .exit = pdc_cpld_i2c_driver_exit },

    /* LCD master and device driver */
    { .init = pdc_lcd_master_init, .exit = pdc_lcd_master_exit },
    { .init = pdc_lcd_i2c_driver_init, .exit = pdc_lcd_i2c_driver_exit },

    /* LED master and device driver */
    { .init = pdc_led_master_init, .exit = pdc_led_master_exit },
    { .init = pdc_led_gpio_driver_init, .exit = pdc_led_gpio_driver_exit },
#endif
    {}
};

// 注册驱动
static int pdc_register_driver(struct pdc_subdriver *driver) {
    int ret;
    if (driver->init) {
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
    struct pdc_subdriver *driver, *tmp;

    list_for_each_entry_safe_reverse(driver, tmp, &pdc_private_data_instance->driver_list, list) {
        if (driver->exit) {
            driver->exit();
        }
        list_del(&driver->list);
    }
}

// 模块初始化
static int __init pdc_init(void) {
    int i, ret;

    // 分配私有数据结构体
    pdc_private_data_instance = kzalloc(sizeof(*pdc_private_data_instance), GFP_KERNEL);
    if (!pdc_private_data_instance) {
        pr_err("Failed to allocate pdc_private_data_instance\n");
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&pdc_private_data_instance->driver_list);

    // 初始化 PDC 总线
    ret = pdc_bus_init();
    if (ret) {
        pr_err("Failed to initialize PDC bus\n");
        kfree(pdc_private_data_instance);
        return ret;
    }

    // 注册并初始化所有驱动
    for (i = 0; sub_drivers[i].init; i++) {
        ret = pdc_register_driver(&sub_drivers[i]);
        if (ret) {
            pr_err("Failed to register driver %d\n", i);
            pdc_unregister_drivers();
            pdc_bus_exit();
            kfree(pdc_private_data_instance);
            return ret;
        }
    }

    pr_info("PDC driver framework initialized\n");
    return 0;
}

// 模块退出
static void __exit pdc_exit(void) {
    pdc_unregister_drivers();
    pdc_bus_exit();
    kfree(pdc_private_data_instance);
    pr_info("PDC driver framework unregistered\n");
}

module_init(pdc_init);
module_exit(pdc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC Driver Framework.");
