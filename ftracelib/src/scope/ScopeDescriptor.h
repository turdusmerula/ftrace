/**
  @file		ScopeDescriptor.h
  @author	S. Besombes
  @date		january 2015
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

#ifndef SCOPE_DESC_H
#define SCOPE_DESC_H

#include <string>
#include <map>

namespace ftrace {

/**
* @class ScopeDesc
* Used to store a scope descriptor.
*/
class ScopeDescriptor
{
public:
    ScopeDescriptor() ;

    /**
    * Scope mangled Name, may be empty if not defined.
    */
    std::string mangledName_ ;

    /**
    * Scope Name.
    * If _mangleName is used _name will contain the demangled name of the function.
    * The name can also be a free value if the scope is not refering to a function or class member.
    */
    std::string name_ ;

    /**
    * Scope adress if exists.
    */
    void* address_ ;

    /**
    * Global list of scope descriptors.
    * There is one address for each scope allowing a scope to be demangled only once a time.
    */
    static std::map<void*,  ScopeDescriptor*>* descriptors_ ;
} ;

}

#endif
