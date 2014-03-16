#ifndef _SHARED_OBJECTS_H_
#define _SHARED_OBJECTS_H_

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <pthread.h>

using namespace boost::interprocess ;

//Shared memory size
const size_t ShmSize=1024*1024*5 ;	//5 Mo by default

struct ScopeData
{
    /**
    * Function address, not initialized if tree scope
    */
    void* address ;
} ;
typedef allocator<ScopeData, managed_shared_memory::segment_manager> ScopeDataAllocator ;
typedef vector<ScopeData, ScopeDataAllocator> ScopeDataVector ;

//The void* type can not easily be used with map, we use size_t here with an underlying pointer semantic
//it's obviously nasty but avoid lots of compile problems with the template
typedef allocator<std::pair<const size_t, ScopeData>, managed_shared_memory::segment_manager> ScopeDataPairAllocator ;
typedef map<size_t,  ScopeData, std::less<size_t>, ScopeDataPairAllocator> ScopeDataMap ;

struct Scope ;
typedef allocator<std::pair<const size_t, offset_ptr<Scope>>, managed_shared_memory::segment_manager> ScopePtrPairAllocator ;
typedef map<size_t, offset_ptr<Scope>, std::less<size_t>, ScopePtrPairAllocator> ScopePtrMap ;

struct Scope
{
	/**
	 * Global scope data
	 */
	offset_ptr<ScopeData> scopeData ;

	/**
	 * Sub scopes called from this scope
	 */
    offset_ptr<ScopePtrMap> subScopes ;
} ;
typedef allocator<Scope, managed_shared_memory::segment_manager> ScopeAllocator ;
typedef vector<Scope, ScopeAllocator> ScopeVector ;

typedef allocator<offset_ptr<Scope>, managed_shared_memory::segment_manager> ScopePtrAllocator ;
typedef vector<offset_ptr<Scope>, ScopeAllocator> ScopePtrVector ;

struct ThreadData
{
	/**
     * Thread identifier.
     */
    pthread_t threadId ;

    offset_ptr<ScopePtrVector> stack ;

    offset_ptr<Scope> rootScope ;

    offset_ptr<Scope> currScope ;

} ;

typedef allocator<ThreadData, managed_shared_memory::segment_manager> ThreadDataAllocator ;
typedef list<ThreadData, ThreadDataAllocator> ThreadDataList ;

struct ProcessData
{
	/**
	 * Process pid
	 */
	__pid_t pid ;

	offset_ptr<ThreadDataList> threads ;

	/**
	* List of all scopes.
	*/
	offset_ptr<ScopeDataMap> scopes ;
} ;

#endif
