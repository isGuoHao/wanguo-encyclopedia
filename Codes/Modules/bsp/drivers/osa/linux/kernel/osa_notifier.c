#include <linux/notifier.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <linux/module.h>
#include "osa.h"

OSA_LIST_HEAD(nb_list);

struct nb_node {
    struct osa_notifier_block *osa_nb;
    struct notifier_block *nb;
    struct osa_list_head node;
};

static struct osa_notifier_block *osa_find_ob(struct notifier_block *nb)
{
    struct osa_list_head *this = NULL;
    if (osa_list_empty(&nb_list)) {
        osa_printk("find nb failed! nb_list is empty!\n");
        return NULL;
    }
    osa_list_for_each(this, &nb_list) {
        struct nb_node *ns = osa_list_entry(this, struct nb_node, node);
        if (ns->nb == nb) {
            return ns->osa_nb;
        }
    }
    osa_printk("find ob failed!\n");
    return NULL;
}

static int osa_del_nb(struct osa_notifier_block *nb)
{
    struct osa_list_head *this = NULL;
    struct osa_list_head *next = NULL;
    if (osa_list_empty(&nb_list)) {
        osa_printk("find nb failed! nb_list is empty!\n");
        return -1;
    }
    osa_list_for_each_safe(this, next, &nb_list) {
        struct nb_node *ns = osa_list_entry(this, struct nb_node, node);
        if (ns->osa_nb == nb) {
            osa_list_del(this);
            kfree(ns);
            return 0;
        }
    }
    osa_printk("del nb failed!\n");
    return -1;
}

static int osa_notifier(struct notifier_block *nb, unsigned long action, void *data)
{
    struct osa_notifier_block *ob = osa_find_ob(nb);
    if ((ob != NULL) && (ob->notifier_call != NULL)) {
        return ob->notifier_call(ob, action, data);
    }
    return 0;
}

int osa_register_reboot_notifier(struct osa_notifier_block *ob)
{
    struct notifier_block *nb = NULL;
    struct nb_node *node = NULL;

    nb = kmalloc(sizeof(struct notifier_block), GFP_KERNEL);
    if (nb == NULL) {
        osa_printk("osa_register_reboot_notifier malloc nb failed!\n");
        return -1;
    }
    memset(nb, 0, sizeof(struct notifier_block));

    node = kmalloc(sizeof(struct nb_node), GFP_KERNEL);
    if (node == NULL) {
        osa_printk("osa_register_reboot_notifier kmalloc nb_node failed!\n");
        kfree(nb);
        return -1;
    }
    memset(node, 0, sizeof(struct nb_node));

    nb->notifier_call = osa_notifier;
    register_reboot_notifier(nb);
    ob->notifier_block = nb;
    node->osa_nb = ob;
    node->nb = nb;
    osa_list_add(&(node->node), &nb_list);
    return 0;
}
EXPORT_SYMBOL(osa_register_reboot_notifier);

int osa_unregister_reboot_notifier(struct osa_notifier_block *nb)
{
    if (nb != NULL) {
        osa_del_nb(nb);
        unregister_reboot_notifier((struct notifier_block *)nb->notifier_block);
        kfree((struct notifier_block *)nb->notifier_block);
    }
    return 0;
}
EXPORT_SYMBOL(osa_unregister_reboot_notifier);


