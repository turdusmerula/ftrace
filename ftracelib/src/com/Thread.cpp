/**
@file Thread.cpp
@author S. Besombes
@date january 2010
@version $Revision: 1.1.1 $
@brief Demangling functionalities

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
MA 02110-1301, USA.
*/


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
