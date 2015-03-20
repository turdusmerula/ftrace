/**
@file Print.h
@author S. Besombes
@date january 2010
@version $Revision: 1.1.1 $
@brief Configuration file reader

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
MA 02110-1301, USA.
*/


#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdarg.h>

/*
 * ef_number is the largest unsigned integer we'll need. On systems that
 * support 64-bit pointers, this may be "unsigned long long".
 */
#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif

/*
 * NBBY is the number of bits per byte. Some systems define it in
 * <sys/param.h> .
 */
#ifndef	NBBY
#define	NBBY	8
#endif

namespace ftrace {

void printNumber(ef_number number, ef_number base)
	__attribute__((no_instrument_function)) ;

void vprint(const char * pattern, va_list args)
	__attribute__((no_instrument_function)) ;

void Abort(const char * pattern, ...)
	__attribute__((no_instrument_function)) ;

void Exit(const char * pattern, ...)
	__attribute__((no_instrument_function)) ;

void Print(const char * pattern, ...)
	__attribute__((no_instrument_function)) ;

}

#endif
