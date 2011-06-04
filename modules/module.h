/*
* 	Mihttpd/modules/module.h
*
*	(c) 2011  yalog 
*/
/*
*	这个文件主要用来注册http扩展处理模块
*/
#include ""

struct module {
	void (* handler)(request_t *); //模块的处理挂载钩子
};

//这里注册模块挂载表
struct module modules[] = {
	&handler_cgi,
	NULL
};