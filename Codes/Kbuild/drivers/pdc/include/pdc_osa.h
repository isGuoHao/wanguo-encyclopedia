#ifndef _BSP_OSA_H_
#define _BSP_OSA_H_

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
#include <linux/list.h>
#include <linux/poll.h>
#include <linux/version.h>  // 用于内核版本检测

// 自定义文件操作结构体
struct osa_proc_fops {
    int (*open)(struct inode *inode, struct file *file);
    ssize_t (*read)(struct file *file, char __user *buf, size_t count, loff_t *ppos);
    ssize_t (*write)(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
    loff_t (*llseek)(struct file *file, loff_t offset, int whence);
    int (*release)(struct inode *inode, struct file *file);
    long (*unlocked_ioctl)(struct file *file, unsigned int cmd, unsigned long arg);
    unsigned int (*poll)(struct file *file, poll_table *wait);
    int (*mmap)(struct file *file, struct vm_area_struct *vma);
};

// 定义一个结构体来封装proc文件信息
struct bsp_proc_info {
    const char *name;                  // proc 文件名称
    mode_t mode;                       // 文件权限
    struct proc_dir_entry *parent;     // 父目录
    const struct osa_proc_fops *fops;  // 自定义文件操作结构体
    struct proc_dir_entry **proc_file; // proc 文件条目指针
};

// 定义一个结构体来封装cdev信息
struct bsp_cdev_info {
    struct class *class;               // 设备类
    const char *name;                  // 设备名称
    const struct file_operations *fops;      // 文件操作结构体
    dev_t *devno;                      // 设备号
    mode_t mode;                       // 设备权限
    struct cdev *cdev;                 // 字符设备结构体
};



// 创建cdev的接口
int bsp_create_cdev(struct bsp_cdev_info *info);

// 卸载cdev的接口
void bsp_unregister_cdev(struct bsp_cdev_info *info);

// 创建proc文件的接口
int bsp_create_proc(struct bsp_proc_info *info);

// 卸载proc文件的接口
void bsp_remove_proc(struct bsp_proc_info *info);

// 定义一个宏来选择合适的class_create函数
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 3, 0)
#define osa_class_create(name) class_create(THIS_MODULE, name)
#else
#define osa_class_create(name) class_create(name)
#endif


// 适配层
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(5, 5, 0))
void struct file_operations *osa_to_file_fops(const struct file_operations *file_ops, const struct osa_proc_fops *fops);
else
void osa_to_proc_fops(const struct proc_ops *proc_ops, const struct osa_proc_fops *fops);
#endif


#endif // _BSP_OSA_H_
