#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/slab.h>

#define CPLD_DEV_NAME "cpld_dev"
#define CPLD_MAJOR 0 // 动态分配主设备号

struct cpld_device {
    struct spi_device *spi_dev;
    struct i2c_client *i2c_client;
    int (*read)(u8 reg, u8 *data);
    int (*write)(u8 reg, u8 data);
    struct list_head list;
};

struct cpld_driver_data {
    dev_t dev_num;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct list_head devices;
};

static struct cpld_driver_data *cpld_driver_instance;

// SPI 读写
static int cpld_spi_read(struct cpld_device *cpld_dev, u8 reg, u8 *data)
{
    struct spi_message msg;
    struct spi_transfer xfer[2];
    u8 tx_buf[2] = {reg, 0};
    u8 rx_buf[2];

    spi_message_init(&msg);
    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = &tx_buf[0];
    xfer[0].len = 1;
    spi_message_add_tail(&xfer[0], &msg);

    xfer[1].rx_buf = &rx_buf[1];
    xfer[1].len = 1;
    spi_message_add_tail(&xfer[1], &msg);

    if (spi_sync(cpld_dev->spi_dev, &msg) != 0)
        return -EIO;

    *data = rx_buf[1];
    return 0;
}

static int cpld_spi_write(struct cpld_device *cpld_dev, u8 reg, u8 data)
{
    struct spi_message msg;
    struct spi_transfer xfer;
    u8 tx_buf[2] = {reg, data};

    spi_message_init(&msg);
    memset(&xfer, 0, sizeof(xfer));

    xfer.tx_buf = tx_buf;
    xfer.len = 2;
    spi_message_add_tail(&xfer, &msg);

    if (spi_sync(cpld_dev->spi_dev, &msg) != 0)
        return -EIO;

    return 0;
}

// I2C 读写
static int cpld_i2c_read(struct cpld_device *cpld_dev, u8 reg, u8 *data)
{
    struct i2c_msg msgs[2];
    u8 buf[1] = {reg};

    msgs[0].addr = cpld_dev->i2c_client->addr;
    msgs[0].flags = 0; // Write
    msgs[0].len = 1;
    msgs[0].buf = buf;

    msgs[1].addr = cpld_dev->i2c_client->addr;
    msgs[1].flags = I2C_M_RD; // Read
    msgs[1].len = 1;
    msgs[1].buf = data;

    if (i2c_transfer(cpld_dev->i2c_client->adapter, msgs, 2) != 2)
        return -EIO;

    return 0;
}

static int cpld_i2c_write(struct cpld_device *cpld_dev, u8 reg, u8 data)
{
    struct i2c_msg msg;
    u8 buf[2] = {reg, data};

    msg.addr = cpld_dev->i2c_client->addr;
    msg.flags = 0; // Write
    msg.len = 2;
    msg.buf = buf;

    if (i2c_transfer(cpld_dev->i2c_client->adapter, &msg, 1) != 1)
        return -EIO;

    return 0;
}

// SPI 探测函数
static int cpld_spi_probe(struct spi_device *spi)
{
    struct cpld_device *cpld_dev;
    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev)
        return -ENOMEM;

    cpld_dev->spi_dev = spi;
    cpld_dev->read = cpld_spi_read;
    cpld_dev->write = cpld_spi_write;

    list_add(&cpld_dev->list, &cpld_driver_instance->devices);
    pr_info("CPLD SPI device probed\n");
    return 0;
}

// SPI 移除函数
static void cpld_spi_remove(struct spi_device *spi)
{
    struct cpld_device *cpld_dev, *tmp;

    list_for_each_entry_safe(cpld_dev, tmp, &cpld_driver_instance->devices, list) {
        if (cpld_dev->spi_dev == spi) {
            list_del(&cpld_dev->list);
            kfree(cpld_dev);
            pr_info("CPLD SPI device removed\n");
            break;
        }
    }
}

static const struct of_device_id cpld_spi_of_match[] = {
    { .compatible = "wanguo,cpld-spi" },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, cpld_spi_of_match);

// 定义 spi_device_id 表
static const struct spi_device_id cpld_spi_id[] = {
    { "cpld_spi", 0 },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(spi, cpld_spi_id);

static struct spi_driver cpld_spi_driver = {
    .driver = {
        .name = "cpld_spi",
        .of_match_table = cpld_spi_of_match,
    },
    .probe = cpld_spi_probe,
    .remove = cpld_spi_remove,
    .id_table = cpld_spi_id, // 添加 id_table 字段
};

// I2C 探测函数
static int cpld_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct cpld_device *cpld_dev;
    cpld_dev = kzalloc(sizeof(*cpld_dev), GFP_KERNEL);
    if (!cpld_dev)
        return -ENOMEM;

    cpld_dev->i2c_client = client;
    cpld_dev->read = cpld_i2c_read;
    cpld_dev->write = cpld_i2c_write;

    list_add(&cpld_dev->list, &cpld_driver_instance->devices);
    pr_info("CPLD I2C device probed\n");
    return 0;
}

// I2C 移除函数
static void cpld_i2c_remove(struct i2c_client *client)
{
    struct cpld_device *cpld_dev, *tmp;

    list_for_each_entry_safe(cpld_dev, tmp, &cpld_driver_instance->devices, list) {
        if (cpld_dev->i2c_client == client) {
            list_del(&cpld_dev->list);
            kfree(cpld_dev);
            pr_info("CPLD I2C device removed\n");
            break;
        }
    }
}

static const struct of_device_id cpld_i2c_of_match[] = {
    { .compatible = "wanguo,cpld-i2c" },
    { /* end of list */ }
};
MODULE_DEVICE_TABLE(of, cpld_i2c_of_match);

static const struct i2c_device_id cpld_i2c_id[] = {
    { "cpld_i2c", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, cpld_i2c_id);

static struct i2c_driver cpld_i2c_driver = {
    .driver = {
        .name = "cpld_i2c",
        .of_match_table = cpld_i2c_of_match,
    },
    .probe = cpld_i2c_probe,
    .remove = cpld_i2c_remove,
    .id_table = cpld_i2c_id,
};

static ssize_t cpld_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    struct cpld_device *cpld_dev = filp->private_data;
    u8 reg, data;

    if (count != 1) {
        pr_err("Invalid read request size\n");
        return -EINVAL;
    }

    if (copy_from_user(&reg, buf, 1))
        return -EFAULT;

    if (cpld_dev->read(cpld_dev, reg, &data) < 0)
        return -EIO;

    if (copy_to_user(buf, &data, 1))
        return -EFAULT;

    return 1;
}

static ssize_t cpld_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    struct cpld_device *cpld_dev = filp->private_data;
    u8 reg, data;

    if (count != 2) {
        pr_err("Invalid write request size\n");
        return -EINVAL;
    }

    if (copy_from_user(&reg, buf, 1))
        return -EFAULT;

    if (copy_from_user(&data, buf + 1, 1))
        return -EFAULT;

    if (cpld_dev->write(cpld_dev, reg, data) < 0)
        return -EIO;

    return 2;
}

static int cpld_open(struct inode *inode, struct file *filp)
{
    struct cpld_device *cpld_dev;

    list_for_each_entry(cpld_dev, &cpld_driver_instance->devices, list) {
        if (iminor(inode) == MINOR(cpld_dev->spi_dev->dev.devt) ||
            iminor(inode) == MINOR(cpld_dev->i2c_client->dev.devt)) {
            filp->private_data = cpld_dev;
            return 0;
        }
    }

    return -ENODEV;
}

static const struct file_operations cpld_fops = {
    .owner = THIS_MODULE,
    .open = cpld_open,
    .read = cpld_read,
    .write = cpld_write,
};

static int __init cpld_init(void)
{
    int ret;

    // 分配和初始化驱动数据结构
    cpld_driver_instance = kzalloc(sizeof(*cpld_driver_instance), GFP_KERNEL);
    if (!cpld_driver_instance) {
        pr_err("Failed to allocate memory for cpld_driver_instance\n");
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&cpld_driver_instance->devices);

    // 分配设备号
    ret = alloc_chrdev_region(&cpld_driver_instance->dev_num, 0, 1, CPLD_DEV_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate char device region\n");
        kfree(cpld_driver_instance);
        return ret;
    }

    // 创建字符设备
    cdev_init(&cpld_driver_instance->cdev, &cpld_fops);
    cpld_driver_instance->cdev.owner = THIS_MODULE;
    ret = cdev_add(&cpld_driver_instance->cdev, cpld_driver_instance->dev_num, 1);
    if (ret < 0) {
        pr_err("Failed to add char device\n");
        kfree(cpld_driver_instance);
        unregister_chrdev_region(cpld_driver_instance->dev_num, 1);
        return ret;
    }

    // 创建设备类和设备节点
    cpld_driver_instance->class = class_create(CPLD_DEV_NAME);
    if (IS_ERR(cpld_driver_instance->class)) {
        pr_err("Failed to create class\n");
        cdev_del(&cpld_driver_instance->cdev);
        kfree(cpld_driver_instance);
        unregister_chrdev_region(cpld_driver_instance->dev_num, 1);
        return PTR_ERR(cpld_driver_instance->class);
    }

    cpld_driver_instance->device = device_create(cpld_driver_instance->class, NULL, cpld_driver_instance->dev_num, NULL, CPLD_DEV_NAME);
    if (IS_ERR(cpld_driver_instance->device)) {
        pr_err("Failed to create device\n");
        class_destroy(cpld_driver_instance->class);
        cdev_del(&cpld_driver_instance->cdev);
        kfree(cpld_driver_instance);
        unregister_chrdev_region(cpld_driver_instance->dev_num, 1);
        return PTR_ERR(cpld_driver_instance->device);
    }

    // 注册 SPI 和 I2C 驱动
    spi_register_driver(&cpld_spi_driver);
    i2c_add_driver(&cpld_i2c_driver);

    pr_info("CPLD driver initialized\n");
    return 0;
}

static void __exit cpld_exit(void)
{
    // 卸载 SPI 和 I2C 驱动
    spi_unregister_driver(&cpld_spi_driver);
    i2c_del_driver(&cpld_i2c_driver);

    // 删除设备节点和类
    device_destroy(cpld_driver_instance->class, cpld_driver_instance->dev_num);
    class_destroy(cpld_driver_instance->class);

    // 删除字符设备
    cdev_del(&cpld_driver_instance->cdev);

    // 注销设备号
    unregister_chrdev_region(cpld_driver_instance->dev_num, 1);

    // 释放内存
    struct cpld_device *cpld_dev, *tmp;
    list_for_each_entry_safe(cpld_dev, tmp, &cpld_driver_instance->devices, list) {
        list_del(&cpld_dev->list);
        kfree(cpld_dev);
    }
    kfree(cpld_driver_instance);

    pr_info("CPLD driver exited\n");
}

module_init(cpld_init);
module_exit(cpld_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("CPLD Device Driver");
