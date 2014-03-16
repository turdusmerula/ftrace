/**
  @file		Demangling.cpp
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

#include "com/Demangling.h"

#include <libiberty.h>
#include <stdlib.h>
#include <demangle.h>

//-----------------------------------------------------------------------------
void Demangling::getCaller(vstring &source_, vstring &symbol_, int depth_)
{
    void* calls[k_DEPTH];
    const size_t size = backtrace(calls, k_DEPTH);
    char** symbols = backtrace_symbols(calls, size);

    source_ = symbols[depth_] ;
    symbol_ = symbols[depth_] ;

    //printf("%d  %s\n", symbol.rfind(')'), symbol.c_str()) ;
    //Pos of parenthesis
    int lpos=symbol_.rfind('(') ;
    int rpos=symbol_.rfind(')') ;

    if(lpos>0)
    {
        //Extract demangling infos
        symbol_.erase(rpos) ;
        symbol_.erase(0, lpos+1) ;

        //Extract file name from pattern
        source_.erase(lpos) ;

    }
    //printf("--1> %s\n", symbol_.c_str()) ;
    //printf("--2> %s\n", source_.c_str()) ;

    char* result=cplus_demangle(symbol_.c_str(), DMGL_ANSI|DMGL_VERBOSE) ;
    //TODO: eliminate malloc here (maybe intercept malloc for cplus_demangle?)
    //TODO: malloc should allocate a MemoryPool bloc and keep it for the vstring
    if(result)
    {
    	symbol_ = result ;
    	free(result);
    }
    //printf("--3> %s\n", s_result.c_str()) ;
}
//-----------------------------------------------------------------------------

