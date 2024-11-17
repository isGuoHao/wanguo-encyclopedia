#ifndef __OSA_DEBUG_H__
#define __OSA_DEBUG_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

#define OSA_BUG() \
    do {           \
    } while (1)

#define OSA_ASSERT(expr)                       \
    do {                                        \
        if (!(expr)) {                          \
            osa_printk("\nASSERT failed at:\n" \
                        "  >Condition: %s\n",   \
                #expr);                         \
            OSA_BUG();                         \
        }                                       \
    } while (0)

#define OSA_BUG_ON(expr)                                                               \
    do {                                                                                \
        if (expr) {                                                                     \
            osa_printk("BUG: failure at %d/%s()!\n", __LINE__, __func__); \
            OSA_BUG();                                                                 \
        }                                                                               \
    } while (0)

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

/**
 * osa_printk - 打印调试信息
 * @fmt: 格式字符串
 * @...: 可变参数列表
 * 返回: 打印的字符数
 */
extern int osa_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/**
 * osa_panic - 触发系统崩溃
 * @fmt: 格式字符串
 * @fun: 当前函数名
 * @line: 当前行号
 * @msg: 错误消息
 */
extern void osa_panic(const char *fmt, const char *fun, int line, const char *);

#endif // __OSA_DEBUG_H__
