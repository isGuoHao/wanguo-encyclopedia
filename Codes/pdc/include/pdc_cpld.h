#ifndef _PDC_CPLD_H_
#define _PDC_CPLD_H_

#include <linux/device.h>
#include <linux/i2c.h>
#include "pdc.h"

struct pdc_cpld_device {
    struct pdc_device *pdc_dev;
    struct i2c_client *i2c_client;
    // Add any CPLD-specific data here
};

// Device allocation and free functions
int pdc_cpld_device_alloc(struct pdc_cpld_device **cpld_dev);
void pdc_cpld_device_free(struct pdc_cpld_device *cpld_dev);

// Device registration and unregistration functions
int pdc_cpld_device_register(struct pdc_cpld_device *cpld_dev);
void pdc_cpld_device_unregister(struct pdc_cpld_device *cpld_dev);

// Master initialization and exit functions
int pdc_cpld_master_init(void);
void pdc_cpld_master_exit(void);

// Driver initialization and exit functions
int pdc_cpld_i2c_driver_init(void);
void pdc_cpld_i2c_driver_exit(void);

#endif /* _PDC_CPLD_H_ */
