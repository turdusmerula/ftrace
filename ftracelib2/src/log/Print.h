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

#endif
