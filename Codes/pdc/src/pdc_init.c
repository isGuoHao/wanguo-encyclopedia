#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include "pdc.h"
#include "pdc_cpld.h"
#include "pdc_lcd.h"


static int __init pdc_driver_init(void) {
    int ret;

    // Initialize PDC bus
    ret = pdc_bus_init();
    if (ret) {
        goto err_bus_init;
    }

    // Initialize CPLD master
    ret = pdc_cpld_master_init();
    if (ret) {
        goto err_cpld_master_init;
    }

    // Initialize LCD master
    ret = pdc_lcd_master_init();
    if (ret) {
        goto err_lcd_master_init;
    }

    // Initialize CPLD driver
    ret = pdc_cpld_i2c_driver_init();
    if (ret) {
        goto err_cpld_driver_init;
    }

    // Initialize LCD driver
    ret = pdc_lcd_i2c_driver_init();
    if (ret) {
        goto err_lcd_driver_init;
    }

    return 0;

err_lcd_driver_init:
    pdc_cpld_i2c_driver_exit();
err_cpld_driver_init:
    pdc_lcd_master_exit();
err_lcd_master_init:
    pdc_cpld_master_exit();
err_cpld_master_init:
    pdc_bus_exit();
err_bus_init:
    return ret;
}

static void __exit pdc_driver_exit(void) {
    pdc_lcd_i2c_driver_exit();
    pdc_cpld_i2c_driver_exit();
    pdc_lcd_master_exit();
    pdc_cpld_master_exit();
    pdc_bus_exit();
}

module_init(pdc_driver_init);
module_exit(pdc_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PDC Driver Framework");
