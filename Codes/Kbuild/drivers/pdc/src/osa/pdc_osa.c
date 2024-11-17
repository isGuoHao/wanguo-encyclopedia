#include <linux/version.h>

#include "bsp_osa.h"


// 创建cdev的接口
int bsp_create_cdev(struct bsp_cdev_info *info) {
    struct cdev *cdev;
    int err;

    // 动态分配设备号
    alloc_chrdev_region(info->devno, 0, 1, info->name);

    // 创建cdev
    cdev = cdev_alloc();
    if (!cdev) {
        pr_err("Failed to allocate cdev\n");
        unregister_chrdev_region(*info->devno, 1);
        return -ENOMEM;
    }

    cdev->ops = info->fops;
    cdev->owner = THIS_MODULE;
    err = cdev_add(cdev, *info->devno, 1);
    if (err) {
        pr_err("Failed to add cdev\n");
        kfree(cdev);
        unregister_chrdev_region(*info->devno, 1);
        return err;
    }

    // 保存cdev
    info->cdev = cdev;

    // 创建设备类
    device_create(info->class, NULL, *info->devno, NULL, info->name);

    return 0;
}

// 卸载cdev的接口
void bsp_unregister_cdev(struct bsp_cdev_info *info) {
    if (info->cdev) {
        cdev_del(info->cdev);
        unregister_chrdev_region(*info->devno, 1);
        device_destroy(info->class, *info->devno);
        kfree(info->cdev);
        info->cdev = NULL;
    }
}


#if (LINUX_VERSION_CODE <= KERNEL_VERSION(5, 5, 0))

static void osa_to_file_fops(struct file_operations *file_ops, const struct osa_proc_fops *fops) {
    file_ops->owner = THIS_MODULE;
    file_ops->open = fops->open;
    file_ops->read = fops->read;
    file_ops->write = fops->write;
    file_ops->llseek = fops->llseek;
    file_ops->release = fops->release;
    file_ops->unlocked_ioctl = fops->unlocked_ioctl;
    file_ops->poll = fops->poll;
    file_ops->mmap = fops->mmap;
}

// 创建proc文件的接口
int bsp_create_proc(struct bsp_proc_info *info) {
    struct proc_dir_entry *entry;
    struct file_operations file_ops;

    osa_to_file_fops(&file_ops, info->fops);
    entry = proc_create_data(info->name, info->mode, info->parent, &file_ops, NULL);
    if (!entry) {
        return -ENOMEM;
    }

    *info->proc_file = entry;
    return 0;
}

#else

static void osa_to_proc_fops(struct proc_ops *proc_ops, const struct osa_proc_fops *fops) {
    proc_ops->proc_open = fops->open;
    proc_ops->proc_read = fops->read;
    proc_ops->proc_write = fops->write;
    proc_ops->proc_lseek = fops->llseek;
    proc_ops->proc_release = fops->release;
    proc_ops->proc_ioctl = fops->unlocked_ioctl;
    proc_ops->proc_poll = fops->poll;
    proc_ops->proc_mmap = fops->mmap;
}

// 创建proc文件的接口
int bsp_create_proc(struct bsp_proc_info *info) {
    struct proc_dir_entry *entry;
    struct proc_ops proc_ops;

    osa_to_proc_fops(&proc_ops, info->fops);
    entry = proc_create_data(info->name, info->mode, info->parent, &proc_ops, NULL);
    if (!entry) {
        return -ENOMEM;
    }

    *info->proc_file = entry;
    return 0;
}

#endif



// 卸载proc文件的接口
void bsp_remove_proc(struct bsp_proc_info *info) {
    if (*info->proc_file) {
        remove_proc_entry(info->name, info->parent);
        *info->proc_file = NULL;
    }
}
