/*
*	Mihttpd/core/connection.h
*
*	(c) 2011 yalog
*/

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "event.h"
#include "mem_pool.h"

#define NCONNECTION 1024*4

typedef struct connection_s {
	struct event_s *read;	//关联读事件
	struct event_s *write;	//关联写事件
	int fd;	//创建的连接socket 文件描述符
	//关联监听信息
	mem_pool_t *pool;	//关联内存池
	void *data; //关联数据
}connection_t;

void connection_init();
connection_t * connection_get();
void connection_free(connection_t *c);
void http_init_connection(connection_t *);

#endif