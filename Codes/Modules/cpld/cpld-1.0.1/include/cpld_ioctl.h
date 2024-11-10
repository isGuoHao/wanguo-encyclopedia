#ifndef __CPLD_IOCTL_H__
#define __CPLD_IOCTL_H__

#include <linux/ioctl.h>

struct ioctl_args {
    int dev_id;
    unsigned char addr;
    unsigned char value;
};

#define CPLD_MINORS 16 // 支持的设备数量
#define CPLD_MAGIC 'c'
#define IOCTL_CPLD_READ _IOR(CPLD_MAGIC, 1, struct ioctl_args)
#define IOCTL_CPLD_WRITE _IOW(CPLD_MAGIC, 2, struct ioctl_args)

#endif /* __CPLD_IOCTL_H__ */
