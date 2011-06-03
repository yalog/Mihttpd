/*
*	Mihttpd/core/file_cache.c
*
*	(c) 2011 yalog
*/
/*
*	实现整个文件或者是文件块(用rang来实现)的缓存底层支持，而不是系统提供的缓冲机制，能够在文件关闭后依然存在
*/

#include "../include/types.h"
#include "file_cache.h"

#define AVG_FILE_SIZE 11000	//平均文件大小，初步统计平均文件大小为11k，这个参数可以根据实际网站的统计来设置
#define CACHE_FACTOR 1000	//缓存因子，用于判断一个文件是否足够不经常被访问，时间/次数
/*
*	负责文件缓冲区的初始化操作
*/
void file_cache_init(int size_MB)
{
}

/*
*	判断一个文件是否已经缓存了
*/
bool_t	is_cached(const char *filename)
{
}

/*
*	返回文件缓存信息
*	参数返回缓存信息
*/
void file_cache_info(const char *filename, cache_info *cinfo)
{
}
/*
*	尝试文件缓存,成功返回缓存区首地址，失败返回NULL
*	这个函数有一个缓存算法，只有满足缓存条件的才会缓存
*/
void * try_cache(const char *filename, int file_size)
{
}

/*
*	缓存交换，当缓存区满后交换缓存区内容
*	这个函数实现的一个缓存替换算法
*/
static void swap_file_cache(const char *filename, int file_size)
{
}

/*
*	判断一个文件缓存是否过期
*/
static void bool_t is_old(const char *filename)
{
}