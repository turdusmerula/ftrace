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

////-----------------------------------------------------------------------------
//void Scope::computeGlobalScopes(ThreadData* thread_)
//{
//    std::map<void*,  Scope*>::iterator iscope ;
//    for(iscope=thread_->_threadScopeList->begin() ; iscope!=thread_->_threadScopeList->end() ; iscope++)
//    {
//        //Init scope total elapsed time
//        iscope->second->_time = 0 ;
//        iscope->second->_scopeTime = 0 ;
//        iscope->second->_instTime = 0 ;
//
//        //Init scope total calls
//        iscope->second->_callNum = 0 ;
//
//        Scope::computeGlobalMemStats(iscope->second) ;
//    }
//    Scope::recursiveComputeGlobalScopes(thread_->_threadRootLogger->_rootScope) ;
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveComputeGlobalScopes(Scope* scope_)
//{
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//    {
//        if((*iscope).second!=NULL)
//        {
//            //Compute time
//            (*iscope).second->_globalScope->_time += (*iscope).second->_time ;
//            (*iscope).second->_globalScope->_instTime += (*iscope).second->_instTime ;
//            //Compute scope time
//            //(*iscope).second->_scope->_time += (*iscope).second->_time ;
//            //Compute calls
//            (*iscope).second->_globalScope->_callNum += (*iscope).second->_callNum ;
//
//            recursiveComputeGlobalScopes((*iscope).second) ;
//        }
//    }
//}
//
////-----------------------------------------------------------------------------
//uint64_t Scope::computeScopeTime(Scope* scope_)
//{
//	uint64_t Res ;
//
//	Res = scope_->_time ;
//
//	//Substract time elapsed in subscopes
//	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
//	for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//	{
//		if(iscope->second->_time<Res)
//		{
//			Res -= iscope->second->_time ;
//		}
//		else
//		{
//			Res = 0 ;
//		}
//	}
//
//	return Res ;
//}
//
////-----------------------------------------------------------------------------
//uint64_t Scope::computeInstScopeTime(Scope* scope_)
//{
//	uint64_t Res ;
//
//	Res = scope_->_instTime ;
//
//	//Substract time elapsed in subscopes
//	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
//	for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//	{
//		if(iscope->second->_time<Res)
//		{
//			Res -= iscope->second->_instTime ;
//		}
//		else
//		{
//			Res = 0 ;
//		}
//	}
//
//	return Res ;
//}
//
////-----------------------------------------------------------------------------
//uint64_t Scope::recursiveComputeInstScopeTime(Scope* scope_)
//{
//	uint64_t Res ;
//
//	Res = scope_->_instTime ;
//
//	//Soustraction du temps passé dans les sous portées
//	std::vector< std::pair<void*, Scope*> >::iterator iscope ;
//	for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//	{
//		Res += recursiveComputeInstScopeTime((*iscope).second) ;
//
//	}
//
//	return Res ;
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveFilterScopes(Logger* logger_, Scope* srcScope_, Scope* dstScope_)
//{
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=srcScope_->_childScopes.begin() ; iscope!=srcScope_->_childScopes.end() ; iscope++)
//    {
//        Scope* currScope=dstScope_ ;
//
//        //test if scope must be filtered
//        if((iscope->second->_globalScope==NULL && Logger::isFiltered(iscope->second->_name, logger_)==false) ||
//            (iscope->second->_globalScope!=NULL && Logger::isFiltered(iscope->second->_globalScope->_name, logger_)==false))
//        {
//            //Scope is not filtered, add the scope to the new tree
//            currScope = new Scope ;
//            currScope->_name = iscope->second->_name ;
//            currScope->_address = iscope->second->_address ;
//            currScope->_parentScope = dstScope_ ;
//            currScope->_globalScope = iscope->second->_globalScope ;
//            dstScope_->_childScopes.push_back(std::pair<void*,  Scope*>(currScope->_address, currScope)) ;
//            currScope->_currTime = iscope->second->_currTime ;
//            currScope->_time = iscope->second->_time ;
//            currScope->_scopeTime = iscope->second->_scopeTime ;
//            currScope->_instTime = iscope->second-> _instTime;
//            currScope->_callNum = iscope->second->_callNum ;
//            currScope->_filter = iscope->second->_filter ;
//
//            //Initialize memory statistics
//            currScope->_scopeMemStats._allocNum = iscope->second->_scopeMemStats._allocNum ;
//            currScope->_scopeMemStats._allocSize = iscope->second->_scopeMemStats._allocSize ;
//            currScope->_scopeMemStats._allocTime = iscope->second->_scopeMemStats._allocTime ;
//            currScope->_scopeMemStats._freeNum = iscope->second->_scopeMemStats._freeNum ;
//
//            //Copy owner blocks
//            std::vector< std::pair<Scope*, MemStats*> >::iterator iowner ;
//            for(iowner=iscope->second->_memOwnerBlocks.begin() ; iowner!=iscope->second->_memOwnerBlocks.end() ; iowner++)
//                currScope->_memOwnerBlocks.push_back(*iowner) ;
//        }
//        else
//        {
//            currScope->_scopeMemStats._allocNum += iscope->second->_scopeMemStats._allocNum ;
//            currScope->_scopeMemStats._allocSize += iscope->second->_scopeMemStats._allocSize ;
//            currScope->_scopeMemStats._allocTime += iscope->second->_scopeMemStats._allocTime ;
//            currScope->_scopeMemStats._freeNum += iscope->second->_scopeMemStats._freeNum ;
//
//            //Copy owner blocks
//            std::vector< std::pair<Scope*, MemStats*> >::iterator iowner ;
//            for(iowner=iscope->second->_memOwnerBlocks.begin() ; iowner!=iscope->second->_memOwnerBlocks.end() ; iowner++)
//                currScope->_memOwnerBlocks.push_back(*iowner) ;
//        }
//        recursiveFilterScopes(logger_, iscope->second, currScope) ;
//    }
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveConcatenateScopes(Scope* scope_)
//{
////    bool end ;
////
////    std::vector< std::pair<void*,  Scope*> >::iterator iscope0 ;    //Scope n
////
////    do
////    {
////        //No changes to begin
////        end = true ;
////
////        //Loop in child of scope
////        std::vector< std::pair<void*,  Scope*> >::iterator iscope1 ;    //Scope n+1
////        iscope0 = scope_->_childScopes.begin() ;
////        if(iscope0!=scope_->_childScopes.end())
////        {
////            iscope1 = scope_->_childScopes.begin() ;
////            iscope1++ ;
////        }
////        else
////            iscope1 = scope_->_childScopes.begin() ;
////
////        while(iscope1!=scope_->_childScopes.end())
////        {
//////            for(int var=0 ; var<tab ; var++) printf("  ") ;
//////            printf("iscope0: %s\n", iscope0->second->_globalScope->_name.c_str()) ;
//////
////            if(iscope0->second->_globalScope->_address==iscope1->second->_globalScope->_address)
////            {
////                //Concatenate scope0 with scope1
////                iscope0->second->_currTime += iscope1->second->_currTime ;
////                iscope0->second->_time += iscope1->second->_time ;
////                iscope0->second->_scopeTime += iscope1->second->_scopeTime ;
////                iscope0->second->_instTime += iscope1->second-> _instTime;
////                iscope0->second->_callNum += iscope1->second->_callNum ;
////
//////                for(int var=0 ; var<tab ; var++) printf("  ") ;
//////                printf("concatenate: %lx - %lx\n", iscope0->second->_globalScope->_address, iscope1->second->_globalScope->_address) ;
//////                for(int var=0 ; var<tab ; var++) printf("  ") ;
//////                printf("concatenate: %s - %s\n", iscope0->second->_globalScope->_name.c_str(), iscope1->second->_globalScope->_name.c_str()) ;
////
////                //Concatenate memory stats
////                printf("aaaaaaaaaaaaa\n") ;
////                concatenateScopesMemstat(iscope1->second, iscope0->second) ;
////
////                //Place childs of scope1 in scope0
////                std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
////                for(iscope=iscope1->second->_childScopes.begin() ; iscope!=iscope1->second->_childScopes.end() ; iscope++)
////                {
////                    iscope0->second->_childScopes.push_back(std::pair<void*,  Scope*>(iscope0->second->_globalScope->_address, iscope->second)) ;
////                }
////
////                //Remove iscope1
////                delete iscope1->second ;
////                scope_->_childScopes.erase(iscope1) ;
////
////                end = false ;
////
////                //Stop concatenate for this loop
////                break ;
////            }
////            else
////            {
////                iscope0++ ;
////                iscope1++ ;
////            }
////        }
////
////        //Loop while at least one chanche occured or end of list is reached
////    }
////    while(end==false) ;
////
////    //concatenation is done in scope, process remaining childs
////    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
////    for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
////    {
////        recursiveConcatenateScopes(iscope->second) ;
////    }
//
//    bool end ;
//
//    do
//    {
//        //No changes to begin
//        end = true ;
//
//        //Loop in child of scope
//        std::vector< std::pair<void*,  Scope*> >::iterator iscope0 ;    //Scope n
//        for(iscope0=scope_->_childScopes.begin() ; iscope0!=scope_->_childScopes.end() && end==true ; iscope0++)
//        {
//            std::vector< std::pair<void*,  Scope*> >::iterator iscope1 ;    //Scope n+1
//            iscope1 = iscope0 ;
//            iscope1++ ;
//            for( ; iscope1!=scope_->_childScopes.end() && end==true ; iscope1++)
//            {
//                if(iscope0->second->_globalScope->_address==iscope1->second->_globalScope->_address)
//                {
//                    //Concatenate scope0 with scope1
//                    iscope0->second->_currTime += iscope1->second->_currTime ;
//                    iscope0->second->_time += iscope1->second->_time ;
//                    iscope0->second->_scopeTime += iscope1->second->_scopeTime ;
//                    iscope0->second->_instTime += iscope1->second-> _instTime;
//                    iscope0->second->_callNum += iscope1->second->_callNum ;
//
//                    //Concatenate memory stats
//                    concatenateScopesMemstat(iscope1->second, iscope0->second) ;
//
//                    //Place childs of scope1 in scope0
//                    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//                    for(iscope=iscope1->second->_childScopes.begin() ; iscope!=iscope1->second->_childScopes.end() ; iscope++)
//                    {
//                        iscope0->second->_childScopes.push_back(std::pair<void*,  Scope*>(iscope0->second->_globalScope->_address, iscope->second)) ;
//                    }
//
//                    //Remove iscope1
//                    delete iscope1->second ;
//                    scope_->_childScopes.erase(iscope1) ;
//
//                    end = false ;
//                }
//            }
//        }
//
//        //Loop while at least one chanche occured or end of list is reached
//    }
//    while(end==false) ;
//
//    //concatenation is done in scope, process remaining childs
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//    {
//        recursiveConcatenateScopes(iscope->second) ;
//    }
//
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveClearScopes(Scope* scope_)
//{
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//    {
//        recursiveClearScopes(iscope->second) ;
//        delete iscope->second ;
//    }
//}
//
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

////-----------------------------------------------------------------------------
//std::vector< std::pair<Scope*, Scope::MemStats*> >::iterator Scope::find(std::vector< std::pair<Scope*, Scope::MemStats*> >& list_, Scope* scope_, std::vector< std::pair<void*,  Scope*> >::iterator* istart_)
//{
//    std::vector< std::pair<Scope*, MemStats*> >::iterator iup ;
//
//    //The up iterator search from the beginning to the end, down iterator search from end to beginning
//    iup = list_.begin() ;
//
//    while(iup!=list_.end())
//    {
//        if((*iup).first==scope_)
//            return iup ;
//
//        iup++ ;
//    }
//
//    return list_.end() ;
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveConcatenateScopesMemstat(Scope* scope_)
//{
//    bool end ;
//
//    do
//    {
//        //No changes to begin
//        end = true ;
//
//        //Loop in memory stats of scope
//        std::vector< std::pair<Scope*, MemStats*> >::iterator iowner0 ;    //stat n
//        for(iowner0=scope_->_memOwnerBlocks.begin() ; iowner0!=scope_->_memOwnerBlocks.end() && end==true ; iowner0++)
//        {
//            std::vector< std::pair<Scope*, MemStats*> >::iterator iowner1 ;    //stat n+1
//            iowner1 = iowner0 ;
//            iowner1++ ;
//            for( ; iowner1!=scope_->_memOwnerBlocks.end() && end==true ; iowner1++)
//            {
//                if(iowner0->first->_address==iowner1->first->_address)
//                {
//                    //Concatenate memory stats
//                    iowner0->second->_allocNum += iowner1->second->_allocNum ;
//                    iowner0->second->_allocSize += iowner1->second->_allocSize ;
//                    iowner0->second->_allocTime += iowner1->second->_allocTime ;
//                    iowner0->second->_freeNum += iowner1->second->_freeNum ;
//
//                    //Remove iscope1
//                    delete iowner1->second ;
//                    scope_->_memOwnerBlocks.erase(iowner1) ;
//
//                    end = false ;
//                }
//            }
//        }
//
//        //Loop while at least one change occured or end of list is reached
//    }
//    while(end==false) ;
//
//
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=scope_->_childScopes.begin() ; iscope!=scope_->_childScopes.end() ; iscope++)
//    {
//        recursiveConcatenateScopesMemstat(iscope->second) ;
//    }
//}
//
////-----------------------------------------------------------------------------
//void Scope::concatenateScopesMemstat(Scope* srcscope_, Scope* dstscope_)
//{
//    //Concatenate memory stats
//    dstscope_->_scopeMemStats._allocNum += srcscope_->_scopeMemStats._allocNum ;
//    dstscope_->_scopeMemStats._allocSize += srcscope_->_scopeMemStats._allocSize ;
//    dstscope_->_scopeMemStats._allocTime += srcscope_->_scopeMemStats._allocTime ;
//    dstscope_->_scopeMemStats._freeNum += srcscope_->_scopeMemStats._freeNum ;
//
//    //Concatenate owner blocks
//    std::vector< std::pair<Scope*, MemStats*> >::iterator isrcblock ;
//    for(isrcblock=srcscope_->_memOwnerBlocks.begin() ; isrcblock!=srcscope_->_memOwnerBlocks.end() ; isrcblock++)
//    {
//        //Search for similar block in dstscope_
//        std::vector< std::pair<Scope*, Scope::MemStats*> >::iterator idstblock ;
//        idstblock = Scope::find(dstscope_->_memOwnerBlocks, isrcblock->first) ;
//        if(idstblock==dstscope_->_memOwnerBlocks.end())
//        {
//            //Memory block not found, create it
//            Scope::MemStats* stats=new Scope::MemStats ;
//
//            stats->_allocNum = isrcblock->second->_allocNum ;
//            stats->_allocSize = isrcblock->second->_allocSize ;
//            stats->_allocTime = isrcblock->second->_allocTime ;
//            stats->_freeNum = isrcblock->second->_freeNum ;
//
//            dstscope_->_memOwnerBlocks.push_back(std::pair<Scope*, MemStats*>(isrcblock->first, stats)) ;
//            printf("aa\n") ;
//        }
//        else
//        {
//            idstblock->second->_allocNum += isrcblock->second->_allocNum ;
//            idstblock->second->_allocSize += isrcblock->second->_allocSize ;
//            idstblock->second->_allocTime += isrcblock->second->_allocTime ;
//            idstblock->second->_freeNum += isrcblock->second->_freeNum ;
//            printf("bb\n") ;
//        }
//        printf("cc\n") ;
//    }
//
//}
//
////-----------------------------------------------------------------------------
//void Scope::computeGlobalMemStats(Scope* scope_)
//{
//    //Init memory usage
//    scope_->_scopeMemStats._allocSize = 0 ;
//    scope_->_scopeMemStats._allocNum = 0 ;
//    scope_->_scopeMemStats._freeNum = 0 ;
//    scope_->_scopeMemStats._allocTime = 0 ;
//
//    recursiveComputeGlobalMemStats(Logger::_threadRootLogger->_rootScope, scope_) ;
//}
//
////-----------------------------------------------------------------------------
//void Scope::recursiveComputeGlobalMemStats(Scope* currScope_, Scope* scope_)
//{
//    std::vector< std::pair<void*,  Scope*> >::iterator iscope ;
//    for(iscope=currScope_->_childScopes.begin() ; iscope!=currScope_->_childScopes.end() ; iscope++)
//    {
//        if((*iscope).second!=NULL && (*iscope).second->_globalScope==scope_)
//        {
//            //Compute memory usage
//            scope_->_scopeMemStats._allocSize += iscope->second->_scopeMemStats._allocSize ;
//            scope_->_scopeMemStats._allocNum += iscope->second->_scopeMemStats._allocNum ;
//            scope_->_scopeMemStats._freeNum += iscope->second->_scopeMemStats._freeNum ;
//            scope_->_scopeMemStats._allocTime += iscope->second->_scopeMemStats._allocTime ;
//        }
//
//        recursiveComputeGlobalMemStats((*iscope).second, scope_) ;
//    }
//}
//
