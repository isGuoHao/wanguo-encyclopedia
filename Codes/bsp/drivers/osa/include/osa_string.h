#ifndef __OSA_STRING_H__
#define __OSA_STRING_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

/* 本文件中没有宏定义 */

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

/* 本文件中没有结构体定义 */

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

/**
 * osa_strcpy - 复制字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * 返回: 目标字符串的指针
 * 说明: 将源字符串s2复制到目标字符串s1中，直到遇到终止符'\0'。
 */
extern char *osa_strcpy(char *s1, const char *s2);

/**
 * osa_strncpy - 复制指定长度的字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * @size: 要复制的字符数
 * 返回: 目标字符串的指针
 * 说明: 将源字符串s2的前size个字符复制到目标字符串s1中。如果s2的长度小于size，则剩余部分用'\0'填充。
 */
extern char *osa_strncpy(char *s1, const char *s2, int size);

/**
 * osa_strlcpy - 安全地复制字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * @size: 目标缓冲区的大小
 * 返回: 源字符串的长度
 * 说明: 将源字符串s2复制到目标字符串s1中，最多复制size-1个字符，并确保s1以'\0'结尾。
 */
extern int osa_strlcpy(char *s1, const char *s2, int size);

/**
 * osa_strcat - 连接两个字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * 返回: 目标字符串的指针
 * 说明: 将源字符串s2追加到目标字符串s1的末尾。
 */
extern char *osa_strcat(char *s1, const char *s2);

/**
 * osa_strncat - 连接指定长度的两个字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * @size: 要连接的字符数
 * 返回: 目标字符串的指针
 * 说明: 将源字符串s2的前size个字符追加到目标字符串s1的末尾。
 */
extern char *osa_strncat(char *s1, const char *s2, int size);

/**
 * osa_strlcat - 安全地连接字符串
 * @s1: 目标字符串
 * @s2: 源字符串
 * @size: 目标缓冲区的大小
 * 返回: 目标字符串的长度
 * 说明: 将源字符串s2追加到目标字符串s1的末尾，最多追加size-1个字符，并确保s1以'\0'结尾。
 */
extern int osa_strlcat(char *s1, const char *s2, int size);

/**
 * osa_strcmp - 比较两个字符串
 * @s1: 第一个字符串
 * @s2: 第二个字符串
 * 返回: 如果s1等于s2返回0，如果s1小于s2返回负值，如果s1大于s2返回正值
 * 说明: 比较两个字符串s1和s2，直到遇到终止符'\0'。
 */
extern int osa_strcmp(const char *s1, const char *s2);

/**
 * osa_strncmp - 比较指定长度的两个字符串
 * @s1: 第一个字符串
 * @s2: 第二个字符串
 * @size: 要比较的字符数
 * 返回: 如果s1等于s2返回0，如果s1小于s2返回负值，如果s1大于s2返回正值
 * 说明: 比较两个字符串s1和s2的前size个字符。
 */
extern int osa_strncmp(const char *s1, const char *s2, int size);

/**
 * osa_strnicmp - 忽略大小写比较指定长度的两个字符串
 * @s1: 第一个字符串
 * @s2: 第二个字符串
 * @size: 要比较的字符数
 * 返回: 如果s1等于s2返回0，如果s1小于s2返回负值，如果s1大于s2返回正值
 * 说明: 比较两个字符串s1和s2的前size个字符，忽略大小写。
 */
extern int osa_strnicmp(const char *s1, const char *s2, int size);

/**
 * osa_strcasecmp - 忽略大小写比较两个字符串
 * @s1: 第一个字符串
 * @s2: 第二个字符串
 * 返回: 如果s1等于s2返回0，如果s1小于s2返回负值，如果s1大于s2返回正值
 * 说明: 比较两个字符串s1和s2，忽略大小写。
 */
extern int osa_strcasecmp(const char *s1, const char *s2);

/**
 * osa_strncasecmp - 忽略大小写比较指定长度的两个字符串
 * @s1: 第一个字符串
 * @s2: 第二个字符串
 * @n: 要比较的字符数
 * 返回: 如果s1等于s2返回0，如果s1小于s2返回负值，如果s1大于s2返回正值
 * 说明: 比较两个字符串s1和s2的前n个字符，忽略大小写。
 */
extern int osa_strncasecmp(const char *s1, const char *s2, int n);

/**
 * osa_strchr - 查找字符在字符串中的首次出现位置
 * @s: 字符串
 * @n: 要查找的字符
 * 返回: 指向首次出现位置的指针，如果没有找到则返回NULL
 * 说明: 在字符串s中查找字符n的首次出现位置。
 */
extern char *osa_strchr(const char *s, int n);

/**
 * osa_strnchr - 查找字符在字符串中的首次出现位置（限定长度）
 * @s: 字符串
 * @count: 要搜索的最大字符数
 * @c: 要查找的字符
 * 返回: 指向首次出现位置的指针，如果没有找到则返回NULL
 * 说明: 在字符串s的前count个字符中查找字符c的首次出现位置。
 */
extern char *osa_strnchr(const char *s, int count, int c);

/**
 * osa_strrchr - 查找字符在字符串中的最后一次出现位置
 * @s: 字符串
 * @c: 要查找的字符
 * 返回: 指向最后一次出现位置的指针，如果没有找到则返回NULL
 * 说明: 在字符串s中查找字符c的最后一次出现位置。
 */
extern char *osa_strrchr(const char *s, int c);

/**
 * osa_strstr - 查找子字符串在字符串中的首次出现位置
 * @s1: 主字符串
 * @s2: 子字符串
 * 返回: 指向首次出现位置的指针，如果没有找到则返回NULL
 * 说明: 在主字符串s1中查找子字符串s2的首次出现位置。
 */
extern char *osa_strstr(const char *s1, const char *s2);

/**
 * osa_strnstr - 查找子字符串在字符串中的首次出现位置（限定长度）
 * @s1: 主字符串
 * @s2: 子字符串
 * @n: 要搜索的最大字符数
 * 返回: 指向首次出现位置的指针，如果没有找到则返回NULL
 * 说明: 在主字符串s1的前n个字符中查找子字符串s2的首次出现位置。
 */
extern char *osa_strnstr(const char *s1, const char *s2, int n);

/**
 * osa_strlen - 获取字符串长度
 * @s: 字符串
 * 返回: 字符串的长度
 * 说明: 计算字符串s的长度，不包括终止符'\0'。
 */
extern int osa_strlen(const char *s);

/**
 * osa_strnlen - 获取字符串长度（限定最大长度）
 * @s: 字符串
 * @size: 最大长度
 * 返回: 字符串的长度，不超过size
 * 说明: 计算字符串s的长度，最多计算size个字符，不包括终止符'\0'。
 */
extern int osa_strnlen(const char *s, int size);

/**
 * osa_strpbrk - 查找字符串中第一个匹配字符集的字符
 * @s1: 字符串
 * @s2: 字符集
 * 返回: 指向第一个匹配字符的指针，如果没有找到则返回NULL
 * 说明: 在字符串s1中查找第一个出现在字符集s2中的字符。
 */
extern char *osa_strpbrk(const char *s1, const char *s2);

/**
 * osa_strsep - 从字符串中分离出一个标记字段
 * @s: 指向字符串的指针的指针
 * @ct: 分隔符集合
 * 返回: 分离出的标记字段，如果没有找到分隔符则返回NULL
 * 说明: 从字符串s中分离出一个由分隔符集合ct中的字符分隔的标记字段，并将该字段替换为'\0'。
 */
extern char *osa_strsep(char **s, const char *ct);

/**
 * osa_strspn - 计算字符串中前缀字符集的长度
 * @s: 字符串
 * @accept: 接受的字符集
 * 返回: 前缀字符集的长度
 * 说明: 计算字符串s中从开头开始连续出现的属于字符集accept的字符的长度。
 */
extern int osa_strspn(const char *s, const char *accept);

/**
 * osa_strcspn - 计算字符串中前缀非字符集的长度
 * @s: 字符串
 * @reject: 拒绝的字符集
 * 返回: 前缀非字符集的长度
 * 说明: 计算字符串s中从开头开始连续出现的不属于字符集reject的字符的长度。
 */
extern int osa_strcspn(const char *s, const char *reject);


extern void *osa_memset(void *str, int c, int count);
extern void *osa_memcpy(void *s1, const void *s2, int count);
extern void *osa_memmove(void *s1, const void *s2, int count);
extern void *osa_memscan(void *addr, int c, int size);
extern int osa_memcmp(const void *cs, const void *ct, int count);
extern void *osa_memchr(const void *s, int c, int n);
extern void *osa_memchr_inv(const void *s, int c, int n);

extern unsigned long long osa_strtoull(const char *cp, char **endp, unsigned int base);
extern unsigned long osa_strtoul(const char *cp, char **endp, unsigned int base);
extern long osa_strtol(const char *cp, char **endp, unsigned int base);
extern long long osa_strtoll(const char *cp, char **endp, unsigned int base);
extern int osa_snprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osa_scnprintf(char *buf, int size, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern int osa_sprintf(char *buf, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
extern int osa_sscanf(const char *buf, const char *fmt, ...);

#endif // __OSA_STRING_H__
