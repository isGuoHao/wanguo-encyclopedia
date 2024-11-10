#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/uaccess.h>  // 使用 copy_to_user, copy_from_user
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "cpld_private.h"
#include "cpld_ioctl.h"

struct cpld_driver_data {
    dev_t dev;
    struct cdev cdev;
    struct class *class;
    struct mutex mutex; // 互斥锁，防止并发访问
    struct list_head devices; // 链表头
    struct proc_dir_entry *proc_file; // procfs 文件指针
};

static struct cpld_driver_data cpld_data;

static struct cpld_device *find_cpld_device(int device_id) {
    struct cpld_device *cpld_dev;

    list_for_each_entry(cpld_dev, &cpld_data.devices, list) {
        if (cpld_dev->device_id == device_id) {
            return cpld_dev;
        }
    }
    return NULL;
}

static int cpld_open(struct inode *inode, struct file *file) {
    pr_info("Device opened.\n");
    return 0;
}

static int cpld_release(struct inode *inode, struct file *file) {
    pr_info("Device released.\n");
    return 0;
}

static long cpld_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct ioctl_args args;
    struct cpld_device *cpld_dev;
    int ret = -EINVAL;

    if (copy_from_user(&args, (struct ioctl_args __user *)arg, sizeof(args))) {
        return -EFAULT;
    }

    mutex_lock(&cpld_data.mutex);
    cpld_dev = find_cpld_device(args.dev_id);
    if (!cpld_dev) {
        mutex_unlock(&cpld_data.mutex);
        return -ENODEV;
    }

    switch (cmd) {
        case IOCTL_CPLD_READ:
            pr_info("CPLD IOCTL READ\n");
            if (cpld_dev->ops->read(cpld_dev->client, args.addr, &args.value) == 0) {
                if (copy_to_user((struct ioctl_args __user *)arg, &args, sizeof(args))) {
                    ret = -EFAULT;
                } else {
                    ret = 0;
                }
            }
            break;
        case IOCTL_CPLD_WRITE:
            pr_info("CPLD IOCTL WRITE\n");
            if (cpld_dev->ops->write(cpld_dev->client, args.addr, args.value) == 0) {
                ret = 0;
            }
            break;
    }
    mutex_unlock(&cpld_data.mutex);

    return ret;
}

static const struct file_operations cpld_fops = {
    .owner = THIS_MODULE,
    .open = cpld_open,
    .release = cpld_release,
    .unlocked_ioctl = cpld_ioctl,
};

void cpld_register_new_device(struct cpld_device *cpld_dev) {
    mutex_lock(&cpld_data.mutex);
    list_add(&cpld_dev->list, &cpld_data.devices);
    mutex_unlock(&cpld_data.mutex);
}

void cpld_unregister_device(struct cpld_device *cpld_dev) {
    mutex_lock(&cpld_data.mutex);
    list_del(&cpld_dev->list);
    mutex_unlock(&cpld_data.mutex);
}

// 用于解析用户输入的结构体
struct cpld_proc_cmd {
    int device_id;
    u8 addr;
    u8 value;
    char op; // 'r' for read, 'w' for write
};

static int cpld_proc_show(struct seq_file *m, void *v) {
    struct cpld_device *cpld_dev;

    seq_printf(m, "CPLD Devices:\n");
    mutex_lock(&cpld_data.mutex);
    list_for_each_entry(cpld_dev, &cpld_data.devices, list) {
        const char *type_str;

        switch (cpld_dev->type) {
            case CPLD_TYPE_SPI:
                type_str = "SPI";
                break;
            case CPLD_TYPE_I2C:
                type_str = "I2C";
                break;
            case CPLD_TYPE_I3C:
                type_str = "I3C";
                break;
            default:
                type_str = "Unknown";
                break;
        }

        seq_printf(m, "ID: %d, Type: %s\n", cpld_dev->device_id, type_str);
    }
    mutex_unlock(&cpld_data.mutex);

    return 0;
}

static int cpld_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, cpld_proc_show, NULL);
}

static ssize_t cpld_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    struct cpld_proc_cmd cmd;
    char kbuf[64];
    int ret;

    if (count >= sizeof(kbuf)) {
        return -EINVAL;
    }

    if (copy_from_user(kbuf, buf, count)) {
        return -EFAULT;
    }

    kbuf[count] = '\0';

    // 解析命令
    ret = sscanf(kbuf, "%d %c %hhx %hhx", &cmd.device_id, &cmd.op, &cmd.addr, &cmd.value);
    if (ret < 3 || (ret == 3 && cmd.op == 'w')) {
        pr_err("Invalid input format\n");
        return -EINVAL;
    }

    mutex_lock(&cpld_data.mutex);
    struct cpld_device *cpld_dev = find_cpld_device(cmd.device_id);
    if (!cpld_dev) {
        mutex_unlock(&cpld_data.mutex);
        pr_err("Device not found\n");
        return -ENODEV;
    }

    switch (cmd.op) {
        case 'r':
            if (cpld_dev->ops->read(cpld_dev->client, cmd.addr, &cmd.value) == 0) {
                pr_info("Read from device %d, address 0x%02x: 0x%02x\n", cmd.device_id, cmd.addr, cmd.value);
            } else {
                pr_err("Read failed\n");
            }
            break;
        case 'w':
            if (cpld_dev->ops->write(cpld_dev->client, cmd.addr, cmd.value) == 0) {
                pr_info("Write to device %d, address 0x%02x: 0x%02x\n", cmd.device_id, cmd.addr, cmd.value);
            } else {
                pr_err("Write failed\n");
            }
            break;
        default:
            pr_err("Invalid operation\n");
            ret = -EINVAL;
            break;
    }

    mutex_unlock(&cpld_data.mutex);

    return ret < 0 ? ret : count;
}

static const struct proc_ops cpld_proc_fops = {
    .proc_open = cpld_proc_open,
    .proc_read = seq_read,
    .proc_write = cpld_proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init cpld_init(void) {
    int ret;

    INIT_LIST_HEAD(&cpld_data.devices);
    mutex_init(&cpld_data.mutex);

    ret = alloc_chrdev_region(&cpld_data.dev, 0, 1, "cpld");
    if (ret < 0) {
        pr_err("Failed to allocate char device region\n");
        goto out;
    }

    cpld_data.class = class_create("cpld_class");
    if (IS_ERR(cpld_data.class)) {
        ret = PTR_ERR(cpld_data.class);
        pr_err("Failed to create class\n");
        goto unregister_chrdev;
    }

    cdev_init(&cpld_data.cdev, &cpld_fops);
    cpld_data.cdev.owner = THIS_MODULE;
    ret = cdev_add(&cpld_data.cdev, cpld_data.dev, 1);
    if (ret) {
        pr_err("Failed to add cdev\n");
        goto destroy_class;
    }

    device_create(cpld_data.class, NULL, cpld_data.dev, NULL, "cpld");

    pr_info("CPLD core initialized\n");

    ret = cpld_spi_init();
    if (ret) {
        pr_err("Failed to register SPI driver\n");
        goto cleanup_device;
    }

    ret = cpld_i2c_init();
    if (ret) {
        pr_err("Failed to register I2C driver\n");
        goto cleanup_spi;
    }

    ret = cpld_i3c_init(); // 初始化 I3C 驱动
    if (ret) {
        pr_err("Failed to register I3C driver\n");
        goto cleanup_i2c;
    }

    // 注册 procfs 文件
    cpld_data.proc_file = proc_create("cpld_devices", 0644, NULL, &cpld_proc_fops);
    if (!cpld_data.proc_file) {
        pr_err("Failed to create proc file\n");
        ret = -ENOMEM;
        goto cleanup_i3c;
    }

    pr_info("All drivers registered successfully\n");

    return 0;

cleanup_i3c:
    cpld_i3c_exit();
cleanup_i2c:
    cpld_i2c_exit();
cleanup_spi:
    cpld_spi_exit();
cleanup_device:
    device_destroy(cpld_data.class, cpld_data.dev);
    cdev_del(&cpld_data.cdev);
destroy_class:
    class_destroy(cpld_data.class);
unregister_chrdev:
    unregister_chrdev_region(cpld_data.dev, 1);
out:
    return ret;
}

static void __exit cpld_exit(void) {
    // 注销 procfs 文件
    if (cpld_data.proc_file) {
        remove_proc_entry("cpld_devices", NULL);
    }

    cpld_i3c_exit(); // 卸载 I3C 驱动
    cpld_i2c_exit();
    cpld_spi_exit();
    device_destroy(cpld_data.class, cpld_data.dev);
    cdev_del(&cpld_data.cdev);
    class_destroy(cpld_data.class);
    unregister_chrdev_region(cpld_data.dev, 1);
}

module_init(cpld_init);
module_exit(cpld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple CPLD driver for Linux");
