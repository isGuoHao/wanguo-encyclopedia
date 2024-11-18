#ifndef _PDC_LCD_H_
#define _PDC_LCD_H_

#include <linux/device.h>
#include <linux/i2c.h>
#include "pdc.h"

struct pdc_lcd_device {
    struct pdc_device *pdc_dev;
    struct i2c_client *i2c_client;
    // Add any CPLD-specific data here
};

// Device allocation and free functions
int pdc_lcd_device_alloc(struct pdc_lcd_device **lcd_dev);
void pdc_lcd_device_free(struct pdc_lcd_device *lcd_dev);

// Device registration and unregistration functions
int pdc_lcd_device_register(struct pdc_lcd_device *lcd_dev);
void pdc_lcd_device_unregister(struct pdc_lcd_device *lcd_dev);

// Master initialization and exit functions
int pdc_lcd_master_init(void);
void pdc_lcd_master_exit(void);

// Driver initialization and exit functions
int pdc_lcd_i2c_driver_init(void);
void pdc_lcd_i2c_driver_exit(void);

#endif /* _PDC_LCD_H_ */
