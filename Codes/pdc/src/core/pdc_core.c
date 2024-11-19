#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>

#include <linux/slab.h>
#include <linux/list.h>
#include <linux/idr.h>
#include <linux/version.h>

#include "pdc.h"


struct pdc_bus pdc_bus_instance;


static int pdc_device_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    const struct pdc_device *pdcdev = dev_to_pdcdev(dev);
    const char *name = pdcdev->name;
    int id = pdcdev->id;
    const char *master_name = pdcdev->master ? pdcdev->master->name : "unknown";

    // 生成 MODALIAS 字符串
    return add_uevent_var(env, "MODALIAS=pdc:master%s-id%04X-name%s", master_name, id, name);
}



const struct device_type pdc_device_type = {
    .name = "pdc_device",
    //.groups = pdc_device_groups,
    .uevent = pdc_device_uevent,
};


const struct pdc_device_id *pdc_match_id(const struct pdc_device_id *id, struct pdc_device *pdcdev)
{
	if (!(id && pdcdev))
		return NULL;

	while (id->name[0]) {
		if (strcmp(pdcdev->name, id->name) == 0)
			return id;
		id++;
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(pdc_match_id);


#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
static int pdc_device_match(struct device *dev, const struct device_driver *drv) {
#else
static int pdc_device_match(struct device *dev, struct device_driver *drv) {
#endif
	struct pdc_device *pdcdev;
	struct pdc_driver *pdcdrv;

	if (dev->type != &pdc_device_type)
		return 0;

	pdcdev = dev_to_pdcdev(dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
    pdcdrv = drv_to_pdcdrv(drv);
#else
    pdcdrv = drv_to_pdcdrv(drv);
#endif

	if (pdc_match_id(pdcdrv->id_table, pdcdev))
		return 1;

	return 0;
}


static int pdc_device_probe(struct device *dev)
{
	struct pdc_device *pdcdev = dev_to_pdcdev(dev);
	struct pdc_driver *driver = drv_to_pdcdrv(dev->driver);

	return driver->probe(pdcdev);
}

static void pdc_device_remove(struct device *dev)
{
	struct pdc_device *pdcdev = dev_to_pdcdev(dev);
	struct pdc_driver *driver = drv_to_pdcdrv(dev->driver);

	if (driver->remove)
		driver->remove(pdcdev);
}

const struct bus_type pdc_bus_type = {
    .name = "pdc",
    .match = pdc_device_match,
    .probe = pdc_device_probe,      // i3c_device_probe i2c_device_remove
    .remove = pdc_device_remove,      // i3c_device_remove i2c_device_remove
};


static int pdc_driver_probe(struct device *dev) {
    struct pdc_device *pdc_dev = dev_to_pdcdev(dev);
    struct pdc_driver *driver = drv_to_pdcdrv(dev->driver);

    if (driver->probe)
        return driver->probe(pdc_dev);

    return 0;
}

static int pdc_driver_remove(struct device *dev) {
    struct pdc_device *pdc_dev = dev_to_pdcdev(dev);
    struct pdc_driver *driver = drv_to_pdcdrv(dev->driver);

    if (driver->remove)
        driver->remove(pdc_dev);

    return 0;
}

int pdc_driver_register(struct pdc_driver *driver) {
    driver->driver.probe = pdc_driver_probe;
    driver->driver.remove = pdc_driver_remove;
    return driver_register(&driver->driver);
}

void pdc_driver_unregister(struct pdc_driver *driver) {
    driver_unregister(&driver->driver);
}

int pdc_device_alloc(struct pdc_device **device, struct pdc_master *master) {
    struct pdc_device *dev;
    int id;

    dev = kmalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->master = master;
    id = idr_alloc(&master->device_idr, dev, 0, 0, GFP_KERNEL);
    if (id < 0) {
        kfree(dev);
        return id;
    }

    dev->id = id;
    INIT_LIST_HEAD(&dev->node);

    *device = dev;
    return 0;
}

void pdc_device_free(struct pdc_device *device) {
    if (device) {
        idr_remove(&device->master->device_idr, device->id);
        kfree(device);
    }
}

static void pdc_device_release(struct device *dev) {
    struct pdc_device *device = container_of(dev, struct pdc_device, dev);
    pdc_device_free(device);
}

int pdc_device_register(struct pdc_device *device) {
    int ret;

    device_initialize(&device->dev);
    device->dev.bus = &pdc_bus_type;
    device->dev.parent = &device->master->dev;
    device->dev.type = &pdc_device_type;
    device->dev.release = pdc_device_release;  // 设置 release 回调

    // 检查设备名称是否已设置
    if (dev_name(&device->dev)) {
        ret = 0;
    } else if (device->dev.bus && device->dev.bus->dev_name) {
        ret = dev_set_name(&device->dev, "%s%u", device->dev.bus->dev_name, device->id);
    } else {
        printk(KERN_ERR "[WANGUO] (%s:%d) Device name not set and no bus name available\n", __func__, __LINE__);
        ret = -EINVAL;
    }

    if (ret == 0) {
        ret = device_add(&device->dev);
        if (ret) {
            printk(KERN_ERR "Failed to add PDC device\n");
            return ret;
        }

        list_add_tail(&device->node, &device->master->slaves);
    }

    return ret;
}

void pdc_device_unregister(struct pdc_device *device) {
    if (device) {
        list_del(&device->node);
        device_unregister(&device->dev);
    }
}

int pdc_master_alloc(struct pdc_master **master, const char *name) {
    *master = kzalloc(sizeof(**master), GFP_KERNEL);
    if (!*master)
        return -ENOMEM;

    (*master)->name = kstrdup(name, GFP_KERNEL);
    if (!(*master)->name) {
        kfree(*master);
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&(*master)->slaves);
    INIT_LIST_HEAD(&(*master)->node);
    idr_init(&(*master)->device_idr);

    return 0;
}

void pdc_master_free(struct pdc_master *master) {
    if (master) {
        idr_destroy(&master->device_idr);
        kfree(master->name);
        kfree(master);
    }
}

static void pdc_master_release(struct device *dev) {
    struct pdc_master *master = container_of(dev, struct pdc_master, dev);
    pdc_master_free(master);
}

int pdc_master_register(struct pdc_master *master) {
    int ret;

    master->dev.release = pdc_master_release;  // 设置 release 回调

    // 检查设备名称是否已设置
    if (dev_name(&master->dev)) {
        ret = 0;
    } else if (master->dev.bus && master->dev.bus->dev_name) {
        ret = dev_set_name(&master->dev, "%s%u", master->dev.bus->dev_name, master->dev.id);
    } else {
        dump_stack();
        printk(KERN_ERR "[WANGUO] (%s:%d) Master device name not set and no bus name available\n", __func__, __LINE__);
        ret = -EINVAL;
    }

    if (ret == 0) {
        ret = device_register(&master->dev);
        if (ret) {
            printk(KERN_ERR "Failed to register PDC master device, ret: %d\n", ret);
            return ret;
        }

        list_add_tail(&master->node, &pdc_bus_instance.masters);
    }

    return ret;
}

void pdc_master_unregister(struct pdc_master *master) {
    if (master) {
        list_del(&master->node);
        device_unregister(&master->dev);
    }
}

int pdc_bus_init(void) {
    int ret;

    // Initialize the PDC bus instance
    INIT_LIST_HEAD(&pdc_bus_instance.masters);

    printk(KERN_INFO "PDC Bus initialized\n");

    ret = bus_register(&pdc_bus_type);
    if (ret) {
        printk(KERN_ERR "Failed to register PDC bus type\n");
        return ret;
    }

    return 0;
}

void pdc_bus_exit(void) {
    // Unregister the PDC bus type
    bus_unregister(&pdc_bus_type);

    // Clean up the PDC bus instance
    while (!list_empty(&pdc_bus_instance.masters)) {
        struct pdc_master *master = list_first_entry(&pdc_bus_instance.masters, struct pdc_master, node);
        list_del(&master->node);
        pdc_master_free(master);
    }

    printk(KERN_INFO "PDC Bus exited\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PDC Bus Driver");
