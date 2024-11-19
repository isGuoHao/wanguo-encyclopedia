#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/idr.h>
#include <linux/of.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_submodule.h"



struct pdc_bus pdc_bus_instance;

/*                                                                              */
/*                         pdc_device_type                                      */
/*                                                                              */
static int pdc_device_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    const struct pdc_device *pdcdev = dev_to_pdcdev(dev);
    const char *name = pdcdev->name;
    int id = pdcdev->id;
    const char *master_name = pdcdev->master ? pdcdev->master->name : "unknown";

    // 生成 MODALIAS 字符串
    return add_uevent_var(env, "MODALIAS=pdc:master%s-id%04X-name%s", master_name, id, name);
}


// 设备属性定义
static ssize_t id_show(struct device *dev, struct device_attribute *da, char *buf)
{
    const struct pdc_device *pdcdev = dev_to_pdcdev(dev);
    return sprintf(buf, "%d\n", pdcdev->id);
}
static DEVICE_ATTR_RO(id);

static ssize_t name_show(struct device *dev, struct device_attribute *da, char *buf)
{
    const struct pdc_device *pdcdev = dev_to_pdcdev(dev);
    return sprintf(buf, "%s\n", pdcdev->name);
}
static DEVICE_ATTR_RO(name);

static ssize_t master_name_show(struct device *dev, struct device_attribute *da, char *buf)
{
    const struct pdc_device *pdcdev = dev_to_pdcdev(dev);
    const char *master_name = pdcdev->master ? pdcdev->master->name : "unknown";
    return sprintf(buf, "%s\n", master_name);
}
static DEVICE_ATTR_RO(master_name);

// 属性组定义
extern struct device_attribute dev_attr_id;
extern struct device_attribute dev_attr_name;
extern struct device_attribute dev_attr_master_name;


static struct attribute *pdc_device_attrs[] = {
    &dev_attr_id.attr,
    &dev_attr_name.attr,
    &dev_attr_master_name.attr,
    NULL,
};

// 将pdc_device(_addr)注册到pdc_deivce(_groups)和pdc_deivce(_groups)
ATTRIBUTE_GROUPS(pdc_device);

const struct device_type pdc_device_type = {
    .name = "pdc_device",
    .groups = pdc_device_groups,
    .uevent = pdc_device_uevent,
};


/*                                                                              */
/*                            pdc_bus_type                                      */
/*                                                                              */
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

static DEFINE_IDR(pdc_bus_idr);
static DEFINE_MUTEX(pdc_core_lock);
static int __pdc_first_dynamic_bus_num;
static BLOCKING_NOTIFIER_HEAD(pdc_bus_notifier);

const struct bus_type pdc_bus_type = {
    .name = "pdc",
    .match = pdc_device_match,
    .probe = pdc_device_probe,      // i3c_device_probe i2c_device_remove
    .remove = pdc_device_remove,      // i3c_device_remove i2c_device_remove
};



/*                                                                              */
/*                            pdc_master_type                                   */
/*                                                                              */
#if 0
#define dev_to_pdc_dev(__dev)	container_of_const(__dev, struct pdc_device, dev)

static struct pdc_master *dev_to_pdc_master(struct device *dev)
{
	return container_of(dev, struct pdc_master, dev);
}

static struct pdc_bus *dev_to_pdc_bus(struct device *dev)
{
	struct pdc_master *master;

	if (dev->type == &pdc_device_type)
		return dev_to_pdc_dev(dev)->bus;

	master = dev_to_pdc_master(dev);

	return &master->bus;
}

static void i3c_masterdev_release(struct device *dev)
{
	struct pdc_master *master = dev_to_pdc_master(dev);
	struct pdc_bus *bus = dev_to_pdc_bus(dev);

	if (master->wq)
		destroy_workqueue(master->wq);

	WARN_ON(!list_empty(&bus->devs.i2c) || !list_empty(&bus->devs.i3c));
	i3c_bus_cleanup(bus);

	of_node_put(dev->of_node);
}


const struct device_type i3c_masterdev_type = {
	.groups	= i3c_masterdev_groups,
};
EXPORT_SYMBOL_GPL(i3c_masterdev_type);


#endif

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

#define DEBUG_PDC_BUS_NOTIFIER 0        /* 通知器,具体功能还没看懂，后续添加 */

#if DEBUG_PDC_BUS_NOTIFIER
static int pdcdev_notify(struct notifier_block *nb, unsigned long action, void *data)
{
    struct device *dev = data;

    switch (action) {
    case BUS_NOTIFY_ADD_DEVICE:
        pr_info("PDC: Device added: %s\n", dev_name(dev));
        // 进行设备添加的处理
        break;
    case BUS_NOTIFY_DEL_DEVICE:
        pr_info("PDC: Device removed: %s\n", dev_name(dev));
        // 进行设备移除的处理
        break;
    default:
        break;
    }

    return NOTIFY_OK;
}

static struct notifier_block i2cdev_notifier = {
    .notifier_call = pdcdev_notify,
};

static struct notifier_block spidev_notifier = {
    .notifier_call = pdcdev_notify,
};

#endif


static int pdc_bus_init(void)
{
	int ret;

	ret = of_alias_get_highest_id("pdc");
	if (ret >= 0) {
		mutex_lock(&pdc_core_lock);
		__pdc_first_dynamic_bus_num = ret + 1;
		mutex_unlock(&pdc_core_lock);
	}

#if DEBUG_PDC_BUS_NOTIFIER
    // 注册 I2C 总线通知器
        ret = bus_register_notifier(&i2c_bus_type, &i2cdev_notifier);
        if (ret) {
            pr_err("PDC: Failed to register I2C notifier: %d\n", ret);
            return ret;
        }

        // 注册 SPI 总线通知器
        ret = bus_register_notifier(&spi_bus_type, &spidev_notifier);
        if (ret) {
            pr_err("PDC: Failed to register SPI notifier: %d\n", ret);
            bus_unregister_notifier(&i2c_bus_type, &i2cdev_notifier);
            return ret;
        }


	ret = bus_register(&pdc_bus_type);
	if (ret)
		goto out_unreg_notifier;

	return 0;

out_unreg_notifier:
	bus_unregister_notifier(&i2c_bus_type, &i2cdev_notifier);
#else
    ret = bus_register(&pdc_bus_type);
    if (ret)
        return ret;

#endif

	return 0;
}

static void pdc_bus_exit(void)
{
#if DEBUG_PDC_BUS_NOTIFIER
        // 取消注册 I2C 总线通知器
        bus_unregister_notifier(&i2c_bus_type, &i2cdev_notifier);

        // 取消注册 SPI 总线通知器
        bus_unregister_notifier(&spi_bus_type, &spidev_notifier);
#endif

	idr_destroy(&pdc_bus_idr);
	bus_unregister(&pdc_bus_type);
}


// 定义一个私有数据结构体
struct pdc_private_data {
    struct class *pdc_class;
    struct proc_dir_entry *proc_pdc_dir;
};

// 私有数据指针
static struct pdc_private_data *pdc_private_data_instance;

// 模块初始化
static int __init pdc_init(void) {
    int ret;

    // 分配私有数据结构体
    pdc_private_data_instance = kzalloc(sizeof(*pdc_private_data_instance), GFP_KERNEL);
    if (!pdc_private_data_instance) {
        pr_err("Failed to allocate pdc_private_data_instance\n");
        return -ENOMEM;
    }

    // 初始化 PDC 总线
    ret = pdc_bus_init();
    if (ret) {
        pr_err("Failed to initialize PDC bus\n");
        kfree(pdc_private_data_instance);
        return ret;
    }

    // 初始化所有子模块驱动
    ret = pdc_submodule_register_drivers();
    if (ret) {
        pr_err("Failed to initialize subdrivers\n");
        pdc_bus_exit();
        kfree(pdc_private_data_instance);
        return ret;
    }

    pr_info("PDC driver framework initialized\n");
    return 0;
}

// 模块退出
static void __exit pdc_exit(void) {
    pdc_submodule_unregister_drivers();
    pdc_bus_exit();
    kfree(pdc_private_data_instance);
    pr_info("PDC driver framework unregistered\n");
}

module_init(pdc_init);
module_exit(pdc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PDC Bus Driver");
