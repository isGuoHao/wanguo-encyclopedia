#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "pdc.h"
#include "pdc_cpld.h"

static struct pdc_master *pdc_cpld_master;

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
    list_add_tail(&cpld_dev->pdc_dev->node, &pdc_cpld_master->devices);

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
        return ret;
    }

    // Initialize the list head for devices
    INIT_LIST_HEAD(&pdc_cpld_master->devices);

    // Register the master
    ret = pdc_master_register(pdc_cpld_master);
    if (ret) {
        pdc_master_free(pdc_cpld_master);
        return ret;
    }

    return 0;
}

void pdc_cpld_master_exit(void) {
    // Cleanup CPLD master
    printk(KERN_INFO "CPLD Master exited\n");

    // Unregister the master
    pdc_master_unregister(pdc_cpld_master);
    pdc_master_free(pdc_cpld_master);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC CPLD Master Module");
