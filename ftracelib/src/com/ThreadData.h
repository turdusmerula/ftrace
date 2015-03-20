/**
  @file		ThreadData.h
  @author	S. Besombes
  @date		october 2011
  @version	$Revision: 1.1.1 $
  @brief	Configuration file reader

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

#ifndef _THREADDATA_H
#define _THREADDATA_H

#include <string>
#include <list>
#include <map>
#include <stack>
#include <pthread.h>
#include <unistd.h>

namespace ftrace {

class Scope ;
class Logger ;


/**
 * Data of the thread that must be protected by mutex.
 */
class ThreadData
{
public:

	/**
     * Thread identifier.
     */
    pthread_t id_ ;

	/**
	 * Counter incremented for each new thread.
	 */
    static size_t count_ ;

    /**
     * Thread creation number, each new thread gets an increased by 1 value.
     */
    size_t number_ ;


    /**
     * Start time for the thread.
     */
    uint64_t startTime_ ;

    /**
     * End time for the thread.
     */
    uint64_t endTime_ ;


    /**
     * List of every created threads for the process.
     */
    static std::list<ThreadData*>* threads_ ;



    /**
    * List of scopes.
    * One per thread.
    */
    std::map<void*,  Scope*>* scopes_ ;

    /**
    * Call stack.
    * One per thread.
    */
    std::stack<Scope*>* scopeStack_ ;

    /**
    * Current scope.
    * One per thread.
    */
    Scope* currScope_ ;

    /**
    * Prev scope.
    * One per thread.
    */
    Scope* prevScope_ ;

    /**
    * root scope that owns every other scopes.
    * One per thread.
    */
    Scope* rootScope_ ;

    /**
     * Constructor.
     */
    ThreadData()
    	__attribute__((no_instrument_function)) ;

    /**
     * Get thread mutex.
     */
    void lock()
    	__attribute__((no_instrument_function)) ;


    /**
     * Release thread mutex.
     */
    void unlock()
    	__attribute__((no_instrument_function)) ;


    /**
     * Get thread mutex.
     */
    static void globalLock()
    	__attribute__((no_instrument_function)) ;


    /**
     * Release thread mutex.
     */
    static void globalUnlock()
    	__attribute__((no_instrument_function)) ;


private:
    /**
     * Init thread mutex.
     */
    void initLock() ;

    /*
     * mutex to enable multithreaded operation
     */
    pthread_mutex_t mutex_ ;

    /*
     * mutex to enable multithreaded operation
     */
    static pthread_mutex_t globalMutex_ ;

} ;

/**
 * This object is used as a spy to trace thread beginning and end of life.
 */
class ThreadLiveSpy
{
public:
    ThreadLiveSpy() ;
    ~ThreadLiveSpy() ;
} ;

/**
 * Current data for thread.
 */
extern thread_local ThreadData* threadData ;

}

#endif
