//设计一个模块，要求列出系统中所有内核线程的程序名、PID、进程状态、进程优先级、父进程的PID



```c
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/sched/signal.h>
#include<linux/sched.h>

MODULE_LICENSE("GPL");

static int __init show_all_kernel_thread_init(void)
{
    struct task_struct *P;
    prink("%-20s%-6s%-6s%-6s","Name","PID","State","Prio","PPID");
    prink("--------------------------------------");
    for_each_process(p)
    {
        if(p->m==NULL)
        {
            prink("%-20s%-6d%-6d%-6d%-6d",p->comm,p->pid,p->state,p->normal_prio,p->parent->pid);
        }
    }
    return 0;
}

static void __exit show_all_kernel_thread_exit(void)
{
    printk("[Show All Kernel Thread] Module Uninstalled.")
}

module_init(show_all_kernel_thread_init);
module_exit(show_all_kernel_thread_exit);
```

