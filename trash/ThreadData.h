/**
  @file		Confing.h
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

#ifndef THREADDATA_H
#define THREADDATA_H

#include <string>
#include <list>
#include <map>
#include <pthread.h>
#include <unistd.h>

class Scope ;
class Logger ;

/**
 * Data of the thread that must be protected by mutex.
 */
class ThreadData
{
private:
	/**
	 * Init thread mutex.
	 */
	void initLock() ;

	/*
	 * mutex to enable multithreaded operation
	 */
	pthread_mutex_t _mutex ;

	/*
	 * mutex to enable multithreaded operation
	 */
	static pthread_mutex_t _globalMutex ;

public:

	/**
     * Thread identifier.
     */
    pthread_t _threadId ;

	/**
	 * Counter incremented for each new thread.
	 */
    static size_t _threadCounter ;

    /**
     * Thread number.
     */
    size_t _threadNumber ;

	/**
     * List of every created threads for the process.
     */
    static std::list<ThreadData*>* _threads ;

    /**
     * Current data for thread.
     */
    static __thread ThreadData* _threadData ;


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


    //-----------------------------------------------------------------------------
    //Thread data section
    /**
    * List of scopes.
    */
    std::map<void*,  Scope*>* _threadScopeList ;

    /**
    * Current root logger for the thread.
    */
    Logger* _threadRootLogger ;

} ;


#endif
