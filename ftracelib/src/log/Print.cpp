/**
@file Print.cpp
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


#include "log/Print.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

using namespace ftrace ;

/*
 * These routines do their printing without using stdio. Stdio can't
 * be used because it calls malloc(). Internal routines of a malloc()
 * debugger should not re-enter malloc(), so stdio is out.
 */

/*
 * NUMBER_BUFFER_SIZE is the longest character string that could be needed
 * to represent an unsigned integer, assuming we might print in base 2.
 */
#define	NUMBER_BUFFER_SIZE	(sizeof(ef_number) * NBBY)

void ftrace::printNumber(ef_number number, ef_number base)
{
	char		buffer[NUMBER_BUFFER_SIZE];
	char *		s = &buffer[NUMBER_BUFFER_SIZE];
	int		size;
	
	do {
		ef_number	digit;

		if ( --s == buffer )
			Abort("Internal error printing number.");

		digit = number % base;

		if ( digit < 10 )
			*s = '0' + digit;
		else
			*s = 'a' + digit - 10;

	} while ( (number /= base) > 0 );

	size = &buffer[NUMBER_BUFFER_SIZE] - s;

	if ( size > 0 )
		write(2, s, size);
}

void ftrace::vprint(const char * pattern, va_list args)
{
	static const char	bad_pattern[] =
	 "\nBad pattern specifier %%%c in EF_Print().\n";
	const char *	s = pattern;
	char		c;

	while ( (c = *s++) != '\0' ) {
		if ( c == '%' ) {
			c = *s++;
			switch ( c ) {
			case '%':
				(void) write(2, &c, 1);
				break;
			case 'a':
				/*
				 * Print an address passed as a void pointer.
				 * The type of ef_number must be set so that
				 * it is large enough to contain all of the
				 * bits of a void pointer.
				 */
				printNumber(
				 (ef_number)va_arg(args, void *)
				,0x10);
				break;
			case 's':
				{
					const char *	string;
					size_t		length;

					string = va_arg(args, char *);
					length = strlen(string);

					(void) write(2, string, length);
				}
				break;
			case 'd':
				{
					int	n = va_arg(args, int);

					if ( n < 0 ) {
						char	c = '-';
						write(2, &c, 1);
						n = -n;
					}
					printNumber(n, 10);
				}
				break;
			case 'l':
				{
					int	n = va_arg(args, long);

					if ( n < 0 ) {
						char	c = '-';
						write(2, &c, 1);
						n = -n;
					}
					printNumber(n, 10);
				}
				break;
			case 'x':
				write(2, "0x", 2) ;
				printNumber(va_arg(args, u_int), 0x10);
				break;
			case 'c':
			        { /*Cast used, since char gets promoted to int in ... */
					char	c = (char) va_arg(args, int);
					
					(void) write(2, &c, 1);
				}
				break;
			default:
				{
					Print(bad_pattern, c);
				}
		
			}
		}
		else
			(void) write(2, &c, 1);
	}
}

void ftrace::Abort(const char* pattern, ...)
{
	va_list	args;

	va_start(args, pattern);

	Print("\nftrace: ");
	vprint(pattern, args);
	Print("\n");

	va_end(args);

	/*
	 * I use kill(getpid(), SIGILL) instead of abort() because some
	 * mis-guided implementations of abort() flush stdio, which can
	 * cause malloc() or free() to be called.
	 */
	kill(getpid(), SIGILL);
	/* Just in case something handles SIGILL and returns, exit here. */
	_exit(-1);
}

void ftrace::Exit(const char * pattern, ...)
{
	va_list	args;

	va_start(args, pattern);

	Print("\nElectricFence Exiting: ");
	vprint(pattern, args);
	Print("\n");

	va_end(args);

	/*
	 * I use _exit() because the regular exit() flushes stdio,
	 * which may cause malloc() or free() to be called.
	 */
	_exit(-1);
}

void ftrace::Print(const char * pattern, ...)
{
	va_list	args;

	va_start(args, pattern);
	vprint(pattern, args);
	va_end(args);
}
