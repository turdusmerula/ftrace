/**
  @file		Config.cpp
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

#include "com/Config.h"
#include "log/Logger.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>

using namespace ftrace ;

//-----------------------------------------------------------------------------
bool Config::loadConfFile(const std::string &filename_)
{
    //puts("Config::loadConfFile") ;
	FILE* file ;

    //Patterns to match
    boost::regex rePattern("(ignore|include) (.+)", boost::regex_constants::extended)  ;
    boost::regex reTrace("trace=(true|false)", boost::regex_constants::extended);
    boost::regex reFormat("format=(text|json)", boost::regex_constants::extended);
    boost::regex reComment("#.*", boost::regex_constants::extended) ;
    boost::regex reSpace("^[ \t\n\r]*$", boost::regex_constants::extended) ;

    //Matches for the pattern
    boost::cmatch matches ;

    Logger* currLogger=Logger::rootLogger_ ;

    //Open file
    file = fopen(filename_.c_str() , "rb" ) ;
    if(file==NULL)
    {
    	std::cerr << "Error opening file " << filename_ << std::endl ;
    	exit(1) ;
    }

    // do not log anything by default
    if(Logger::rootLogger_)
        Logger::rootLogger_->trace_ = false ;

    int line=0 ;
    do
    {
        char s[2000] ;
        char* res=fgets(s, 2000, file) ;
        char* buff=s ;
        line++ ;

        if(res!=NULL)
        {
            // ltrim spaces
            while(isspace(*buff))
                buff++ ;
            // remove \n
            while(*res!='\0')
            {
                if(*res=='\n')
                    *res = '\0' ;
                else
                    res ++ ;
            }

            //printf("%s\n", buff) ;

            if(boost::regex_match(buff, matches, rePattern))
            {
                //printf("------ rePattern !! ------\n") ;
                //Add an ignore/include pattern to current logger
                Pattern* pattern=new Pattern ;
                if(matches[1]=="ignore")
                    pattern->ignore_ = true ;
                else
                    pattern->ignore_ = false ;
                pattern->pattern_ = boost::regex(std::string(matches[2]),  boost::regex_constants::extended) ;
                currLogger->pattern_.push_back(pattern) ;
            }
            else if(boost::regex_match(buff, matches, reTrace))
            {
                //printf("------ reTrace !! ------\n") ;
                //Set trace mode on/off (off is default)
                if(matches[1]=="true")
                    Logger::rootLogger_->trace_ = true ;
            }
            else if(boost::regex_match(buff, matches, reFormat))
            {
                //printf("------ reTrace !! ------\n") ;
                //Set trace mode on/off (off is default)
                if(matches[1]=="text")
                    Logger::rootLogger_->format_ = Logger::eText ;
                else if(matches[1]=="json")
                    Logger::rootLogger_->format_ = Logger::eJson ;
            }
            else if(boost::regex_match(buff, matches, reComment))
            {
                //printf("------ reComment !! ------\n") ;
                //Nothing to do with comments
            }
            else if(boost::regex_match(buff, matches, reSpace))
            {
                //printf("------ reSpace !! ------\n") ;
                //Nothing to do with empty lines
            }
            else
            {
                fprintf(stderr, "Syntax error at line %d while opening %s !\n", line, filename_.c_str()) ;
                //Error reading file
                return false ;
            }

//             for (unsigned int i=0 ; i<matches.size() ; i++)
//             {
//                // sub_match::first and sub_match::second are iterators that
//                // refer to the first and one past the last chars of the
//                // matching subexpression
//                string match(matches[i].first, matches[i].second);
//                printf("--> matches[%d]: %s\n", i, match.c_str()) ;
//             }
        }
    }
    while(!feof(file)) ;

    fclose(file) ;
    return true ;
}


