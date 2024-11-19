#ifndef _PDC_H_
#define _PDC_H_

#include <linux/device.h>
#include <linux/idr.h>
#include <linux/mod_devicetable.h>


/*                                                                              */
/*                                公共数据类型声明                                      */
/*                                                                              */

extern const  struct device_type pdc_device_type;
extern const  struct bus_type pdc_bus_type;

/*                                                                              */
/*                                公共数据类型定义                                      */
/*                                                                              */

struct pdc_device {
    int id;
    const char *name;
    struct pdc_master *master;
    struct device dev;
    struct list_head node;
};

struct pdc_master {
    const char *name;
    struct device dev;
    struct list_head slaves;
    struct list_head node;
    struct idr device_idr;
};

#define PDC_NAME_SIZE	20

struct pdc_device_id {
	char name[PDC_NAME_SIZE];
	kernel_ulong_t driver_data;
};

struct pdc_driver {
    struct device_driver driver;
    const struct pdc_device_id *id_table;
    int (*probe)(struct pdc_device *dev);
    void (*remove)(struct pdc_device *dev);
};

struct pdc_bus {
    struct bus_type bus_type;
    struct device *dev;
    struct list_head masters;
};


/*                                                                              */
/*                                    函数声明                                      */
/*                                                                              */

static inline struct pdc_driver *drv_to_pdcdrv(struct device_driver *drv)
{
	return container_of(drv, struct pdc_driver, driver);
}

struct device *pdcdev_to_dev(struct pdc_device *pdcdev);

/**
 * dev_to_pdcdev() - Returns the I3C device containing @dev
 * @__dev: device object
 *
 * Return: a pointer to an PDC device object.
 */
#define dev_to_pdcdev(__dev)	container_of_const(__dev, struct pdc_device, dev)




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


/*                                                                              */
/*                              私有公共数据类型定义                                      */
/*                                                                              */

// 定义 PDC 子驱动结构体
struct pdc_subdriver {
    int (*init)(void);          // 初始化函数指针
    void (*exit)(void);         // 退出函数指针
    struct list_head list;      // 用于链表管理
};



#endif /* _PDC_H_ */
