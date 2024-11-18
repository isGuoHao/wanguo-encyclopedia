#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/version.h>

#include "pdc.h"
#include "pdc_led.h"

static int pdc_led_gpio_probe(struct platform_device *pdev) {
    struct pdc_led_device *led_dev;
    int ret;
    int gpio_num;

    printk(KERN_INFO "LED GPIO Device probed\n");

    ret = pdc_led_device_alloc(&led_dev);
    if (ret) {
        return ret;
    }

    // Get the GPIO number from the platform data or device tree
    gpio_num = of_get_named_gpio(pdev->dev.of_node, "led-gpio", 0);
    if (!gpio_is_valid(gpio_num)) {
        dev_err(&pdev->dev, "Invalid GPIO number\n");
        pdc_led_device_free(led_dev);
        return -ENODEV;
    }

    // Request the GPIO
    ret = devm_gpio_request_one(&pdev->dev, gpio_num, GPIOF_OUT_INIT_LOW, "pdc_led");
    if (ret) {
        dev_err(&pdev->dev, "Failed to request GPIO\n");
        pdc_led_device_free(led_dev);
        return ret;
    }

    // Set the GPIO in the LED device structure
    led_dev->control.gpio = gpio_to_desc(gpio_num);

    // Register the device
    ret = pdc_led_device_register(led_dev);
    if (ret) {
        pdc_led_device_free(led_dev);
        return ret;
    }

    platform_set_drvdata(pdev, led_dev);

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
static int pdc_led_gpio_remove(struct platform_device *pdev) {
#else
static void pdc_led_gpio_remove(struct platform_device *pdev) {
#endif
    struct pdc_led_device *led_dev = platform_get_drvdata(pdev);

    printk(KERN_INFO "LED GPIO Device removed\n");

    pdc_led_device_unregister(led_dev);
    pdc_led_device_free(led_dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
    return 0;
#endif
}


#if 0
// dts 配置示意:
/ {
    led_gpio: led_gpio {
        compatible = "pdc,led-gpio";
        led-gpio = <&gpio1 17 0>; // 假设GPIO 17是LED的引脚
    };
};
#endif

static const struct of_device_id pdc_led_gpio_of_match[] = {
    { .compatible = "pdc,led-gpio", },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, pdc_led_gpio_of_match);

static struct platform_driver pdc_led_gpio_driver = {
    .driver = {
        .name = "pdc_led_gpio",
        .owner = THIS_MODULE,
        .of_match_table = pdc_led_gpio_of_match,
    },
    .probe = pdc_led_gpio_probe,
    .remove = pdc_led_gpio_remove,
};

int pdc_led_gpio_driver_init(void) {
    int ret;

    printk(KERN_INFO "LED GPIO Driver initialized\n");

    ret = platform_driver_register(&pdc_led_gpio_driver);
    if (ret) {
        printk(KERN_ERR "Failed to register LED GPIO driver\n");
    }

    return ret;
}

void pdc_led_gpio_driver_exit(void) {
    printk(KERN_INFO "LED GPIO Driver exited\n");

    platform_driver_unregister(&pdc_led_gpio_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanguo");
MODULE_DESCRIPTION("PDC LED GPIO Driver.");
