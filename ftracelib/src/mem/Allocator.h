#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "mem/Memory.h"

/**
 * This is the most simple memory allocator (and maybe not very performant).
 * It consist in a simple list of slots (one ore more pages), each slot containing
 * a linked list of blocks.
 */
class Allocator
{
public:
	Allocator() {} ;
	virtual ~Allocator()=0 ;

	/*
	 * initialize sets up the memory allocation arena and the run-time
	 * configuration information.
	 */
	static void initialize(void) ;


	virtual void* memalign(size_t alignment, size_t userSize)=0 ;

	virtual void free(void* address)=0 ;

	virtual void* realloc(void* oldBuffer, size_t newSize)=0 ;

	virtual void* malloc(size_t size)=0 ;

	virtual void* calloc(size_t nelem, size_t elsize)=0 ;

	virtual void* valloc (size_t size)=0 ;
};


#endif
