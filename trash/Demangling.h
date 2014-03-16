/**
  @file		Demangling.h
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Demangling functionalities

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

#ifndef DEMANGLING_H
#define DEMANGLING_H

/**
* Includes for demangler, from binutils
*/
#include <execinfo.h>

#include <objects/vstring.h>

/**
* Default depth for stack trace.
*/
const size_t k_DEPTH=5 ;


class Demangling
{
private:
public:

    /**
    * Demangle the name of the caller.
    * @param source_ Computed source of the symbol.
    * @param symbol_ Computed symbol name.
    * @param depth_ Depth to retrieve in stack, 0 returns getCaller.
    */
    static void getCaller(vstring &source_, vstring &symbol_, int depth_=2)
         __attribute__((no_instrument_function)) ;
} ;

extern const char* __progname ;

#endif
