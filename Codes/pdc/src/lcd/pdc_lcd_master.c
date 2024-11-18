#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_lcd.h"

static struct pdc_master *pdc_lcd_master;
static struct class *pdc_lcd_class;

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
    list_add_tail(&lcd_dev->pdc_dev->node, &pdc_lcd_master->slaves);

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

    printk(KERN_INFO "LCD Master initialized\n");

    ret = pdc_master_alloc(&pdc_lcd_master, "pdc_lcd");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to allocate PDC master\n", __func__, __LINE__);
        return ret;
    }

    INIT_LIST_HEAD(&pdc_lcd_master->slaves);

    // Create the class for the PDC LCD devices
#if LINUX_VERSION_CODE > KERNEL_VERSION(6, 7, 0)
    pdc_lcd_class = class_create("pdc_lcd_class");
#else
    pdc_lcd_class = class_create(THIS_MODULE, "pdc_lcd_class");
#endif
    if (IS_ERR(pdc_lcd_class)) {
        ret = PTR_ERR(pdc_lcd_class);
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to create class: %d\n", __func__, __LINE__, ret);
        pdc_master_free(pdc_lcd_master);
        return ret;
    }

    // Set the device name
    ret = dev_set_name(&pdc_lcd_master->dev, "pdc_lcd");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to set device name: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_lcd_class);
        pdc_master_free(pdc_lcd_master);
        return ret;
    }

    pdc_lcd_master->dev.class = pdc_lcd_class;

    // Register the master device
    ret = pdc_master_register(pdc_lcd_master);
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to register PDC master: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_lcd_class);
        pdc_master_free(pdc_lcd_master);
        return ret;
    }

    return 0;
}

void pdc_lcd_master_exit(void) {
    printk(KERN_INFO "LCD Master exited\n");

    // Unregister the master device
    pdc_master_unregister(pdc_lcd_master);

    // Free the master device
    pdc_master_free(pdc_lcd_master);

    // Destroy the class
    class_destroy(pdc_lcd_class);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC LCD Master Module.");
