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
#include <mutex>

using namespace ftrace ;

//-----------------------------------------------------------------------------
std::list<Logger*>* Logger::loggers_=nullptr ;
Logger* Logger::rootLogger_=nullptr ;
//__thread std::vector<bool>* Logger::_threadScopeChildExist=NULL ;
thread_local int Logger::threadTab_=0 ;

Logger::Logger()
    :   root_(false),
        timing_(Timing::eAuto),
        trace_(false),
        tab_(0)
{
    if(loggers_==nullptr)
        loggers_ = new std::list<Logger*> ;
//    loggers_->push_back(this) ;

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
        timing_(Timing::eAuto),
        trace_(true),
        tab_(0)
{
}

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


	//Output file creation
	std::string filename ;

	filename = filename_ ;
	filename += "_" ;
	filename += boost::lexical_cast<std::string>(getpid()) ;
	filename += "_" ;
	filename += Timing::getDate() ;
	filename += "_" ;
	filename += __progname ;
    if(format_==eText)
		filename += ".stats" ;
    else
		filename += ".json" ;

    std::ofstream fout ;
    fout.open(filename.c_str(), std::ios::out) ;

    if(format_==eText)
    	logTextScopes(fout) ;
    else
    	logJsonScopes(fout) ;

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
    case Column::eTime:
        col->type_ = Column::eTime ;
        col->title_ = "Time" ;
        break ;
    default:
        col->type_ = Column::eError ;
        col->title_ = "##ERROR##" ;
    }
    col->width_ = col->title_.size() ;
}
//-----------------------------------------------------------------------------

void Logger::logTextScopes(std::ostream &fout)
{
    columns_.clear() ;

    addColumn(Column::eName) ;
    addColumn(Column::eSource) ;
    addColumn(Column::eAddress) ;
    addColumn(Column::eId) ;
    addColumn(Column::eParentId) ;
    addColumn(Column::eCall) ;
    addColumn(Column::eTime) ;

	for(auto& thread : *ThreadData::threads_)
	{
		fout << "_______________________________________________________________________________" << std::endl  ;
		fout << "Thread " <<  thread->id_ << std::endl << std::endl ;

		tab_ = 0 ;

		recursiveLogTextScopes(thread->rootScope_) ;
		logColumns(fout) ;

		fout << std::endl << std::endl << std::endl ;
	}

}
//-----------------------------------------------------------------------------

void Logger::recursiveLogTextScopes(Scope* _scope)
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

        data.time_ = _scope->data_->time_ ;

		logScope(&data) ;
    }

    for(auto& scope : *_scope->childs_)
    {
        recursiveLogTextScopes(scope.second) ;
    }
}
//-----------------------------------------------------------------------------

void Logger::logJsonScopes(std::ostream &fout)
{
	fout << "{" << std::endl ;

	fout << "'functions': [" << std::endl ;
	bool first=true ;
	for(auto& desc : *Scope::descriptors_)
	{
		if(first==false)
			fout << "," << std::endl ;
		fout << "{" ;
		fout << "name:" << "'" << std::hex << desc.second->name_ << "', " ;
		fout << "addr:" << "'" << std::hex << desc.second->address_ << "', " ;
		fout << "source:" << "'" << std::hex << desc.second->mangledName_ << "'" ;
		fout << "}" << std::endl ;

		first = false ;
	}
	fout << "]," << std::endl ;

	first = true ;
	fout << "'threads': [" << std::endl ;
	for(auto& thread : *ThreadData::threads_)
	{
		if(first==false)
			fout << "," << std::endl ;
		fout << "{thread: {" ;
		fout << "'id': " << std::hex << "'" << thread->id_ << "'," ;
		fout << "scopes: [" ;
		recursiveLogJsonScopes(thread->rootScope_, fout) ;
		fout << "]}}" << std::endl ;

		first = false ;
	}
	fout << "]" << std::endl ;

	fout << "}" << std::endl ;
}

//-----------------------------------------------------------------------------
void Logger::recursiveLogJsonScopes(Scope* _scope, std::ostream &fout)
{
    LogScopeData data ;

    if(_scope->descriptor_!=nullptr)
    {
    	fout << "{" ;
    	fout << "'addr': " << std::hex << "'" << _scope->descriptor_->address_ << "'," ;
    	fout << "'calls': " << std::dec << _scope->data_->callNum_ << ","  ;
    	fout << "'time': " << _scope->data_->time_  ;
		if(_scope->childs_->size()>0)
		{
			fout << ", scopes: [" << std::endl ;

			bool first=true ;
			for(auto& scope : *_scope->childs_)
			{
				if(first==false)
					fout << "," ;
				recursiveLogJsonScopes(scope.second, fout) ;
				first = false ;
			}
			fout << "]" ;
		}
		fout << "}" ;

//        data.name = _scope->descriptor_->name_ ;
//        data.source = _scope->descriptor_->mangledName_ ;
//        data.address = _scope->descriptor_->address_ ;
//
//        data.id = _scope->id_ ;
//        if(_scope->parentScope_)
//            data.parentId = _scope->parentScope_->id_ ;
//        else
//            data.parentId = 0 ;
//
//        data.call = _scope->data_->callNum_ ;
//
//        data.time_ = _scope->data_->time_ ;
//
//		logScope(&data) ;
    }
    else
    {
		bool first=true ;
    	for(auto& scope : *_scope->childs_)
        {
    		if(first==false)
    			fout << ", " ;
            recursiveLogJsonScopes(scope.second, fout) ;
            first = false ;
        }
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
        case Column::eTime:
            col->lines_.push_back(boost::lexical_cast<std::string>(Timing::computeTime(_data->time_, timing_))) ;
            break ;
        default:
            col->lines_.push_back("######") ;
        }
        if(col->lines_.back().size()>col->width_)
            col->width_ = col->lines_.back().size() ;
    }

	tab_-- ;
}

//-----------------------------------------------------------------------------
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

void Logger::logScopeEntry(Scope* _scope)
{
	static std::mutex print_mutex;

	if(trace_==false || _scope->descriptor_==nullptr || _scope->descriptor_->filter_==true)
        return ;

	print_mutex.lock() ;

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

	print_mutex.unlock() ;
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

