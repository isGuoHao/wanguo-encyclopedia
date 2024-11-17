#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/module.h>
#include "osa.h"

OSA_LIST_HEAD(wq_list);
struct wq_node {
    struct osa_work_struct *osa_work;
    struct work_struct *work;
    struct osa_list_head node;
};

static struct osa_work_struct *osa_find_work(struct work_struct *work)
{
    struct osa_list_head *this = NULL;
    if (osa_list_empty(&wq_list)) {
        osa_printk("find work failed! wq_list is empty!\n");
        return NULL;
    }
    osa_list_for_each(this, &wq_list) {
        struct wq_node *ws = osa_list_entry(this, struct wq_node, node);
        if (ws->work == work) {
            return ws->osa_work;
        }
    }
    osa_printk("find work failed!\n");
    return NULL;
}

static int osa_del_work(struct work_struct *work)
{
    struct osa_list_head *this = NULL;
    struct osa_list_head *next = NULL;
    if (osa_list_empty(&wq_list)) {
        osa_printk("find work failed! wq_list is empty!\n");
        return -1;
    }
    osa_list_for_each_safe(this, next, &wq_list) {
        struct wq_node *ws = osa_list_entry(this, struct wq_node, node);
        if (ws->work == work) {
            osa_list_del(this);
            kfree(ws);
            return 0;
        }
    }
    osa_printk("del work failed!\n");
    return -1;
}

static void osa_work_func(struct work_struct *work)
{
    struct osa_work_struct *ow = osa_find_work(work);
    if ((ow != NULL) && (ow->func != NULL)) {
        ow->func(ow);
    }
}

int osa_init_work(struct osa_work_struct *work, osa_work_func_t func)
{
    struct work_struct *w = NULL;
    struct wq_node *w_node = NULL;
    w = kmalloc(sizeof(struct work_struct), GFP_ATOMIC);
    if (w == NULL) {
        osa_printk("osa_init_work kmalloc failed!\n");
        return -1;
    }

    w_node = kmalloc(sizeof(struct wq_node), GFP_ATOMIC);
    if (w_node == NULL) {
        osa_printk("osa_init_work kmalloc failed!\n");
        kfree(w);
        return -1;
    }
    INIT_WORK(w, osa_work_func);
    work->work = w;
    work->func = func;
    w_node->osa_work = work;
    w_node->work = w;
    osa_list_add(&(w_node->node), &wq_list);
    return 0;
}
EXPORT_SYMBOL(osa_init_work);

int osa_schedule_work(struct osa_work_struct *work)
{
    if ((work != NULL) && (work->work != NULL)) {
        return (int)schedule_work(work->work);
    } else {
        return (int)false;
    }
}
EXPORT_SYMBOL(osa_schedule_work);

void osa_destroy_work(struct osa_work_struct *work)
{
    if ((work != NULL) && (work->work != NULL)) {
        osa_del_work(work->work);
        kfree((struct work_struct *)work->work);
        work->work = NULL;
    }
}
EXPORT_SYMBOL(osa_destroy_work);


