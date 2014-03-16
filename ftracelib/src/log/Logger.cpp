/**
  @file		Logger.cpp
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Logger for statistics files.

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

#include "com/Demangling.h"
#include "log/Logger.h"
#include "com/Timing.h"
#include "scope/Scope.h"
#include "com/ThreadData.h"

#include <fstream>
#include <iomanip>
#include <iostream>

//-----------------------------------------------------------------------------
std::list<Logger*>* Logger::_loggers=NULL ;
__thread Logger* Logger::_threadRootLogger=NULL ;
__thread bool Logger::_threadLogMessagesFlag=false ;
__thread int Logger::_threadLogTabs=0 ;
__thread std::vector<bool>* Logger::_threadScopeChildExist=NULL ;
__thread int Logger::_threadDepth=0 ;

//-----------------------------------------------------------------------------
bool Logger::isFiltered(const std::string& name_, Logger* logger_)
{
    //puts("Logger::isFiltered") ;
    bool Ret=false ;

    for(unsigned int filter=0 ; filter<logger_->_pattern.size() ; filter++)
    {
        //boost::regex reTrace("trace=(true|false)", boost::regex_constants::extended);
        if(boost::regex_match(name_, logger_->_pattern.at(filter)->_pattern)==true)
        {
            if(logger_->_pattern.at(filter)->_ignore==true)
                Ret = true ;
            else
                Ret = false ;
        }
    }

    return Ret ;
}

//-----------------------------------------------------------------------------
void Logger::logStats()
{
    //puts("Logger::logStats") ;

    //Lock all existing threads before logging to avoid data corruption because during the execution of this
    //function some remaining alive threads may continue working.
    std::list<ThreadData*>::iterator ithread ;
	for(ithread=ThreadData::_threads->begin() ; ithread!=ThreadData::_threads->end() ; ithread++)
	{
        //test if scope must be filtered
		(*ithread)->lock() ;
	}


	//Init datas
    if(Logger::_threadScopeChildExist==NULL)
    {
        _threadScopeChildExist = new std::vector<bool>() ;
    }

	//Output file creation
	std::string filename ;

	filename = "/tmp" ;
	filename += "/" ;
	filename += boost::lexical_cast<std::string>(getpid()) ;
	filename += "_" ;
	filename += Timing::getDate() ;
	filename += "_" ;
	filename += __progname ;
	filename += ".stats" ;

    std::ofstream fout ;
    fout.open(filename.c_str(), std::ios::out) ;

    if(_loggers!=NULL)
    {
    	//printf("%ld\n", _loggers->size()) ;

    	std::list<Logger*>::iterator ilogger ;
    	for(ilogger=_loggers->begin() ; ilogger!=_loggers->end() ; ilogger++)
		{
			if((*ilogger)->_root==false)
			{
				//Only print non root loggers

				fout << "_______________________________________________________________________________" << std::endl  ;
				fout << "Thread " <<  (*ilogger)->_threadData->_threadId << std::endl << std::endl ;

				if((*ilogger)->_treeLog==false)
				{
					Scope::computeGlobalScopes((*ilogger)->_threadData) ;
					Logger::logGlobalScopes(*ilogger, (*ilogger)->_threadData, fout) ;
					Logger::logColumns(*ilogger, fout) ;
				}
				else
				{
					Logger::logTreeScopes(*ilogger, fout) ;
					Logger::logColumns(*ilogger, fout) ;
				}
				fout << std::endl << std::endl << std::endl ;
			}
		}
    }
    fout.close() ;

    //Unlock all thread
	for(ithread=ThreadData::_threads->begin() ; ithread!=ThreadData::_threads->end() ; ithread++)
	{
        //test if scope must be filtered
		(*ithread)->unlock() ;
	}

}

//-----------------------------------------------------------------------------
void Logger::addColumn(Column::ColEnum type_, Logger* logger_)
{
    //puts("Logger::addColumn") ;

	Column* col=new Column ;
    logger_->_columns.push_back(col) ;
    col->_type = type_ ;
    col->_width = 0 ;

    switch(type_)
    {
    case Column::eCall:
        col->_type = Column::eCall ;
        col->_title = "Calls" ;
        break ;
    case Column::eAddress:
        col->_type = Column::eAddress ;
        col->_title = "Address" ;
        break ;
    case Column::eSource:
        col->_type = Column::eSource ;
        col->_title = "Source" ;
        break ;
    case Column::eName:
        col->_type = Column::eName ;
        col->_title = "Name" ;
        break ;
    case Column::eTreeName:
        col->_type = Column::eTreeName ;
        col->_title = "Name" ;
        break ;
    case Column::eTotalTime:
        col->_type = Column::eTotalTime ;
        col->_title = "TotalTime" ;
        break ;
    case Column::eAvgTime:
        col->_type = Column::eAvgTime ;
        col->_title = "AvgTime" ;
        break ;
    case Column::eTotalScopeTime:
        col->_type = Column::eTotalScopeTime ;
        col->_title = "TotalScopeTime" ;
        break ;
    case Column::eAvgScopeTime:
        col->_type = Column::eAvgScopeTime ;
        col->_title = "AvgScopeTime" ;
        break ;
    case Column::eInstTotalTime:
        col->_type = Column::eInstTotalTime ;
        col->_title = "InstTotalTime" ;
        break ;
    case Column::eInstAvgTime:
        col->_type = Column::eInstAvgTime ;
        col->_title = "InstAvgTime" ;
        break ;
    case Column::eInstTotalScopeTime:
        col->_type = Column::eInstTotalScopeTime ;
        col->_title = "InstTotalScopeTime" ;
        break ;
    case Column::eInstAvgScopeTime:
        col->_type = Column::eInstAvgScopeTime ;
        col->_title = "InstAvgScopeTime" ;
        break ;
    case Column::eRealTotalTime:
        col->_type = Column::eRealTotalTime ;
        col->_title = "RealTotalTime" ;
        break ;
    case Column::eRealAvgTime:
        col->_type = Column::eRealAvgTime ;
        col->_title = "RealAvgTime" ;
        break ;
    case Column::eAllocSize:
        col->_type = Column::eAllocSize ;
        col->_title = "AllocSize" ;
        break ;
    case Column::eAllocNum:
        col->_type = Column::eAllocNum ;
        col->_title = "AllocNum" ;
        break ;
    case Column::eTotalAllocTime:
        col->_type = Column::eTotalAllocTime ;
        col->_title = "TotalAllocTime" ;
        break ;
    case Column::eFreeNum:
        col->_type = Column::eFreeNum ;
        col->_title = "FreeNum" ;
        break ;
    case Column::eAllocSource:
        col->_type = Column::eAllocSource ;
        col->_title = "AllocSource" ;
        break ;
    default:
        col->_type = Column::eError ;
        col->_title = "##ERROR##" ;
    }
    col->_width = col->_title.size() ;
}

//-----------------------------------------------------------------------------
void Logger::logColumns(Logger* logger_, std::ostream &fout_)
{
    //puts("Logger::logColumns") ;

	fout_.setf(std::ios::left) ;

    if(logger_->_columns.size()==0)
    	return ;

    //Display titles
    std::vector<Column*>::iterator icol ;
    for(icol=logger_->_columns.begin() ; icol!=logger_->_columns.end() ; icol++)
    {
        fout_ << std::setw((*icol)->_width+2) << (*icol)->_title << "|" ;
    }
    fout_ << std::endl ;

    //Display data
    for(unsigned int line=0 ; line<logger_->_columns.front()->_lines.size() ; line++)
    {
        std::vector<Column*>::iterator icol ;
        for(icol=logger_->_columns.begin() ; icol!=logger_->_columns.end() ; icol++)
        {
            fout_ << std::setw((*icol)->_width+2) << (*icol)->_lines[line] << "|" ;
        }
        fout_ << std::endl ;
    }
}

//-----------------------------------------------------------------------------
void Logger::logGlobalScopes(Logger* logger_, ThreadData* thread_, std::ostream &fout_)
{
    //puts("Logger::logGlobalScopes") ;

	//Compute all scopes data
    Scope::computeGlobalScopes(thread_) ;

    //Purge current logger before appending data
    for(unsigned int col=0 ; col<logger_->_columns.size() ; col++)
    {
        logger_->_columns[col]->_lines.clear() ;
    }

    //Output global scope stats
	std::map<void*,  Scope*>::iterator iscope ;
	for(iscope=thread_->_threadScopeList->begin() ; iscope!=thread_->_threadScopeList->end() ; iscope++)
	{
        //test if scope must be filtered
        if((iscope->second->_globalScope==NULL && isFiltered(iscope->second->_name, logger_)==false) ||
            (iscope->second->_globalScope!=NULL && isFiltered(iscope->second->_globalScope->_name, logger_)==false))
            logScope(logger_, iscope->second, fout_) ;
	}
}

//-----------------------------------------------------------------------------
void Logger::logScope(Logger* logger_, Scope* scope_, std::ostream& fout_)
{
    //puts("Logger::logScope") ;

	uint64_t ScopeTime ;
    bool ShowAllocSource=false ;    //Indicates if column eAllocSource is present

	_threadLogTabs++ ;

    ScopeTime = Scope::computeScopeTime(scope_) ;
    std::vector<Column*>::iterator icol ;
    for(icol=logger_->_columns.begin() ; icol!=logger_->_columns.end() ; icol++)
    {
        //Log columns
        switch((*icol)->_type)
        {
        case Column::eCall:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_callNum)) ;
            break ;
        case Column::eAddress:
            if(scope_->_globalScope!=NULL)
                (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_globalScope->_address)) ;
            else
                (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_address)) ;
            break ;
        case Column::eSource:
            if(scope_->_globalScope!=NULL)
                (*icol)->_lines.push_back(scope_->_globalScope->_source) ;
            else
                (*icol)->_lines.push_back(scope_->_source) ;
            break ;
        case Column::eName:
            if(scope_->_globalScope!=NULL)
                (*icol)->_lines.push_back(scope_->_globalScope->_name) ;
            else
                (*icol)->_lines.push_back(scope_->_name) ;
            break ;
        case Column::eTreeName:
            if(scope_->_globalScope!=NULL)
                (*icol)->_lines.push_back(Logger::computeTab()+scope_->_globalScope->_name) ;
            else
                (*icol)->_lines.push_back(computeTab()+scope_->_name) ;
            break ;
        case Column::eTotalTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time, logger_))) ;
            break ;
        case Column::eAvgTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time/scope_->_callNum, logger_))) ;
            break ;
        case Column::eTotalScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(ScopeTime, logger_))) ;
            break ;
        case Column::eAvgScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(ScopeTime/scope_->_callNum, logger_))) ;
            break ;
        case Column::eInstTotalTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_instTime, logger_))) ;
            break ;
        case Column::eInstAvgTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_instTime/scope_->_callNum, logger_))) ;
            break ;
        case Column::eInstTotalScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
            break ;
        case Column::eInstAvgScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
            break ;
        case Column::eRealTotalTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time-scope_->_instTime, logger_))) ;
            break ;
        case Column::eRealAvgTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime((scope_->_time-scope_->_instTime)/scope_->_callNum, logger_))) ;
            break ;
        case Column::eAllocSize:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._allocSize)) ;
            break ;
        case Column::eAllocNum:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._allocNum)) ;
            break ;
        case Column::eTotalAllocTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_scopeMemStats._allocTime, logger_))) ;
            break ;
        case Column::eFreeNum:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._freeNum)) ;
            break ;
        case Column::eAllocSource:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("")) ;
            ShowAllocSource = true ;
            break ;
        default:
            (*icol)->_lines.push_back("ERROR!") ;
        }
        if((*icol)->_lines.back().size()>(*icol)->_width)
            (*icol)->_width = (*icol)->_lines.back().size() ;
    }

    //Log memory stats
    if(ShowAllocSource==true)
        logOwnerBlocks(logger_, scope_, fout_) ;

	_threadLogTabs-- ;
}

//-----------------------------------------------------------------------------
void Logger::logOwnerBlocks(Logger* logger_, Scope* scope_, std::ostream& fout_)
{
    //puts("Logger::logOwnerBlocks") ;

    std::vector< std::pair<Scope*, Scope::MemStats*> >::iterator istats ;
    for(istats=scope_->_memOwnerBlocks.begin() ; istats!=scope_->_memOwnerBlocks.end() ; istats++)
    {
        logOwnerBlock(logger_, istats->first, istats->second, fout_) ;
    }
}

//-----------------------------------------------------------------------------
void Logger::logOwnerBlock(Logger* logger_, Scope* scope_, Scope::MemStats* stats_, std::ostream& fout_)
{
    //puts("Logger::logOwnerBlock") ;

    std::vector<Column*>::iterator icol ;
    for(icol=logger_->_columns.begin() ; icol!=logger_->_columns.end() ; icol++)
    {
        //Log columns
        switch((*icol)->_type)
        {
        case Column::eCall:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eAddress:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eSource:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eName:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eTreeName:
            (*icol)->_lines.push_back(Logger::computeTab()) ;
            break ;
        case Column::eTotalTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eAvgTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eTotalScopeTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eAvgScopeTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eInstTotalTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eInstAvgTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eInstTotalScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
            break ;
        case Column::eInstAvgScopeTime:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
            break ;
        case Column::eRealTotalTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eRealAvgTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eAllocSize:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eAllocNum:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eTotalAllocTime:
            (*icol)->_lines.push_back("") ;
            break ;
        case Column::eFreeNum:
            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(stats_->_freeNum)) ;
            break ;
        case Column::eAllocSource:
            if(scope_->_globalScope!=NULL)
                (*icol)->_lines.push_back(scope_->_globalScope->_name) ;
            else
                (*icol)->_lines.push_back(scope_->_name) ;
            break ;
        default:
            (*icol)->_lines.push_back("ERROR!") ;
        }
        if((*icol)->_lines.back().size()>(*icol)->_width)
            (*icol)->_width = (*icol)->_lines.back().size() ;
    }
}

//-----------------------------------------------------------------------------
void Logger::logTreeScopes(Logger* logger_, std::ostream &fout_)
{
    //puts("Logger::logTreeScopes") ;

    //Purge current logger before appending data
    for(unsigned int col=0 ; col<logger_->_columns.size() ; col++)
    {
        logger_->_columns[col]->_lines.clear() ;
    }

    //Clear Scope tree
    if(logger_->_rootScope!=NULL)
        Scope::recursiveClearScopes(logger_->_rootScope) ;

    //Add the root scope, does not correspond to any function
    Scope* FilterScope=new Scope ;
    FilterScope->_globalScope = NULL ;
    FilterScope->_currTime = 0 ;
    FilterScope->_time = 0 ;
    FilterScope->_instTime = 0 ;
    FilterScope->_callNum = 0 ;
    FilterScope->_parentScope = NULL ;
    FilterScope->_filter = true ;

    //compute filtered tree for this logger
    Scope::recursiveFilterScopes(logger_, logger_->_rootLogger->_rootScope, FilterScope) ;
    Scope::recursiveConcatenateScopes(FilterScope) ;
    Scope::recursiveConcatenateScopesMemstat(FilterScope) ;

    logger_->_rootScope = FilterScope ;

    _threadScopeChildExist->clear() ;
    _threadDepth = 1 ;
    _threadScopeChildExist->push_back(false) ;

	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
	unsigned int scope ;
	for(iscope=logger_->_rootScope->_childScopes.begin(), scope=0 ; iscope!=logger_->_rootScope->_childScopes.end() ; iscope++, scope++)
	{
		//Indique aux niveaux inférieurs que l'on est en train de traiter le dernier élément sur le niveau courant
		if(scope<logger_->_rootScope->_childScopes.size()-1)
			_threadScopeChildExist->at(_threadDepth-1) = true ;
		else
			_threadScopeChildExist->at(_threadDepth-1) = false ;

		Logger::recusiveLogScopes(logger_, iscope->second, fout_) ;
	}
}

//-----------------------------------------------------------------------------
std::string Logger::computeTab()
{
    //puts("Logger::computeTab") ;

    std::string Res ;

	int depth ;
	for(depth=0 ; depth<_threadDepth-1 ; depth++)
	{
		if((*_threadScopeChildExist)[depth]==true)
		{
			Res += ": " ;
		}
		else
		{
			Res += "  " ;
		}
	}

	//Res += ('0'+depth%10) ;
	//Res += " " ;
	Res += "+ " ;

	return Res ;
}

//-----------------------------------------------------------------------------
void Logger::recusiveLogScopes(Logger* logger_, Scope* scope_, std::ostream& fout_)
{
    //puts("Logger::recusiveLogScopes") ;

    logScope(logger_, scope_, fout_) ;

    _threadScopeChildExist->push_back(false) ;
    _threadDepth++ ;

	//Appel de la fonction récursive pour les portées inférieures
	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
	unsigned int scope ;
	for(iscope=scope_->_childScopes.begin(), scope=0 ; iscope!=scope_->_childScopes.end() ; iscope++, scope++)
	{
		//Indique aux niveaux inférieurs que l'on est en train de traiter le dernier élément sur le niveau courant
		if(scope<scope_->_childScopes.size()-1)
			_threadScopeChildExist->at(_threadDepth-1) = true ;
		else
			_threadScopeChildExist->at(_threadDepth-1) = false ;

		recusiveLogScopes(logger_, iscope->second, fout_) ;
	}

    _threadDepth-- ;
    _threadScopeChildExist->pop_back() ;
}



/**
* Print tabs to standard output.
* @param tabNum_ Number of tabs to print.
*/
void Logger::printTabs(int tabNum_)
{
    //puts("Logger::printTabs") ;

    for(int tab=0 ; tab<tabNum_ ; tab++)
        printf("  ") ;
}
//-----------------------------------------------------------------------------

/**
* Print entry or exit point.
* @param entry_ true if entry point must be printed, false else
*/
void Logger::printEntryExit(bool entry_)
{
    //puts("Logger::printEntryExit") ;

    if(entry_==true)
    {
        printTabs(Scope::_threadTabNum) ;
        printf(">>") ;
        Scope::_threadTabNum++ ;
    }
    else
    {
        Scope::_threadTabNum-- ;
        printTabs(Scope::_threadTabNum) ;
        printf("<<") ;

    }
}
//-----------------------------------------------------------------------------

/**
* Debug function used to check call tree.
*/
void Logger::printTree(Scope* data_)
{
    //puts("Logger::printTree") ;

    static int tab=0 ;

    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
    for(iscope=data_->_childScopes.begin() ; iscope!=data_->_childScopes.end() ; iscope++)
    {
        printTabs(tab) ;
        std::cout << ">>" ;
        std::cout << iscope->second->_globalScope->_name << "  " << iscope->first << " [" << iscope->second->_callNum << "]  " << iscope->second->_time << std::endl ;
        tab++ ;

        printTree(iscope->second) ;

        tab-- ;
        printTabs(tab) ;
        std::cout << "<<" ;
        std::cout << iscope->second->_globalScope->_name << "  " << iscope->first << " [" << iscope->second->_callNum << "]" << std::endl ;

    }

    //std::cout << "Time consumed by logger: " << scopeLoggerTime << std::endl ;
}
