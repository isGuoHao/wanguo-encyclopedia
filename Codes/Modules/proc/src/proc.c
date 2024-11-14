#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/types.h>

#define PROC_FILE_NAME "proc_test"

static struct proc_dir_entry *proc_file_entry;

struct proc_test_cmd {
    int device_id;
    u8 addr_h;
    u8 addr_l;
    u8 writeval;
    u8 readlen;
    char op; // 'r' for read, 'w' for write
};

// 命令解析函数
static ssize_t proc_test_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[128];
    struct proc_test_cmd cmd;

    if (copy_from_user(kbuf, buf, count))
    {
        return -EFAULT;
    }

    kbuf[count] = '\0';
    int ret = sscanf(kbuf, "%d -%c %hhx %hhx %hhd %hhx", &cmd.device_id, &cmd.op, &cmd.addr_h, &cmd.addr_l,
                                                            &cmd.readlen, &cmd.writeval);
    if (ret != 6) {
        printk(KERN_ERR "Invalid command format\n");
        return -EINVAL;
    }

    printk(KERN_INFO "Received cmd: device_id=%d, op=%c, addr_h=0x%02x, addr_l=0x%02x, readlen=%d, writeval=0x%02x\n",
                                            cmd.device_id, cmd.op, cmd.addr_h, cmd.addr_l, cmd.readlen, cmd.writeval);

    return count;
}

// 读取函数
static ssize_t proc_test_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    const char *data = "This is a test message from /proc/proc_test\n";
    size_t len = strlen(data);
    if (*ppos >= len)
        return 0;
    if (count > len - *ppos)
        count = len - *ppos;
    if (copy_to_user(buf, data + *ppos, count))
        return -EFAULT;
    *ppos += count;
    return count;
}

// 文件操作结构体
static const struct proc_ops fops = {
    .proc_read = proc_test_read,
    .proc_write = proc_test_write,
};

static int __init proc_test_init(void) {
    proc_file_entry = proc_create_data(PROC_FILE_NAME, 0666, NULL, &fops, NULL);
    if (proc_file_entry == NULL) {
        remove_proc_entry(PROC_FILE_NAME, NULL);
        return -ENOMEM;
    }
    printk(KERN_INFO "Module loaded.\n");
    return 0;
}

static void __exit proc_test_exit(void) {
    remove_proc_entry(PROC_FILE_NAME, NULL);
    printk(KERN_INFO "Module unloaded.\n");
}

module_init(proc_test_init);
module_exit(proc_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple module for /proc file system with read/write support.");
