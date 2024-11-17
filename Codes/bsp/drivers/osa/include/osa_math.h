#ifndef __OSA_MATH_H__
#define __OSA_MATH_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

/**
 * osa_max - 返回两个值中的较大者
 * @x: 第一个值
 * @y: 第二个值
 * 返回: 较大的值
 * 说明: 使用GCC的扩展语法来确定两个值中的较大者，同时确保类型安全。
 */
#define osa_max(x, y) ({                            \
        __typeof__(x) _max1 = (x);                  \
        __typeof__(y) _max2 = (y);                  \
        (void) (&_max1 == &_max2);                  \
        _max1 > _max2 ? _max1 : _max2; })

/**
 * osa_min - 返回两个值中的较小者
 * @x: 第一个值
 * @y: 第二个值
 * 返回: 较小的值
 * 说明: 使用GCC的扩展语法来确定两个值中的较小者，同时确保类型安全。
 */
#define osa_min(x, y) ({                            \
    __typeof__(x) _min1 = (x);                      \
    __typeof__(y) _min2 = (y);                      \
    (void) (&_min1 == &_min2);                      \
    _min1 < _min2 ? _min1 : _min2; })

/**
 * osa_abs - 返回值的绝对值
 * @x: 输入值
 * 返回: 绝对值
 * 说明: 根据输入值的大小选择合适的类型来计算绝对值，确保结果正确。
 */
#define osa_abs(x) ({                               \
    long ret;                                       \
    if (sizeof(x) == sizeof(long)) {                \
        long __x = (x);                             \
        ret = (__x < 0) ? -__x : __x;               \
    } else {                                        \
        int __x = (x);                              \
        ret = (__x < 0) ? -__x : __x;               \
    }                                               \
    ret;                                            \
})

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

/* 本文件中没有结构体定义 */

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

/**
 * osa_div_u64 - 64位无符号整数除法
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算64位无符号整数的商。
 */
extern unsigned long long osa_div_u64(unsigned long long dividend, unsigned int divisor);

/**
 * osa_div_s64 - 64位有符号整数除法
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算64位有符号整数的商。
 */
extern long long osa_div_s64(long long dividend, int divisor);

/**
 * osa_div64_u64 - 64位无符号整数除法
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算两个64位无符号整数的商。
 */
extern unsigned long long osa_div64_u64(unsigned long long dividend, unsigned long long divisor);

/**
 * osa_div64_s64 - 64位有符号整数除法
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算两个64位有符号整数的商。
 */
extern long long osa_div64_s64(long long dividend, long long divisor);

/**
 * osa_div_u64_rem - 64位无符号整数除法并返回余数
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算64位无符号整数的商，并通过引用参数返回余数。
 */
extern unsigned long long osa_div_u64_rem(unsigned long long dividend, unsigned int divisor);

/**
 * osa_div_s64_rem - 64位有符号整数除法并返回余数
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算64位有符号整数的商，并通过引用参数返回余数。
 */
extern long long osa_div_s64_rem(long long dividend, int divisor);

/**
 * osa_div64_u64_rem - 64位无符号整数除法并返回余数
 * @dividend: 被除数
 * @divisor: 除数
 * 返回: 商
 * 说明: 计算两个64位无符号整数的商，并通过引用参数返回余数。
 */
extern unsigned long long osa_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);

/**
 * osa_random - 生成随机数
 * 返回: 生成的随机数
 * 说明: 生成一个32位的随机数。
 */
extern unsigned int osa_random(void);

#define osa_max(x, y) ({                            \
        __typeof__(x) _max1 = (x);                  \
        __typeof__(y) _max2 = (y);                  \
        (void) (&_max1 == &_max2);              \
        _max1 > _max2 ? _max1 : _max2; })

#define osa_min(x, y) ({                \
    __typeof__(x) _min1 = (x);          \
     __typeof__(y) _min2 = (y);          \
     (void) (&_min1 == &_min2);      \
     _min1 < _min2 ? _min1 : _min2; })

#define osa_abs(x) ({                \
    long ret;                         \
    if (sizeof(x) == sizeof(long)) {  \
        long __x = (x);               \
        ret = (__x < 0) ? -__x : __x; \
    } else {                          \
        int __x = (x);                \
        ret = (__x < 0) ? -__x : __x; \
    }                                 \
    ret;                              \
})

#endif // __OSA_MATH_H__
