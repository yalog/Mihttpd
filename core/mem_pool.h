/*
*	Mihttpd/core/mem_pool.h
*
*	(c) 2011 yalog
*/

#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#include <pthread.h>

#define MEM_CHUNK_DEFAULT_SIZE (4 * 4096) //默认内存池块的大小
#define MEM_CHUNK_MAX_SIZE (20 * 4096)	//最大内存块的大小，超过次大小的块

//生成按要求对齐的内存块大小
#define ALIGN(size, boundary)\
	((size) + ((boundary) - 1)) & ~((boundary) - 1)
	
#define MEM_ALIGN(size) ALIGN(size, 4096)	//按4k对齐

typedef int mem_size_t;

//内存池的分配的基本分配单元，内存块
typedef struct mem_chunk_s{
	mem_size_t size;	//块的尺寸
	mem_size_t free_size;	//块剩余的
	struct mem_chunk_s *next;
	char *first_avail;	//标志可以用内存的起始位置
	char *endp;	//标志可以用内存的结束位置
}mem_chunk_t;

typedef struct mem_pool_s{
	mem_chunk_t *first;	//第一个分配内存块
	mem_chunk_t *last;	//最新分配内存块
	mem_chunk_t *current;	//当前可分配的内存块
	pthread_mutex_t lock;	//多线程操作内存池的锁
} mem_pool_t;

mem_pool_t *mem_pool_create();
void mem_pool_destroy(mem_pool_t *pool);
void *mem_palloc(mem_pool_t *pool, mem_size_t size);

#endif