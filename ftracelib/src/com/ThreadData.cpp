/**
  @file		ThreadData.cpp
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
#include "log/Logger.h"


using namespace ftrace ;

//-----------------------------------------------------------------------------
pthread_mutex_t ThreadData::globalMutex_ ;
std::list<ThreadData*>* ThreadData::threads_=nullptr ;
size_t ThreadData::count_=0 ;
thread_local ThreadData* ftrace::threadData=nullptr ;
static thread_local ThreadLiveSpy threadSpy ;
//-----------------------------------------------------------------------------
ThreadData::ThreadData()
    : endTime_(0)
{
	initLock() ;

	startTime_ = Timing::getTime() ;

    globalLock() ;
    if(threads_==nullptr)
        threads_ = new std::list<ThreadData*> ;
    threads_->push_back(this) ;
    number_ = count_ ;
    count_++ ;
    globalUnlock() ;

    id_ = pthread_self() ;
    scopes_ = new std::map<void*,  Scope*> ;
    scopeStack_ = new std::stack<Scope*> ;

    rootScope_ = new Scope(nullptr) ;
    scopeStack_->push(rootScope_) ;
    currScope_ = rootScope_ ;
}

//-----------------------------------------------------------------------------
void ThreadData::initLock()
{
	bool initGlobal=false ;

	//Init thread mutex
	pthread_mutex_init(&mutex_, NULL) ;

	if(initGlobal==false)
	{
		//Init global mutex
		pthread_mutex_init(&globalMutex_, NULL) ;

		initGlobal = true ;
	}
}

//-----------------------------------------------------------------------------
void ThreadData::lock()
{
    //printf("%ld lock\n", pthread_self()) ;
	pthread_mutex_lock(&mutex_) ;
}

//-----------------------------------------------------------------------------
void ThreadData::unlock()
{
    //printf("%ld unlock\n", pthread_self()) ;
	pthread_mutex_unlock(&mutex_) ;
}

//-----------------------------------------------------------------------------
void ThreadData::globalLock()
{
    //printf("%ld globalLock\n", pthread_self()) ;
	pthread_mutex_lock(&globalMutex_) ;
}

//-----------------------------------------------------------------------------
void ThreadData::globalUnlock()
{
    //printf("%ld globalUnlock\n", pthread_self()) ;
	pthread_mutex_unlock(&globalMutex_) ;
}

//-----------------------------------------------------------------------------
ThreadLiveSpy::ThreadLiveSpy()
{
}

//-----------------------------------------------------------------------------
ThreadLiveSpy::~ThreadLiveSpy()
{
    // the destructor is called when thread is dying
    // add thread destruction time
    threadData->endTime_ = Timing::getTime() ;
}

