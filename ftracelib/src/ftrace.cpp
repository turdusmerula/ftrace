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

/**
* Internal includes.
*/
#include "log/Logger.h"
#include "scope/Scope.h"
#include "com/Timing.h"
#include "com/Demangling.h"
#include "com/Config.h"
#include "com/Thread.h"
#include "log/Print.h"
#include <stdio.h>
#include <iostream>
#include "com/ThreadData.h"


using namespace std ;

//static AllocatorEFence allocatorEFence ;
//static AllocatorSimple allocatorSimple ;
//static AllocatorSystem allocatorSystem ;

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

/**
* Indicates if we are in an ftrace code section.
*/
__thread bool _threadFtraceFlag=false ;

//-----------------------------------------------------------------------------
/**
* Gcc internal entry function.
*/
extern "C"
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	if(_threadFtraceFlag==true)
        return ;

    uint64_t profTime=Timing::getTime() ;//Time consumed by instrument

	//Create thread data
	if(ThreadData::_threadData==NULL)
		ThreadData::_threadData = new ThreadData() ;

	//Lock thread data
	ThreadData::_threadData->lock() ;

	//Prevent logging for instrument code
    _threadFtraceFlag = true ;

    //printf("%lx %lu\n", Scope::_scopeList, (unsigned long)pthread_self()) ;
    if(Scope::_threadScopeList==NULL)
    {
    	//If we enter a new thread every Scope static members will be uninitilized, we need an init for this thread
    	ThreadData::globalLock() ;
    	__cyg_profile_func_init() ;
    	ThreadData::_threadCounter++ ;
    	ThreadData::_threadData->_threadNumber = ThreadData::_threadCounter ;
    	ThreadData::globalUnlock() ;
    }

    Scope* parentScope ;        //Parent scope for the current scope
    Scope* globalScope=NULL ;   //global scope for the current scope

	//Takes current scope from stack
    parentScope = Scope::_threadScopeStack->top() ;

    //Search scope in parent scope
    std::vector< std::pair<void*, Scope*> >::iterator ipscope ;
    ipscope = Scope::find(parentScope->_childScopes, this_fn) ;
    if(ipscope==parentScope->_childScopes.end())
    {
        //Scope not found in the parent scope

        //Create new child scope
        Scope::_threadCurrScope = new Scope() ;
        Scope::_threadCurrScope->_parentScope = parentScope ;

        //Init calls
        Scope::_threadCurrScope->_callNum = 0 ;

        //Init times
        Scope::_threadCurrScope->_time = 0 ;
        Scope::_threadCurrScope->_scopeTime = 0 ;
        Scope::_threadCurrScope->_instTime = 0 ;

        //Init memory stats
        Scope::_threadCurrScope->_scopeMemStats._allocSize = 0 ;
        Scope::_threadCurrScope->_scopeMemStats._allocNum = 0 ;
        Scope::_threadCurrScope->_scopeMemStats._freeNum = 0 ;
        Scope::_threadCurrScope->_scopeMemStats._allocTime = 0 ;

        //Search scope in global list
        std::map<void*, Scope*>::iterator icscope ;
        icscope = Scope::_threadScopeList->find(this_fn) ;
        if(icscope==Scope::_threadScopeList->end())
        {
            //Node not found in the global scope list

            //Create new global scope for this scope
            globalScope = new Scope() ;
            Scope::_threadCurrScope->_globalScope = globalScope ;
            globalScope->_globalScope = NULL ;

            //Demangle scope
            Demangling::getCaller(globalScope->_source, globalScope->_name) ;

            //Init calls
            globalScope->_callNum = 0 ;

            //Init times
            globalScope->_time = 0 ;
            globalScope->_scopeTime = 0 ;
            globalScope->_instTime = 0 ;

            //Init memory stats
            globalScope->_scopeMemStats._allocSize = 0 ;
            globalScope->_scopeMemStats._allocNum = 0 ;
            globalScope->_scopeMemStats._freeNum = 0 ;
            globalScope->_scopeMemStats._allocTime = 0 ;

            //Store address
            globalScope->_address = this_fn ;

            //Test if scope must be filtered in case scope are displayed
            if(Logger::_threadLogMessagesFlag==true)
            {
            	ThreadData::globalLock() ;
                globalScope->_filter = Logger::isFiltered(globalScope->_name, Logger::_threadRootLogger) ;
            	ThreadData::globalUnlock() ;
            }
			else
                globalScope->_filter = true ;

            //Add to global list
            Scope::_threadScopeList->operator[](this_fn) = globalScope ;

        }
        else
        {
            //Node found in global list but not in scope

            //Add scope to current data
            globalScope = icscope->second ;
            Scope::_threadCurrScope->_globalScope = globalScope ;
        }

        //Add scope data to local tree branch
        parentScope->_childScopes.push_back(std::pair<void*, Scope*>(this_fn, Scope::_threadCurrScope)) ;
    }
    else
    {
        //Scope data found in parent scope
        Scope::_threadCurrScope = ipscope->second ;
    }

    //Increment calls num
    Scope::_threadCurrScope->_callNum++ ;
    Scope::_threadCurrScope->_globalScope->_callNum++ ;

    //Add current scope to call stack
    Scope::_threadScopeStack->push(Scope::_threadCurrScope) ;

    //Function entry print
    if(Logger::_threadLogMessagesFlag==true && Scope::_threadCurrScope->_globalScope->_filter==false)
    {
    	ThreadData::globalLock() ;
    	printf("%lld\t|", ThreadData::_threadData->_threadNumber) ;
        Logger::printEntryExit(true) ;
        if(Scope::_threadCurrScope->_globalScope->_name.size()==0)
        {
			printf(" %lx [%lu, %lu]\n",
				(long unsigned int)this_fn,
				Scope::_threadCurrScope->_callNum,
				Scope::_threadCurrScope->_globalScope->_callNum) ;
        }
        else
        {
			printf(" %s [%lu, %lu]\n",
				Scope::_threadCurrScope->_globalScope->_name.c_str(),
				Scope::_threadCurrScope->_callNum,
				Scope::_threadCurrScope->_globalScope->_callNum) ;
        }
		ThreadData::globalUnlock() ;
    }

    //Add elapsed time in instrument to parent scope
    Scope::_threadCurrScope->_parentScope->_instTime += Timing::getTime()-profTime ;

    //Initialize time as near as the end of the instrument function as possible to measure the more accurate possible time
    Scope::_threadCurrScope->_currTime = Timing::getTime() ;

	//Accept logging
	_threadFtraceFlag = false ;

	//Unlock thread data
	ThreadData::_threadData->unlock() ;

	//puts("<< __cyg_profile_func_enter") ;
}

//-----------------------------------------------------------------------------
/**
* Gcc internal exit function.
*/
extern "C"
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
    if(_threadFtraceFlag==true)
        return ;

    uint64_t profTime=Timing::getTime() ; //Time consumed by profiler

	//Lock thread data
	ThreadData::_threadData->lock() ;

	//Prevent logging for instrument code
    _threadFtraceFlag = true ;

    //Pop scope data from stack
    Scope::_threadCurrScope = Scope::_threadScopeStack->top() ;

    //Add elapsed time in instrument to parent scope
    Scope::_threadCurrScope->_instTime += Timing::getTime()-profTime ;

    //Measure time as near as the beginning of the instrument function as possible to measure the more accurate possible time
    Scope::_threadCurrScope->_time += Timing::getTime()-Scope::_threadCurrScope->_currTime ;

    profTime = Timing::getTime() ;
    //the time elapsed in the beginning must me counted in the current scope instrument time
    //_currScope->_instTime += getTime()-profTime ;
    //profTime = getTime() ;

    //Function exit print
    if(Logger::_threadLogMessagesFlag==true && Scope::_threadCurrScope->_globalScope->_filter==false)
    {
    	ThreadData::globalLock() ;
    	printf("%lld\t|", ThreadData::_threadData->_threadNumber) ;
        Logger::printEntryExit(false) ;
        if(Scope::_threadCurrScope->_globalScope->_name.size()==0)
        {
			printf(" %lx [%lu, %lu] (%s)\n",
				(long unsigned int)this_fn,
				Scope::_threadCurrScope->_callNum,
				Scope::_threadCurrScope->_globalScope->_callNum,
				Timing::computeTime(Scope::_threadCurrScope->_time, Logger::_threadRootLogger).c_str()) ;
        }
        else
        {
			printf(" %s [%lu, %lu] (%s)\n",
				Scope::_threadCurrScope->_globalScope->_name.c_str(),
				Scope::_threadCurrScope->_callNum,
				Scope::_threadCurrScope->_globalScope->_callNum,
				Timing::computeTime(Scope::_threadCurrScope->_time, Logger::_threadRootLogger).c_str()) ;
        }
    	ThreadData::globalUnlock() ;
    }

    //Pop scope data from stack
    Scope::_threadScopeStack->pop() ;

    //Memory check
    //mcheck(0) ;

    //Add elapsed time in instrument
    Scope::_threadCurrScope->_parentScope->_instTime += Timing::getTime()-profTime ;

	//Accept logging
    _threadFtraceFlag = false ;

    //Lock thread data
	ThreadData::_threadData->unlock() ;

	//puts("<< __cyg_profile_func_exit") ;

}

//-----------------------------------------------------------------------------
/**
* Initialize profiler.
* No thread lock in this function because it's a nested call inside a locked function.
*/
void __cyg_profile_func_init()
{
	//puts(">> __cyg_profile_func_init") ;

	bool initScope=false ;	//Indicate if we must initialize the Scope
	bool initLogger=false ;	//Indicate if we must initialize loggers

    //Initialization of internal instrument data
    if(Scope::_threadScopeList==NULL)
    {
    	////puts("initScope") ;
    	initScope = true ;
    }
    if(Logger::_loggers==NULL)
    {
    	////puts("initLogger") ;
    	initLogger = true ;
    }

    if(initLogger==true)
    {
		//Create logger list
		Logger::_loggers = new std::list<Logger*> ;
		ThreadData::_threads = new std::list<ThreadData*> ;
    }


    if(initScope==true)
    {
    	//If we enter a new thread every Scope static members will be uninitialized
    	Scope::_threadScopeList = new std::map<void*,  Scope*>() ;
    	Scope::_threadScopeStack = new std::stack<Scope*>() ;

        //Initialize memory statistics
        Scope::_threadMemStats._allocSize = 0 ;
        Scope::_threadMemStats._allocNum = 0 ;
        Scope::_threadMemStats._freeNum = 0 ;
        Scope::_threadMemStats._allocTime = 0 ;
    }


    if(initScope==true)
    {
		//create first logger, using only for display
		Logger::_threadRootLogger = new Logger() ;
		Logger::_threadRootLogger->_root = true ;
		Logger::_threadRootLogger->_timing = Logger::eAuto ;
    	Logger::_loggers->push_back(Logger::_threadRootLogger) ;

        //Create thread data
        Logger::_threadRootLogger->_threadData = new ThreadData() ;
        Logger::_threadRootLogger->_threadData->_threadId = pthread_self() ;
        Logger::_threadRootLogger->_threadData->_threadRootLogger = Logger::_threadRootLogger ;
        Logger::_threadRootLogger->_threadData->_threadScopeList = Scope::_threadScopeList ;
        ThreadData::_threads->push_back(Logger::_threadRootLogger->_threadData) ;

		//Add the root scope, does not correspond to any function
		Logger::_threadRootLogger->_rootScope = new Scope()	 ;
		Logger::_threadRootLogger->_rootScope->_globalScope = NULL ;
		Logger::_threadRootLogger->_rootScope->_currTime = 0 ;
		Logger::_threadRootLogger->_rootScope->_time = 0 ;
		Logger::_threadRootLogger-> _rootScope->_instTime = 0 ;
		Logger::_threadRootLogger->_rootScope->_callNum = 0 ;
		Logger::_threadRootLogger->_rootScope->_parentScope = NULL ;
		Logger::_threadRootLogger->_rootScope->_filter = true ;

    	//Push the root scope in stack
    	Scope::_threadScopeStack->push(Logger::_threadRootLogger->_rootScope) ;

		//Initialize logguer
		std::string logFile=string((getenv("FTRACE")==NULL?"":getenv("FTRACE"))) ;
		if(logFile=="true")
		{
			Logger::_threadLogMessagesFlag = true ;
		}
		else if(logFile!="")
		{
			Config::loadConfFile(logFile) ;
		}
		else
		{
			Logger::_threadLogMessagesFlag = false ;
		}
    }

	//puts("<< __cyg_profile_func_init") ;

}


void ftrace_init(void)
{
	//puts("ftrace_init") ;

	//Initialize mutex
	initLock() ;

//	allocator = (Allocator*)&allocatorSystem ;
/*	AllocatorSystem::initialize() ;
    AllocatorEFence::initialize() ;
	AllocatorSimple::initialize() ;*/

	//Initialize logger
    //__cyg_profile_func_init() ;
}

void ftrace_unload(void)
{
	//puts("ftrace_unload") ;
	//Output log file
	ThreadData::globalLock() ;
	_threadFtraceFlag = true ;
	Logger::logStats() ;
	_threadFtraceFlag = false ;
	ThreadData::globalUnlock() ;
}


