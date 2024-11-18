#ifndef _PDC_H_
#define _PDC_H_

#include <linux/device.h>
#include <linux/idr.h>

struct pdc_device {
    struct device dev;
    struct pdc_master *master;
    struct list_head node;
    int id;
};

struct pdc_master {
    const char *name;
    struct device dev;
    struct list_head slaves;
    struct list_head node;
    struct idr device_idr;
};

struct pdc_driver {
    int (*probe)(struct pdc_device *dev);
    void (*remove)(struct pdc_device *dev);
    struct device_driver driver;
};

// 定义 PDC 子驱动结构体
struct pdc_subdriver {
    int (*init)(void);          // 初始化函数指针
    void (*exit)(void);         // 退出函数指针
    struct list_head list;      // 用于链表管理
};

struct pdc_bus {
    struct bus_type bus_type;
    struct device *dev;
    struct list_head masters;
};

#define to_pdc_device(d) container_of(d, struct pdc_device, dev)
#define to_pdc_driver(d) container_of(d, struct pdc_driver, driver)

// driver
int pdc_driver_register(struct pdc_driver *driver);
void pdc_driver_unregister(struct pdc_driver *driver);

// device
int pdc_device_alloc(struct pdc_device **device, struct pdc_master *master);
void pdc_device_free(struct pdc_device *device);
int pdc_device_register(struct pdc_device *device);
void pdc_device_unregister(struct pdc_device *device);

// master
int pdc_master_alloc(struct pdc_master **master, const char *name);
void pdc_master_free(struct pdc_master *master);
int pdc_master_register(struct pdc_master *master);
void pdc_master_unregister(struct pdc_master *master);

// bus
int pdc_bus_init(void);
void pdc_bus_exit(void);

#endif /* _PDC_H_ */
