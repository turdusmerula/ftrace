/**
 @file		Memory.cpp
 @author	S. Besombes
 @date		Mars 2015
 @version	$Revision: 1.1.1 $
 @brief	Demangling functionalities

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
 MA 02110-1301, USA.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/**
* Indicates if we are in an ftrace code section.
*/
extern thread_local bool threadFtraceFlag ;

static void* (*real_malloc)(size_t)=NULL ;
static void (*real_free)(void *__ptr)=NULL ;
static void* (*real_calloc)(size_t __nmemb, size_t __size) ;
static void* (*real_realloc)(void *__ptr, size_t __size) ;
static void* (*real_memalign)(size_t __alignment, size_t __size) ;
static void* (*real_valloc)(size_t __size) ;
static void* (*real_pvalloc)(size_t __size) ;

static void mtrace_init(void)
{
    real_malloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc") ;
    if(real_malloc==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

    real_free = (void (*)(void*))dlsym(RTLD_NEXT, "free") ;
    if(real_free==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

//    real_calloc = (void* (*)(size_t, size_t))dlsym(RTLD_NEXT, "calloc") ;
//    if(real_calloc==NULL)
//    {
//        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
//        exit(1) ;
//    }

    real_realloc = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc") ;
    if(real_realloc==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

    real_memalign = (void* (*)(size_t, size_t))dlsym(RTLD_NEXT, "memalign") ;
    if(real_memalign==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

    real_valloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "valloc") ;
    if(real_valloc==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

    real_pvalloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "pvalloc") ;
    if(real_pvalloc==NULL)
    {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror()) ;
        exit(1) ;
    }

}

void* malloc(size_t size)
{
    void *result=NULL ;

    if(real_malloc==NULL)
        mtrace_init() ;

    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        result = real_malloc(size) ;
    }
    else
    {
        //TODO: add memory statistics
        result = real_malloc(size) ;
    }

    return result ;
}

void free(void* __ptr)
{
    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        real_free(__ptr) ;
    }
    else
    {
        //TODO: add memory statistics
        real_free(__ptr) ;
    }
}

//void* calloc(size_t __nmemb, size_t __size)
//{
//    void *result=NULL ;
//
//    if(threadFtraceFlag)
//    {
//        // we are in instrument so do not add stats for memory here
//        result = real_calloc(__nmemb, __size) ;
//    }
//    else
//    {
//        //TODO: add memory statistics
//        result = real_calloc(__nmemb, __size) ;
//    }
//
//    return result ;
//}
//
void* realloc(void *__ptr, size_t __size)
{
    void *result=NULL ;

    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        result = real_realloc(__ptr, __size) ;
    }
    else
    {
        //TODO: add memory statistics
        result = real_realloc(__ptr, __size) ;
    }

    return result ;
}

void* memalign(size_t __alignment, size_t __size)
{
    void *result=NULL ;

    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        result = real_memalign(__alignment, __size) ;
    }
    else
    {
        //TODO: add memory statistics
        result = real_memalign(__alignment, __size) ;
    }

    return result ;
}

void* valloc(size_t __size)
{
    void *result=NULL ;

    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        result = real_valloc(__size) ;
    }
    else
    {
        //TODO: add memory statistics
        result = real_valloc(__size) ;
    }

    return result ;
}

void* pvalloc(size_t __size)
{
    void *result=NULL ;

    if(threadFtraceFlag)
    {
        // we are in instrument so do not add stats for memory here
        result = real_pvalloc(__size) ;
    }
    else
    {
        //TODO: add memory statistics
        result = real_pvalloc(__size) ;
    }

    return result ;
}

