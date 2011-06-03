/*
*	Mihttpd/core/inet.h
*
*	(c) 2011 yalog
*/

#ifndef _INET_H_
#define _INET_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include "event.h"

//这些东西其实应该由配置文件来决定的，但是为了集中我们这次的目的，将配置都以宏的形式硬编码
#define LISTEN_PORT 80
#define LISTEN_BACKLOG 20
 
typedef int socket_t;

typedef struct {
	socket_t s;
	struct sockaddr_in ls_addr;
	void (* handler)(connection_t *ev);	//accept后的处理钩子
}listen_t;

listen_t listening;

#endif