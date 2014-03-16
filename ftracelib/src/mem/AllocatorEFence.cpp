/*
 * Electric Fence - Red-Zone memory allocator.
 * Bruce Perens, 1988, 1993
 * 
 * This is a special version of malloc() and company for debugging software
 * that is suspected of overrunning or underrunning the boundaries of a
 * malloc buffer, or touching free memory.
 *
 * It arranges for each malloc buffer to be followed (or preceded)
 * in the address space by an inaccessable virtual memory page,
 * and for free memory to be inaccessable. If software touches the
 * inaccessable page, it will get an immediate segmentation
 * fault. It is then trivial to uncover the offending code using a debugger.
 *
 * An advantage of this product over most malloc debuggers is that this one
 * detects reading out of bounds as well as writing, and this one stops on
 * the exact instruction that causes the error, rather than waiting until the
 * next boundary check.
 *
 * There is one product that debugs malloc buffer overruns
 * better than Electric Fence: "Purify" from Purify Systems, and that's only
 * a small part of what Purify does. I'm not affiliated with Purify, I just
 * respect a job well done.
 *
 * This version of malloc() should not be linked into production software,
 * since it tremendously increases the time and memory overhead of malloc().
 * Each malloc buffer will consume a minimum of two virtual memory pages,
 * this is 16 kilobytes on many systems. On some systems it will be necessary
 * to increase the amount of swap space in order to debug large programs that
 * perform lots of allocation, because of the per-buffer overhead.
 */

#include "mem/AllocatorEFence.h"
#include "mem/Page.h"
#include "log/Print.h"
#include "com/Config.h"

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

AllocatorEFence::Slot* AllocatorEFence::allocationList=0 ;
size_t AllocatorEFence::allocationListSize=0 ;
size_t AllocatorEFence::slotCount=0 ;
size_t AllocatorEFence::unUsedSlots=0 ;
size_t AllocatorEFence::slotsPerPage=0 ;
bool AllocatorEFence::internalUse=0 ;
bool AllocatorEFence::noAllocationListProtection=0 ;
size_t AllocatorEFence::bytesPerPage=0 ;


void AllocatorEFence::initialize(void)
{
	size_t	size=Config::MEMORY_CREATION_SIZE ;
	size_t	slack ;
	Slot*	slot ;

	//Get the run-time configuration of the virtual memory page size.
 	bytesPerPage = Page::getPageSize() ;

	//Figure out how many Slot structures to allocate at one time.
	slotCount = bytesPerPage/sizeof(Slot) ;
	slotsPerPage = bytesPerPage/sizeof(Slot) ;
	allocationListSize = bytesPerPage ;

	if(allocationListSize>size)
		size = allocationListSize ;

	slack = size%bytesPerPage ;
	if(slack!=0)
		size += bytesPerPage-slack ;

	//Allocate memory, and break it up into two malloc buffers. The
	//first buffer will be used for Slot structures, the second will
	//be marked free.
	allocationList = (Slot*)Page::CreatePage(size) ;
	slot = allocationList ;
	memset((char*)allocationList, 0, allocationListSize) ;

	slot[0].internalSize = allocationListSize ;
	slot[0].userSize = allocationListSize ;
	slot[0].internalAddress = allocationList ;
	slot[0].userAddress = allocationList ;
	slot[0].mode = INTERNAL_USE ;

	if(size>allocationListSize)
	{
		slot[1].userAddress = ((char*)slot[0].internalAddress)+slot[0].internalSize ;
		slot[1].internalAddress = slot[1].userAddress ;
		slot[1].internalSize = size - slot[0].internalSize ;
		slot[1].userSize = slot[1].internalSize ;
		slot[1].mode = FREE ;
	}

	//Deny access to the free page, so that we will detect any software
	//that treads upon free memory.
	Page::DenyAccessPage(slot[1].internalAddress, slot[1].internalSize) ;

	//Account for the two slot structures that we've used.
	unUsedSlots = slotCount-2 ;
}

void AllocatorEFence::allocateMoreSlots(void)
{
	size_t	newSize=allocationListSize+bytesPerPage ;
	void*	newAllocation ;
	void*	oldAllocation=allocationList ;

	Page::AllowAccessPage(allocationList, allocationListSize) ;
	noAllocationListProtection = true ;
	internalUse = true ;

	newAllocation = ::malloc(newSize) ;
	memcpy(newAllocation, allocationList, allocationListSize) ;
	memset(&(((char *)newAllocation)[allocationListSize]), 0, bytesPerPage) ;

	allocationList = (Slot*)newAllocation ;
	allocationListSize = newSize ;
	slotCount += slotsPerPage ;
	unUsedSlots += slotsPerPage ;

	::free(oldAllocation) ;

	//Keep access to the allocation list open at this point, because
	//I am returning to memalign(), which needs that access.
	noAllocationListProtection = false ;
	internalUse = false ;
}

AllocatorEFence::Slot* AllocatorEFence::slotForUserAddress(void* address)
{
	register Slot* slot=allocationList ;
	register size_t	count=slotCount ;

	for( ; count>0 ; count--)
	{
		if(slot->userAddress==address)
			return slot ;
		slot++ ;
	}

	return 0 ;
}

AllocatorEFence::Slot* AllocatorEFence::slotForInternalAddress(void* address)
{
	register Slot* slot=allocationList ;
	register size_t	count=slotCount ;

	for( ; count>0 ; count--)
	{
		if (slot->internalAddress==address)
			return slot ;
		slot++ ;
	}

	return 0 ;
}

AllocatorEFence::Slot* AllocatorEFence::slotForInternalAddressPreviousTo(void* address)
{
	register Slot* slot=allocationList ;
	register size_t	count=slotCount ;

	for ( ; count>0 ; count--)
	{
		if(((char *)slot->internalAddress)+slot->internalSize==address)
			return slot ;
		slot++ ;
	}

	return 0 ;
}

void AllocatorEFence::internalError(void)
{
	Abort("Internal error in allocator.") ;
}

void* AllocatorEFence::memalign(size_t alignment, size_t userSize)
{
	register Slot*	slot ;
	register size_t	count ;
	Slot*	fullSlot=0 ;
	Slot*	emptySlots[2] ;
	size_t	internalSize ;
	size_t	slack ;
	char*	address ;

	if(userSize==0 && !Config::EF_ALLOW_MALLOC_0)
		Abort("Allocating 0 bytes, probably a bug.") ;

	//If EF_PROTECT_BELOW is set, all addresses returned by malloc()
	//and company will be page-aligned.
 	if(!Config::EF_PROTECT_BELOW && alignment>1)
	{
		slack = userSize%alignment ;
		if(slack!=0)
			userSize += alignment-slack ;
	}

	//The internal size of the buffer is rounded up to the next page-size
	//boudary, and then we add another page's worth of memory for the
	//dead page.
	internalSize = userSize+bytesPerPage ;
	slack = internalSize%bytesPerPage ;
	if(slack!=0)
		internalSize += bytesPerPage-slack ;

	//These will hold the addresses of two empty Slot structures, that
	//can be used to hold information for any memory I create, and any
	//memory that I mark free.
	emptySlots[0] = 0 ;
	emptySlots[1] = 0 ;

	//The internal memory used by the allocator is currently
	//inaccessable, so that errant programs won't scrawl on the
	//allocator's arena. I'll un-protect it here so that I can make
	//a new allocation. I'll re-protect it before I return.
	if(!noAllocationListProtection)
		Page::AllowAccessPage(allocationList, allocationListSize) ;

	//If I'm running out of empty slots, create some more before
	//I don't have enough slots left to make an allocation.
	if(!internalUse && unUsedSlots<7)
		allocateMoreSlots() ;

	//Iterate through all of the slot structures. Attempt to find a slot
	//containing free memory of the exact right size. Accept a slot with
	//more memory than we want, if the exact right size is not available.
	//Find two slot structures that are not in use. We will need one if
	//we split a buffer into free and allocated parts, and the second if
	//we have to create new memory and mark it as free.
	for(slot=allocationList, count=slotCount ; count> 0 ; count--)
	{
		if(slot->mode==FREE && slot->internalSize>=internalSize)
		{
			if(!fullSlot || slot->internalSize<fullSlot->internalSize)
			{
				fullSlot = slot ;
				if(slot->internalSize==internalSize && emptySlots[0])
					break ;	/* All done, */
			}
		}
		else if(slot->mode==NOT_IN_USE)
		{
			if(!emptySlots[0])
				emptySlots[0] = slot ;
			else if(!emptySlots[1])
				emptySlots[1] = slot ;
			else if(fullSlot && fullSlot->internalSize==internalSize)
				break ;	/* All done. */
		}
		slot++ ;
	}

	if(!emptySlots[0])
		internalError() ;

	if(!fullSlot)
	{
		//I get here if I haven't been able to find a free buffer
		//with all of the memory I need. I'll have to create more
		//memory. I'll mark it all as free, and then split it into
		//free and allocated portions later.
		size_t chunkSize=Config::MEMORY_CREATION_SIZE ;

		if(!emptySlots[1])
			internalError() ;

		if(chunkSize<internalSize)
			chunkSize = internalSize ;

		slack = chunkSize%bytesPerPage ;
		if(slack!=0)
			chunkSize += bytesPerPage-slack ;

		//Use up one of the empty slots to make the full slot.
		fullSlot = emptySlots[0] ;
		emptySlots[0] = emptySlots[1] ;
		fullSlot->internalAddress = Page::CreatePage(chunkSize) ;
		fullSlot->internalSize = chunkSize;
		fullSlot->mode = FREE ;
		unUsedSlots-- ;
	}

	//If I'm allocating memory for the allocator's own data structures,
	//mark it INTERNAL_USE so that no errant software will be able to
	//free it.
	if(internalUse)
		fullSlot->mode = INTERNAL_USE ;
	else
		fullSlot->mode = ALLOCATED ;

	//If the buffer I've found is larger than I need, split it into
	//an allocated buffer with the exact amount of memory I need, and
	//a free buffer containing the surplus memory.
	if(fullSlot->internalSize>internalSize)
	{
		emptySlots[0]->internalSize = fullSlot->internalSize-internalSize ;
		emptySlots[0]->internalAddress = ((char*)fullSlot->internalAddress)+internalSize ;
		emptySlots[0]->mode = FREE ;
		fullSlot->internalSize = internalSize ;
		unUsedSlots-- ;
	}

	if(!Config::EF_PROTECT_BELOW)
	{
		//Arrange the buffer so that it is followed by an inaccessable
		//memory page. A buffer overrun that touches that page will
		//cause a segmentation fault.
		address = (char*)fullSlot->internalAddress ;

		//Set up the "live" page.
		if(internalSize-bytesPerPage>0)
			Page::AllowAccessPage(fullSlot->internalAddress, internalSize-bytesPerPage) ;

		address += internalSize - bytesPerPage ;

		//Set up the "dead" page.
		Page::DenyAccessPage(address, bytesPerPage) ;

		//Figure out what address to give the user.
		address -= userSize ;
	}
	else
	{	//EF_PROTECT_BELOW!=0
		//Arrange the buffer so that it is preceded by an inaccessable
		//memory page. A buffer underrun that touches that page will
		//cause a segmentation fault.
		address = (char*)fullSlot->internalAddress ;

		//Set up the "dead" page.
		Page::DenyAccessPage(address, bytesPerPage) ;

		address += bytesPerPage ;

		//Set up the "live" page.
		if(internalSize-bytesPerPage>0)
			Page::AllowAccessPage(address, internalSize-bytesPerPage) ;
	}

	fullSlot->userAddress = address ;
	fullSlot->userSize = userSize ;

	//Make the pool's internal memory inaccessable, so that the program
	//being debugged can't stomp on it.
	if(!internalUse)
		Page::DenyAccessPage(allocationList, allocationListSize) ;

	return address ;
}

void AllocatorEFence::free(void* address)
{
	Slot*	slot ;
	Slot*	previousSlot=0 ;
	Slot*	nextSlot=0 ;

	if(allocationList==0)
		Abort("free() called before first malloc().") ;

	if(!noAllocationListProtection)
		Page::AllowAccessPage(allocationList, allocationListSize) ;

	slot = slotForUserAddress(address) ;

	if(!slot)
		Abort("free(%a): address not from malloc().", address) ;

	if(slot->mode!=ALLOCATED)
	{
		if(internalUse && slot->mode==INTERNAL_USE)
			/* Do nothing. */ ;
		else
			Abort("free(%a): freeing free memory.", address) ;
	}

	if(Config::EF_PROTECT_FREE)
		slot->mode = PROTECTED ;
	else
		slot->mode = FREE ;

	if(Config::EF_FREE_WIPES)
		memset(slot->userAddress, 0xbd, slot->userSize) ;

	previousSlot = slotForInternalAddressPreviousTo(slot->internalAddress) ;
	nextSlot = slotForInternalAddress(((char *)slot->internalAddress)+slot->internalSize) ;

	if(previousSlot && (previousSlot->mode==FREE || previousSlot->mode==PROTECTED))
	{
		//Coalesce previous slot with this one.
		previousSlot->internalSize += slot->internalSize ;
		if(Config::EF_PROTECT_FREE)
			previousSlot->mode = PROTECTED ;

		slot->internalAddress = 0 ;
		slot->userAddress = 0 ;
		slot->internalSize = 0 ;
		slot->userSize = 0 ;
		slot->mode = NOT_IN_USE ;
		slot = previousSlot ;
		unUsedSlots++ ;
	}

	if(nextSlot && (nextSlot->mode==FREE || nextSlot->mode==PROTECTED))
	{
		//Coalesce next slot with this one.
		slot->internalSize += nextSlot->internalSize ;
		nextSlot->internalAddress = 0 ;
		nextSlot->userAddress = 0 ;
		nextSlot->internalSize = 0 ;
		nextSlot->userSize = 0 ;
		nextSlot->mode = NOT_IN_USE ;
		unUsedSlots++ ;
	}

	slot->userAddress = slot->internalAddress ;
	slot->userSize = slot->internalSize ;

	//Free memory is _always_ set to deny access. When EF_PROTECT_FREE
	//is true, free memory is never reallocated, so it remains access
	//denied for the life of the process. When EF_PROTECT_FREE is false,
	//the memory may be re-allocated, at which time access to it will be
	// allowed again.
	Page::DenyAccessPage(slot->internalAddress, slot->internalSize) ;

	if(!noAllocationListProtection )
		Page::DenyAccessPage(allocationList, allocationListSize) ;
}

void* AllocatorEFence::realloc(void* oldBuffer, size_t newSize)
{
	void* newBuffer=this->malloc(newSize) ;

	if(oldBuffer)
	{
		size_t size ;
		Slot * slot ;

		Page::AllowAccessPage(allocationList, allocationListSize) ;
		noAllocationListProtection = 1 ;

		slot = slotForUserAddress(oldBuffer) ;

		if(slot==0)
			Abort("realloc(%a, %d): address not from malloc().", oldBuffer, newSize) ;

		size = slot->userSize ;
		if(newSize<size)
			size = newSize ;

		if(size>0)
			memcpy(newBuffer, oldBuffer, size) ;

		this->free(oldBuffer) ;
		noAllocationListProtection = 0 ;
		Page::DenyAccessPage(allocationList, allocationListSize) ;

		if(size<newSize)
			memset(&(((char*)newBuffer)[size]), 0, newSize-size) ;

		//Internal memory was re-protected in free()
	}

	return newBuffer ;
}

void* AllocatorEFence::malloc(size_t size)
{
	void* allocation ;

	allocation = memalign(Config::EF_ALIGNMENT, size);

	return allocation;
}

void* AllocatorEFence::calloc(size_t nelem, size_t elsize)
{
	size_t	size=nelem*elsize ;
	void* allocation ;

	allocation = malloc(size) ;
	memset(allocation, 0, size) ;

	return allocation ;
}

void* AllocatorEFence::valloc (size_t size)
{
	void* allocation ;

	allocation = memalign(bytesPerPage, size) ;

	return allocation ;
}
