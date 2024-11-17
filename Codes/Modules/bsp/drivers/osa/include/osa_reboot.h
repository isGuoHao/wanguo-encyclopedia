#ifndef __OSA_REBOOT_H__
#define __OSA_REBOOT_H__

/* -------------------------------------------------------------- */
/*                             宏定义                                */
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/*                            结构体定义                               */
/* -------------------------------------------------------------- */

struct osa_notifier_block {
    int (*notifier_call)(struct osa_notifier_block *nb, unsigned long action, void *data);
    void *notifier_block;
};

typedef int (*osa_notifier_fn_t)(struct osa_notifier_block *nb, unsigned long action, void *data);

/* -------------------------------------------------------------- */
/*                             函数声明                                */
/* -------------------------------------------------------------- */

extern int osa_register_reboot_notifier(struct osa_notifier_block *nb);
extern int osa_unregister_reboot_notifier(struct osa_notifier_block *nb);

#endif // __OSA_REBOOT_H__
