#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Memory pool to allocate aligned blocks of memory in virtual memory area.
//Not thread safe.

class MemoryPool
{
public:
	struct Pool
	{
		Pool* prev ;
		Pool* next ;
		size_t poolSize ;
		size_t usedSize ;
		uint8_t* values ;
	} ;


	template <class Type> static Type* add(size_t size=sizeof(Type))
		__attribute__((no_instrument_function)) ;

	static const Pool* getFirst()
		__attribute__((no_instrument_function)) ;
	static const Pool* getCurrent()
		__attribute__((no_instrument_function)) ;
protected:
	MemoryPool()
		__attribute__((no_instrument_function)) ;

	static const size_t DefaultPoolPagesNum=10 ;

	static Pool* first ;
	static Pool* current ;

	static void createPool(size_t minsize)
		__attribute__((no_instrument_function)) ;
} ;


template <class Type>
Type* MemoryPool::add(size_t size)
{
	uint8_t* address ;
	size_t needed=size+size%8 ;	//align memory to blocks of 8 bytes

	if(current==nullptr || current->usedSize+needed>=current->poolSize)
	{
		createPool(needed) ;
		address = current->values ;
	}
	else
		address = current->values+current->usedSize ;

	//TODO: this way of creating the object is evil, wont work with every object
	Type val ;
	memcpy((void*)address, &val, size) ;
//	((Type*)address)->Type() ;

	current->usedSize += needed ;
	return (Type*)address ;
}

#endif // _MEMORYPOOL_H_
