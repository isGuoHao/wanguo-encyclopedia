#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include <linux/list.h>

struct led_device {
    struct list_head list;
    bool initialized;
	struct device *dev;
    struct led_ops *ops;
    // 其他成员变量
};

struct led_ops {
	struct device_ops *ops;
    int (*set_brightness)(struct led_device *led_dev, int brightness);
    int (*set_state)(struct led_device *led_dev, bool state);  // 添加 set_state 成员
    // 其他操作
};


int led_register(struct led_device *led_dev, const char *name);
void led_unregister(struct led_device *led_dev);
int bsp_led_init(void);
void bsp_led_exit(void);

#endif // _BSP_LED_H_
