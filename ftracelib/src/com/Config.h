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
	/**
    * Read the configuration file and create the loggers.
    * @param filename_ File name and path of file to open
    */
    static bool loadConfFile(const std::string &filename_)
    	__attribute__((no_instrument_function)) ;
} ;

#endif
