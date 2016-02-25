#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "stack.h"
#include "assert.h"

static void __init test_stack(void)
{
    LIST_HEAD(data_stack);
    stack_entry_t *tos = NULL;
    const char *tos_data = NULL;
    const char* test_data[] = { "1", "2", "3", "4" };
    long i = 0;

    pr_alert("Testing basic stack");

    for (i = 0; i != ARRAY_SIZE(test_data); ++i) {
        stack_push(&data_stack,
            create_stack_entry((void*)test_data[i])
        );
    }

    for (i = ARRAY_SIZE(test_data) - 1; i >= 0; --i) {
        tos = stack_pop(&data_stack);
        tos_data = STACK_ENTRY_DATA(tos, const char*);
        delete_stack_entry(tos);
        printk(KERN_ALERT "%s == %s\n", tos_data, test_data[i]);
        assert(!strcmp(tos_data, test_data[i]));
    }

    assert(stack_empty(&data_stack));
}

static int __init print_processes_backwards(void)
{
    int ret = 0;
    LIST_HEAD(processes_stack);
    struct task_struct *tsk;

    for_each_process(tsk) {
        stack_entry_t *entry;
        char *comm_buf = kmalloc(sizeof(tsk->comm), GFP_KERNEL);

        if (!comm_buf) {
            ret = -ENOMEM;
            break;
        }
        get_task_comm(comm_buf, tsk);
        entry = create_stack_entry(comm_buf);
        if (!entry) {
            ret = -ENOMEM;
            break;
        }
        stack_push(&processes_stack, entry);
    }

    while (!stack_empty(&processes_stack)) {
        stack_entry_t *entry;
        char *comm_buf;

        entry = stack_pop(&processes_stack);
        comm_buf = STACK_ENTRY_DATA(entry, char *);
        delete_stack_entry(entry);
        if (!ret) {
            printk(KERN_ALERT "%s\n", comm_buf);
        }

        kfree(comm_buf);
    }

    return ret;
}

static int __init ll_init(void)
{
    printk(KERN_ALERT "Hello, linked_lists\n");
    test_stack();
    return print_processes_backwards();
}

static void __exit ll_exit(void)
{
    printk(KERN_ALERT "Goodbye, linked_lists!\n");
}

module_init(ll_init);
module_exit(ll_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linked list exercise module");
MODULE_AUTHOR("Kernel hacker!");
