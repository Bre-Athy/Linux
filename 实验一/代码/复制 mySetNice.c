/*syscall_64.tbl
 * 333 64 mySetNice __x64_sys_mySetNice
 */

/*unistd.c
 * #define NR_mySetNice 333
 * __SYSCALL(__NR_mySetNice,sys_mySetNice)
 */

#sys.c
SYSCALL_DEFINE5(setnice,pid_t,pid,int,flag,int,nicevalue,void __user *,prio,void __user *,nice)
{
	int mprio=0;
	int mnice=0;
	int error=0;
	struct task_struct *p;
	printk("SetNice Start!\n");
	for(p=&init_task;(p=next_task(p))!=&init_task;)
	{
		printk("This is For CYCLE!   ");
		if(p->pid==pid)
		{
			printk("Pointer GO!");
			if(flag==0)
			{
				printk("Flag == 0\n");
				mprio=task_prio(p);
				mnice=task_nice(p);
				copy_to_user(prio,&mprio,sizeof(task_prio(p)));
				copy_to_user(nice,&mnice,sizeof(task_nice(p)));
			}else if(flag==1)
			{
				printk("Flag == 1\n");
				set_user_nice(p,nicevalue);
				mprio=task_prio(p);
				mnice=task_nice(p);
				copy_to_user(nice,&mnice,sizeof(task_nice(p)));
				copy_to_user(prio,&mprio,sizeof(task_prio(p)));
			}else
			{
				printk("Flag == Nothing\n");
				error=-EFAULT;
			}
			printk("Pointer END!  \n");
			return error;
		}
	}
	printk("The system calls	BUG!!!!!!!\n	");
	error=-EFAULT;
	return error;
}

