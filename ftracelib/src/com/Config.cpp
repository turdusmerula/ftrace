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
    boost::regex reCategory("\\[(global|logger)\\]", boost::regex_constants::extended) ;
    boost::regex reFile("file=(.+)", boost::regex_constants::extended) ;
    boost::regex reType("type=(tree|flat|raw)", boost::regex_constants::extended) ;
    boost::regex reTiming("timing=(auto|seconds|ticks)", boost::regex_constants::extended) ;
    boost::regex reScope("scope=(process|thread)", boost::regex_constants::extended) ;
    boost::regex reTrace("trace=(true|false)", boost::regex_constants::extended);
    boost::regex rePattern("(ignore|include) (.+)", boost::regex_constants::extended)  ;
    boost::regex reColumns("columns=(.*)", boost::regex_constants::extended) ;
    boost::regex reColumn("("
    		"%Name|"
    		"%Address|"
    		"%Source|"
    		"%Call|"
    		"%TotalTime|"
    		"%AvgTime|"
    		"%TotalScopeTime|"
    		"%AvgScopeTime|"
    		"%InstTotalTime|"
    		"%InstAvgTime|"
    		"%InstTotalScopeTime|"
    		"%InstAvgScopeTime|"
    		"%RealTotalTime|"
    		"%RealAvgTime|"
    		"%RealScopeTime|"
    		"%RealAvgScopeTime|"
    		"%AllocSize|"
    		"%AllocNum|"
    		"%TotalAllocTime|"
    		"%FreeNum|"
    		"%AllocSource"
    		")", boost::regex_constants::extended) ;
    boost::regex reComment("#.*", boost::regex_constants::extended) ;
    boost::regex reSpace("^[ \t\n\r]*$", boost::regex_constants::extended) ;
//    boost::regex reManager("manager=(efence|simple|none)", boost::regex_constants::extended) ;
//    boost::regex reEF_ALIGNMENT("EF_ALIGNMENT=[0-9]+", boost::regex_constants::extended) ;
//    boost::regex reEF_PROTECT_FREE("EF_PROTECT_FREE=(true|false)", boost::regex_constants::extended) ;
//    boost::regex reEF_PROTECT_BELOW("EF_PROTECT_BELOW=(true|false)", boost::regex_constants::extended) ;
//    boost::regex reEF_ALLOW_MALLOC_0("EF_ALLOW_MALLOC_0=(true|false)", boost::regex_constants::extended) ;
//    boost::regex reEF_FREE_WIPES("EF_FREE_WIPES=(true|false)", boost::regex_constants::extended) ;

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

            if(boost::regex_match(buff, matches, reCategory))
            {
                printf("------ reCategory !! ------\n") ;
                if(matches[1]=="global")
                    currLogger = Logger::rootLogger_ ;
                else
                {
                    currLogger = new Logger ;
                    currLogger->root_ = false ;
                }
            }
            else if(boost::regex_match(buff, matches, reFile))
            {
                printf("------ reFile !! ------\n") ;

                if(matches[1]!="")
                    currLogger->filename_ = matches[1];
            }
            else if(boost::regex_match(buff, matches, reType))
            {
                printf("------ reType !! ------\n") ;
                if(matches[1]=="tree")
                    currLogger->logType_ = Logger::eTree ;
                else if(matches[2]=="flat")
                    currLogger->logType_ = Logger::eFlat ;
                else
                    currLogger->logType_ = Logger::eRaw ;
            }
            else if(boost::regex_match(buff, matches, reTiming))
            {
                printf("------ reTiming !! ------\n") ;
                if(matches[1]=="auto")
                    currLogger->timing_ = Timing::eAuto ;
                else if(matches[2]=="second")
                    currLogger->timing_ = Timing::eSecond ;
                else
                    currLogger->timing_ = Timing::eTicks ;

            }
            else if(boost::regex_match(buff, matches, reScope))
            {
                printf("------ reScope !! ------\n") ;
                if(matches[1]=="process")
                    currLogger->logScope_ = Logger::eProcess ;
                else
                    currLogger->logScope_ = Logger::eThread ;
            }
            else if(boost::regex_match(buff, matches, reTrace))
            {
                printf("------ reTrace !! ------\n") ;
                //Set trace mode on/off (off is default)
                if(matches[1]=="true")
                    Logger::rootLogger_->trace_ = true ;
            }
            else if(boost::regex_match(buff, matches, rePattern))
            {
                printf("------ rePattern !! ------\n") ;
                //Add an ignore/include pattern to current logger
                Pattern* pattern=new Pattern ;
                if(matches[1]=="ignore")
                    pattern->ignore_ = true ;
                else
                    pattern->ignore_ = false ;
                pattern->pattern_ = boost::regex(std::string(matches[2]),  boost::regex_constants::extended) ;
                currLogger->pattern_.push_back(pattern) ;
            }
            else if(boost::regex_match(buff, matches, reColumns))
            {
                printf("------ reLogger !! ------\n") ;

                typedef boost::tokenizer<boost::char_separator<char> > tokenizer ;
                boost::char_separator<char> sep("%") ;
                tokenizer tokens(std::string(matches[1]), sep) ;

                for(tokenizer::iterator tok_iter=tokens.begin() ; tok_iter!=tokens.end() ; ++tok_iter)
                {
                    std::string col="%"+*tok_iter ; //.substr(0, 2) ;

                    boost::trim(col) ;
                    printf("------ columns: %s !! ------\n", col.c_str()) ;

                    if(col=="%Call")
                        currLogger->addColumn(Column::eCall) ;
//                    else if(col=="%Name" && currLogger->logType_!=Logger::eTree)
//                        currLogger->addColumn(Column::eTreeName) ;
                    else if(col=="%Name")
                        currLogger->addColumn(Column::eName) ;
                    else if(col=="%Source")
                        currLogger->addColumn(Column::eSource) ;
//                    else if(col=="%TotalTime")
//                        currLogger->addColumn(Column::eTotalTime) ;
//                    else if(col=="%AvgTime")
//                        currLogger->addColumn(Column::eAvgTime) ;
//                    else if(col=="%TotalScopeTime")
//                        currLogger->addColumn(Column::eTotalScopeTime) ;
//                    else if(col=="%AvgScopeTime")
//                        currLogger->addColumn(Column::eAvgScopeTime) ;
//                    else if(col=="%InstTotalTime")
//                        currLogger->addColumn(Column::eInstTotalTime) ;
//                    else if(col=="%InstAvgTime")
//                        currLogger->addColumn(Column::eInstAvgTime) ;
//                    else if(col=="%InstTotalScopeTime")
//                        currLogger->addColumn(Column::eInstTotalScopeTime) ;
//                    else if(col=="%InstAvgScopeTime")
//                        currLogger->addColumn(Column::eInstAvgScopeTime) ;
//                    else if(col=="%RealTotalTime")
//                        currLogger->addColumn(Column::eRealTotalTime) ;
//                    else if(col=="%RealAvgTime")
//                        currLogger->addColumn(Column::eRealAvgTime) ;
//                    else if(col=="%Address")
//                        currLogger->addColumn(Column::eAddress) ;
//                    else if(col=="%Source")
//                        currLogger->addColumn(Column::eSource) ;
//                    else if(col=="%AllocSize")
//                        currLogger->addColumn(Column::eAllocSize) ;
//                    else if(col=="%AllocNum")
//                        currLogger->addColumn(Column::eAllocNum) ;
//                    else if(col=="%TotalAllocTime")
//                        currLogger->addColumn(Column::eTotalAllocTime) ;
//                    else if(col=="%FreeNum")
//                        currLogger->addColumn(Column::eFreeNum) ;
//                    else if(col=="%AllocSource")
//                        currLogger->addColumn(Column::eAllocSource) ;
                    else
                        fprintf(stderr, "Syntax error at line %d, incorrect column %s !\n", line, col.c_str()) ;
                }
            }
//            else if(boost::regex_match(buff, matches, reManager))
//            {
//            	//Memory manager
//
//            }
            else if(boost::regex_match(buff, matches, reComment))
            {
                printf("------ reComment !! ------\n") ;
                //Nothing to do with comments
            }
            else if(boost::regex_match(buff, matches, reSpace))
            {
                printf("------ reSpace !! ------\n") ;
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


