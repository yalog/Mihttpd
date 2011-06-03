/*
*	Mihttpd/core/thread_pool.c
*
*	(c) 2011 yalog
*/
/*
*	这个模块主要实现对线程池的自动管理和维护功能
*/
#include <semaphore.h>
#include <pthread.h>

#include "event.h"
#include "log.h"

#define MAX_THREAD_NUM 8	//达到工作线程上线时就抛出系统忙
#define MIN_THREAD_NUM 4

static sem_t posted_event_num;//标志事件的信号量

static pthread_t *workers_free;	//这里用于动态的控制工作线程数目，暂时还未实现这个功能
static pthread_t *workers;

/*
*	通过信号量的方式，分派工作任务给工作线程
*/
void thread_dispatch()
{
	sem_post(&posted_event_num);
}

/*
*	工作线程
*/
static void *worker(void *data)
{
	event_t *ev;
	
	for (;;) {	//循环处理事件
		sem_wait(&posted_event_num);	//没有需要处理的事件时，就等待
		log_debug("process a new event");
	
		ev = event_posted_get();
		if (ev->active) {
			ev.handler(ev);			
		}
	}
}

/*
*	 初始化线程池
*/
void thread_pool_init()
{
	int n, err;
	pthread_t pid;
	
	sem_init(&posted_event_num, 0, 0);	//系统启动时需要处理的事件任务为0
	
	//创建固定数目的线程池
	for (n = 0; n < MIN_THREAD_NUM; n++) {
		if ((err = pthread_create(&pid, NULL, worker, NULL)) != 0) {
			log_error(err, "pthread_create()");
		}
	}
}