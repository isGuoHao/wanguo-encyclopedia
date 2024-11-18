#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_led.h"

static struct pdc_master *pdc_led_master;
static struct class *pdc_led_class;

int pdc_led_device_register(struct pdc_led_device *led_dev) {
    int ret;

    if (!pdc_led_master) {
        return -ENODEV;
    }

    led_dev->pdc_dev->master = pdc_led_master;

    // Register the device with the PDC core
    ret = pdc_device_register(led_dev->pdc_dev);
    if (ret) {
        return ret;
    }

    // Add the device to the master's list
    list_add_tail(&led_dev->pdc_dev->node, &pdc_led_master->slaves);

    return 0;
}

void pdc_led_device_unregister(struct pdc_led_device *led_dev) {
    if (led_dev && led_dev->pdc_dev) {
        // Remove the device from the master's list
        list_del(&led_dev->pdc_dev->node);

        // Unregister the device with the PDC core
        pdc_device_unregister(led_dev->pdc_dev);
        pdc_device_free(led_dev->pdc_dev);
    }
}

int pdc_led_device_alloc(struct pdc_led_device **led_dev) {
    int ret;
    struct pdc_device *base_dev;

    *led_dev = kzalloc(sizeof(**led_dev), GFP_KERNEL);
    if (!*led_dev) {
        return -ENOMEM;
    }

    ret = pdc_device_alloc(&base_dev, pdc_led_master);
    if (ret) {
        kfree(*led_dev);
        return ret;
    }

    (*led_dev)->pdc_dev = base_dev;

    return 0;
}

void pdc_led_device_free(struct pdc_led_device *led_dev) {
    if (led_dev) {
        if (led_dev->pdc_dev) {
            pdc_device_free(led_dev->pdc_dev);
        }
        kfree(led_dev);
    }
}

int pdc_led_master_init(void) {
    int ret;

    printk(KERN_INFO "LED Master initialized\n");

    ret = pdc_master_alloc(&pdc_led_master, "pdc_led");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to allocate PDC master\n", __func__, __LINE__);
        return ret;
    }

    INIT_LIST_HEAD(&pdc_led_master->slaves);

    // Create the class for the PDC LED devices
#if LINUX_VERSION_CODE > KERNEL_VERSION(6, 7, 0)
    pdc_led_class = class_create("pdc_led_class");
#else
    pdc_led_class = class_create(THIS_MODULE, "pdc_led_class");
#endif
    if (IS_ERR(pdc_led_class)) {
        ret = PTR_ERR(pdc_led_class);
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to create class: %d\n", __func__, __LINE__, ret);
        pdc_master_free(pdc_led_master);
        return ret;
    }

    // Set the device name
    ret = dev_set_name(&pdc_led_master->dev, "pdc_led");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to set device name: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_led_class);
        pdc_master_free(pdc_led_master);
        return ret;
    }

    pdc_led_master->dev.class = pdc_led_class;

    // Register the master device
    ret = pdc_master_register(pdc_led_master);
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to register PDC master: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_led_class);
        pdc_master_free(pdc_led_master);
        return ret;
    }

    return 0;
}

void pdc_led_master_exit(void) {
    printk(KERN_INFO "LED Master exited\n");

    // Unregister the master device
    pdc_master_unregister(pdc_led_master);

    // Free the master device
    pdc_master_free(pdc_led_master);

    // Destroy the class
    class_destroy(pdc_led_class);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC LED Master Module.");
