#include "osa.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/version.h>

char *osa_strcpy(char *dest, const char *src)
{
    return strcpy(dest, src);
}
EXPORT_SYMBOL(osa_strcpy);
char *osa_strncpy(char *dest, const char *src, int count)
{
    return strncpy(dest, src, count);
}
EXPORT_SYMBOL(osa_strncpy);

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
int osa_strlcpy(char *dest, const char *src, int size)
{
    return strlcpy(dest, src, size);
}
EXPORT_SYMBOL(osa_strlcpy);
#endif
char *osa_strcat(char *dest, const char *src)
{
    return strcat(dest, src);
}
EXPORT_SYMBOL(osa_strcat);
char *osa_strncat(char *dest, const char *src, int count)
{
    return strncat(dest, src, count);
}
EXPORT_SYMBOL(osa_strncat);
int osa_strlcat(char *dest, const char *src, int count)
{
    return strlcat(dest, src, count);
}
EXPORT_SYMBOL(osa_strlcat);
int osa_strcmp(const char *cs, const char *ct)
{
    return strcmp(cs, ct);
}
EXPORT_SYMBOL(osa_strcmp);
int osa_strncmp(const char *cs, const char *ct, int count)
{
    return strncmp(cs, ct, count);
}
EXPORT_SYMBOL(osa_strncmp);
int osa_strnicmp(const char *s1, const char *s2, int len)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
    return strnicmp(s1, s2, len);
#endif
    return 0;
}
EXPORT_SYMBOL(osa_strnicmp);
int osa_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}
EXPORT_SYMBOL(osa_strcasecmp);
int osa_strncasecmp(const char *s1, const char *s2, int n)
{
    return strncasecmp(s1, s2, n);
}
EXPORT_SYMBOL(osa_strncasecmp);
char *osa_strchr(const char *s, int c)
{
    return strchr(s, c);
}
EXPORT_SYMBOL(osa_strchr);
char *osa_strnchr(const char *s, int count, int c)
{
    return strnchr(s, count, c);
}
EXPORT_SYMBOL(osa_strnchr);
char *osa_strrchr(const char *s, int c)
{
    return strrchr(s, c);
}
EXPORT_SYMBOL(osa_strrchr);
char *osa_strstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}
EXPORT_SYMBOL(osa_strstr);
char *osa_strnstr(const char *s1, const char *s2, int len)
{
    return strnstr(s1, s2, len);
}
EXPORT_SYMBOL(osa_strnstr);
int osa_strlen(const char *s)
{
    return strlen(s);
}
EXPORT_SYMBOL(osa_strlen);
int osa_strnlen(const char *s, int count)
{
    return strnlen(s, count);
}
EXPORT_SYMBOL(osa_strnlen);
char *osa_strpbrk(const char *cs, const char *ct)
{
    return strpbrk(cs, ct);
}
EXPORT_SYMBOL(osa_strpbrk);
char *osa_strsep(char **s, const char *ct)
{
    return strsep(s, ct);
}
EXPORT_SYMBOL(osa_strsep);
int osa_strspn(const char *s, const char *accept)
{
    return strspn(s, accept);
}
EXPORT_SYMBOL(osa_strspn);
int osa_strcspn(const char *s, const char *reject)
{
    return strcspn(s, reject);
}
EXPORT_SYMBOL(osa_strcspn);
void *osa_memset(void *str, int c, int count)
{
    return memset(str, c, count);
}
EXPORT_SYMBOL(osa_memset);
void *osa_memcpy(void *dest, const void *src, int count)
{
    return memcpy(dest, src, count);
}
EXPORT_SYMBOL(osa_memcpy);
void *osa_memmove(void *dest, const void *src, int count)
{
    return memmove(dest, src, count);
}
EXPORT_SYMBOL(osa_memmove);
void *osa_memscan(void *addr, int c, int size)
{
    return memscan(addr, c, size);
}
EXPORT_SYMBOL(osa_memscan);
int osa_memcmp(const void *cs, const void *ct, int count)
{
    return memcmp(cs, ct, count);
}
EXPORT_SYMBOL(osa_memcmp);
void *osa_memchr(const void *s, int c, int n)
{
    return memchr(s, c, n);
}
EXPORT_SYMBOL(osa_memchr);
void *osa_memchr_inv(const void *start, int c, int bytes)
{
    return memchr_inv(start, c, bytes);
}
EXPORT_SYMBOL(osa_memchr_inv);
unsigned long long osa_strtoull(const char *cp, char **endp, unsigned int base)
{
    return simple_strtoull(cp, endp, base);
}
EXPORT_SYMBOL(osa_strtoull);
unsigned long osa_strtoul(const char *cp, char **endp, unsigned int base)
{
    return simple_strtoul(cp, endp, base);
}
EXPORT_SYMBOL(osa_strtoul);
long osa_strtol(const char *cp, char **endp, unsigned int base)
{
    return simple_strtol(cp, endp, base);
}
EXPORT_SYMBOL(osa_strtol);
long long osa_strtoll(const char *cp, char **endp, unsigned int base)
{
    return simple_strtoll(cp, endp, base);
}
EXPORT_SYMBOL(osa_strtoll);
int osa_snprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(osa_snprintf);
int osa_scnprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vscnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(osa_scnprintf);
int osa_sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, INT_MAX, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(osa_sprintf);
int osa_sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsscanf(buf, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(osa_sscanf);

int osa_vsnprintf(char *str, int size, const char *fmt, osa_va_list args)
{
    return vsnprintf(str, size, fmt, args);
}
EXPORT_SYMBOL(osa_vsnprintf);



