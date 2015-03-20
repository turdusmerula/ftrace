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

#include <mem/Memory.h>
#include <scope/ScopeData.h>
#include <scope/ScopeDescriptor.h>
#include <scope/MemoryData.h>

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
    * Memory data for the scope.
    */
    MemoryData* scopeMemStats_ ;


    /**
    * Parent scope data for tree scope.
    * Nullptr if no parent.
    */
    Scope* parentScope_ ;

    /**
    * List of called scopes.
    */
    std::vector< std::pair<uint64_t,  Scope*> >* childs_ ;

//    /**
//    * Recompute global scopes using scope tree.
//    * @param thread_ Thread containing data.
//    */
//    static void computeGlobalScopes(ThreadData* thread_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Compute total time elapsed in scope without time elapsed in subscopes
//    * @param scope_ Scope with childs to cumpute
//    */
//    static uint64_t computeScopeTime(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Compute total time elapsed in instrument for a scope
//    * @param scope_ Scope with childs to cumpute
//    */
//    static uint64_t computeInstScopeTime(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively erase all scopes of a tree.
//    * @param scope_ Scope data and its childs to clear
//    */
//    static void recursiveClearScopes(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively compute a filtered tree based upon a source tree and a scope filtering patterns.
//    * @param logger_ Logger containing the filter to apply.
//    * @param srcScope_ Source scopes data and its childs to cumpute.
//    * @param dstScope_ Destination scope contaning the new filtered scope tree.
//    */
//    static void recursiveFilterScopes(Logger* logger_, Scope* srcScope_, Scope* dstScope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively concatenate all identical subscopes of a tree when they are in the same scope.
//    * @param scope_ Scope data and its childs to concatenate.
//    */
//    static void recursiveConcatenateScopes(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
    /**
    * Find a function in a function list.
    * @param list_ List to search in.
    * @param func_ Function to search.
    * @param istart_ Iterator to start the search, used for optimisation if given.
    * @return iterator on the function found.
    */
    std::vector< std::pair<uint64_t,  Scope*> >::iterator findChild(void* func_, std::vector< std::pair<uint64_t,  Scope*> >::iterator* istart_=nullptr)
        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Find a scope in the memory owner list.
//    * @param list_ List to search in.
//    * @param Scope Scope to search.
//    * @param istart_ Iterator to start the search, used for optimisation if given.
//    * @return iterator on the Scope found.
//    */
////    static std::vector< std::pair<Scope*, MemStats*> >::iterator find(std::vector< std::pair<Scope*, MemStats*> >& list_, Scope* scope_, std::vector< std::pair<void*,  Scope*> >::iterator* istart_=NULL)
////        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Concatenate memory statistics of two scopes.
//    * @param srcscope_ Source scope for concatenation.
//    * @param dstscope_ Destination scope for concatenation.
//    */
//    static void concatenateScopesMemstat(Scope* srcscope_, Scope* dstscope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Compute memory statistics for a global scope.
//    * @param scope_ Global scope to compute.
//    */
//    static void computeGlobalMemStats(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively concatenate memory statistics for a scope tree.
//    * @param scope_ Starting scope.
//    */
//    static void recursiveConcatenateScopesMemstat(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//
//private:
//    /**
//    * Compute total time elapsed in instrument for a scope
//    * @param scope_ Scope with childs to cumpute
//    */
//    uint64_t recursiveComputeInstScopeTime(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively compute totals for each scope of the tree.
//    * @param scope_ Scope data and its childs to cumpute
//    */
//    static void recursiveComputeGlobalScopes(Scope* scope_)
//        __attribute__((no_instrument_function)) ;
//
//    /**
//    * Recursively compute memory statistics for a global scope.
//    * @param currScope_ Current scope of the tree to compute.
//    * @param scope_ Global scope to compute.
//    */
//    static void recursiveComputeGlobalMemStats(Scope* currScope_, Scope* scope_)
//        __attribute__((no_instrument_function)) ;
} ;

}

#endif
