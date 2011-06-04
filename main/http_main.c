/*
* 	Mihttpd/main/http_main.c
*
*	(c) 2011  yalog 
*/
/*
*	这个是关于整个服务器启动和初始化相关操作的文件
*/
#include <unistd.h>
#include <stdio.h>


#define WORK_DIR "/var/www/"

/*
*	整个系统的入口函数;
*	暂时还没有想好处理什么启动参数;
*/
int main(int agrc, char *agrv[])
{
	int pid;
	
	//创建守护进程
	pid = fork();
	if (pid > 0) {
		exit(0);
	}
	else if (pid < 0) {
		printf("fork() Mihttpd start abort\n");
		exit(1);
	}
	setsid();
	
	//底层初始化
	
	
	return 0;
}