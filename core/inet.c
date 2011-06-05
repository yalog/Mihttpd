/*
*	Mihttpd/core/inet.c
*
*	(c) 2011 yalog
*/
/*
*	负责直接的socket网络层的处理，并建立一些抽象接口
*/

#include <sys/fcntl.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#include "inet.h"
#include "../include/types.h"
#include "log.h"
#include "connection.h"

/*
*	初始化socket
*/
/*
*	设置套接字为非阻塞
*/
int setnonblocking(socket_t s)
{
	int flags;
	
	flags = fcntl(s, F_GETFL);
	flags |= O_NONBLOCK;
	
	return fcntl(s, F_SETFL, flags);
}

/*
*	初始化网络
*/
int inet_init()
{
	socket_t s;
	int buf_size = 8*1024;
	int reuse = 1;
	
	if ((listening.s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(errno, "socket()");
		exit(1);
	}
	
	if (setsockopt(listening.s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1 || 
		setsockopt(listening.s, SOL_SOCKET, SO_REUSEADDR, &buf_size, sizeof(buf_size)) == -1) {
		log_error(errno, "setsockopt()");
		exit(1);
	}
	
	bzero(&listening.ls_addr, sizeof(listening.ls_addr));
	listening.ls_addr.sin_family = AF_INET;
	listening.ls_addr.sin_port = htons(LISTEN_PORT);
	listening.ls_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listening.s, (struct sockaddr *)&listening.ls_addr, sizeof(listening.ls_addr)) == -1) {
		log_error(errno, "bind()");
		exit(1);
	}
	
	if (listen(listening.s, LISTEN_BACKLOG) == -1) {
		log_error(errno, "listen()");
		exit(1);
	}
	
	setnonblocking(listening.s);
	
	//注册一个accept()后的处理钩子
	//listening.handler = http_connection_init;
}