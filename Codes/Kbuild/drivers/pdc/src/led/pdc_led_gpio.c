#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include "pdc.h"
#include "pdc_led.h"

static struct gpio_desc *led_gpio;

static struct led_device led_device;

int __init pdc_led_gpio_init_module(void);
void __exit pdc_led_gpio_exit_module(void);



static int pdc_led_gpio_init(void) {
    // 初始化LED GPIO
    pr_info("LED GPIO initialized\n");
    return 0;
}

static int pdc_led_gpio_deinit(void) {
    // 释放LED GPIO资源
    pr_info("LED GPIO deinitialized\n");
    return 0;
}

static int pdc_led_gpio_set_state(struct led_device *led_dev, bool state) {
    // 设置LED状态
    gpiod_set_value(led_gpio, state);
    return 0;
}

static struct device_ops led_gpio_device_ops = {
    .init = pdc_led_gpio_init,
    .deinit = pdc_led_gpio_deinit,
};

static struct led_ops led_gpio_ops = {
    .ops = &led_gpio_device_ops,
    .set_state = pdc_led_gpio_set_state,
};

static int pdc_led_gpio_probe(struct platform_device *pdev) {
    int ret;

    // 获取LED GPIO
    led_gpio = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        ret = PTR_ERR(led_gpio);
        pr_err("Failed to get LED GPIO: %d\n", ret);
        return ret;
    }

    pr_info("LED GPIO probed\n");

    led_device.ops = &led_gpio_ops;
    led_device.dev = &pdev->dev;
    led_device.initialized = false;  // 初始状态未初始化

    ret = led_register(&led_device, "led_gpio");
    if (ret) {
        pr_err("Failed to register LED GPIO device\n");
        return ret;
    }

    return 0;
}

static int pdc_led_gpio_remove(struct platform_device *pdev) {
    pr_info("LED GPIO removed\n");
    led_unregister(&led_device);
    return 0;
}

static const struct of_device_id pdc_led_gpio_of_match[] = {
    { .compatible = "my-led-gpio" },
    { }
};

static struct platform_driver pdc_led_gpio_driver = {
    .driver = {
        .name = "my-led-gpio",
        .of_match_table = pdc_led_gpio_of_match,
    },
    .probe = pdc_led_gpio_probe,
    .remove = pdc_led_gpio_remove,
};

int __init pdc_led_gpio_init_module(void) {
    return platform_driver_register(&pdc_led_gpio_driver);
}

void __exit pdc_led_gpio_exit_module(void) {
    platform_driver_unregister(&pdc_led_gpio_driver);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED GPIO Driver");
MODULE_AUTHOR("Your Name");
