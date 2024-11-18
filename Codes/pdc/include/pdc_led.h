#ifndef _PDC_LED_H_
#define _PDC_LED_H_

#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include "pdc.h"

struct pdc_led_device {
    struct pdc_device *pdc_dev;
    union {
        struct gpio_desc *gpio;
        struct pwm_device *pwm;
    } control;
    // Add any LED-specific data here
};

// Device allocation and free functions
int pdc_led_device_alloc(struct pdc_led_device **led_dev);
void pdc_led_device_free(struct pdc_led_device *led_dev);

// Device registration and unregistration functions
int pdc_led_device_register(struct pdc_led_device *led_dev);
void pdc_led_device_unregister(struct pdc_led_device *led_dev);

// Master initialization and exit functions
int pdc_led_master_init(void);
void pdc_led_master_exit(void);

// Driver initialization and exit functions (if needed)
int pdc_led_gpio_driver_init(void);
void pdc_led_gpio_driver_exit(void);

#endif /* _PDC_LED_H_ */
