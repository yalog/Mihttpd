/*
*	Mihttpd/core/mem_pool.c
*
*	(c) 2011 yalog
*/
/*
*	这是对内存池技术的实现模块
*/

#include <stdlib.h>

#include "mem_pool.h"
#include "log.h"

#define MEM_POOL_SIZE 4096*4	//内存池创建时为16k

static mem_chunk_t *create_chunk(mem_size_t);

/*
*	创建一个内存池
*/
mem_pool_t *mem_pool_create()
{
	mem_chunk_t  *chunk;
	mem_pool_t *pool;
	
	chunk = (mem_chunk_t *)malloc(MEM_POOL_SIZE);
	if (chunk == NULL) {
		log_error(0, "malloc() memery use up");
		return NULL;
	}
	pool = (mem_pool_t *)((char *)chunk + sizeof(mem_chunk_t));
	
	chunk->size = MEM_POOL_SIZE;
	chunk->free_size = MEM_POOL_SIZE - sizeof(mem_pool_t) - sizeof(mem_chunk_t);
	chunk->next = NULL;
	chunk->first_avail = (char *)(pool) + sizeof(mem_pool_t);
	chunk->endp = (char *)(chunk) + MEM_POOL_SIZE - 1;
	
	pool->first = chunk;
	pool->last = chunk;
	pool->current = chunk;
	pthread_mutex_init(&pool->lock, 0);
	
	log_debug("memory pool created");
	
	return pool;
}

/*
*	销毁一个内存池
*/
void mem_pool_destroy(mem_pool_t *pool)
{
	mem_chunk_t *cur_chunk, *next_chunk;
	
	next_chunk = pool->first;
	do {
		cur_chunk = next_chunk;
		next_chunk = cur_chunk->next;
		
		free(cur_chunk);
	} while(next_chunk != NULL);
	
	pthread_mutex_destroy(&pool->lock);
}

/*
*	在指定内存池中分配内存
*/
void *mem_palloc(mem_pool_t *pool, mem_size_t size)
{
	//到底如何使用内存池的锁，再依情况而定，暂时不用锁
	mem_chunk_t *chunk = pool->current;
	void *mem_addr = NULL;
	
	if (size > chunk->free_size) {
		chunk = pool->first;
		while (chunk != NULL && size > chunk->free_size) {
			chunk = chunk->next;
		}
		
		if (chunk == NULL) {
			chunk = create_chunk(size);
			if (chunk == NULL) {
				return NULL;
			}
			pool->current = chunk;
			pool->last->next = chunk;
			pool->last = chunk;
		}
	}
	
	mem_addr = chunk->first_avail;
	chunk->first_avail += size;
	chunk->free_size -= size;
	
	return mem_addr;
}

/*
*	分配内存块
*/
static mem_chunk_t *create_chunk(mem_size_t size)
{
	mem_chunk_t *chunk;
	
	if (size + sizeof(mem_chunk_t) <= MEM_CHUNK_DEFAULT_SIZE) {
		size = MEM_CHUNK_DEFAULT_SIZE; //保证内存块的最小分配
	}
	else {
		size = MEM_ALIGN(size + sizeof(mem_chunk_t));
	}
	
	chunk = (mem_chunk_t *)malloc(size);
	if (chunk == NULL) {
		log_error(0, "malloc() memery use up");
		return NULL;
	}
	chunk->size = size;
	chunk->free_size = size - sizeof(mem_chunk_t);
	chunk->next = NULL;
	chunk->first_avail = (char *)chunk + sizeof(mem_chunk_t);
	chunk->endp = (char *)chunk + size - 1;
	
	log_debug("memory chunk alloced");
	return chunk;
}