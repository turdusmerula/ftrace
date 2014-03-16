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

class Logger ;  	//Declared here to avoid include recursion
class ThreadData ;	//Declared here to avoid include recursion

/**
 * For each function there is two scope block:
 * - one for the current scope, it is part of the call tree, there may be at final more than one if the function is called from many other functions
 * - one global for all calls dedicated to global stats
 */


/**
* @class Scope
* Used to store data for a scope.
*/
class Scope
{
public:

    /**
    * Scope Name, not filled if tree scope.
    */
    std::string _name ;

    /**
    * Function adress, not initialized if tree scope
    */
    void* _address ;

    /**
    * Function source, name not demangled
    */
    std::string _source ;

    /**
    * Parent scope data for tree scope.
    */
    Scope* _parentScope ;

    /**
    * Global scope data for tree scope, uninitialized if global scope.
    */
    Scope* _globalScope ;

    /**
    * List of called scopes.
    */
    std::vector< std::pair<void*,  Scope*> > _childScopes ;


    /**
    * Current time for the scope, used only by profiling functions.
    */
    uint64_t _currTime ;

    /**
    * Total elapsed time for the scope.
    */
    uint64_t _time ;

    /**
    * Time elapsed inside the scope without time inside subscopes.
    */
    uint64_t _scopeTime ;

    /**
    * Time elpased in instrument for scope called.
    */
    uint64_t _instTime ;


    /**
    * Calls number for the scope.
    */
    uint64_t _callNum ;

    /**
    * Indicates wether scope must be filtered or not.
    */
    bool _filter ;


    /**
    * List of scopes.
    * One per thread.
    */
    static __thread std::map<void*,  Scope*>* _threadScopeList ;

    /**
    * Call stack.
    * One per thread.
    */
    static __thread std::stack<Scope*>* _threadScopeStack ;

    /**
    * Current scope, necessary to avoid infinite recursion inside _scopeStack.
    * One per thread.
    */
    static __thread Scope* _threadCurrScope ;

    /**
    * Tab num for printing.
    * One per thread.
    */
    static __thread int _threadTabNum ;


    /**
    * Struct containing stats for memory.
    */
    struct MemStats
    {
        /**
        * Bytes allocated by scope.
        */
        uint64_t _allocSize ;

        /**
        * Number of blocks allocated.
        */
        uint32_t _allocNum ;

        /**
        * Number of blocks freed.
        */
        uint32_t _freeNum ;

        /**
        * Total time elapsed in allocations.
        */
        uint64_t _allocTime ;

    } ;

    /**
    * Global memory statistics. One per thread.
    */
    static __thread MemStats _threadMemStats ;

    /**
    * Memory statistics for the scope.
    */
    MemStats _scopeMemStats ;

    /**
    * List of owner scopes for freed blocks.
    */
    std::vector< std::pair<Scope*, MemStats*> > _memOwnerBlocks ;



    /**
    * Recompute global scopes using scope tree.
    * @param thread_ Thread containing data.
    */
    static void computeGlobalScopes(ThreadData* thread_)
        __attribute__((no_instrument_function)) ;

    /**
    * Compute total time elapsed in scope without time elapsed in subscopes
    * @param scope_ Scope with childs to cumpute
    */
    static uint64_t computeScopeTime(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Compute total time elapsed in instrument for a scope
    * @param scope_ Scope with childs to cumpute
    */
    static uint64_t computeInstScopeTime(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively erase all scopes of a tree.
    * @param scope_ Scope data and its childs to clear
    */
    static void recursiveClearScopes(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively compute a filtered tree based upon a source tree and a scope filtering patterns.
    * @param logger_ Logger containing the filter to apply.
    * @param srcScope_ Source scopes data and its childs to cumpute.
    * @param dstScope_ Destination scope contaning the new filtered scope tree.
    */
    static void recursiveFilterScopes(Logger* logger_, Scope* srcScope_, Scope* dstScope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively concatenate all identical subscopes of a tree when they are in the same scope.
    * @param scope_ Scope data and its childs to concatenate.
    */
    static void recursiveConcatenateScopes(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Find a function in a function list.
    * @param list_ List to search in.
    * @param func_ Function to search.
    * @param istart_ Iterator to start the search, used for optimisation if given.
    * @return iterator on the function found.
    */
    static std::vector< std::pair<void*,  Scope*> >::iterator find(std::vector< std::pair<void*,  Scope*> >& list_, void* func_, std::vector< std::pair<void*,  Scope*> >::iterator* istart_=NULL)
        __attribute__((no_instrument_function)) ;

    /**
    * Find a scope in the memory owner list.
    * @param list_ List to search in.
    * @param Scope Scope to search.
    * @param istart_ Iterator to start the search, used for optimisation if given.
    * @return iterator on the Scope found.
    */
    static std::vector< std::pair<Scope*, MemStats*> >::iterator find(std::vector< std::pair<Scope*, MemStats*> >& list_, Scope* scope_, std::vector< std::pair<void*,  Scope*> >::iterator* istart_=NULL)
        __attribute__((no_instrument_function)) ;

    /**
    * Concatenate memory statistics of two scopes.
    * @param srcscope_ Source scope for concatenation.
    * @param dstscope_ Destination scope for concatenation.
    */
    static void concatenateScopesMemstat(Scope* srcscope_, Scope* dstscope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Compute memory statistics for a global scope.
    * @param scope_ Global scope to compute.
    */
    static void computeGlobalMemStats(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively concatenate memory statistics for a scope tree.
    * @param scope_ Starting scope.
    */
    static void recursiveConcatenateScopesMemstat(Scope* scope_)
        __attribute__((no_instrument_function)) ;


private:
    /**
    * Compute total time elapsed in instrument for a scope
    * @param scope_ Scope with childs to cumpute
    */
    uint64_t recursiveComputeInstScopeTime(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively compute totals for each scope of the tree.
    * @param scope_ Scope data and its childs to cumpute
    */
    static void recursiveComputeGlobalScopes(Scope* scope_)
        __attribute__((no_instrument_function)) ;

    /**
    * Recursively compute memory statistics for a global scope.
    * @param currScope_ Current scope of the tree to compute.
    * @param scope_ Global scope to compute.
    */
    static void recursiveComputeGlobalMemStats(Scope* currScope_, Scope* scope_)
        __attribute__((no_instrument_function)) ;
} ;

#endif
