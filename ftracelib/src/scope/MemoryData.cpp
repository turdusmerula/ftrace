/**
 @file		MemoryData.cpp
 @author	S. Besombes
 @date		Mars 2015
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

#include "scope/MemoryData.h"

using namespace ftrace ;

MemoryData::MemoryData()
    :    blocksAllocated_(0),
         bytesAllocated_(0),
         blocksFreed_(0),
         bytesFreed_(0)
{

}
