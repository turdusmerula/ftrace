/**
  @file		Timing.cpp
  @author	S. Besombes
  @date		october 2011
  @version	$Revision: 1.1.1 $
  @brief	Time measurement functions.

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

#include <stdio.h>
#include "com/ThreadData.h"

pthread_mutex_t ThreadData::_globalMutex ;

std::list<ThreadData*>* ThreadData::_threads=NULL ;
__thread ThreadData* ThreadData::_threadData=NULL ;
size_t ThreadData::_threadCounter=0 ;

//-----------------------------------------------------------------------------
ThreadData::ThreadData()
{
	initLock() ;
}

//-----------------------------------------------------------------------------
void ThreadData::initLock()
{
	bool initGlobal=false ;

	//Init thread mutex
	pthread_mutex_init(&_mutex, NULL) ;

	if(initGlobal==false)
	{
		//Init global mutex
		pthread_mutex_init(&_globalMutex, NULL) ;

		initGlobal = true ;
	}
}

//-----------------------------------------------------------------------------
void ThreadData::lock()
{
    //printf("%ld lock\n", pthread_self()) ;
	pthread_mutex_lock(&_mutex) ;
}

//-----------------------------------------------------------------------------
void ThreadData::unlock()
{
    //printf("%ld unlock\n", pthread_self()) ;
	pthread_mutex_unlock(&_mutex) ;
}

//-----------------------------------------------------------------------------
void ThreadData::globalLock()
{
    //printf("%ld globalLock\n", pthread_self()) ;
	pthread_mutex_lock(&_globalMutex) ;
}

//-----------------------------------------------------------------------------
void ThreadData::globalUnlock()
{
    //printf("%ld globalUnlock\n", pthread_self()) ;
	pthread_mutex_unlock(&_globalMutex) ;
}
