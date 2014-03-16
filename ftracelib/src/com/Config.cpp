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


//-----------------------------------------------------------------------------
bool Config::loadConfFile(const std::string &filename_)
{
    //puts("Config::loadConfFile") ;
	FILE* file ;
    char buff[1000] ;

    //Patterns to match
    boost::regex reTrace("trace=(true|false)", boost::regex_constants::extended);
    boost::regex rePattern("(ignore|include) (.+)", boost::regex_constants::extended) ;
    boost::regex reLogger("logger=(tree|global)/(auto|second|ticks)/(.*)", boost::regex_constants::extended) ;
    boost::regex reColumn("("
    		"%Name|"
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
    		"%Address|"
    		"%Source|"
    		"%AllocSize|"
    		"%AllocNum|"
    		"%TotalAllocTime|"
    		"%FreeNum|"
    		"%AllocSource"
    		")+", boost::regex_constants::extended) ;
    boost::regex reComment("#.*", boost::regex_constants::extended) ;
    boost::regex reSpace("^[ \t\n\r]*$", boost::regex_constants::extended) ;
    boost::regex reManager("manager=(efence|simple|none)", boost::regex_constants::extended) ;
    boost::regex reEF_ALIGNMENT("EF_ALIGNMENT=[0-9]+", boost::regex_constants::extended) ;
    boost::regex reEF_PROTECT_FREE("EF_PROTECT_FREE=(true|false)", boost::regex_constants::extended) ;
    boost::regex reEF_PROTECT_BELOW("EF_PROTECT_BELOW=(true|false)", boost::regex_constants::extended) ;
    boost::regex reEF_ALLOW_MALLOC_0("EF_ALLOW_MALLOC_0=(true|false)", boost::regex_constants::extended) ;
    boost::regex reEF_FREE_WIPES("EF_FREE_WIPES=(true|false)", boost::regex_constants::extended) ;

    //Matches for the pattern
    boost::cmatch matches ;

    Logger* currLogger=Logger::_threadRootLogger ;

    //Open file
    file = fopen(filename_.c_str() , "rb" ) ;
    if(file==NULL)
    {
    	std::cerr << "Error opening file " << filename_ << std::endl ;
    	return false ;
    }

    Logger::_threadLogMessagesFlag = false ;

    int line=0 ;
    do
    {
        char* res=fgets(buff, 1000, file) ;
        buff[strlen(buff)-1] = '\0' ;
        line++ ;

        if(res!=NULL)
        {
            //printf("%s\n", buff) ;

            if(boost::regex_match(buff, matches, reTrace))
            {
                //printf("------ reTrace !! ------\n") ;
                //Set trace mode on/off (off is default)
                if(matches[1]=="true")
                    Logger::_threadLogMessagesFlag = true ;
            }
            else if(boost::regex_match(buff, matches, rePattern))
            {
                //printf("------ rePattern !! ------\n") ;
                //Add an ignore/include pattern to current logger
                Pattern* pattern=new Pattern ;
                if(matches[1]=="ignore")
                    pattern->_ignore = true ;
                else
                    pattern->_ignore = false ;
                pattern->_pattern = boost::regex(std::string(matches[2]),  boost::regex_constants::extended) ;
                currLogger->_pattern.push_back(pattern) ;
            }
            else if(boost::regex_match(buff, matches, reLogger))
            {
                //printf("------ reLogger !! ------\n") ;
                currLogger = new Logger ;
                currLogger->_root = false ;
                currLogger->_rootLogger = Logger::_threadRootLogger ;
                currLogger->_threadData = Logger::_threadRootLogger->_threadData ;
                Logger::_loggers->push_back(currLogger) ;

                if(matches[1]=="global")
                    currLogger->_treeLog = false ;
                else
                    currLogger->_treeLog = true ;

                if(matches[2]=="auto")
                    currLogger->_timing = Logger::eAuto ;
                else if(matches[2]=="second")
                    currLogger->_timing = Logger::eSecond ;
                else
                    currLogger->_timing = Logger::eTicks ;

                std::string cols=matches[3] ;
                if(boost::regex_match(cols.c_str(), matches, reColumn))
                {
                	typedef boost::tokenizer<boost::char_separator<char> > tokenizer ;
                	boost::char_separator<char> sep("%") ;
                	tokenizer tokens(cols, sep) ;

                	for(tokenizer::iterator tok_iter=tokens.begin() ; tok_iter!=tokens.end() ; ++tok_iter)
                	{
						std::string col="%"+*tok_iter ; //.substr(0, 2) ;

						if(col=="%Call")
							Logger::addColumn(Column::eCall, currLogger) ;
						else if(col=="%Name" && currLogger->_treeLog==false)
							Logger::addColumn(Column::eName, currLogger) ;
						else if(col=="%Name" && currLogger->_treeLog==true)
							Logger::addColumn(Column::eTreeName, currLogger) ;
						else if(col=="%TotalTime")
							Logger::addColumn(Column::eTotalTime, currLogger) ;
						else if(col=="%AvgTime")
							Logger::addColumn(Column::eAvgTime, currLogger) ;
						else if(col=="%TotalScopeTime")
							Logger::addColumn(Column::eTotalScopeTime, currLogger) ;
						else if(col=="%AvgScopeTime")
							Logger::addColumn(Column::eAvgScopeTime, currLogger) ;
						else if(col=="%InstTotalTime")
							Logger::addColumn(Column::eInstTotalTime, currLogger) ;
						else if(col=="%InstAvgTime")
							Logger::addColumn(Column::eInstAvgTime, currLogger) ;
						else if(col=="%InstTotalScopeTime")
							Logger::addColumn(Column::eInstTotalScopeTime, currLogger) ;
						else if(col=="%InstAvgScopeTime")
							Logger::addColumn(Column::eInstAvgScopeTime, currLogger) ;
						else if(col=="%RealTotalTime")
							Logger::addColumn(Column::eRealTotalTime, currLogger) ;
						else if(col=="%RealAvgTime")
							Logger::addColumn(Column::eRealAvgTime, currLogger) ;
						else if(col=="%Address")
							Logger::addColumn(Column::eAddress, currLogger) ;
						else if(col=="%Source")
							Logger::addColumn(Column::eSource, currLogger) ;
						else if(col=="%AllocSize")
							Logger::addColumn(Column::eAllocSize, currLogger) ;
						else if(col=="%AllocNum")
							Logger::addColumn(Column::eAllocNum, currLogger) ;
						else if(col=="%TotalAllocTime")
							Logger::addColumn(Column::eTotalAllocTime, currLogger) ;
						else if(col=="%FreeNum")
							Logger::addColumn(Column::eFreeNum, currLogger) ;
						else if(col=="%AllocSource")
							Logger::addColumn(Column::eAllocSource, currLogger) ;
		                else
		                    fprintf(stderr, "Syntax error at line %d, incorrect column %s !\n", line, col.c_str()) ;
                	}
                }
                else
                    fprintf(stderr, "Syntax error at line %d, incorrect columns %s !\n", line, cols.c_str()) ;

                currLogger->_rootScope = NULL ;
            }
            else if(boost::regex_match(buff, matches, reManager))
            {
            	//Memory manager

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


