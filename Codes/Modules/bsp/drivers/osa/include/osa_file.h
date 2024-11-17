#ifndef __OSA_FILE_H__
#define __OSA_FILE_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

#define OSA_O_ACCMODE     00000003
#define OSA_O_RDONLY      00000000
#define OSA_O_WRONLY      00000001
#define OSA_O_RDWR        00000002
#define OSA_O_CREAT       00000100

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

extern void *osa_klib_fopen(const char *filename, int flags, int mode);
extern void osa_klib_fclose(void *filp);
extern int osa_klib_fwrite(const char *buf, int len, void *filp);
extern int osa_klib_fread(char *buf, unsigned int len, void *filp);

#endif // __OSA_FILE_H__
