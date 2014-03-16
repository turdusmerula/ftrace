/**
  @file		ftrace.cpp
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.0.0 $
  @brief	Function tracing tool for gcc

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
#include <fcntl.h>
#include <unistd.h>

#include <ftrace.h>

#include <SharedObjects.h>

using namespace boost::interprocess ;

//TODO: intercept create/close thread

bool init=false ;

//Pipe for sending commands to monitoring process
static int pipefd=-1 ;	//will be <0 if pipe is not opened
static managed_shared_memory shm ;

//Shared memory blocks
//Process data
static ProcessData* processData=nullptr ;

//Current data for thread.
static __thread ThreadData* threadData=nullptr ;

char* getShmName()
	__attribute__((no_instrument_function)) ;

ScopeData* createScopeData(void* scopeptr)
	__attribute__((no_instrument_function)) ;

Scope* createScope(void* scopeptr)
	__attribute__((no_instrument_function)) ;

//----------------------------------------------------------------------------
/**
* Prototypes
*/
extern "C"
{
//Functions defined by gcc and to be overriden to catch entry and exit from functions
void __cyg_profile_func_enter(void *this_fn, void *call_site)
    __attribute__((used))
    __attribute__((no_instrument_function)) ;

void __cyg_profile_func_exit(void *this_fn, void *call_site)
	__attribute__((used))
	__attribute__((no_instrument_function)) ;
}

void __cyg_profile_func_init()
     __attribute__((no_instrument_function)) ;

void ftrace_init(void)
	__attribute__ ((constructor))
	__attribute__((no_instrument_function)) ;
void ftrace_unload(void)
	__attribute__ ((destructor))
	__attribute__((no_instrument_function)) ;

void sendCommand(int id, void* command, size_t size)
	__attribute__((no_instrument_function)) ;

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
* Gcc internal entry function.
*/
extern "C"
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	if(init==false)
	{
		__cyg_profile_func_init() ;
		init = true ;
	}

	if(processData==nullptr)
		return ;

	//Create thread data for the thread
	if(threadData==nullptr)
	{
		//TODO: take process mutex
		//Create thread data
		processData->threads->push_back(ThreadData()) ;

		//Initialize local thread static thread data
		threadData = &processData->threads->back() ;
		threadData->threadId = pthread_self() ;
		threadData->currScope = nullptr ;
		threadData->rootScope = nullptr ;
		//TODO: release process mutex

	    //Send thread to monitor
		NewThread newthread ;
		newthread.pid = getpid() ;
		newthread.tid = pthread_self() ;
	    sendCommand(CommandNewThread, &newthread, sizeof(NewThread)) ;
	}

	if(threadData->currScope==nullptr)
	{
		//Scope does not exist yet for this thread, it's the first time we enter
		//a function inside this thread
		threadData->currScope = createScope(call_site) ;
		threadData->rootScope = threadData->currScope ;
	}
	else
	{
		Scope* scope=nullptr ;

		//Search for scope inside current scope

	}
}

//-----------------------------------------------------------------------------
/**
* Gcc internal exit function.
*/
extern "C"
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
}

//-----------------------------------------------------------------------------
/**
* Initialize profiler.
* No thread lock in this function because it's a nested call inside a locked function.
*/
void __cyg_profile_func_init()
{
	//open pipe, if opening does not work monitoring won't do the job
    if((pipefd=open("/tmp/ftrace.pipe", O_WRONLY | O_NONBLOCK))<0)
    {
    	perror("Pipe ftrace can not be opened") ;
    	return ;
    }

    try {
    	//Create process shared memory, the key is the pid
    	shared_memory_object::remove(getShmName()) ;
    } catch(...) {
        puts("Error removing SHM") ;
    }

    try {
    	puts(getShmName()) ;
    	shm = managed_shared_memory(create_only, getShmName(), ShmSize) ;

		//Create process data
		processData = shm.construct<ProcessData>("processData")() ;

		//Create threads data
		processData->threads = (ThreadDataList*)shm.allocate(sizeof(ThreadDataList)) ;
		new(static_cast<void*>(processData->threads.get()))ThreadDataList(shm.get_segment_manager()) ;

		processData->scopes = (ScopeDataMap*)shm.allocate(sizeof(ScopeDataMap)) ;
		new(static_cast<void*>(processData->scopes.get()))ScopeDataMap(std::less<size_t>(), shm.get_segment_manager()) ;
    } catch(...) {
        shared_memory_object::remove(getShmName()) ;
        puts("Error creating SHM") ;
        return ;
    }

    //Attach process to monitor
    Attach attach ;
    attach.pid = getpid() ;
    attach.shmsize = ShmSize ;
    sendCommand(CommandAttach, &attach, sizeof(Attach)) ;

    //Read configuration
    //TODO
}


void ftrace_init(void)
{
	//!Be carefull, this function is called before global and static variables
	//are initialized!
}

void ftrace_unload(void)
{
	//Detach process from monitor
    Detach detach ;
    detach.pid = getpid() ;
    sendCommand(CommandDetach, (void*)&detach, sizeof(Detach)) ;

    //TODO: here we are going to release all shared memory, so it's important to wait for monitor to say ok
}

void sendCommand(int id, void* command, size_t size)
{
	//Do nothing if pipe is not opened
	if(pipefd<0)
		return ;

	//TODO: protect pipe with a mutex
	uint8_t c=id ;
	write(pipefd, &c, sizeof(uint8_t)) ;
	write(pipefd, command, size) ;
}

char* getShmName()
{
	char spid[50]={0} ;
	static char shmname[50]="ftrace" ;
	bool created=false ;

	if(created==true)
		return shmname ;

	//We can not be sure that sprintf wont use any malloc so here we have a custom way for writing the pid in the name
	char *sp=&spid[1] ;	//reversed string, 0 char is at the beginning
	char *dp=&shmname[6] ;
	__pid_t pid=getpid() ;
	while(pid>0)
	{
		*sp = '0'+pid%10 ;
		pid /= 10 ;
		sp++ ;
	}
	sp-- ;
	while(*sp!='\0')
	{
		*dp = *sp ;
		dp++ ;
		sp-- ;
	}
	*dp = '\0' ;

	created = true ;
	return shmname ;
}

ScopeData* createScopeData(void* scopeptr)
{
	ScopeData* scopedata=nullptr ;

    try {
		//Add new scope data to list of known scopes
		auto iscopedata=processData->scopes->insert(std::pair<size_t, ScopeData>((size_t)scopeptr, ScopeData())).first ;
		scopedata = &iscopedata->second ;

		scopedata->address = scopeptr ;
    } catch(...) {
        puts("Error creating scope data in SHM") ;
    }

    return scopedata ;
}

Scope* createScope(void* scopeptr)
{
	Scope* scope=nullptr ;

    try {
    	//Create scope
		scope = (Scope*)shm.allocate(sizeof(Scope)) ;
		new(static_cast<void*>(scope))Scope() ;

		//Retrieve scope data
		if(processData->scopes->find((size_t)scopeptr)==processData->scopes->end())
		{
			//Scope data does not exist yet, create it
			scope->scopeData = createScopeData(scopeptr) ;
		}
		else
		{
			//Fill with existing scope data
			scope->scopeData = &processData->scopes->at((size_t)scopeptr) ;
		}
    } catch(...) {
        puts("Error creating scope in SHM") ;
    }

    return scope ;
}
