#ifndef __OSA_PROC_H__
#define __OSA_PROC_H__

// 定义一个 proc 文件条目结构体
typedef struct osa_proc_dir_entry {
    char name[50];  // 文件或目录的名称
    void *proc_dir_entry;  // 内核中的 proc 文件条目指针
    int (*open)(struct osa_proc_dir_entry *entry);  // 打开文件的回调函数
    int (*read)(struct osa_proc_dir_entry *entry);  // 读取文件的回调函数
    int (*write)(struct osa_proc_dir_entry *entry, const char *buf, int count, long long *ppos);  // 写入文件的回调函数
    void *private;  // 私有数据指针，用于存储额外的信息
    void *seqfile;  // 序列化文件指针，用于 seq_file 接口
    struct osa_list_head node;  // 链表节点，用于将条目链接到列表中
} osa_proc_entry_t;

// 创建一个新的 proc 文件条目
extern osa_proc_entry_t *osa_create_proc_entry(const char *name, osa_proc_entry_t *parent);

// 在 proc 文件系统中创建一个新的目录
extern osa_proc_entry_t *osa_proc_mkdir(const char *name, osa_proc_entry_t *parent);

// 删除一个 proc 文件条目
extern void osa_remove_proc_entry(const char *name, osa_proc_entry_t *parent);

// 向序列化文件中打印格式化的字符串
extern void osa_seq_printf(osa_proc_entry_t *entry, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#endif // __OSA_PROC_H__
