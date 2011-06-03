/*
*	Mihttpd/core/log.h
*
*	(c) 2011 yalog
*/

#ifndef _LOG_H_
#define _LOG_H_

#define log_error(error_no, msg) _log_error(error_no, msg, __FILE__, __LINE__)
#define log_debug(msg) _log_debug(msg, __FILE__, __LINE__)

void _log_error(int error_no, char *msg, char *filename, int line);
void _log_debug(char *msg, char *filename,int line);
void log_access(char *msg);

#endif