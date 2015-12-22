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
using namespace ftrace ;

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

void ftrace_init(void)
	__attribute__ ((constructor))
	__attribute__((no_instrument_function)) ;
void ftrace_unload(void)
	__attribute__ ((destructor))
	__attribute__((no_instrument_function)) ;

void loadConfFile()
    __attribute__((no_instrument_function)) ;

//-----------------------------------------------------------------------------

/**
* Indicates if we are in an ftrace code section.
*/
thread_local bool threadFtraceFlag=false ;

//-----------------------------------------------------------------------------
/**
* Gcc internal entry function.
*/
extern "C"
void __cyg_profile_func_enter(void* this_fn, void* call_site)
{
    //Measure time as near as the beginning of the instrument function as possible to measure the more accurate possible time
    uint64_t intime=Timing::getTime() ;

    //Avoid reentrant calls
	if(threadFtraceFlag==true)
        return ;

    //Prevent logging for instrument code
    threadFtraceFlag = true ;

	//Create thread data
	if(threadData==nullptr)
	{
	    threadData = new ThreadData ;

	    //Lock thread data
	    threadData->lock() ;

        // init Logger static members
        Logger() ;

	    // init ScopeDescriptor static members
        ScopeDescriptor() ;

        // load conf file if exists
        loadConfFile() ;
	}
	else
	{
	    //Lock thread data
	    threadData->lock() ;
	}

    //Parent scope for the current scope taken from stack
    Scope* parentScope=threadData->scopeStack_->top() ;

    //Search scope in parent scope
    // TODO: improve the search speed by using a map
    std::vector< std::pair<uint64_t, Scope*> >::iterator ipscope=parentScope->findChild(this_fn) ;
    if(ipscope==parentScope->childs_->end())
    {
        //Scope not found in the parent scope

        //Create new child scope
        threadData->prevScope_ = threadData->currScope_ ;
        threadData->currScope_ = new Scope(this_fn) ;
        threadData->currScope_->parentScope_ = parentScope ;
        parentScope->childs_->push_back(std::pair<uint64_t, Scope*>(threadData->currScope_->id_, threadData->currScope_)) ;

        //Search scope descriptor
        std::map<void*, ScopeDescriptor*>::iterator icscopedesc ;
        icscopedesc = Scope::descriptors_->find(this_fn) ;
        if(icscopedesc==Scope::descriptors_->end())
        {
            //Descriptor not found

            //Create new descriptor
            ThreadData::globalLock() ;
            ScopeDescriptor* descriptor=new ScopeDescriptor ;
            threadData->currScope_->descriptor_ = descriptor ;
            Scope::descriptors_->operator[](this_fn) = descriptor ;
            ThreadData::globalUnlock() ;

            //Demangle scope
            Demangling::getCaller(descriptor->mangledName_, descriptor->name_) ;

            //Store address
            descriptor->address_ = this_fn ;

            //Test if scope must be filtered in case scope are displayed
            ThreadData::globalLock() ;
            descriptor->filter_ = Logger::rootLogger_->isFiltered(descriptor->name_) ;
            ThreadData::globalUnlock() ;
        }
        else
        {
            //Descriptor found

            //Add descriptor to current scope
            threadData->currScope_->descriptor_ = icscopedesc->second ;
        }
    }
    else
    {
        //Scope found in parent
        threadData->prevScope_ = threadData->currScope_ ;
        threadData->currScope_ = ipscope->second ;
    }

    //Add current scope to call stack
    threadData->scopeStack_->push(threadData->currScope_) ;

    // reinit current scope calls num
//    if(threadData->prevScope_!=threadData->currScope_)
//        threadData->currScope_->data_->callNum_ = 1 ;

    //Increment calls num
    threadData->currScope_->data_->callNum_++ ;
    threadData->currScope_->threadData_->callNum_++ ;
    threadData->currScope_->processData_->callNum_++ ;

    //Function entry print
    Logger::rootLogger_->logScopeEntry(threadData->currScope_) ;

    //Initialize time as near as the end of the instrument function as possible to measure the more accurate possible time
    uint64_t outtime=Timing::getTime() ;
    threadData->currScope_->data_->currTime_ = outtime ;
    if(parentScope)
    	parentScope->data_->instTime_ += outtime-intime ;

	//Accept logging
	threadFtraceFlag = false ;

	//Unlock thread data
	threadData->unlock() ;

	//puts("<< __cyg_profile_func_enter") ;
}

//-----------------------------------------------------------------------------
/**
* Gcc internal exit function.
*/
extern "C"
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
    //Measure time as near as the beginning of the instrument function as possible to measure the more accurate possible time
    uint64_t intime=Timing::getTime() ;

    if(threadFtraceFlag==true)
        return ;

	//Lock thread data
	threadData->lock() ;

	//Prevent logging for instrument code
    threadFtraceFlag = true ;

    // this is the current scope beeing exited
    Scope* exitScope=threadData->currScope_ ;

    //Pop scope data from stack
    threadData->currScope_ = threadData->scopeStack_->top() ;

    threadData->currScope_->data_->time_ += intime-threadData->currScope_->data_->currTime_ ;
    threadData->currScope_->threadData_->time_ += intime-threadData->currScope_->data_->currTime_ ;
    threadData->currScope_->processData_->time_ += intime-threadData->currScope_->data_->currTime_ ;
    threadData->currScope_->data_->currTime_ = intime-threadData->currScope_->data_->currTime_ ;

    // Function exit print
    Logger::rootLogger_->logScopeExit(threadData->currScope_) ;

    //Pop scope data from stack
    threadData->scopeStack_->pop() ;

    //Initialize time as near as the end of the instrument function as possible to measure the more accurate possible time
    uint64_t outtime=Timing::getTime() ;
    threadData->currScope_->parentScope_->data_->instTime_ += outtime-intime ;

    //Lock thread data
	threadData->unlock() ;

	//Accept logging
    threadFtraceFlag = false ;

	//puts("<< __cyg_profile_func_exit") ;
}
//-----------------------------------------------------------------------------

void loadConfFile()
{
    static bool loaded=false ;

    //Load it only once
    if(loaded==true)
        return ;
    loaded = true ;

    //Initialize logguer
    std::string logFile=string((getenv("FTRACE")==NULL?"":getenv("FTRACE"))) ;
    if(logFile=="true")
        Logger::rootLogger_->trace_ = true ;
    else if(logFile!="")
        Config::loadConfFile(logFile) ;
    else
        Logger::rootLogger_->trace_ = true ;

    Logger* logger=new Logger() ;
    logger->root_ = false ;
    logger->filename_ = "profile" ;
    logger->timing_ = Timing::eTicks ;
    logger->format_ = Logger::eJson ;
    Logger::loggers_->push_back(logger) ;

}

void ftrace_init(void)
{
	//puts("ftrace_init") ;

	//Initialize mutex
	initLock() ;
}

void ftrace_unload(void)
{
	//puts("ftrace_unload") ;
	//Output log file
	ThreadData::globalLock() ;
	threadFtraceFlag = true ;

	for(auto& logger : *Logger::loggers_)
	    logger->log() ;

	//	Logger::logStats() ;
	threadFtraceFlag = false ;
	ThreadData::globalUnlock() ;
}


