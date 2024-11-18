#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "pdc.h"
#include "pdc_lcd.h"

static struct pdc_master *pdc_lcd_master;

int pdc_lcd_device_register(struct pdc_lcd_device *lcd_dev) {
    int ret;

    if (!pdc_lcd_master) {
        return -ENODEV;
    }

    lcd_dev->pdc_dev->master = pdc_lcd_master;

    // Register the device with the PDC core
    ret = pdc_device_register(lcd_dev->pdc_dev);
    if (ret) {
        return ret;
    }

    // Add the device to the master's list
    list_add_tail(&lcd_dev->pdc_dev->node, &pdc_lcd_master->devices);

    return 0;
}

void pdc_lcd_device_unregister(struct pdc_lcd_device *lcd_dev) {
    if (lcd_dev && lcd_dev->pdc_dev) {
        // Remove the device from the master's list
        list_del(&lcd_dev->pdc_dev->node);

        // Unregister the device with the PDC core
        pdc_device_unregister(lcd_dev->pdc_dev);
        pdc_device_free(lcd_dev->pdc_dev);
    }
}

int pdc_lcd_device_alloc(struct pdc_lcd_device **lcd_dev) {
    int ret;
    struct pdc_device *base_dev;

    *lcd_dev = kzalloc(sizeof(**lcd_dev), GFP_KERNEL);
    if (!*lcd_dev) {
        return -ENOMEM;
    }

    ret = pdc_device_alloc(&base_dev, pdc_lcd_master);
    if (ret) {
        kfree(*lcd_dev);
        return ret;
    }

    (*lcd_dev)->pdc_dev = base_dev;

    return 0;
}

void pdc_lcd_device_free(struct pdc_lcd_device *lcd_dev) {
    if (lcd_dev) {
        if (lcd_dev->pdc_dev) {
            pdc_device_free(lcd_dev->pdc_dev);
        }
        kfree(lcd_dev);
    }
}

int pdc_lcd_master_init(void) {
    int ret;

    printk(KERN_INFO "CPLD Master initialized\n");

    ret = pdc_master_alloc(&pdc_lcd_master, "pdc_lcd");
    if (ret) {
        return ret;
    }

    // Initialize the list head for devices
    INIT_LIST_HEAD(&pdc_lcd_master->devices);

    // Register the master
    ret = pdc_master_register(pdc_lcd_master);
    if (ret) {
        pdc_master_free(pdc_lcd_master);
        return ret;
    }

    return 0;
}

void pdc_lcd_master_exit(void) {
    // Cleanup CPLD master
    printk(KERN_INFO "CPLD Master exited\n");

    // Unregister the master
    pdc_master_unregister(pdc_lcd_master);
    pdc_master_free(pdc_lcd_master);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC LCD Master Module");
