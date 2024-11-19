#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/list.h>

#include "pdc.h"
#include "pdc_submodule.h"

#include "pdc_cpld.h"
#include "pdc_lcd.h"
#include "pdc_led.h"

struct list_head pdc_submodule_driver_list;           // 保存已经注册的驱动信息

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

// 注册单个子模块驱动
static int pdc_submodule_register_driver(struct pdc_subdriver *driver) {
    int ret;

    if (driver->init) {
        ret = driver->init();
        if (ret) {
            return ret;
        }
    }

    list_add_tail(&driver->list, &pdc_submodule_driver_list);
    return 0;
}

// 注销单个子模块驱动
static int pdc_submodule_unregister_driver(struct pdc_subdriver *driver) {

    if (driver->exit) {
        driver->exit();
    }

    list_del(&driver->list);
    return 0;
}



// 初始化所有子模块驱动
int pdc_submodule_register_drivers(void)
{
    int i, ret;

    INIT_LIST_HEAD(&pdc_submodule_driver_list);

    for (i = 0; sub_drivers[i].init; i++) {
        ret = pdc_submodule_register_driver(&sub_drivers[i]);
        if (ret) {
            pr_err("Failed to register driver %d\n", i);
            pdc_submodule_unregister_drivers();
            return ret;
        }
    }

    return 0;
}


// 注销所有子模块驱动
void pdc_submodule_unregister_drivers(void)
{
    struct pdc_subdriver *driver, *tmp;

    list_for_each_entry_safe_reverse(driver, tmp, &pdc_submodule_driver_list, list) {
        pdc_submodule_unregister_driver(driver);
    }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC Submodule Driver Framework.");
