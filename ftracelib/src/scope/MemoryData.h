/**
  @file		MemoryData.h
  @author	S. Besombes
  @date		january 2015
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

#ifndef _SCOPE_MEMORYDATA_H
#define _SCOPE_MEMORYDATA_H

namespace ftrace {

class MemoryData
{
public:
    MemoryData() ;

    /**
     * Number of blocks allocated
     */
    unsigned long long blocksAllocated_ ;

    /**
     * Number of bytes allocated
     */
    unsigned long long bytesAllocated_ ;

    /**
     * Number of blocks freed
     */
    unsigned long long blocksFreed_ ;

    /**
     * Number of bytes freed
     */
    unsigned long long bytesFreed_ ;
} ;

}

#endif