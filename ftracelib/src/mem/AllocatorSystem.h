#ifndef __ALLOCATORSYSTEM_H__
#define __ALLOCATORSYSTEM_H__

#include "mem/Allocator.h"
#include "mem/Memory.h"

/**
 * This is the most simple memory allocator (and maybe not very performant).
 * It consist in a simple list of slots (one ore more pages), each slot containing
 * a linked list of blocks.
 */
class AllocatorSystem
{
protected:
public:
	AllocatorSystem() {} ;
	virtual ~AllocatorSystem() {} ;

	/*
	 * initialize sets up the memory allocation arena and the run-time
	 * configuration information.
	 */
	static void initialize(void) ;


	virtual void* memalign(size_t alignment, size_t userSize) ;

	virtual void free(void* address) ;

	virtual void* realloc(void* oldBuffer, size_t newSize) ;

	virtual void* malloc(size_t size) ;

	virtual void* calloc(size_t nelem, size_t elsize) ;

	virtual void* valloc (size_t size) ;
};


#endif
