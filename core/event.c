/*
*	Mihttpd/core/event.c
*
*	(c) 2011 yalog
*/
/*
*	这是基于epoll事件触发之上建立起来的一个事件处理模块
*/

#include <sys/epoll.h>
#include <stdlib.h>
#include <errno.h>

#include "event.h"
#include "connection.h"
#include "inet.h"
#include "log.h"
#include "thread_pool.h"
#include "../include/types.h"

//配置
#define NEVENTS 20	//最大返回监听量
#define TIMEOUT 100 //超时时间，毫秒

static struct epoll_event event_list[NEVENTS];
static int epoll_fd;

static struct event_queue_s posted_events;	//待处理的事件队列
static event_t read_event[NCONNECTION];	//预分配的读写事件
static event_t wirte_event[NCONNECTION];

/*
*	事件处理模块初始化，事件队列初始化
*/
void event_init(connection_t *c_free)
{
	int n;
	struct epoll_event ev;
	
	//初始化事件队列
	posted_events.head = NULL;
	posted_events.tail = NULL;
	pthread_mutex_init(&posted_events.lock, 0);
	
	for	(n = 0; n < NCONNECTION; n++) {
		c_free[n].read = &read_event[n];
		c_free[n].wirte = &wirte_event[n];
	}
	
	epoll_fd = epoll_create(NEVENTS);
	//注册连接请求事件
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = &listening;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listening.s, &ev) == -1) {
		log_error(errno, "epoll_ctl()");
		exit(0);
	}
}

/*
*	添加一个事件监听
*/
int event_add(event_t *ev, int event)
{
	connection_t *c;
	int pre_event, op;
	event_t *e;
	struct epoll_event ee;
	
	c = ev->data;
	if (event == EPOLLIN) {	//如果epoll event已经添加过就需要，修改
		pre_event = EPOLLOUT;
		e = c->write;
	}
	else {
		pre_event = EPOLLIN;
		e = c->read;
	}
	
	if (e->active) {
		op = EPOLL_CTL_MOD;
		event |= pre_event;
	}
	else {
		op = EPOLL_CTL_ADD;
	}
	
	ee.events = event;
	ee.data.ptr = c;
	
	log_debug("epoll event add");
	
	if (epoll_wait(epoll_fd, op, c->fd, &ee) == -1) {
		log_error(errno, "epoll_wait() add");
		return ERROR;
	}
	
	ev->active = 1;
	
	return OK;
}

/*
*	删除一个事件监听
*/
int event_del(event_t *ev, int event)
{
	connection_t *c;
	event_t *e;
	struct epoll_event ee;
	int op;
	
	c = ev->data;
	if (event == EPOLLIN) {
		e = c->write;
		pre_event = EPOLLOUT
	}
	else {
		e = c->read;
		pre_event = EPOLLIN;
	}
	
	if (e->active) {
		op = EPOLL_CTL_MOD;
		ee.events = pre_event;
		ee.data.ptr = c;
	}
	else {
		op = EPOLL_CTL_DEL;
		ee.events = 0;
		ee.data.ptr = NULL;
	}
	
	log_debug("epoll event add");
	
	if (epoll_ctl(epoll_fd, op, c->fd, &ee) == -1) {
		log_error(errno, "epoll_ctl() del");
		return ERROR;
	}
	
	ev->active = 0;
	
	return OK;
}

/*
*	添加一个事件到事件队列，入队
*/
void event_posted_add(event_t *ev)
{
	pthread_mutex_lock(&posted_events.lock);
	if (posted_events.tail == NULL) {
		posted_events.head = ev;
		ev->pre = NULL;	//保证队列首尾元素pre 和next指向NULL
	}
	else {
		ev->pre = posted_event.tail;
		posted_event.tail->next = ev;		
	}
	ev->posted = 1;
	ev->next = NULL;
	posted_event.tail = ev;
	pthread_mutex_unlock(&posted_events.lock);
}

/*
*	从事件队列中删除一个事件，
*/
void event_posted_del(event_t *ev)
{
	log_debug("delete a event from event queue");
	pthread_mutex_lock(&posted_events.lock);
	//这里需要双向链表，加速操作
	if (ev->posted) {
		if (posted_events.head != ev && posted_events.tail != ev) {
			ev->pre->next = ev->next;
			ev->next->pre = ev->pre;
		}
		else {
			if (posted_events.head == ev) {	//如果ev是头结点
				posted_events.head  = ev->next; 
			}
			
			if (posted_events.tail == ev) {
				poted_events.tail = ev->pre;
			}
		}
		ev->posted = 0;
	}
	pthread_mutex_unlock(&posted_events.lock);
}

/*
*	从事件队列中获取一个事件，用于工作线程处理，出队
*	通过锁保证线程安全
*	(可以考虑将其删除，或者对一些字段设置成为原子类型)
*/
event_t *event_posted_get()
{
	event_t *ev;
	//加锁
	pthread_mutex_lock(&posted_events.lock);
	if (posted_events.head == NULL) {
		ev = NULL;
	}
	else {
		ev = posted_events.head;
		posted_events.head = ev->next;
		if (posted_events.head == NULL) {
			posted_events.tail = NULL;
		}
		else {
			posted_events.head->pre = NULL;
		}
	}
	//解锁
	pthread_mutex_unlock(&posted_events.lock);
	
	ev->posted = 0;
	return ev;
}

/*
*	事件循环处理
*/
void event_process()
{
	int i, n, fd;
	connection_t *c;
	event_t *ev;
	
	//事件监听循环
	for (;;) {
		if ((n = epoll_wait(epoll_fd, event_list, NEVENTS, TIMEOUT)) == -1) {
			if (errno == EINTR) {
				continue;
			}
			else {
				log_error(errno, "epoll_wait()");
				exit(1);
			}
		}
		
		for (i = 0; i < n; i++) {
			if (event_list[i].data.ptr == &listening) {
				//处理所有的accept
				do {
					fd = accept(listening.s, (struct sockaddr *)&listening.ls_addr, sizeof(listening.ls_addr));
					if (fd == -1 && errno == EAGAIN) {
						break;
					}
					
					c = connection_get(fd);
					if (c == NULL) { //当连接数过多时，丢弃
						break;
						close(fd);
						log_debug("connection overload and discard");
					}
					else {
						http_init_connection(c); //初始化一个HTTP连接，功能的衔接点(link)
						log_access("connection reached");
					}
				}while(1);
			}
			else {
				//将发生的事件，放入事件队列中
				log_debug("A event Occurred");
				c = event_list[i].data.ptr;
				//在这里debug一下events的值，这里可以对event增加一个ready字段，发生事件后就标志，处理完后就取消
				if ((event_list[i].events & EPOLLIN) && c->read.active) {
					event_posted_add(c.read);
				}
				
				if ((event_list[i].events & EPOLLOUT) && c->write.active) {
					event_posted_add(c.write);
				}
				//给工作线程分派事件，处理
				thread_dispatch();
			}
		}	
	}
}