/**
  @file		Scope.cpp
  @author	S. Besombes
  @date		january 2010
  @version	$Revision: 1.1.1 $
  @brief	Scope tree function managing.

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

#include "scope/Scope.h"
#include "log/Logger.h"
#include "com/ThreadData.h"

using namespace ftrace ;

std::map<void*,  ScopeDescriptor*>* Scope::descriptors_=nullptr ;
thread_local std::map<void*, ScopeData*>* Scope::threadDatas_=nullptr ;
std::map<void*, ScopeData*>* Scope::processDatas_=nullptr ;
uint64_t Scope::nextId_=0 ;


Scope::Scope(void* _address)
    : descriptor_(nullptr),
      parentScope_(nullptr)
{
    childs_= new std::vector< std::pair<uint64_t,  Scope*> > ;

    if(descriptors_==nullptr)
        descriptors_ = new std::map<void*,  ScopeDescriptor*> ;
    if(threadDatas_==nullptr)
        threadDatas_ = new std::map<void*, ScopeData*> ;
    if(processDatas_==nullptr)
        processDatas_ = new std::map<void*, ScopeData*> ;

    data_ = new ScopeData ;

    // nullptr address means that we are creating a rootScope, there won't be any descriptor
    if(_address)
    {
        std::map<void*, ScopeData*>::iterator ithreaddata ;
        ithreaddata = threadDatas_->find(_address) ;
        if(ithreaddata==threadDatas_->end())
            threadDatas_->operator[](_address) = new ScopeData ;
        threadData_ = threadDatas_->operator[](_address) ;

        std::map<void*, ScopeData*>::iterator iprocessdata ;
        iprocessdata = processDatas_->find(_address) ;
        if(iprocessdata==processDatas_->end())
            processDatas_->operator[](_address) = new ScopeData ;
        processData_ = processDatas_->operator[](_address) ;
    }

    id_ = nextId_ ;
    nextId_++ ;
}

//-----------------------------------------------------------------------------
std::vector< std::pair<uint64_t,  Scope*> >::iterator Scope::findChild(void* func_, std::vector< std::pair<uint64_t,  Scope*> >::iterator* istart_)
{
    //TODO: optimize this function
    std::vector< std::pair<uint64_t,  Scope*> >::iterator iup ;

    //The up iterator search from the beginning to the end, down iterator search from end to beginning
    iup = childs_->begin() ;

    while(iup!=childs_->end())
    {
        if((*iup).second->descriptor_ && (*iup).second->descriptor_->address_==func_)
            return iup ;

        iup++ ;
    }

    return childs_->end() ;

}
