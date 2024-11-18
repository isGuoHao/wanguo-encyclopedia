#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_cpld.h"

static struct pdc_master *pdc_cpld_master;
static struct class *pdc_cpld_class;

int pdc_cpld_device_register(struct pdc_cpld_device *cpld_dev) {
    int ret;

    if (!pdc_cpld_master) {
        return -ENODEV;
    }

    cpld_dev->pdc_dev->master = pdc_cpld_master;

    // Register the device with the PDC core
    ret = pdc_device_register(cpld_dev->pdc_dev);
    if (ret) {
        return ret;
    }

    // Add the device to the master's list
    list_add_tail(&cpld_dev->pdc_dev->node, &pdc_cpld_master->slaves);

    return 0;
}

void pdc_cpld_device_unregister(struct pdc_cpld_device *cpld_dev) {
    if (cpld_dev && cpld_dev->pdc_dev) {
        // Remove the device from the master's list
        list_del(&cpld_dev->pdc_dev->node);

        // Unregister the device with the PDC core
        pdc_device_unregister(cpld_dev->pdc_dev);
        pdc_device_free(cpld_dev->pdc_dev);
    }
}

int pdc_cpld_device_alloc(struct pdc_cpld_device **cpld_dev) {
    int ret;
    struct pdc_device *base_dev;

    *cpld_dev = kzalloc(sizeof(**cpld_dev), GFP_KERNEL);
    if (!*cpld_dev) {
        return -ENOMEM;
    }

    ret = pdc_device_alloc(&base_dev, pdc_cpld_master);
    if (ret) {
        kfree(*cpld_dev);
        return ret;
    }

    (*cpld_dev)->pdc_dev = base_dev;

    return 0;
}

void pdc_cpld_device_free(struct pdc_cpld_device *cpld_dev) {
    if (cpld_dev) {
        if (cpld_dev->pdc_dev) {
            pdc_device_free(cpld_dev->pdc_dev);
        }
        kfree(cpld_dev);
    }
}

int pdc_cpld_master_init(void) {
    int ret;

    printk(KERN_INFO "CPLD Master initialized\n");

    ret = pdc_master_alloc(&pdc_cpld_master, "pdc_cpld");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to allocate PDC master\n", __func__, __LINE__);
        return ret;
    }

    INIT_LIST_HEAD(&pdc_cpld_master->slaves);

    // Create the class for the PDC CPLD devices
#if LINUX_VERSION_CODE > KERNEL_VERSION(6, 7, 0)
    pdc_cpld_class = class_create("pdc_cpld_class");
#else
    pdc_cpld_class = class_create(THIS_MODULE, "pdc_cpld_class");
#endif
    if (IS_ERR(pdc_cpld_class)) {
        ret = PTR_ERR(pdc_cpld_class);
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to create class: %d\n", __func__, __LINE__, ret);
        pdc_master_free(pdc_cpld_master);
        return ret;
    }

    // Set the device name
    ret = dev_set_name(&pdc_cpld_master->dev, "pdc_cpld");
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to set device name: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_cpld_class);
        pdc_master_free(pdc_cpld_master);
        return ret;
    }

    pdc_cpld_master->dev.class = pdc_cpld_class;

    // Register the master device
    ret = pdc_master_register(pdc_cpld_master);
    if (ret) {
        printk(KERN_ERR "[WANGUO] (%s:%d) Failed to register PDC master: %d\n", __func__, __LINE__, ret);
        class_destroy(pdc_cpld_class);
        pdc_master_free(pdc_cpld_master);
        return ret;
    }

    return 0;
}

void pdc_cpld_master_exit(void) {
    printk(KERN_INFO "CPLD Master exited\n");

    // Unregister the master device
    pdc_master_unregister(pdc_cpld_master);

    // Free the master device
    pdc_master_free(pdc_cpld_master);

    // Destroy the class
    class_destroy(pdc_cpld_class);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC CPLD Master Module.");
