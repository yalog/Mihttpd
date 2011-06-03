/*
*	Mihttpd/core/event.h
*
*	(c) 2011 yalog
*/
#ifndef _EVENT_H_
#define _EVENT_H_

#include <pthread.h>

#define EVENT_SOCKET_ACCEPT 0
#define EVENT_SOCKET_READ 1
#define EVENT_SOCKET_WRITE 2
#define EVENT_PIPE_READ 3
#define EVENT_PIPE_WRITE 4
#define EVENT_TIME_UP 5

typedef struct event_s{
	int type;	//事件类型
	void *data;	//通常与一些数据关联起来，比如connection_t 、http_request等
	int fd;	//与这个事件关联的文件描述
	int active;	//标志这个事件是不是活跃
	int posted; //标志是否放置在事件队列中
	void (* handler)(event_t *);	//事件处理句柄
	struct event_s *pre;	//事件链接，至于双向链表，是为了快速删除一个事件队列中的事件
	struct event_s *next;
}event_t;

struct event_queue_s {
	event_t *head;
	event_t *tail;
	pthread_mutex_t lock;
};

void event_init(connection_t *c);
event_t *event_posted_get();
void event_process();
int event_add(event_t *ev, int event);
int event_del(event_t *ev, int event);
void event_posted_add(event_t *ev);
void event_posted_del(event_t *ev);

#endif