#include "mem/Memory.h"
#include "com/Thread.h"
#include "com/Config.h"
#include "mem/Page.h"
#include "mem/Allocator.h"

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <mcheck.h>


//Global variable used to deactivate the memory callback
bool SystemAllocator=true ;
Allocator* allocator=NULL ;

//extern "C"
//{
//void* malloc(size_t size)
//	__attribute__((no_instrument_function)) ;
//
//}
//
//extern "C"
//void free(void* address)
//{
//	lock() ;
//
//	//Print("--- free ---\n") ;
//
//	if(address==0)
//    {
//    	unlock() ;
//		return ;
//    }
//
//    allocator->free(address) ;
//	unlock() ;
//}
//
//extern "C"
//void* realloc(void* oldBuffer, size_t newSize)
//{
//	void* res ;
//
//	lock() ;
//	//Print("--- realloc ---\n") ;
//	res = allocator->realloc(oldBuffer, newSize) ;
//	unlock() ;
//
//	return res ;
//}
//
//extern "C"
//void* malloc(size_t size)
//{
//	void* res ;
//
//	lock() ;
//	//Print("--- malloc ---\n") ;
//	res = allocator->malloc(size) ;
//	unlock() ;
//
//	return res ;
//}
//
//extern "C"
//void* calloc(size_t nelem, size_t elsize)
//{
//	void* res ;
//
//	lock() ;
//	//Print("--- calloc ---\n") ;
//	res = allocator->calloc(nelem, elsize) ;
//	unlock() ;
//
//	return res ;
//}
//
//extern "C"
//void* valloc (size_t size)
//{
//	void* res ;
//
//	lock() ;
//	//Print("--- valloc ---\n") ;
//	res = allocator->valloc(size) ;
//	unlock() ;
//
//	return res ;
//}
