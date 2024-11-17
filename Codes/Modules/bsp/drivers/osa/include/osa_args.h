#ifndef _OSA_ARGS_H_
#define _OSA_ARGS_H_

#include <linux/stdarg.h>

// 定义可变参数宏
#ifndef _OSA_VA_LIST
#define _OSA_VA_LIST
#define osa_va_list       va_list
#define osa_va_arg(ap, T) va_arg(ap, T)
#define osa_va_end(ap)    va_end(ap)
#define osa_va_start(ap, A) va_start(ap, A)
#endif /* _OSA_VA_LIST */

// 定义一个空字符串常量
#define NULL_STRING        "NULL"

// 可变参数打印函数，类似于 printk
extern void osa_vprintk(const char *fmt, osa_va_list args);

// 可变参数格式化字符串函数，类似于 snprintf
extern int osa_vsnprintf(char *str, int size, const char *fmt, osa_va_list args);

// 如果配置了快照启动
#ifdef CONFIG_SNAPSHOT_BOOT

// 用户模式助手等待类型
#ifndef OSA_UMH_WAIT_PROC
#define OSA_UMH_WAIT_PROC 2 /* 等待进程完成 */
#endif

// 调用用户模式助手并强制执行
extern int osa_call_usermodehelper_force(char *path, char **argv, char **envp, int wait);

#endif /* CONFIG_SNAPSHOT_BOOT */

#endif /* _OSA_ARGS_H_ */
