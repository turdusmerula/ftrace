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
#include <inttypes.h>

using namespace ftrace ;

//-----------------------------------------------------------------------------
std::list<Logger*>* Logger::loggers_=nullptr ;
Logger* Logger::rootLogger_=nullptr ;
//__thread std::vector<bool>* Logger::_threadScopeChildExist=NULL ;
thread_local int Logger::threadTab_=0 ;

Logger::Logger()
    :   root_(false),
        logType_(eTree),
        logScope_(eThread),
        timing_(Timing::eAuto),
        trace_(false),
        tab_(0)
{
    if(loggers_==nullptr)
        loggers_ = new std::list<Logger*> ;
    loggers_->push_back(this) ;

    if(rootLogger_==nullptr)
    {
        //create first logger, using only for display
        rootLogger_ = new Logger(true) ;
        rootLogger_->root_ = true ;
        rootLogger_->timing_ = Timing::eAuto ;
        loggers_->push_back(rootLogger_) ;
    }
}
//-----------------------------------------------------------------------------
Logger::Logger(bool)
    :   root_(false),
        logType_(eTree),
        logScope_(eThread),
        timing_(Timing::eAuto),
        trace_(true),
        tab_(0)
{
}


//    //Initialize logguer
//    std::string logFile=string((getenv("FTRACE")==NULL?"":getenv("FTRACE"))) ;
//    if(logFile=="true")
//    {
//      Logger::_threadLogMessagesFlag = true ;
//    }
//    else if(logFile!="")
//    {
//      Config::loadConfFile(logFile) ;
//    }
//    else
//    {
//      Logger::_threadLogMessagesFlag = false ;
//    }
//

//-----------------------------------------------------------------------------
bool Logger::isFiltered(const std::string& name_)
{
    //puts("Logger::isFiltered") ;
    bool Ret=false ;

    for(unsigned int filter=0 ; filter<pattern_.size() ; filter++)
    {
        //boost::regex reTrace("trace=(true|false)", boost::regex_constants::extended);
        if(boost::regex_match(name_, pattern_.at(filter)->pattern_)==true)
        {
            if(pattern_.at(filter)->ignore_==true)
                Ret = true ;
            else
                Ret = false ;
        }
    }

    return Ret ;
}

//-----------------------------------------------------------------------------
void Logger::log()
{
    //puts("Logger::logStats") ;

    //Only print non root loggers
    if(root_==true)
        return ;

    //Lock all existing threads before logging to avoid data corruption because during the execution of this
    //function some remaining alive threads may continue working.
	for(auto& thread : *ThreadData::threads_)
	{
        //test if scope must be filtered
		thread->lock() ;
	}


	//Init datas
//    if(Logger::_threadScopeChildExist==NULL)
//    {
//        _threadScopeChildExist = new std::vector<bool>() ;
//    }

	//Output file creation
	std::string filename ;

	filename = filename_ ;
	filename += "_" ;
	filename += boost::lexical_cast<std::string>(getpid()) ;
	filename += "_" ;
	filename += Timing::getDate() ;
	filename += "_" ;
	filename += __progname ;
	filename += ".stats" ;

    std::ofstream fout ;
    fout.open(filename.c_str(), std::ios::out) ;

    if(logScope_==eProcess)
    {
    }
    else
    {
        for(auto& thread : *ThreadData::threads_)
        {
            fout << "_______________________________________________________________________________" << std::endl  ;
            fout << "Thread " <<  thread->id_ << std::endl << std::endl ;

            tab_ = 0 ;

            switch(logType_)
            {
            case eRaw:
                logRawScopes(thread->rootScope_) ;
                logColumns(fout) ;
                break ;
            }
//            if((*ilogger)->_treeLog==false)
//            {
//                Scope::computeGlobalScopes((*ilogger)->_threadData) ;
//                Logger::logGlobalScopes(*ilogger, (*ilogger)->_threadData, fout) ;
//                Logger::logColumns(*ilogger, fout) ;
//            }
//            else
//            {
//                Logger::logTreeScopes(*ilogger, fout) ;
//                Logger::logColumns(*ilogger, fout) ;
//            }
            fout << std::endl << std::endl << std::endl ;

        }
    }

    fout.close() ;

    //Unlock all thread
    for(auto& thread : *ThreadData::threads_)
	{
        //test if scope must be filtered
		thread->unlock() ;
	}

}

//-----------------------------------------------------------------------------
void Logger::addColumn(Column::ColEnum _type)
{
    //puts("Logger::addColumn") ;

	Column* col=new Column ;
    columns_.push_back(col) ;
    col->type_ = _type ;
    col->width_ = 0 ;

    switch(_type)
    {
    case Column::eName:
        col->type_ = Column::eName ;
        col->title_ = "Name" ;
        break ;
    case Column::eSource:
        col->type_ = Column::eSource ;
        col->title_ = "Source" ;
        break ;
    case Column::eAddress:
        col->type_ = Column::eAddress ;
        col->title_ = "Address" ;
        break ;
    case Column::eId:
        col->type_ = Column::eId ;
        col->title_ = "Id" ;
        break ;
    case Column::eParentId:
        col->type_ = Column::eParentId ;
        col->title_ = "Parent Id" ;
        break ;
    case Column::eCall:
        col->type_ = Column::eCall ;
        col->title_ = "Calls" ;
        break ;
//    case Column::eTreeName:
//        col->type_ = Column::eTreeName ;
//        col->title_ = "Name" ;
//        break ;
//    case Column::eTotalTime:
//        col->type_ = Column::eTotalTime ;
//        col->title_ = "TotalTime" ;
//        break ;
//    case Column::eAvgTime:
//        col->type_ = Column::eAvgTime ;
//        col->title_ = "AvgTime" ;
//        break ;
//    case Column::eTotalScopeTime:
//        col->type_ = Column::eTotalScopeTime ;
//        col->title_ = "TotalScopeTime" ;
//        break ;
//    case Column::eAvgScopeTime:
//        col->type_ = Column::eAvgScopeTime ;
//        col->title_ = "AvgScopeTime" ;
//        break ;
//    case Column::eInstTotalTime:
//        col->type_ = Column::eInstTotalTime ;
//        col->title_ = "InstTotalTime" ;
//        break ;
//    case Column::eInstAvgTime:
//        col->type_ = Column::eInstAvgTime ;
//        col->title_ = "InstAvgTime" ;
//        break ;
//    case Column::eInstTotalScopeTime:
//        col->type_ = Column::eInstTotalScopeTime ;
//        col->title_ = "InstTotalScopeTime" ;
//        break ;
//    case Column::eInstAvgScopeTime:
//        col->type_ = Column::eInstAvgScopeTime ;
//        col->title_ = "InstAvgScopeTime" ;
//        break ;
//    case Column::eRealTotalTime:
//        col->type_ = Column::eRealTotalTime ;
//        col->title_ = "RealTotalTime" ;
//        break ;
//    case Column::eRealAvgTime:
//        col->type_ = Column::eRealAvgTime ;
//        col->title_ = "RealAvgTime" ;
//        break ;
//    case Column::eAllocSize:
//        col->type_ = Column::eAllocSize ;
//        col->title_ = "AllocSize" ;
//        break ;
//    case Column::eAllocNum:
//        col->type_ = Column::eAllocNum ;
//        col->title_ = "AllocNum" ;
//        break ;
//    case Column::eTotalAllocTime:
//        col->type_ = Column::eTotalAllocTime ;
//        col->title_ = "TotalAllocTime" ;
//        break ;
//    case Column::eFreeNum:
//        col->type_ = Column::eFreeNum ;
//        col->title_ = "FreeNum" ;
//        break ;
//    case Column::eAllocSource:
//        col->type_ = Column::eAllocSource ;
//        col->title_ = "AllocSource" ;
//        break ;
    default:
        col->type_ = Column::eError ;
        col->title_ = "##ERROR##" ;
    }
    col->width_ = col->title_.size() ;
}
//-----------------------------------------------------------------------------

void Logger::logRawScopes(Scope* _scope)
{
    columns_.clear() ;

    addColumn(Column::eName) ;
    addColumn(Column::eSource) ;
    addColumn(Column::eAddress) ;
    addColumn(Column::eId) ;
    addColumn(Column::eParentId) ;
    addColumn(Column::eCall) ;

    recursiveLogRawScopes(_scope) ;
}
//-----------------------------------------------------------------------------

void Logger::recursiveLogRawScopes(Scope* _scope)
{
    LogScopeData data ;

    if(_scope->descriptor_!=nullptr)
    {
        data.name = _scope->descriptor_->name_ ;
        data.source = _scope->descriptor_->mangledName_ ;
        data.address = _scope->descriptor_->address_ ;
        data.id = _scope->id_ ;
        if(_scope->parentScope_)
            data.parentId = _scope->parentScope_->id_ ;
        else
            data.parentId = 0 ;

        data.call = _scope->data_->callNum_ ;

        logScope(&data) ;
    }

    for(auto& scope : *_scope->childs_)
    {
        recursiveLogRawScopes(scope.second) ;
    }
}
//-----------------------------------------------------------------------------

void Logger::logColumns(std::ostream &fout_)
{
    //puts("Logger::logColumns") ;

	fout_.setf(std::ios::left) ;

    if(columns_.size()==0)
    	return ;

    //Display titles
    std::vector<Column*>::iterator icol ;
    for(icol=columns_.begin() ; icol!=columns_.end() ; icol++)
    {
        fout_ << std::setw((*icol)->width_+2) << (*icol)->title_ << "|" ;
    }
    fout_ << std::endl ;

    //Display data
    for(unsigned int line=0 ; line<columns_.front()->lines_.size() ; line++)
    {
        std::vector<Column*>::iterator icol ;
        for(icol=columns_.begin() ; icol!=columns_.end() ; icol++)
        {
            fout_ << std::setw((*icol)->width_+2) << (*icol)->lines_[line] << "|" ;
        }
        fout_ << std::endl ;
    }
}
//
////-----------------------------------------------------------------------------
//void Logger::logGlobalScopes(Logger* logger_, ThreadData* thread_, std::ostream &fout_)
//{
//    //puts("Logger::logGlobalScopes") ;
//
//	//Compute all scopes data
//    Scope::computeGlobalScopes(thread_) ;
//
//    //Purge current logger before appending data
//    for(unsigned int col=0 ; col<logger_->_columns.size() ; col++)
//    {
//        logger_->_columns[col]->_lines.clear() ;
//    }
//
//    //Output global scope stats
//	std::map<void*,  Scope*>::iterator iscope ;
//	for(iscope=thread_->_threadScopeList->begin() ; iscope!=thread_->_threadScopeList->end() ; iscope++)
//	{
//        //test if scope must be filtered
//        if((iscope->second->_globalScope==NULL && isFiltered(iscope->second->_name, logger_)==false) ||
//            (iscope->second->_globalScope!=NULL && isFiltered(iscope->second->_globalScope->_name, logger_)==false))
//            logScope(logger_, iscope->second, fout_) ;
//	}
//}
//
//-----------------------------------------------------------------------------
void Logger::logScope(LogScopeData* _data)
{
    //puts("Logger::logScope") ;

	tab_++ ;

	for(auto& col : columns_)
    {
        //Log columns
        switch(col->type_)
        {
        case Column::eCall:
            col->lines_.push_back(boost::lexical_cast<std::string>(_data->call)) ;
            break ;
        case Column::eAddress:
            col->lines_.push_back(boost::lexical_cast<std::string>(_data->address)) ;
            break ;
        case Column::eId:
            col->lines_.push_back(boost::lexical_cast<std::string>(_data->id)) ;
            break ;
        case Column::eParentId:
            col->lines_.push_back(boost::lexical_cast<std::string>(_data->parentId)) ;
            break ;
        case Column::eSource:
            col->lines_.push_back(_data->source) ;
            break ;
        case Column::eName:
            col->lines_.push_back(_data->name) ;
            break ;
//        case Column::eTreeName:
//            if(scope_->_globalScope!=NULL)
//                (*icol)->_lines.push_back(Logger::computeTab()+scope_->_globalScope->_name) ;
//            else
//                (*icol)->_lines.push_back(computeTab()+scope_->_name) ;
//            break ;
//        case Column::eTotalTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time, logger_))) ;
//            break ;
//        case Column::eAvgTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time/scope_->_callNum, logger_))) ;
//            break ;
//        case Column::eTotalScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(ScopeTime, logger_))) ;
//            break ;
//        case Column::eAvgScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(ScopeTime/scope_->_callNum, logger_))) ;
//            break ;
//        case Column::eInstTotalTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_instTime, logger_))) ;
//            break ;
//        case Column::eInstAvgTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_instTime/scope_->_callNum, logger_))) ;
//            break ;
//        case Column::eInstTotalScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
//            break ;
//        case Column::eInstAvgScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
//            break ;
//        case Column::eRealTotalTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_time-scope_->_instTime, logger_))) ;
//            break ;
//        case Column::eRealAvgTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime((scope_->_time-scope_->_instTime)/scope_->_callNum, logger_))) ;
//            break ;
//        case Column::eAllocSize:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._allocSize)) ;
//            break ;
//        case Column::eAllocNum:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._allocNum)) ;
//            break ;
//        case Column::eTotalAllocTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(Timing::computeTime(scope_->_scopeMemStats._allocTime, logger_))) ;
//            break ;
//        case Column::eFreeNum:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(scope_->_scopeMemStats._freeNum)) ;
//            break ;
//        case Column::eAllocSource:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("")) ;
//            ShowAllocSource = true ;
//            break ;
        default:
            col->lines_.push_back("######") ;
        }
        if(col->lines_.back().size()>col->width_)
            col->width_ = col->lines_.back().size() ;
    }

	tab_-- ;
}

////-----------------------------------------------------------------------------
//void Logger::logOwnerBlocks(Logger* logger_, Scope* scope_, std::ostream& fout_)
//{
//    //puts("Logger::logOwnerBlocks") ;
//
//    std::vector< std::pair<Scope*, Scope::MemStats*> >::iterator istats ;
//    for(istats=scope_->_memOwnerBlocks.begin() ; istats!=scope_->_memOwnerBlocks.end() ; istats++)
//    {
//        logOwnerBlock(logger_, istats->first, istats->second, fout_) ;
//    }
//}
//
////-----------------------------------------------------------------------------
//void Logger::logOwnerBlock(Logger* logger_, Scope* scope_, Scope::MemStats* stats_, std::ostream& fout_)
//{
//    //puts("Logger::logOwnerBlock") ;
//
//    std::vector<Column*>::iterator icol ;
//    for(icol=logger_->_columns.begin() ; icol!=logger_->_columns.end() ; icol++)
//    {
//        //Log columns
//        switch((*icol)->_type)
//        {
//        case Column::eCall:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eAddress:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eSource:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eName:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eTreeName:
//            (*icol)->_lines.push_back(Logger::computeTab()) ;
//            break ;
//        case Column::eTotalTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eAvgTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eTotalScopeTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eAvgScopeTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eInstTotalTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eInstAvgTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eInstTotalScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
//            break ;
//        case Column::eInstAvgScopeTime:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>("ERROR!")) ;
//            break ;
//        case Column::eRealTotalTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eRealAvgTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eAllocSize:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eAllocNum:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eTotalAllocTime:
//            (*icol)->_lines.push_back("") ;
//            break ;
//        case Column::eFreeNum:
//            (*icol)->_lines.push_back(boost::lexical_cast<std::string>(stats_->_freeNum)) ;
//            break ;
//        case Column::eAllocSource:
//            if(scope_->_globalScope!=NULL)
//                (*icol)->_lines.push_back(scope_->_globalScope->_name) ;
//            else
//                (*icol)->_lines.push_back(scope_->_name) ;
//            break ;
//        default:
//            (*icol)->_lines.push_back("ERROR!") ;
//        }
//        if((*icol)->_lines.back().size()>(*icol)->_width)
//            (*icol)->_width = (*icol)->_lines.back().size() ;
//    }
//}
//
////-----------------------------------------------------------------------------
//void Logger::logTreeScopes(Logger* logger_, std::ostream &fout_)
//{
//    //puts("Logger::logTreeScopes") ;
//
//    //Purge current logger before appending data
//    for(unsigned int col=0 ; col<logger_->_columns.size() ; col++)
//    {
//        logger_->_columns[col]->_lines.clear() ;
//    }
//
//    //Clear Scope tree
//    if(logger_->_rootScope!=NULL)
//        Scope::recursiveClearScopes(logger_->_rootScope) ;
//
//    //Add the root scope, does not correspond to any function
//    Scope* FilterScope=new Scope ;
//    FilterScope->_globalScope = NULL ;
//    FilterScope->_currTime = 0 ;
//    FilterScope->_time = 0 ;
//    FilterScope->_instTime = 0 ;
//    FilterScope->_callNum = 0 ;
//    FilterScope->_parentScope = NULL ;
//    FilterScope->_filter = true ;
//
//    //compute filtered tree for this logger
//    Scope::recursiveFilterScopes(logger_, logger_->_rootLogger->_rootScope, FilterScope) ;
//    Scope::recursiveConcatenateScopes(FilterScope) ;
//    Scope::recursiveConcatenateScopesMemstat(FilterScope) ;
//
//    logger_->_rootScope = FilterScope ;
//
//    _threadScopeChildExist->clear() ;
//    _threadDepth = 1 ;
//    _threadScopeChildExist->push_back(false) ;
//
//	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
//	unsigned int scope ;
//	for(iscope=logger_->_rootScope->_childScopes.begin(), scope=0 ; iscope!=logger_->_rootScope->_childScopes.end() ; iscope++, scope++)
//	{
//		//Indique aux niveaux inférieurs que l'on est en train de traiter le dernier élément sur le niveau courant
//		if(scope<logger_->_rootScope->_childScopes.size()-1)
//			_threadScopeChildExist->at(_threadDepth-1) = true ;
//		else
//			_threadScopeChildExist->at(_threadDepth-1) = false ;
//
//		Logger::recusiveLogScopes(logger_, iscope->second, fout_) ;
//	}
//}
//
////-----------------------------------------------------------------------------
//std::string Logger::computeTab()
//{
//    //puts("Logger::computeTab") ;
//
//    std::string Res ;
//
//	int depth ;
//	for(depth=0 ; depth<_threadDepth-1 ; depth++)
//	{
//		if((*_threadScopeChildExist)[depth]==true)
//		{
//			Res += ": " ;
//		}
//		else
//		{
//			Res += "  " ;
//		}
//	}
//
//	//Res += ('0'+depth%10) ;
//	//Res += " " ;
//	Res += "+ " ;
//
//	return Res ;
//}
//
////-----------------------------------------------------------------------------
//void Logger::recusiveLogScopes(Logger* logger_, Scope* scope_, std::ostream& fout_)
//{
//    //puts("Logger::recusiveLogScopes") ;
//
//    logScope(logger_, scope_, fout_) ;
//
//    _threadScopeChildExist->push_back(false) ;
//    _threadDepth++ ;
//
//	//Appel de la fonction récursive pour les portées inférieures
//	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
//	unsigned int scope ;
//	for(iscope=scope_->_childScopes.begin(), scope=0 ; iscope!=scope_->_childScopes.end() ; iscope++, scope++)
//	{
//		//Indique aux niveaux inférieurs que l'on est en train de traiter le dernier élément sur le niveau courant
//		if(scope<scope_->_childScopes.size()-1)
//			_threadScopeChildExist->at(_threadDepth-1) = true ;
//		else
//			_threadScopeChildExist->at(_threadDepth-1) = false ;
//
//		recusiveLogScopes(logger_, iscope->second, fout_) ;
//	}
//
//    _threadDepth-- ;
//    _threadScopeChildExist->pop_back() ;
//}
//
//
//
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
        printTabs(threadTab_) ;
        printf(">>") ;
        threadTab_++ ;
    }
    else
    {
        threadTab_-- ;
        printTabs(threadTab_) ;
        printf("<<") ;

    }
}
//-----------------------------------------------------------------------------
//
///**
//* Debug function used to check call tree.
//*/
//void Logger::printTree(Scope* data_)
//{
//    //puts("Logger::printTree") ;
//
//    static int tab=0 ;
//
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=data_->_childScopes.begin() ; iscope!=data_->_childScopes.end() ; iscope++)
//    {
//        printTabs(tab) ;
//        std::cout << ">>" ;
//        std::cout << iscope->second->_globalScope->_name << "  " << iscope->first << " [" << iscope->second->_callNum << "]  " << iscope->second->_time << std::endl ;
//        tab++ ;
//
//        printTree(iscope->second) ;
//
//        tab-- ;
//        printTabs(tab) ;
//        std::cout << "<<" ;
//        std::cout << iscope->second->_globalScope->_name << "  " << iscope->first << " [" << iscope->second->_callNum << "]" << std::endl ;
//
//    }
//
//    //std::cout << "Time consumed by logger: " << scopeLoggerTime << std::endl ;
//}
//-----------------------------------------------------------------------------

void Logger::logScopeEntry(Scope* _scope)
{
    if(trace_==false || _scope->descriptor_==nullptr || _scope->descriptor_->filter_==true)
        return ;

    printf("%lu\t|", (uint64_t)threadData->number_) ;
    printEntryExit(true) ;
    if(_scope->descriptor_->name_.size()==0)
    {
        printf(" %lx [%lu, %lu, %lu]\n",
            (uint64_t)_scope->descriptor_->address_,
            _scope->data_-> callNum_,
            _scope->threadData_-> callNum_,
            _scope->processData_->callNum_
            ) ;
    }
    else
    {
        printf(" %s [%lu, %lu, %lu]\n",
            _scope->descriptor_->name_.c_str(),
            _scope->data_-> callNum_,
            _scope->threadData_-> callNum_,
            _scope->processData_->callNum_
            ) ;
    }

}
//-----------------------------------------------------------------------------

void Logger::logScopeExit(Scope* _scope)
{
    if(trace_==false || _scope->descriptor_==nullptr || _scope->descriptor_->filter_==true)
        return ;

    printf("%lu\t|", (uint64_t)threadData->number_) ;
    Logger::printEntryExit(false) ;
    if(_scope->descriptor_->name_.size()==0)
    {
        printf(" %lx (%s)\n",
            (uint64_t)_scope->descriptor_->address_,
            Timing::computeTime(_scope->data_->currTime_, timing_).c_str()
            ) ;
    }
    else
    {
        printf(" %s (%s)\n",
            _scope->descriptor_->name_.c_str(),
            Timing::computeTime(_scope->data_->currTime_, timing_).c_str()
            ) ;
    }

}

