#include "mem/AllocatorSystem.h"

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>

void AllocatorSystem::initialize(void)
{
}

void* AllocatorSystem::memalign(size_t alignment, size_t userSize)
{
	return ::memalign(alignment, userSize) ;
}

void AllocatorSystem::free(void* address)
{
	::free(address) ;
}

void* AllocatorSystem::realloc(void* oldBuffer, size_t newSize)
{
	return ::realloc(oldBuffer, newSize) ;
}

void* AllocatorSystem::malloc(size_t size)
{
	return ::malloc(size) ;
}

void* AllocatorSystem::calloc(size_t nelem, size_t elsize)
{
	return ::calloc(nelem, elsize) ;
}

void* AllocatorSystem::valloc (size_t size)
{
	return ::valloc(size) ;
}

