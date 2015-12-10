/**
  @file		Scope.h
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Scope tree function managing.

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

#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include <stack>
#include <vector>
#include <list>

#include <string>

#include <stdint.h>

#include <scope/ScopeData.h>
#include <scope/ScopeDescriptor.h>

/**
 * For each function there is two scope block:
 * - one for the current scope, it is part of the call tree, there may be at final more than one if the function is called from many other functions
 * - one global for all calls dedicated to global stats
 */

namespace ftrace {

class Logger ;  	//Declared here to avoid include recursion
class ThreadData ;	//Declared here to avoid include recursion


/**
* @class Scope
* Used to store data for a scope.
*/
class Scope
{
public:

    Scope(void* _address) ;

    /**
     * Scope descriptor.
     */
    ScopeDescriptor* descriptor_ ;

    /**
    * Global list of scope descriptors.
    * There is one address for each scope allowing a scope to be demangled only once a time.
    */
    static std::map<void*,  ScopeDescriptor*>* descriptors_ ;


    /**
     * Scope data id.
     */
    uint64_t id_ ;

    /**
     * Scope id.
     */
    static uint64_t nextId_ ;


    /**
     * Data for scope.
     */
    ScopeData* data_ ;

    /**
     * Summary scope data for current thread.
     */
    ScopeData* threadData_ ;

    /**
     * Summary scope data for current process.
     */
    ScopeData* processData_ ;

    /**
     * List of scope data created inside thread.
     */
    static thread_local std::map<void*, ScopeData*>* threadDatas_ ;

    /**
     * List of scope data created inside process.
     */
    static std::map<void*, ScopeData*>* processDatas_ ;


    /**
    * Parent scope data for tree scope.
    * Nullptr if no parent.
    */
    Scope* parentScope_ ;

    /**
    * List of called scopes.
    */
    std::vector< std::pair<uint64_t,  Scope*> >* childs_ ;


    /**
    * Find a function in a function list.
    * @param list_ List to search in.
    * @param func_ Function to search.
    * @param istart_ Iterator to start the search, used for optimisation if given.
    * @return iterator on the function found.
    */
    std::vector< std::pair<uint64_t,  Scope*> >::iterator findChild(void* func_, std::vector< std::pair<uint64_t,  Scope*> >::iterator* istart_=nullptr)
        __attribute__((no_instrument_function)) ;
} ;

}

#endif
