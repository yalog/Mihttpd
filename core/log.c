/*
*	Mihttpd/core/log.c
*
*	(c) 2011 yalog
*/
/*
*	这个文件主要关于日志处理的实现
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define DEBUG 0
#define ERROR 1
#define LOG_LEVEL DEBUG

//这是日志文件的配置
//#define error_log "/var/log/mihttpd_error"
//#define debug_log "/var/log/mihttpd_debug"
//#define access_log "/var/log/mihttpd_access"

#define error_log "./mihttpd_error"
#define debug_log "./mihttpd_debug"
#define access_log "./mihttpd_access"

FILE *mi_error;
FILE *mi_debug;
FILE *mi_access;

/*
*	初始化日志处理
*	将标准输入、输出和错误重定向到文件
*/
void log_init()
{
	struct stat s;
	int fd;
	
	if (stat(error_log, &s) == -1) {
		if (errno == ENOENT) {	//文件不存在
			if ((mi_error = fopen(error_log, "w")) == NULL) {
				printf("error_log init failure\n");
				exit(1);
			}
		}
	}
	else {
		if ((mi_error = fopen(error_log, "a")) == NULL) {
			printf("error_log init failure\n");
			exit(1);
		}
	}
	
	if (stat(access_log, &s) == -1) {
		if (errno == ENOENT) {	//文件不存在
			if ((mi_access = fopen(access_log, "w")) == NULL) {
				printf("access_log init failure\n");
				exit(1);
			}
		}
	}
	else {
		if ((mi_access = fopen(access_log, "a")) == NULL) {
			printf("access_log init failure\n");
			exit(1);
		}
	}
	
	if (LOG_LEVEL == DEBUG ) {
		if (stat(debug_log, &s) == -1) {
			if (errno == ENOENT) {	//文件不存在，创建
				if ((mi_debug = fopen(debug_log, "w")) == NULL) {
					printf("debug_log init failure\n");
					exit(1);
				}
			}
		}
		else {
			if ((mi_debug = fopen(debug_log, "a")) == NULL) {
				printf("debug_log init failure\n");
				exit(1);
			}
		}
	}

	fd = open(error_log, O_APPEND);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
}

/*
*	usage: _log_error( errno, "error", __FILE__,  __LINE__)
*/
void _log_error(int error_no, char *msg, char *filename, int line)
{
	fprintf(mi_error, "%s:%d errno(%d) %s\n", filename, line, error_no, msg);
}

void _log_debug(char *msg, char *filename,int line)
{
	if (LOG_LEVEL == DEBUG) {
		fprintf(mi_debug, "%s:%d %s\n", filename, line, msg);
	}
}

void log_access(char *msg)
{
	//记录访问时间
	fprintf(mi_access, "2011-6-2 %s\n", msg);
}