#include<linux/unistd.h>
#include<sys/syscall.h>
#include<stdio.h>

int main()
{
	int nice = 0;
	int prio = 0;
	int flag = 0;
	int pid = 0;
	int nicevalue = 0;
	printf("请输入 PID:");
	scanf("%d",&pid);
	printf("要改变这个进程的 nice 值吗? 是(1) , 否(0)\n");
	printf("请输入:");
	scanf("%d",&flag);
	if(flag==0)
	{
		if(syscall(335,pid,0,0,&prio,&nice)==0)
		{
			printf("当前 nice :%d,当前  prio :%d",nice,prio);
			return 0;
		}else
		{
			printf("\n系统调用错误，请检查系统调用！\n\n");
			return -1;
		}
	}else if(flag==1)
	{
		printf("请输入一个 nice 值:\n");
		scanf("%d",&nicevalue);
		syscall(335,pid,0,0,&prio,&nice);
		printf("\n\n原来的 nice 值:%d\n",nice);
		syscall(335,pid,1,nicevalue,&prio,&nice);
		printf("当前的 nice 值:%d\n\n\n",nice);
		return 0;
	}else
	{
		printf("输入无效！\n");
		return 0;
	}
}
