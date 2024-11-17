#ifndef _OSA_PLATFORM_H_
#define _OSA_PLATFORM_H_

// 平台驱动注册函数
int osa_platform_driver_register(void *drv);

// 平台驱动注销函数
void osa_platform_driver_unregister(void *drv);

// 根据名称获取平台资源
void *osa_platform_get_resource_byname(void *dev, unsigned int type, const char *name);

// 根据编号获取平台资源
void *osa_platform_get_resource(void *dev, unsigned int type, unsigned int num);

// 获取平台设备的 IRQ 号
int osa_platform_get_irq(void *dev, unsigned int num);

// 根据名称获取平台设备的 IRQ 号
int osa_platform_get_irq_byname(void *dev, const char *name);

// 定义一个模块驱动的宏
#define osa_module_driver(osa_driver, osa_register, osa_unregister, ...) \
    static int __init osa_driver##_init(void)                               \
    {                                                                        \
        return osa_register(&(osa_driver));                                \
    }                                                                        \
    module_init(osa_driver##_init);                                         \
    static void __exit osa_driver##_exit(void)                              \
    {                                                                        \
        osa_unregister(&(osa_driver));                                     \
    }                                                                        \
    module_exit(osa_driver##_exit);

// 定义一个平台模块驱动的宏
#define osa_module_platform_driver(platform_driver)                   \
    osa_module_driver(platform_driver, osa_platform_driver_register, \
        osa_platform_driver_unregister)

#endif /* _OSA_PLATFORM_H_ */
