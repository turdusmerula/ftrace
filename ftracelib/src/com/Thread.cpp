#include "com/Thread.h"
#include <pthread.h>
#include <unistd.h>

/*
 * mutex to enable multithreaded operation
 */
static pthread_mutex_t mutex ;
static pid_t mutexpid=0 ;
static int locknr=0 ;

void initLock()
{
	pthread_mutex_init(&mutex, NULL) ;
}

void lock()
{
    if(pthread_mutex_trylock(&mutex))
    {
       if(mutexpid==getpid())
       {
           locknr++ ;
           return ;
       }
       else
           pthread_mutex_lock(&mutex) ;
    }
    mutexpid = getpid() ;
    locknr = 1 ;
}

void unlock()
{
    locknr-- ;
    if(!locknr)
    {
       mutexpid = 0 ;
       pthread_mutex_unlock(&mutex) ;
    }
}
