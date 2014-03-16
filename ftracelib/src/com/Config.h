/**
  @file		Confing.h
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Configuration file reader

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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
	/*
	 * EF_ALIGNMENT is a global variable used to control the default alignment
	 * of buffers returned by malloc(), calloc(), and realloc(). It is all-caps
	 * so that its name matches the name of the environment variable that is used
	 * to set it. This gives the programmer one less name to remember.
	 * If the value is -1, it will be set from the environment or sizeof(int)
	 * at run time.
	 */
	static int EF_ALIGNMENT ;

	/*
	 * EF_PROTECT_FREE is a global variable used to control the disposition of
	 * memory that is released using free(). It is all-caps so that its name
	 * matches the name of the environment variable that is used to set it.
	 * If its value is greater non-zero, memory released by free is made
	 * inaccessable and never allocated again. Any software that touches free
	 * memory will then get a segmentation fault. If its value is zero, freed
	 * memory will be available for reallocation, but will still be inaccessable
	 * until it is reallocated.
	 */
	static bool EF_PROTECT_FREE ;

	/*
	 * EF_PROTECT_BELOW is used to modify the behavior of the allocator. When
	 * its value is non-zero, the allocator will place an inaccessable page
	 * immediately _before_ the malloc buffer in the address space, instead
	 * of _after_ it. Use this to detect malloc buffer under-runs, rather than
	 * over-runs. It won't detect both at the same time, so you should test your
	 * software twice, once with this value clear, and once with it set.
	 */
	static bool EF_PROTECT_BELOW ;

	/*
	 * EF_ALLOW_MALLOC_0 is set if Electric Fence is to allow malloc(0). I
	 * trap malloc(0) by default because it is a common source of bugs.
	 */
	static bool EF_ALLOW_MALLOC_0 ;

	/*
	 * EF_FREE_WIPES is set if Electric Fence is to wipe the memory content
	 * of freed blocks.  This makes it easier to check if memory is freed or
	 * not
	 */
	static bool EF_FREE_WIPES ;

	/*
	 * MEMORY_CREATION_SIZE is the amount of memory to get from the operating
	 * system at one time. We'll break that memory down into smaller pieces for
	 * malloc buffers. One megabyte is probably a good value.
	 */
	static size_t MEMORY_CREATION_SIZE ;

	/*
	 * PAGE_SLOT_SIZE is the number of pages to allocate to each grow of the slot list
	 */
	static int PAGE_SLOT_SIZE ;

	/**
    * Read the configuration file and create the loggers.
    * @param filename_ File name and path of file to open
    */
    static bool loadConfFile(const std::string &filename_)
    	__attribute__((no_instrument_function)) ;
} ;

#endif
