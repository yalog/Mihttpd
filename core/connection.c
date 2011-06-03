/*
*	Mihttpd/core/connection.c
*
*	(c) 2011 yalog
*/
/*
*	这是在socket基础之上建立对，网络连接的一个连接池处理层
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "connection.h"
#include "mem_pool.h"

static pthread_mutex_t connections_free_lock;
static volatile connection_t *connections;	//已使用的连接
static volatile connection_t *connections_free;	//未使用的连接

/*
*	初始化连接池
*/
void connection_init()
{
	int n;
	connection_t *c;
	
	pthread_mutex_init(&connections_free_lock, 0);
	connections = NULL;
	connections_free = (connection_t *)malloc(MEM_ALIGN(sizeof(connection_t) * NCONNECTION)); //分配连接池
	c = connections_free;
	
	//链接预分配连接
	for (n = 0; n < NCONNECTION - 1; n++) {
		c->data = c + 1;
		c++;
	}
	c->data = NULL;
}

/*
*	获取一个空闲的连接
*/
connection_t * connection_get(socket_t s)
{
	connection_t *c;
	pthread_mutex_lock(&connections_free_lock);
	//获取一个connection
	if (connections_free == NULL) {
		c = NULL;
	}
	else {
		c = connections_free;
		connections_free = c->data;
	}
	pthread_mutex_unlock(&connections_free_lock);
	
	if (c != NULL) {
		c->read.data = c;
		c->write.data = c
		c->fd = s;
		c->pool = mem_pool_create();
		
		//清空event_t属性字段
	}
	else {
		return NULL;
	}
	
	return c;
}

/*
*	释放一个连接到连接池中
*/
void connection_free(connection_t *c)
{
	pthread_mutex_lock(&connections_free_lock);
	//释放一个connection
	c.data = connections_free;
	connections_free = c;
	pthread_mutex_unlock(&connections_free_lock);
}

/*
*	关闭一个连接，关闭操作包括关闭与连接相关的所有信息
*/
void connection_close(connnection_t *c)
{
}

/*
*	初始化一个http请求连接，即处理一个accpet()
*/
void http_init_connection(connection_t *c)
{
	//初始化一个连接信息，并将对应的事件添加到事件监听中
	
	log_debug("http_connection_init");
}