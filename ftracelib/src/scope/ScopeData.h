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

#ifndef SCOPE_DATA_H
#define SCOPE_DATA_H

#include <stdint.h>

namespace ftrace {

/**
* @class ScopeData
* Used to store data for a scope.
*/
class ScopeData
{
public:
    ScopeData() ;


    /**
    * Total elapsed time for the scope.
    */
    uint64_t time_ ;

    /**
    * Calls number for the scope.
    */
    uint64_t callNum_ ;

    /**
    * Current time for the scope, used only by profiling functions.
    */
    uint64_t currTime_ ;

    /**
    * Time elapsed inside instrumentation code
    */
    uint64_t instTime_ ;

} ;

}

#endif
