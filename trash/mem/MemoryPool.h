#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits>
#include <iostream>


//Memory pool to allocate aligned blocks of memory in virtual memory area.
//Not thread safe.

class MemoryPool
{
public:
	static void create()
		__attribute__((no_instrument_function)) ;

	static void destroy()
		__attribute__((no_instrument_function)) ;

	template <class Type> static Type* add()
		__attribute__((no_instrument_function)) ;

	static void* add(size_t size)
		__attribute__((no_instrument_function)) ;

	static size_t getPoolSize() { return poolSize ; }
	static size_t getUsedSize() { return poolSize ; }
protected:
	MemoryPool()
		__attribute__((no_instrument_function)) ;

	static const size_t DefaultPoolSize=5*1024*1024 ;	//Allocate 5Mo by default

	static int shmid ;	//shared memory segment
	static char shmname[50] ;

	static size_t poolSize ;
	static size_t usedSize ;
	static void* pool ;

	static void augment()
		__attribute__((no_instrument_function)) ;
} ;


template <class Type>
Type* MemoryPool::add()
{
	Type* address ;

	address = (Type*)add(sizeof(Type)) ;
	//Placement new, call constructor (no allocation), default constructo must exist
	if(address!=nullptr)
		new(static_cast<void*>(address))Type() ;
	return address ;
}


template <class T>
class MemoryPoolAllocator
{
public:
	// type definitions
	typedef T        		value_type ;
	typedef T*       		pointer ;
	typedef const T* 		const_pointer ;
	typedef T&       		reference ;
	typedef const T& 		const_reference ;
	typedef std::size_t    	size_type ;
	typedef std::ptrdiff_t 	difference_type ;

   // rebind allocator to type U
   template <class U>
   struct rebind
   {
	   typedef MemoryPoolAllocator<U> other ;
   } ;

   // return address of values
   pointer address(reference value) const { return &value ; }
   const_pointer address(const_reference value) const { return &value ; }

   /* constructors and destructor
	* - nothing to do because the allocator has no state
	*/
   MemoryPoolAllocator() throw() { }

   MemoryPoolAllocator(const MemoryPoolAllocator&) throw() { }

   template <class U>
   MemoryPoolAllocator (const MemoryPoolAllocator<U>&) throw() { }

   ~MemoryPoolAllocator() throw() { }

   // return maximum number of elements that can be allocated
   size_type max_size () const throw() { return std::numeric_limits<std::size_t>::max()/sizeof(T) ; }

   // allocate but don't initialize num elements of type T
   pointer allocate(size_type num, const void* p=0)
   {
	   pointer ret = (pointer)(MemoryPool::add(num*sizeof(T))) ;
	   return ret ;
   }

   // initialize elements of allocated storage p with value value
   void construct (pointer p, const T& value)
   {
	   // initialize memory with placement new
	   new(static_cast<void*>(p))T(value) ;
   }

   // destroy elements of initialized storage p
   void destroy (pointer p)
   {
	   //Do nothing, this allocator can only allocate
   }

   // deallocate storage p of deleted elements
   void deallocate (pointer p, size_type num)
   {
	   //Do nothing, this allocator can only allocate
   }
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
bool operator== (const MemoryPoolAllocator<T1>&,
				const MemoryPoolAllocator<T2>&) throw() {
   return true;
}
template <class T1, class T2>
bool operator!= (const MemoryPoolAllocator<T1>&,
				const MemoryPoolAllocator<T2>&) throw() {
   return false;
}

#endif // _MEMORYPOOL_H_
