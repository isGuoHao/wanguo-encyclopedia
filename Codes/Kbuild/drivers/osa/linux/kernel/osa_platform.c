/*
 * Copyright (c) XMEDIA. All rights reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

int osa_platform_driver_register(void *drv);
void osa_platform_driver_unregister(void *drv);
void *osa_platform_get_resource_byname(void *dev, unsigned int type,
                                        const char *name);
void *osa_platform_get_resource(void *dev, unsigned int type, unsigned int num);
int osa_platform_get_irq_byname(void *dev, const char *name);
int osa_platform_get_irq(void *dev, unsigned int num);
#if 0
int osa_platform_driver_register(struct platform_driver *drv)
{
    return __platform_driver_register(drv, THIS_MODULE);
}
void osa_platform_driver_unregister(struct platform_driver *drv)
{
    return platform_driver_unregister(drv);
}
struct resource *osa_platform_get_resource_byname(struct platform_device *dev, unsigned int type, const char *name)
{
    return platform_get_resource_byname(dev, type, name);
}

#else
int osa_platform_driver_register(void *drv)
{
    return __platform_driver_register((struct platform_driver *)drv, THIS_MODULE);
}
EXPORT_SYMBOL(osa_platform_driver_register);

void osa_platform_driver_unregister(void *drv)
{
    return platform_driver_unregister((struct platform_driver *)drv);
}
EXPORT_SYMBOL(osa_platform_driver_unregister);

void *osa_platform_get_resource_byname(void *dev, unsigned int type,
                                        const char *name)
{
    return (void *)platform_get_resource_byname((struct platform_device *)dev, type, name);
}
EXPORT_SYMBOL(osa_platform_get_resource_byname);

void *osa_platform_get_resource(void *dev, unsigned int type, unsigned int num)
{
    return (void *)platform_get_resource((struct platform_device *)dev, type, num);
}
EXPORT_SYMBOL(osa_platform_get_resource);

int osa_platform_get_irq(void *dev, unsigned int num)
{
    return platform_get_irq((struct platform_device *)dev, num);
}
EXPORT_SYMBOL(osa_platform_get_irq);

int osa_platform_get_irq_byname(void *dev, const char *name)
{
    return platform_get_irq_byname((struct platform_device *)dev, name);
}
EXPORT_SYMBOL(osa_platform_get_irq_byname);

#endif
