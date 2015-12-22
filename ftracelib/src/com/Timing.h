/**
  @file		Timing.h
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Time measurement functions.

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

#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>
#include <string>

namespace ftrace {

class Logger ;  //Declared here to avoid include recursion

class Timing
{
public:

    /**
    * Enumeration for display of time in logger.
    */
    typedef enum { eAuto, eSecond, eTicks } TimeEnum ;

    /**
    * Inline function used to read the rdtsc register of X86 processors.
    * @return A 64 bits number containing the value of the rdtsc register.
    */
    #ifdef __i386
    static __inline__ uint64_t getTime()
    	__attribute__((no_instrument_function))
    {
      uint64_t x;
      __asm__ volatile ("rdtsc" : "=A" (x));
      return x;
    }
    #elif __amd64
    static __inline__ uint64_t getTime()
    	__attribute__((no_instrument_function))
    {
      uint64_t a, d;
      __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
      return (d<<32) | a;
    }
    #endif

    /**
    * Read the processor frequency.
    * @return 0 if error or frequecy in Hz.
    */
    static double readCPUFreq()
        __attribute__((no_instrument_function)) ;

    /**
    * Converts a number of ticks to seconds.
    * @param ticks_ Number of ticks.
    * @result Number of seconds.
    */
    static long double tickToSecond(uint64_t ticks_)
        __attribute__((no_instrument_function)) ;

    /**
    * Return the current date with format YYYY/MM/DD-HH:MM:SS
    * @return date computed
    */
    static std::string getDate()
        __attribute__((no_instrument_function)) ;

    /**
    * Compute a string containing a time.
    * @param _time Time in ticks.
    * @param _timing output timing format.
    */
    static std::string computeTime(uint64_t _time, TimeEnum _timing)
        __attribute__((no_instrument_function)) ;

} ;

}

#endif
