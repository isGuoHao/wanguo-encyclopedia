#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "bsp_cpld.h"
#include "bsp.h"
#include "bsp_osa.h"

// 定义设备管理结构体
struct cpld_private_data {
    struct list_head device_list;
    struct mutex device_lock;
    struct bsp_cdev_info *cdev_info;
    struct bsp_proc_info *proc_info;
};

static struct cpld_private_data cpld_priv_data;

// 字符设备文件操作
static int cpld_open(struct inode *inode, struct file *file) {
    return 0;
}

static int cpld_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t cpld_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // 实现读取操作
    return 0;
}

static ssize_t cpld_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    // 实现写入操作
    return 0;
}

static long cpld_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    // 实现ioctl操作
    return 0;
}

static const struct file_operations cpld_fops = {
    .owner = THIS_MODULE,
    .open = cpld_open,
    .release = cpld_release,
    .read = cpld_read,
    .write = cpld_write,
    .unlocked_ioctl = cpld_ioctl,
};

// proc文件操作
static int cpld_proc_open(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t cpld_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // 实现proc文件读取操作
    return 0;
}

static ssize_t cpld_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    // 实现proc文件写入操作
    return 0;
}

static const struct osa_proc_fops cpld_proc_fops = {
    .open = cpld_proc_open,
    .read = cpld_proc_read,
    .write = cpld_proc_write,
};

// 初始化cpld模块私有数据
static int init_manager(void)
{
    int iRet = 0;

    INIT_LIST_HEAD(&cpld_priv_data.device_list);
    mutex_init(&cpld_priv_data.device_lock);

    cpld_priv_data.cdev_info = kzalloc(sizeof(*cpld_priv_data.cdev_info), GFP_KERNEL);
    if (!cpld_priv_data.cdev_info)
        return -ENOMEM;

    cpld_priv_data.proc_info = kzalloc(sizeof(*cpld_priv_data.proc_info), GFP_KERNEL);
    if (!cpld_priv_data.proc_info) {
        kfree(cpld_priv_data.cdev_info);
        return -ENOMEM;
    }

    cpld_priv_data.cdev_info->class = NULL;
    cpld_priv_data.cdev_info->name = kstrdup("cpld", GFP_KERNEL);
    if (!cpld_priv_data.cdev_info->name) {
        kfree(cpld_priv_data.cdev_info);
        kfree(cpld_priv_data.proc_info);
        return -ENOMEM;
    }
    cpld_priv_data.cdev_info->fops = &cpld_fops;
    cpld_priv_data.cdev_info->devno = NULL;
    cpld_priv_data.cdev_info->mode = 0660;
    cpld_priv_data.cdev_info->cdev = NULL;

    cpld_priv_data.proc_info->name = kstrdup("cpld", GFP_KERNEL);
    if (!cpld_priv_data.proc_info->name) {
        kfree(cpld_priv_data.cdev_info->name);
        kfree(cpld_priv_data.cdev_info);
        kfree(cpld_priv_data.proc_info);
        return -ENOMEM;
    }
    cpld_priv_data.proc_info->mode = 0644;
    cpld_priv_data.proc_info->parent = NULL;
    cpld_priv_data.proc_info->fops = &cpld_proc_fops;
    cpld_priv_data.proc_info->proc_file = NULL;


    // 创建字符设备
    cpld_priv_data.cdev_info->devno = 0;
    cpld_priv_data.cdev_info->cdev = NULL;
    iRet = bsp_create_cdev(cpld_priv_data.cdev_info);
    if (iRet) {
        pr_err("Failed to create cdev for CPLD\n");
        return -ENOMEM;
    }

    // 创建proc文件
    cpld_priv_data.proc_info->proc_file = NULL;
    iRet = bsp_create_proc(cpld_priv_data.proc_info);
    if (iRet) {
        pr_err("Failed to create proc file for CPLD\n");
        bsp_unregister_cdev(cpld_priv_data.cdev_info);
        return -ENOMEM;
    }

    return 0;
}

// 清理管理器
static void cleanup_manager(void) {

    bsp_remove_proc(cpld_priv_data.proc_info);
    bsp_unregister_cdev(cpld_priv_data.cdev_info);

    kfree(cpld_priv_data.cdev_info->name);
    kfree(cpld_priv_data.proc_info->name);
    kfree(cpld_priv_data.cdev_info);
    kfree(cpld_priv_data.proc_info);
}

// 注册CPLD设备
int cpld_register(struct cpld_device *cpld_dev, const char *name) {
    int ret;

    mutex_lock(&cpld_priv_data.device_lock);
    if (cpld_dev->initialized) {
        pr_err("CPLD device already initialized\n");
        ret = -EBUSY;
        goto out;
    }

    cpld_dev->initialized = true;
    list_add_tail(&cpld_dev->list, &cpld_priv_data.device_list);

out:
    mutex_unlock(&cpld_priv_data.device_lock);
    return ret;
}

// 注销CPLD设备
void cpld_unregister(struct cpld_device *cpld_dev) {
    mutex_lock(&cpld_priv_data.device_lock);
    if (cpld_dev->initialized) {
        cpld_dev->initialized = false;
        list_del(&cpld_dev->list);
    }
    mutex_unlock(&cpld_priv_data.device_lock);
}

// 模块初始化
int bsp_cpld_init(void) {
    int ret;

    ret = init_manager();
    if (ret) {
        pr_err("Failed to initialize manager\n");
        return ret;
    }

    pr_info("CPLD driver registered\n");

    return 0;
}

// 模块退出
void bsp_cpld_exit(void) {
    pr_info("CPLD driver unregistered\n");
    cleanup_manager();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CPLD Driver with Class and Proc Support");
MODULE_AUTHOR("Your Name");
