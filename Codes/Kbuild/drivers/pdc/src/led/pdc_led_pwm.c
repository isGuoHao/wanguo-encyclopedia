#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pwm.h>
#include <linux/of.h>

#include "pdc.h"
#include "pdc_led.h"

static struct pwm_device *led_pwm;
static struct led_device led_device;

int __init pdc_led_pwm_init_module(void);
void __exit pdc_led_pwm_exit_module(void);



static int pdc_led_pwm_init(void) {
    // 初始化PWM
    pr_info("LED PWM initialized\n");
    return 0;
}

static int pdc_led_pwm_deinit(void) {
    // 释放PWM资源
    pr_info("LED PWM deinitialized\n");
    return 0;
}

static int pdc_led_pwm_set_brightness(struct led_device *led_dev, int brightness) {
    // 设置PWM亮度
    if (brightness < 0 || brightness > 100) {
        pr_err("Invalid brightness value: %d\n", brightness);
        return -EINVAL;
    }

    //pwm_config(led_pwm, brightness * (led_pwm->period / 100), led_pwm->period);
    pwm_enable(led_pwm);
    return 0;
}

static struct device_ops led_pwm_device_ops = {
    .init = pdc_led_pwm_init,
    .deinit = pdc_led_pwm_deinit,
};

static struct led_ops led_pwm_ops = {
    .ops = &led_pwm_device_ops,
    .set_brightness = pdc_led_pwm_set_brightness,
};

static int pdc_led_pwm_probe(struct platform_device *pdev) {
    int ret;

    // 获取PWM设备
    led_pwm = devm_pwm_get(&pdev->dev, NULL);
    if (IS_ERR(led_pwm)) {
        ret = PTR_ERR(led_pwm);
        pr_err("Failed to get PWM device: %d\n", ret);
        return ret;
    }

    pr_info("LED PWM probed\n");

    led_device.ops = &led_pwm_ops;
    led_device.dev = &pdev->dev;
    led_device.initialized = false;  // 初始状态未初始化

    ret = led_register(&led_device, "led_pwm");
    if (ret) {
        pr_err("Failed to register LED PWM device\n");
        return ret;
    }

    return 0;
}

static int pdc_led_pwm_remove(struct platform_device *pdev) {
    pr_info("LED PWM removed\n");
    led_unregister(&led_device);
    pwm_disable(led_pwm);
    return 0;
}

static const struct of_device_id pdc_led_pwm_of_match[] = {
    { .compatible = "my-led-pwm" },
    { }
};

static struct platform_driver pdc_led_pwm_driver = {
    .driver = {
        .name = "my-led-pwm",
        .of_match_table = pdc_led_pwm_of_match,
    },
    .probe = pdc_led_pwm_probe,
    .remove = pdc_led_pwm_remove,
};

int __init pdc_led_pwm_init_module(void) {
    return platform_driver_register(&pdc_led_pwm_driver);
}

void __exit pdc_led_pwm_exit_module(void) {
    platform_driver_unregister(&pdc_led_pwm_driver);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED PWM Driver");
MODULE_AUTHOR("Your Name");
