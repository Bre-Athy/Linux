# Linux进程管理之task_struct结构体

 进程是处于执行期的程序以及它所管理的资源（如打开的文件、挂起的信号、进程状态、地址空间等等）的总称。注意，程序并不是进程，实际上两个或多个进程不仅有可能执行同一程序，而且还有可能共享地址空间等资源。

​    Linux内核通过一个被称为进程描述符的task_struct结构体来管理进程，这个结构体包含了一个进程所需的所有信息。它定义在linux-2.6.38.8/include/linux/sched.h文件中。

 本文将尽力就task_struct结构体所有成员的用法进行简要说明。

​    1、进程状态 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. **volatile long state;**  
2. int exit_state;  

```
	volatile long state;
	int exit_state;
```

​    state成员的可能取值如下： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. \#define TASK_RUNNING        0  
2. \#define TASK_INTERRUPTIBLE  1  
3. \#define TASK_UNINTERRUPTIBLE    2  
4. \#define __TASK_STOPPED      4  
5. \#define __TASK_TRACED       8  
6. /* in tsk->exit_state */  
7. \#define EXIT_ZOMBIE     16  
8. \#define EXIT_DEAD       32  
9. /* in tsk->state again */  
10. \#define TASK_DEAD       64  
11. \#define TASK_WAKEKILL       128  
12. \#define TASK_WAKING     256  

```
#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define __TASK_STOPPED		4
#define __TASK_TRACED		8
/* in tsk->exit_state */
#define EXIT_ZOMBIE		16
#define EXIT_DEAD		32
/* in tsk->state again */
#define TASK_DEAD		64
#define TASK_WAKEKILL		128
#define TASK_WAKING		256
```

​    系统中的每个进程都必然处于以上所列进程状态中的一种。

​    TASK_RUNNING表示进程要么正在执行，要么正要准备执行。

​    TASK_INTERRUPTIBLE表示进程被阻塞（睡眠），直到某个条件变为真。条件一旦达成，进程的状态就被设置为TASK_RUNNING。

​    TASK_UNINTERRUPTIBLE的意义与TASK_INTERRUPTIBLE类似，除了不能通过接受一个信号来唤醒以外。

​    __TASK_STOPPED表示进程被停止执行。

​    __TASK_TRACED表示进程被debugger等进程监视。

​    EXIT_ZOMBIE表示进程的执行被终止，但是其父进程还没有使用wait()等系统调用来获知它的终止信息。

​    EXIT_DEAD表示进程的最终状态。

​    EXIT_ZOMBIE和EXIT_DEAD也可以存放在exit_state成员中。进程状态的切换过程和原因大致如下图（图片来自《Linux Kernel Development》）：

 ![img](http://my.csdn.net/uploads/201204/14/1334337470_8447.jpg)

​    2、进程标识符（PID） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. pid_t pid;  
2. pid_t tgid;  

 

```
	pid_t pid;
	pid_t tgid;
```

​    在CONFIG_BASE_SMALL配置为0的情况下，PID的取值范围是0到32767，即系统中的进程数最大为32768个。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/include/linux/threads.h */  
2. \#define PID_MAX_DEFAULT (CONFIG_BASE_SMALL ? 0x1000 : 0x8000)  

 

```
/* linux-2.6.38.8/include/linux/threads.h */
#define PID_MAX_DEFAULT (CONFIG_BASE_SMALL ? 0x1000 : 0x8000)
```

​    在Linux系统中，一个线程组中的所有线程使用和该线程组的领头线程（该组中的第一个轻量级进程）相同的PID，并被存放在tgid成员中。只有线程组的领头线程的pid成员才会被设置为与tgid相同的值。注意，getpid()系统调用返回的是当前进程的tgid值而不是pid值。

​    3、进程内核栈 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. **void \*stack;**  

 

```
	void *stack;
```

​    进程通过alloc_thread_info函数分配它的内核栈，通过free_thread_info函数释放所分配的内核栈。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/kernel/fork.c */   
2. **static inline struct thread_info \*alloc_thread_info(struct task_struct \*tsk)**  
3. {  
4. \#ifdef CONFIG_DEBUG_STACK_USAGE  
5. ​    gfp_t mask = GFP_KERNEL | __GFP_ZERO;  
6. \#else  
7. ​    gfp_t mask = GFP_KERNEL;  
8. \#endif  
9. ​    **return (struct thread_info \*)__get_free_pages(mask, THREAD_SIZE_ORDER);**  
10. }  
11. **static inline void free_thread_info(struct thread_info \*ti)**  
12. {  
13. ​    free_pages((unsigned long)ti, THREAD_SIZE_ORDER);  
14. }  

 

```
/* linux-2.6.38.8/kernel/fork.c */ 
static inline struct thread_info *alloc_thread_info(struct task_struct *tsk)
{
#ifdef CONFIG_DEBUG_STACK_USAGE
	gfp_t mask = GFP_KERNEL | __GFP_ZERO;
#else
	gfp_t mask = GFP_KERNEL;
#endif
	return (struct thread_info *)__get_free_pages(mask, THREAD_SIZE_ORDER);
}
static inline void free_thread_info(struct thread_info *ti)
{
	free_pages((unsigned long)ti, THREAD_SIZE_ORDER);
}
```

​    其中，THREAD_SIZE_ORDER宏在linux-2.6.38.8/arch/arm/include/asm/thread_info.h文件中被定义为1，也就是说alloc_thread_info函数通过调用__get_free_pages函数分配2个页的内存（它的首地址是8192字节对齐的）。

​    Linux内核通过thread_union联合体来表示进程的内核栈，其中THREAD_SIZE宏的大小为8192。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. **union thread_union {**  
2. ​    **struct thread_info thread_info;**  
3. ​    unsigned long stack[THREAD_SIZE/**sizeof(long)];**  
4. };  

 

```
union thread_union {
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};
```

​    当进程从用户态切换到内核态时，进程的内核栈总是空的，所以ARM的sp寄存器指向这个栈的顶端。因此，内核能够轻易地通过sp寄存器获得当前正在CPU上运行的进程。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/arch/arm/include/asm/current.h */  
2. **static inline struct task_struct \*get_current(void)**  
3. {  
4. ​    **return current_thread_info()->task;**  
5. }  
6.   
7. \#define current (get_current())  
8.   
9. /* linux-2.6.38.8/arch/arm/include/asm/thread_info.h */   
10. **static inline struct thread_info \*current_thread_info(void)**  
11. {  
12. ​    **register unsigned long sp asm ("sp");**  
13. ​    **return (struct thread_info \*)(sp & ~(THREAD_SIZE - 1));**  
14. }  

```
/* linux-2.6.38.8/arch/arm/include/asm/current.h */
static inline struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

#define current (get_current())

/* linux-2.6.38.8/arch/arm/include/asm/thread_info.h */ 
static inline struct thread_info *current_thread_info(void)
{
	register unsigned long sp asm ("sp");
	return (struct thread_info *)(sp & ~(THREAD_SIZE - 1));
}
```

​    进程内核栈与进程描述符的关系如下图：

 ![img](http://my.csdn.net/uploads/201204/14/1334337507_4423.jpg)

​    4、标记 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. unsigned int flags; /* per process flags, defined below */  

 

```
	unsigned int flags;	/* per process flags, defined below */
```

​    flags成员的可能取值如下： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. \#define PF_KSOFTIRQD    0x00000001  /* I am ksoftirqd */  
2. \#define PF_STARTING 0x00000002  /* being created */  
3. \#define PF_EXITING  0x00000004  /* getting shut down */  
4. \#define PF_EXITPIDONE   0x00000008  /* pi exit done on shut down */  
5. \#define PF_VCPU     0x00000010  /* I'm a virtual CPU */  
6. \#define PF_WQ_WORKER    0x00000020  /* I'm a workqueue worker */  
7. \#define PF_FORKNOEXEC   0x00000040  /* forked but didn't exec */  
8. \#define PF_MCE_PROCESS  0x00000080      /* process policy on mce errors */  
9. \#define PF_SUPERPRIV    0x00000100  /* used super-user privileges */  
10. \#define PF_DUMPCORE 0x00000200  /* dumped core */  
11. \#define PF_SIGNALED 0x00000400  /* killed by a signal */  
12. \#define PF_MEMALLOC 0x00000800  /* Allocating memory */  
13. \#define PF_USED_MATH    0x00002000  /* if unset the fpu must be initialized before use */  
14. \#define PF_FREEZING 0x00004000  /* freeze in progress. do not account to load */  
15. \#define PF_NOFREEZE 0x00008000  /* this thread should not be frozen */  
16. \#define PF_FROZEN   0x00010000  /* frozen for system suspend */  
17. \#define PF_FSTRANS  0x00020000  /* inside a filesystem transaction */  
18. \#define PF_KSWAPD   0x00040000  /* I am kswapd */  
19. \#define PF_OOM_ORIGIN   0x00080000  /* Allocating much memory to others */  
20. \#define PF_LESS_THROTTLE 0x00100000 /* Throttle me less: I clean memory */  
21. \#define PF_KTHREAD  0x00200000  /* I am a kernel thread */  
22. \#define PF_RANDOMIZE    0x00400000  /* randomize virtual address space */  
23. \#define PF_SWAPWRITE    0x00800000  /* Allowed to write to swap */  
24. \#define PF_SPREAD_PAGE  0x01000000  /* Spread page cache over cpuset */  
25. \#define PF_SPREAD_SLAB  0x02000000  /* Spread some slab caches over cpuset */  
26. \#define PF_THREAD_BOUND 0x04000000  /* Thread bound to specific cpu */  
27. \#define PF_MCE_EARLY    0x08000000      /* Early kill for mce process policy */  
28. \#define PF_MEMPOLICY    0x10000000  /* Non-default NUMA mempolicy */  
29. \#define PF_MUTEX_TESTER 0x20000000  /* Thread belongs to the rt mutex tester */  
30. \#define PF_FREEZER_SKIP 0x40000000  /* Freezer should not count it as freezable */  
31. \#define PF_FREEZER_NOSIG 0x80000000 /* Freezer won't send signals to it */  

```
#define PF_KSOFTIRQD	0x00000001	/* I am ksoftirqd */
#define PF_STARTING	0x00000002	/* being created */
#define PF_EXITING	0x00000004	/* getting shut down */
#define PF_EXITPIDONE	0x00000008	/* pi exit done on shut down */
#define PF_VCPU		0x00000010	/* I'm a virtual CPU */
#define PF_WQ_WORKER	0x00000020	/* I'm a workqueue worker */
#define PF_FORKNOEXEC	0x00000040	/* forked but didn't exec */
#define PF_MCE_PROCESS  0x00000080      /* process policy on mce errors */
#define PF_SUPERPRIV	0x00000100	/* used super-user privileges */
#define PF_DUMPCORE	0x00000200	/* dumped core */
#define PF_SIGNALED	0x00000400	/* killed by a signal */
#define PF_MEMALLOC	0x00000800	/* Allocating memory */
#define PF_USED_MATH	0x00002000	/* if unset the fpu must be initialized before use */
#define PF_FREEZING	0x00004000	/* freeze in progress. do not account to load */
#define PF_NOFREEZE	0x00008000	/* this thread should not be frozen */
#define PF_FROZEN	0x00010000	/* frozen for system suspend */
#define PF_FSTRANS	0x00020000	/* inside a filesystem transaction */
#define PF_KSWAPD	0x00040000	/* I am kswapd */
#define PF_OOM_ORIGIN	0x00080000	/* Allocating much memory to others */
#define PF_LESS_THROTTLE 0x00100000	/* Throttle me less: I clean memory */
#define PF_KTHREAD	0x00200000	/* I am a kernel thread */
#define PF_RANDOMIZE	0x00400000	/* randomize virtual address space */
#define PF_SWAPWRITE	0x00800000	/* Allowed to write to swap */
#define PF_SPREAD_PAGE	0x01000000	/* Spread page cache over cpuset */
#define PF_SPREAD_SLAB	0x02000000	/* Spread some slab caches over cpuset */
#define PF_THREAD_BOUND	0x04000000	/* Thread bound to specific cpu */
#define PF_MCE_EARLY    0x08000000      /* Early kill for mce process policy */
#define PF_MEMPOLICY	0x10000000	/* Non-default NUMA mempolicy */
#define PF_MUTEX_TESTER	0x20000000	/* Thread belongs to the rt mutex tester */
#define PF_FREEZER_SKIP	0x40000000	/* Freezer should not count it as freezable */
#define PF_FREEZER_NOSIG 0x80000000	/* Freezer won't send signals to it */
```

​    5、表示进程亲属关系的成员 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. **struct task_struct \*real_parent; /\* real parent process \*/**  
2. **struct task_struct \*parent; /\* recipient of SIGCHLD, wait4() reports \*/**  
3. **struct list_head children;  /\* list of my children \*/**  
4. **struct list_head sibling;   /\* linkage in my parent's children list \*/**  
5. **struct task_struct \*group_leader;   /\* threadgroup leader \*/**  

```
	struct task_struct *real_parent; /* real parent process */
	struct task_struct *parent; /* recipient of SIGCHLD, wait4() reports */
	struct list_head children;	/* list of my children */
	struct list_head sibling;	/* linkage in my parent's children list */
	struct task_struct *group_leader;	/* threadgroup leader */
```

​    在Linux系统中，所有进程之间都有着直接或间接地联系，每个进程都有其父进程，也可能有零个或多个子进程。拥有同一父进程的所有进程具有兄弟关系。

​    real_parent指向其父进程，如果创建它的父进程不再存在，则指向PID为1的init进程。

​    parent指向其父进程，当它终止时，必须向它的父进程发送信号。它的值通常与real_parent相同。

​    children表示链表的头部，链表中的所有元素都是它的子进程。

​    sibling用于把当前进程插入到兄弟链表中。

​    group_leader指向其所在进程组的领头进程。

​    6、ptrace系统调用 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. unsigned int ptrace;  
2. **struct list_head ptraced;**  
3. **struct list_head ptrace_entry;**  
4. unsigned long ptrace_message;  
5. siginfo_t *last_siginfo; /* For ptrace use.  */  
6. ifdef CONFIG_HAVE_HW_BREAKPOINT  
7. atomic_t ptrace_bp_refcnt;  
8. endif  

 

```
	unsigned int ptrace;
	struct list_head ptraced;
	struct list_head ptrace_entry;
	unsigned long ptrace_message;
	siginfo_t *last_siginfo; /* For ptrace use.  */
#ifdef CONFIG_HAVE_HW_BREAKPOINT
	atomic_t ptrace_bp_refcnt;
#endif
```

​    成员ptrace被设置为0时表示不需要被跟踪，它的可能取值如下： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/include/linux/ptrace.h */  
2. \#define PT_PTRACED  0x00000001  
3. \#define PT_DTRACE   0x00000002  /* delayed trace (used on m68k, i386) */  
4. \#define PT_TRACESYSGOOD 0x00000004  
5. \#define PT_PTRACE_CAP   0x00000008  /* ptracer can follow suid-exec */  
6. \#define PT_TRACE_FORK   0x00000010  
7. \#define PT_TRACE_VFORK  0x00000020  
8. \#define PT_TRACE_CLONE  0x00000040  
9. \#define PT_TRACE_EXEC   0x00000080  
10. \#define PT_TRACE_VFORK_DONE 0x00000100  
11. \#define PT_TRACE_EXIT   0x00000200  

 

```
/* linux-2.6.38.8/include/linux/ptrace.h */
#define PT_PTRACED	0x00000001
#define PT_DTRACE	0x00000002	/* delayed trace (used on m68k, i386) */
#define PT_TRACESYSGOOD	0x00000004
#define PT_PTRACE_CAP	0x00000008	/* ptracer can follow suid-exec */
#define PT_TRACE_FORK	0x00000010
#define PT_TRACE_VFORK	0x00000020
#define PT_TRACE_CLONE	0x00000040
#define PT_TRACE_EXEC	0x00000080
#define PT_TRACE_VFORK_DONE	0x00000100
#define PT_TRACE_EXIT	0x00000200
```

​    7、Performance Event 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. \#ifdef CONFIG_PERF_EVENTS  
2. ​    **struct perf_event_context \*perf_event_ctxp[perf_nr_task_contexts];**  
3. ​    **struct mutex perf_event_mutex;**  
4. ​    **struct list_head perf_event_list;**  
5. \#endif  

 

```
#ifdef CONFIG_PERF_EVENTS
	struct perf_event_context *perf_event_ctxp[perf_nr_task_contexts];
	struct mutex perf_event_mutex;
	struct list_head perf_event_list;
#endif
```

​    Performance Event是一款随 Linux 内核代码一同发布和维护的性能诊断工具。这些成员用于帮助PerformanceEvent分析进程的性能问题。

​    关于Performance Event工具的介绍可参考文章<http://www.ibm.com/developerworks/cn/linux/l-cn-perf1/index.html?ca=drs-#major1>和<http://www.ibm.com/developerworks/cn/linux/l-cn-perf2/index.html?ca=drs-#major1>。

​    8、进程调度 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. int prio, static_prio, normal_prio;  
2. unsigned int rt_priority;  
3. **const struct sched_class \*sched_class;**  
4. **struct sched_entity se;**  
5. **struct sched_rt_entity rt;**  
6. unsigned int policy;  
7. cpumask_t cpus_allowed;  

 

```
	int prio, static_prio, normal_prio;
	unsigned int rt_priority;
	const struct sched_class *sched_class;
	struct sched_entity se;
	struct sched_rt_entity rt;
	unsigned int policy;
	cpumask_t cpus_allowed;
```

​    实时优先级范围是0到MAX_RT_PRIO-1（即99），而普通进程的静态优先级范围是从MAX_RT_PRIO到MAX_PRIO-1（即100到139）。值越大静态优先级越低。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/include/linux/sched.h */  
2. \#define MAX_USER_RT_PRIO    100  
3. \#define MAX_RT_PRIO     MAX_USER_RT_PRIO  
4.   
5. \#define MAX_PRIO        (MAX_RT_PRIO + 40)  
6. \#define DEFAULT_PRIO        (MAX_RT_PRIO + 20)  

 

```
/* linux-2.6.38.8/include/linux/sched.h */
#define MAX_USER_RT_PRIO	100
#define MAX_RT_PRIO		MAX_USER_RT_PRIO

#define MAX_PRIO		(MAX_RT_PRIO + 40)
#define DEFAULT_PRIO		(MAX_RT_PRIO + 20)
```

​    static_prio用于保存静态优先级，可以通过nice系统调用来进行修改。

​    rt_priority用于保存实时优先级。

​    normal_prio的值取决于静态优先级和调度策略。

​    prio用于保存动态优先级。

​    policy表示进程的调度策略，目前主要有以下五种： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. \#define SCHED_NORMAL        0  
2. \#define SCHED_FIFO      1  
3. \#define SCHED_RR        2  
4. \#define SCHED_BATCH     3  
5. /* SCHED_ISO: reserved but not implemented yet */  
6. \#define SCHED_IDLE      5  

 

```
#define SCHED_NORMAL		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
/* SCHED_ISO: reserved but not implemented yet */
#define SCHED_IDLE		5
```

​    SCHED_NORMAL用于普通进程，通过CFS调度器实现。SCHED_BATCH用于非交互的处理器消耗型进程。SCHED_IDLE是在系统负载很低时使用。

​    SCHED_FIFO（先入先出调度[算法](http://lib.csdn.net/base/datastructure)）和SCHED_RR（轮流调度算法）都是实时调度策略。

​    sched_class结构体表示调度类，目前内核中有实现以下四种： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7292563#) [copy](http://blog.csdn.net/npy_lp/article/details/7292563#) [print](http://blog.csdn.net/npy_lp/article/details/7292563#)[?](http://blog.csdn.net/npy_lp/article/details/7292563#)

1. /* linux-2.6.38.8/kernel/sched_fair.c */   
2. **static const struct sched_class fair_sched_class;**  
3. /* linux-2.6.38.8/kernel/sched_rt.c */  
4. **static const struct sched_class rt_sched_class;**  
5. /* linux-2.6.38.8/kernel/sched_idletask.c */  
6. **static const struct sched_class idle_sched_class;**  
7. /* linux-2.6.38.8/kernel/sched_stoptask.c */  
8. **static const struct sched_class stop_sched_class;**  

 

```
/* linux-2.6.38.8/kernel/sched_fair.c */ 
static const struct sched_class fair_sched_class;
/* linux-2.6.38.8/kernel/sched_rt.c */
static const struct sched_class rt_sched_class;
/* linux-2.6.38.8/kernel/sched_idletask.c */
static const struct sched_class idle_sched_class;
/* linux-2.6.38.8/kernel/sched_stoptask.c */
static const struct sched_class stop_sched_class;
```

​    se和rt都是调用实体，一个用于普通进程，一个用于实时进程，每个进程都有其中之一的实体。

​    cpus_allowed用于控制进程可以在哪里处理器上运行。

 

​        


9、进程地址空间 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. ​    **struct mm_struct \*mm, \*active_mm;**  
2. \#ifdef CONFIG_COMPAT_BRK  
3. ​    unsigned brk_randomized:1;  
4. \#endif  
5. \#if defined(SPLIT_RSS_COUNTING)  
6. ​    **struct task_rss_stat    rss_stat;**  
7. \#endif  

[![img](http://static.blog.csdn.net/images/save_snippets.png)](javascript:;)

```
	struct mm_struct *mm, *active_mm;
#ifdef CONFIG_COMPAT_BRK
	unsigned brk_randomized:1;
#endif
#if defined(SPLIT_RSS_COUNTING)
	struct task_rss_stat	rss_stat;
#endif
```

​    mm指向进程所拥有的内存描述符，而active_mm指向进程运行时所使用的内存描述符。对于普通进程而言，这两个指针变量的值相同。但是，内核线程不拥有任何内存描述符，所以它们的mm成员总是为NULL。当内核线程得以运行时，它的active_mm成员被初始化为前一个运行进程的active_mm值。

​    brk_randomized的用法在http://lkml.indiana.edu/hypermail/[Linux](http://lib.csdn.net/base/linux)/kernel/1104.1/00196.html上有介绍，用来确定对随机堆内存的探测。

​    rss_stat用来记录缓冲信息。 

​    10、判断标志 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. int exit_code, exit_signal;  
2. int pdeath_signal;  /*  The signal sent when the parent dies  */  
3. /* ??? */  
4. unsigned int personality;  
5. unsigned did_exec:1;  
6. unsigned in_execve:1;   /* Tell the LSMs that the process is doing an 
7. ​             \* execve */  
8. unsigned in_iowait:1;  
9.   
10.   
11. /* Revert to default priority/policy when forking */  
12. unsigned sched_reset_on_fork:1;  

```
	int exit_code, exit_signal;
	int pdeath_signal;  /*  The signal sent when the parent dies  */
	/* ??? */
	unsigned int personality;
	unsigned did_exec:1;
	unsigned in_execve:1;	/* Tell the LSMs that the process is doing an
				 * execve */
	unsigned in_iowait:1;


	/* Revert to default priority/policy when forking */
	unsigned sched_reset_on_fork:1;
```

​    exit_code用于设置进程的终止代号，这个值要么是_exit()或exit_group()系统调用参数（正常终止），要么是由内核提供的一个错误代号（异常终止）。

​    exit_signal被置为-1时表示是某个线程组中的一员。只有当线程组的最后一个成员终止时，才会产生一个信号，以通知线程组的领头进程的父进程。

​    pdeath_signal用于判断父进程终止时发送信号。

​    personality用于处理不同的ABI，它的可能取值如下： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **enum {**  
2. ​    PER_LINUX =     0x0000,  
3. ​    PER_LINUX_32BIT =   0x0000 | ADDR_LIMIT_32BIT,  
4. ​    PER_LINUX_FDPIC =   0x0000 | FDPIC_FUNCPTRS,  
5. ​    PER_SVR4 =      0x0001 | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,  
6. ​    PER_SVR3 =      0x0002 | STICKY_TIMEOUTS | SHORT_INODE,  
7. ​    PER_SCOSVR3 =       0x0003 | STICKY_TIMEOUTS |  
8. ​                     WHOLE_SECONDS | SHORT_INODE,  
9. ​    PER_OSR5 =      0x0003 | STICKY_TIMEOUTS | WHOLE_SECONDS,  
10. ​    PER_WYSEV386 =      0x0004 | STICKY_TIMEOUTS | SHORT_INODE,  
11. ​    PER_ISCR4 =     0x0005 | STICKY_TIMEOUTS,  
12. ​    PER_BSD =       0x0006,  
13. ​    PER_SUNOS =     0x0006 | STICKY_TIMEOUTS,  
14. ​    PER_XENIX =     0x0007 | STICKY_TIMEOUTS | SHORT_INODE,  
15. ​    PER_LINUX32 =       0x0008,  
16. ​    PER_LINUX32_3GB =   0x0008 | ADDR_LIMIT_3GB,  
17. ​    PER_IRIX32 =        0x0009 | STICKY_TIMEOUTS,/* IRIX5 32-bit */  
18. ​    PER_IRIXN32 =       0x000a | STICKY_TIMEOUTS,/* IRIX6 new 32-bit */  
19. ​    PER_IRIX64 =        0x000b | STICKY_TIMEOUTS,/* IRIX6 64-bit */  
20. ​    PER_RISCOS =        0x000c,  
21. ​    PER_SOLARIS =       0x000d | STICKY_TIMEOUTS,  
22. ​    PER_UW7 =       0x000e | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,  
23. ​    PER_OSF4 =      0x000f,          /* OSF/1 v4 */  
24. ​    PER_HPUX =      0x0010,  
25. ​    PER_MASK =      0x00ff,  
26. };  

```
enum {
	PER_LINUX =		0x0000,
	PER_LINUX_32BIT =	0x0000 | ADDR_LIMIT_32BIT,
	PER_LINUX_FDPIC =	0x0000 | FDPIC_FUNCPTRS,
	PER_SVR4 =		0x0001 | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
	PER_SVR3 =		0x0002 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_SCOSVR3 =		0x0003 | STICKY_TIMEOUTS |
					 WHOLE_SECONDS | SHORT_INODE,
	PER_OSR5 =		0x0003 | STICKY_TIMEOUTS | WHOLE_SECONDS,
	PER_WYSEV386 =		0x0004 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_ISCR4 =		0x0005 | STICKY_TIMEOUTS,
	PER_BSD =		0x0006,
	PER_SUNOS =		0x0006 | STICKY_TIMEOUTS,
	PER_XENIX =		0x0007 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_LINUX32 =		0x0008,
	PER_LINUX32_3GB =	0x0008 | ADDR_LIMIT_3GB,
	PER_IRIX32 =		0x0009 | STICKY_TIMEOUTS,/* IRIX5 32-bit */
	PER_IRIXN32 =		0x000a | STICKY_TIMEOUTS,/* IRIX6 new 32-bit */
	PER_IRIX64 =		0x000b | STICKY_TIMEOUTS,/* IRIX6 64-bit */
	PER_RISCOS =		0x000c,
	PER_SOLARIS =		0x000d | STICKY_TIMEOUTS,
	PER_UW7 =		0x000e | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
	PER_OSF4 =		0x000f,			 /* OSF/1 v4 */
	PER_HPUX =		0x0010,
	PER_MASK =		0x00ff,
};
```

​    did_exec用于记录进程代码是否被execve()函数所执行。

​    in_execve用于通知LSM是否被do_execve()函数所调用。详见补丁说明：<http://lkml.indiana.edu/hypermail/linux/kernel/0901.1/00014.html>。

​    in_iowait用于判断是否进行iowait计数。

​    sched_reset_on_fork用于判断是否恢复默认的优先级或调度策略。

​    11、时间 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. ​    cputime_t utime, stime, utimescaled, stimescaled;  
2. ​    cputime_t gtime;  
3. \#ifndef CONFIG_VIRT_CPU_ACCOUNTING  
4. ​    cputime_t prev_utime, prev_stime;  
5. \#endif  
6. ​    unsigned long nvcsw, nivcsw; /* context switch counts */  
7. ​    **struct timespec start_time;         /\* monotonic time \*/**  
8. ​    **struct timespec real_start_time;    /\* boot based time \*/**  
9. ​    **struct task_cputime cputime_expires;**  
10. ​    **struct list_head cpu_timers[3];**  
11. \#ifdef CONFIG_DETECT_HUNG_TASK  
12. /* hung task detection */  
13. ​    unsigned long last_switch_count;  
14. \#endif  

```
	cputime_t utime, stime, utimescaled, stimescaled;
	cputime_t gtime;
#ifndef CONFIG_VIRT_CPU_ACCOUNTING
	cputime_t prev_utime, prev_stime;
#endif
	unsigned long nvcsw, nivcsw; /* context switch counts */
	struct timespec start_time; 		/* monotonic time */
	struct timespec real_start_time;	/* boot based time */
	struct task_cputime cputime_expires;
	struct list_head cpu_timers[3];
#ifdef CONFIG_DETECT_HUNG_TASK
/* hung task detection */
	unsigned long last_switch_count;
#endif
```

​    utime/stime用于记录进程在用户态/内核态下所经过的节拍数（定时器）。prev_utime/prev_stime是先前的运行时间，请参考补丁说明<http://lkml.indiana.edu/hypermail/linux/kernel/1003.3/02431.html>。

​    utimescaled/stimescaled也是用于记录进程在用户态/内核态的运行时间，但它们以处理器的频率为刻度。

​    gtime是以节拍计数的虚拟机运行时间（guest time）。

​    nvcsw/nivcsw是自愿（voluntary）/非自愿（involuntary）上下文切换计数。last_switch_count是nvcsw和nivcsw的总和。

​    start_time和real_start_time都是进程创建时间，real_start_time还包含了进程睡眠时间，常用于/proc/pid/stat，补丁说明请参考<http://lkml.indiana.edu/hypermail/linux/kernel/0705.0/2094.html>。

​    cputime_expires用来统计进程或进程组被跟踪的处理器时间，其中的三个成员对应着cpu_timers[3]的三个链表。

​    12、信号处理 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* signal handlers */  
2. ​    **struct signal_struct \*signal;**  
3. ​    **struct sighand_struct \*sighand;**  
4.   
5. ​    sigset_t blocked, real_blocked;  
6. ​    sigset_t saved_sigmask; /* restored if set_restore_sigmask() was used */  
7. ​    **struct sigpending pending;**  
8.   
9. ​    unsigned long sas_ss_sp;  
10. ​    size_t sas_ss_size;  
11. ​    int (*notifier)(**void \*priv);**  
12. ​    **void \*notifier_data;**  
13. ​    sigset_t *notifier_mask;  

```
/* signal handlers */
	struct signal_struct *signal;
	struct sighand_struct *sighand;

	sigset_t blocked, real_blocked;
	sigset_t saved_sigmask;	/* restored if set_restore_sigmask() was used */
	struct sigpending pending;

	unsigned long sas_ss_sp;
	size_t sas_ss_size;
	int (*notifier)(void *priv);
	void *notifier_data;
	sigset_t *notifier_mask;
```

​    signal指向进程的信号描述符。

​    sighand指向进程的信号处理程序描述符。

​    blocked表示被阻塞信号的掩码，real_blocked表示临时掩码。

​    pending存放私有挂起信号的[数据结构](http://lib.csdn.net/base/datastructure)。

​    sas_ss_sp是信号处理程序备用堆栈的地址，sas_ss_size表示堆栈的大小。

​    设备驱动程序常用notifier指向的函数来阻塞进程的某些信号（notifier_mask是这些信号的位掩码），notifier_data指的是notifier所指向的函数可能使用的数据。

​    13、其他

​    （1）、用于保护资源分配或释放的自旋锁 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* Protection of (de-)allocation: mm, files, fs, tty, keyrings, mems_allowed, 
2.  \* mempolicy */  
3. ​    spinlock_t alloc_lock;  

 

```
/* Protection of (de-)allocation: mm, files, fs, tty, keyrings, mems_allowed,
 * mempolicy */
	spinlock_t alloc_lock;
```

​    （2）、进程描述符使用计数，被置为2时，表示进程描述符正在被使用而且其相应的进程处于活动状态。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. atomic_t usage;  

 

```
	atomic_t usage;
```

​    （3）、用于表示获取大内核锁的次数，如果进程未获得过锁，则置为-1。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. int lock_depth;     /* BKL lock depth */  

 

```
	int lock_depth;		/* BKL lock depth */
```

​    （4）、在SMP上帮助实现无加锁的进程切换（unlocked context switches） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_SMP  
2. \#ifdef __ARCH_WANT_UNLOCKED_CTXSW  
3. ​    int oncpu;  
4. \#endif  
5. \#endif  

```
#ifdef CONFIG_SMP
#ifdef __ARCH_WANT_UNLOCKED_CTXSW
	int oncpu;
#endif
#endif
```

​    （5）、preempt_notifier结构体链表 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_PREEMPT_NOTIFIERS  
2. ​    /* list of struct preempt_notifier: */  
3. ​    **struct hlist_head preempt_notifiers;**  
4. \#endif  

 

```
#ifdef CONFIG_PREEMPT_NOTIFIERS
	/* list of struct preempt_notifier: */
	struct hlist_head preempt_notifiers;
#endif
```

​    （6）、FPU使用计数 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. unsigned char fpu_counter;  

 

```
	unsigned char fpu_counter;
```

​    （7）、blktrace是一个针对Linux内核中块设备I/O层的跟踪工具。 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_BLK_DEV_IO_TRACE  
2. ​    unsigned int btrace_seq;  
3. \#endif  

```
#ifdef CONFIG_BLK_DEV_IO_TRACE
	unsigned int btrace_seq;
#endif
```

​    （8）、RCU同步原语 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_PREEMPT_RCU  
2. ​    int rcu_read_lock_nesting;  
3. ​    char rcu_read_unlock_special;  
4. ​    **struct list_head rcu_node_entry;**  
5. \#endif /* #ifdef CONFIG_PREEMPT_RCU */  
6. \#ifdef CONFIG_TREE_PREEMPT_RCU  
7. ​    **struct rcu_node \*rcu_blocked_node;**  
8. \#endif /* #ifdef CONFIG_TREE_PREEMPT_RCU */  
9. \#ifdef CONFIG_RCU_BOOST  
10. ​    **struct rt_mutex \*rcu_boost_mutex;**  
11. \#endif /* #ifdef CONFIG_RCU_BOOST */  

 

```
#ifdef CONFIG_PREEMPT_RCU
	int rcu_read_lock_nesting;
	char rcu_read_unlock_special;
	struct list_head rcu_node_entry;
#endif /* #ifdef CONFIG_PREEMPT_RCU */
#ifdef CONFIG_TREE_PREEMPT_RCU
	struct rcu_node *rcu_blocked_node;
#endif /* #ifdef CONFIG_TREE_PREEMPT_RCU */
#ifdef CONFIG_RCU_BOOST
	struct rt_mutex *rcu_boost_mutex;
#endif /* #ifdef CONFIG_RCU_BOOST */
```

​    （9）、用于调度器统计进程的运行信息 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#if defined(CONFIG_SCHEDSTATS) || defined(CONFIG_TASK_DELAY_ACCT)  
2. ​    **struct sched_info sched_info;**  
3. \#endif  

```
#if defined(CONFIG_SCHEDSTATS) || defined(CONFIG_TASK_DELAY_ACCT)
	struct sched_info sched_info;
#endif
```

​    （10）、用于构建进程链表 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct list_head tasks;**  

 

```
	struct list_head tasks;
```

​    （11）、to limit pushing to one attempt 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_SMP  
2. ​    **struct plist_node pushable_tasks;**  
3. \#endif  

 

```
#ifdef CONFIG_SMP
	struct plist_node pushable_tasks;
#endif
```

​    补丁说明请参考：http://lkml.indiana.edu/hypermail/linux/kernel/0808.3/0503.html

​    （12）、防止内核堆栈溢出 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_CC_STACKPROTECTOR  
2. ​    /* Canary value for the -fstack-protector gcc feature */  
3. ​    unsigned long stack_canary;  
4. \#endif  

```
#ifdef CONFIG_CC_STACKPROTECTOR
	/* Canary value for the -fstack-protector gcc feature */
	unsigned long stack_canary;
#endif
```

​    在GCC编译内核时，需要加上-fstack-protector选项。

​    （13）、PID散列表和链表 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* PID/PID hash table linkage. */  
2. **struct pid_link pids[PIDTYPE_MAX];**  
3. **struct list_head thread_group; //线程组中所有进程的链表**  

 

```
	/* PID/PID hash table linkage. */
	struct pid_link pids[PIDTYPE_MAX];
	struct list_head thread_group; //线程组中所有进程的链表
```

​    （14）、do_fork函数 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct completion \*vfork_done;      /\* for vfork() \*/**  
2. int __user *set_child_tid;      /* CLONE_CHILD_SETTID */  
3. int __user *clear_child_tid;        /* CLONE_CHILD_CLEARTID */  

 

```
	struct completion *vfork_done;		/* for vfork() */
	int __user *set_child_tid;		/* CLONE_CHILD_SETTID */
	int __user *clear_child_tid;		/* CLONE_CHILD_CLEARTID */
```

​    在执行do_fork()时，如果给定特别标志，则vfork_done会指向一个特殊地址。

​    如果copy_process函数的clone_flags参数的值被置为CLONE_CHILD_SETTID或CLONE_CHILD_CLEARTID，则会把child_tidptr参数的值分别复制到set_child_tid和clear_child_tid成员。这些标志说明必须改变子进程用户态地址空间的child_tidptr所指向的变量的值。

​    （15）、缺页统计 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* mm fault and swap info: this can arguably be seen as either mm-specific or thread-specific */  
2. ​    unsigned long min_flt, maj_flt;  

 

```
/* mm fault and swap info: this can arguably be seen as either mm-specific or thread-specific */
	unsigned long min_flt, maj_flt;
```

​    （16）、进程权能 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **const struct cred __rcu \*real_cred; /\* objective and real subjective task** 
2. ​                 \* credentials (COW) */  
3. **const struct cred __rcu \*cred;  /\* effective (overridable) subjective task** 
4. ​                 \* credentials (COW) */  
5. **struct cred \*replacement_session_keyring; /\* for KEYCTL_SESSION_TO_PARENT \*/**  

 

```
	const struct cred __rcu *real_cred; /* objective and real subjective task
					 * credentials (COW) */
	const struct cred __rcu *cred;	/* effective (overridable) subjective task
					 * credentials (COW) */
	struct cred *replacement_session_keyring; /* for KEYCTL_SESSION_TO_PARENT */
```

​    （17）、相应的程序名 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. char comm[TASK_COMM_LEN];  

```
	char comm[TASK_COMM_LEN];
```

​    （18）、文件 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* file system info */  
2. ​    int link_count, total_link_count;  
3. /* filesystem information */  
4. ​    **struct fs_struct \*fs;**  
5. /* open file information */  
6. ​    **struct files_struct \*files;**  

 

```
/* file system info */
	int link_count, total_link_count;
/* filesystem information */
	struct fs_struct *fs;
/* open file information */
	struct files_struct *files;
```

​    fs用来表示进程与文件系统的联系，包括当前目录和根目录。

​    files表示进程当前打开的文件。

​    （19）、进程通信（SYSVIPC） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_SYSVIPC  
2. /* ipc stuff */  
3. ​    **struct sysv_sem sysvsem;**  
4. \#endif  

```
#ifdef CONFIG_SYSVIPC
/* ipc stuff */
	struct sysv_sem sysvsem;
#endif
```

​    （20）、处理器特有数据 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* CPU-specific state of this task */  
2. ​    **struct thread_struct thread;**  

 

```
/* CPU-specific state of this task */
	struct thread_struct thread;
```

​    （21）、命名空间 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* namespaces */  
2. ​    **struct nsproxy \*nsproxy;**  

 

```
/* namespaces */
	struct nsproxy *nsproxy;
```

​    （22）、进程审计 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. ​    **struct audit_context \*audit_context;**  
2. \#ifdef CONFIG_AUDITSYSCALL  
3. ​    uid_t loginuid;  
4. ​    unsigned int sessionid;  
5. \#endif  

 

```
	struct audit_context *audit_context;
#ifdef CONFIG_AUDITSYSCALL
	uid_t loginuid;
	unsigned int sessionid;
#endif
```

​    （23）、secure computing 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. seccomp_t seccomp;  

 

```
	seccomp_t seccomp;
```

​    （24）、用于copy_process函数使用CLONE_PARENT 标记时 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* Thread group tracking */  
2. ​    u32 parent_exec_id;  
3. ​    u32 self_exec_id;  

 

```
/* Thread group tracking */
   	u32 parent_exec_id;
   	u32 self_exec_id;
```

​    （25）、中断 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_GENERIC_HARDIRQS  
2. ​    /* IRQ handler threads */  
3. ​    **struct irqaction \*irqaction;**  
4. \#endif  
5. \#ifdef CONFIG_TRACE_IRQFLAGS  
6. ​    unsigned int irq_events;  
7. ​    unsigned long hardirq_enable_ip;  
8. ​    unsigned long hardirq_disable_ip;  
9. ​    unsigned int hardirq_enable_event;  
10. ​    unsigned int hardirq_disable_event;  
11. ​    int hardirqs_enabled;  
12. ​    int hardirq_context;  
13. ​    unsigned long softirq_disable_ip;  
14. ​    unsigned long softirq_enable_ip;  
15. ​    unsigned int softirq_disable_event;  
16. ​    unsigned int softirq_enable_event;  
17. ​    int softirqs_enabled;  
18. ​    int softirq_context;  
19. \#endif  

```
#ifdef CONFIG_GENERIC_HARDIRQS
	/* IRQ handler threads */
	struct irqaction *irqaction;
#endif
#ifdef CONFIG_TRACE_IRQFLAGS
	unsigned int irq_events;
	unsigned long hardirq_enable_ip;
	unsigned long hardirq_disable_ip;
	unsigned int hardirq_enable_event;
	unsigned int hardirq_disable_event;
	int hardirqs_enabled;
	int hardirq_context;
	unsigned long softirq_disable_ip;
	unsigned long softirq_enable_ip;
	unsigned int softirq_disable_event;
	unsigned int softirq_enable_event;
	int softirqs_enabled;
	int softirq_context;
#endif
```

​    （26）、task_rq_lock函数所使用的锁 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* Protection of the PI data structures: */  
2. raw_spinlock_t pi_lock;  

 

```
	/* Protection of the PI data structures: */
	raw_spinlock_t pi_lock;
```

​    （27）、基于PI协议的等待互斥锁，其中PI指的是priority inheritance（优先级继承） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_RT_MUTEXES  
2. ​    /* PI waiters blocked on a rt_mutex held by this task */  
3. ​    **struct plist_head pi_waiters;**  
4. ​    /* Deadlock detection and priority inheritance handling */  
5. ​    **struct rt_mutex_waiter \*pi_blocked_on;**  
6. \#endif  

 

```
#ifdef CONFIG_RT_MUTEXES
	/* PI waiters blocked on a rt_mutex held by this task */
	struct plist_head pi_waiters;
	/* Deadlock detection and priority inheritance handling */
	struct rt_mutex_waiter *pi_blocked_on;
#endif
```

​    （28）、死锁检测 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_DEBUG_MUTEXES  
2. ​    /* mutex deadlock detection */  
3. ​    **struct mutex_waiter \*blocked_on;**  
4. \#endif  

```
#ifdef CONFIG_DEBUG_MUTEXES
	/* mutex deadlock detection */
	struct mutex_waiter *blocked_on;
#endif
```

​    （29）、lockdep，参见内核说明文档linux-2.6.38.8/Documentation/lockdep-design.txt 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_LOCKDEP  
2. \# define MAX_LOCK_DEPTH 48UL  
3. ​    u64 curr_chain_key;  
4. ​    int lockdep_depth;  
5. ​    unsigned int lockdep_recursion;  
6. ​    **struct held_lock held_locks[MAX_LOCK_DEPTH];**  
7. ​    gfp_t lockdep_reclaim_gfp;  
8. \#endif  

 

```
#ifdef CONFIG_LOCKDEP
# define MAX_LOCK_DEPTH 48UL
	u64 curr_chain_key;
	int lockdep_depth;
	unsigned int lockdep_recursion;
	struct held_lock held_locks[MAX_LOCK_DEPTH];
	gfp_t lockdep_reclaim_gfp;
#endif
```

​    （30）、JFS文件系统 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* journalling filesystem info */  
2. ​    **void \*journal_info;**  

```
/* journalling filesystem info */
	void *journal_info;
```

​    （31）、块设备链表 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. /* stacked block device info */  
2. ​    **struct bio_list \*bio_list;**  

 

```
/* stacked block device info */
	struct bio_list *bio_list;
```

​    （32）、内存回收 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct reclaim_state \*reclaim_state;**  

 

```
	struct reclaim_state *reclaim_state;
```

​    （33）、存放块设备I/O数据流量信息

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct backing_dev_info \*backing_dev_info;**  

 

```
	struct backing_dev_info *backing_dev_info;
```

​    （34）、I/O调度器所使用的信息 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct io_context \*io_context;**  

```
	struct io_context *io_context;
```

​    （35）、记录进程的I/O计数 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct task_io_accounting ioac;**  
2. **if defined(CONFIG_TASK_XACCT)**  
3. u64 acct_rss_mem1;  /* accumulated rss usage */  
4. u64 acct_vm_mem1;   /* accumulated virtual memory usage */  
5. cputime_t acct_timexpd; /* stime + utime since last update */  
6. endif  

 

```
	struct task_io_accounting ioac;
#if defined(CONFIG_TASK_XACCT)
	u64 acct_rss_mem1;	/* accumulated rss usage */
	u64 acct_vm_mem1;	/* accumulated virtual memory usage */
	cputime_t acct_timexpd;	/* stime + utime since last update */
#endif
```

​    在Ubuntu 11.04上，执行cat获得进程1的I/O计数如下： 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. $ sudo cat /proc/1/io  

 

```
$ sudo cat /proc/1/io
```

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. rchar: 164258906  
2. wchar: 455212837  
3. syscr: 388847  
4. syscw: 92563  
5. read_bytes: 439251968  
6. write_bytes: 14143488  
7. cancelled_write_bytes: 2134016  

```
rchar: 164258906
wchar: 455212837
syscr: 388847
syscw: 92563
read_bytes: 439251968
write_bytes: 14143488
cancelled_write_bytes: 2134016
```

​    输出的数据项刚好是task_io_accounting结构体的所有成员。

​    （36）、CPUSET功能 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_CPUSETS  
2. ​    nodemask_t mems_allowed;    /* Protected by alloc_lock */  
3. ​    int mems_allowed_change_disable;  
4. ​    int cpuset_mem_spread_rotor;  
5. ​    int cpuset_slab_spread_rotor;  
6. \#endif  

 

```
#ifdef CONFIG_CPUSETS
	nodemask_t mems_allowed;	/* Protected by alloc_lock */
	int mems_allowed_change_disable;
	int cpuset_mem_spread_rotor;
	int cpuset_slab_spread_rotor;
#endif
```

​    （37）、Control Groups 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_CGROUPS  
2. ​    /* Control Group info protected by css_set_lock */  
3. ​    **struct css_set __rcu \*cgroups;**  
4. ​    /* cg_list protected by css_set_lock and tsk->alloc_lock */  
5. ​    **struct list_head cg_list;**  
6. \#endif  
7. \#ifdef CONFIG_CGROUP_MEM_RES_CTLR /* memcg uses this to do batch job */  
8. ​    **struct memcg_batch_info {**  
9. ​        int do_batch;   /* incremented when batch uncharge started */  
10. ​        **struct mem_cgroup \*memcg; /\* target memcg of uncharge \*/**  
11. ​        unsigned long bytes;        /* uncharged usage */  
12. ​        unsigned long memsw_bytes; /* uncharged mem+swap usage */  
13. ​    } memcg_batch;  
14. \#endif  

```
#ifdef CONFIG_CGROUPS
	/* Control Group info protected by css_set_lock */
	struct css_set __rcu *cgroups;
	/* cg_list protected by css_set_lock and tsk->alloc_lock */
	struct list_head cg_list;
#endif
#ifdef CONFIG_CGROUP_MEM_RES_CTLR /* memcg uses this to do batch job */
	struct memcg_batch_info {
		int do_batch;	/* incremented when batch uncharge started */
		struct mem_cgroup *memcg; /* target memcg of uncharge */
		unsigned long bytes; 		/* uncharged usage */
		unsigned long memsw_bytes; /* uncharged mem+swap usage */
	} memcg_batch;
#endif
```

​    （38）、futex同步机制 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_FUTEX  
2. ​    **struct robust_list_head __user \*robust_list;**  
3. \#ifdef CONFIG_COMPAT  
4. ​    **struct compat_robust_list_head __user \*compat_robust_list;**  
5. \#endif  
6. ​    **struct list_head pi_state_list;**  
7. ​    **struct futex_pi_state \*pi_state_cache;**  
8. \#endif  

```
#ifdef CONFIG_FUTEX
	struct robust_list_head __user *robust_list;
#ifdef CONFIG_COMPAT
	struct compat_robust_list_head __user *compat_robust_list;
#endif
	struct list_head pi_state_list;
	struct futex_pi_state *pi_state_cache;
#endif
```

​    （39）、非一致内存访问（NUMA  Non-Uniform Memory Access） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_NUMA  
2. ​    **struct mempolicy \*mempolicy;    /\* Protected by alloc_lock \*/**  
3. ​    short il_next;  
4. \#endif  

 

```
#ifdef CONFIG_NUMA
	struct mempolicy *mempolicy;	/* Protected by alloc_lock */
	short il_next;
#endif
```

​    （40）、文件系统互斥资源 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. atomic_t fs_excl;   /* holding fs exclusive resources */  

 

```
	atomic_t fs_excl;	/* holding fs exclusive resources */
```

​    （41）、RCU链表 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct rcu_head rcu;**  

 

```
	struct rcu_head rcu;
```

​    （42）、管道 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct pipe_inode_info \*splice_pipe;**  

 

```
	struct pipe_inode_info *splice_pipe;
```

​    （43）、延迟计数 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef  CONFIG_TASK_DELAY_ACCT  
2. ​    **struct task_delay_info \*delays;**  
3. \#endif  

 

```
#ifdef	CONFIG_TASK_DELAY_ACCT
	struct task_delay_info *delays;
#endif
```

​    （44）、fault injection，参考内核说明文件linux-2.6.38.8/Documentation/fault-injection/fault-injection.txt 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_FAULT_INJECTION  
2. ​    int make_it_fail;  
3. \#endif  

```
#ifdef CONFIG_FAULT_INJECTION
	int make_it_fail;
#endif
```

​    （45）、FLoating proportions 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct prop_local_single dirties;**  

 

```
	struct prop_local_single dirties;
```

​    （46）、Infrastructure for displayinglatency 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_LATENCYTOP  
2. ​    int latency_record_count;  
3. ​    **struct latency_record latency_record[LT_SAVECOUNT];**  
4. \#endif  

 

```
#ifdef CONFIG_LATENCYTOP
	int latency_record_count;
	struct latency_record latency_record[LT_SAVECOUNT];
#endif
```

​    （47）、time slack values，常用于poll和select函数 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. unsigned long timer_slack_ns;  
2. unsigned long default_timer_slack_ns;  

```
	unsigned long timer_slack_ns;
	unsigned long default_timer_slack_ns;
```

​    （48）、socket控制消息（control message） 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. **struct list_head    \*scm_work_list;**  

 

```
	struct list_head	*scm_work_list;
```

​    （49）、ftrace跟踪器 

**[cpp]** [view plain](http://blog.csdn.net/npy_lp/article/details/7335187#) [copy](http://blog.csdn.net/npy_lp/article/details/7335187#) [print](http://blog.csdn.net/npy_lp/article/details/7335187#)[?](http://blog.csdn.net/npy_lp/article/details/7335187#)

1. \#ifdef CONFIG_FUNCTION_GRAPH_TRACER  
2. ​    /* Index of current stored address in ret_stack */  
3. ​    int curr_ret_stack;  
4. ​    /* Stack of return addresses for return function tracing */  
5. ​    **struct ftrace_ret_stack \*ret_stack;**  
6. ​    /* time stamp for last schedule */  
7. ​    unsigned long long ftrace_timestamp;  
8. ​    /* 
9. ​     \* Number of functions that haven't been traced 
10. ​     \* because of depth overrun. 
11. ​     */  
12. ​    atomic_t trace_overrun;  
13. ​    /* Pause for the tracing */  
14. ​    atomic_t tracing_graph_pause;  
15. \#endif  
16. \#ifdef CONFIG_TRACING  
17. ​    /* state flags for use by tracers */  
18. ​    unsigned long trace;  
19. ​    /* bitmask of trace recursion */  
20. ​    unsigned long trace_recursion;  
21. \#endif /* CONFIG_TRACING */ 