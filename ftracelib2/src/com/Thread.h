#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <unistd.h>

void lock() ;
void unlock() ;
void initLock() ;

struct ThreadData
{
	/**
     * Thread identifier.
     */
    pthread_t _threadId ;

    /**
     * Thread number.
     */
    size_t _threadNumber ;
} ;

#endif
