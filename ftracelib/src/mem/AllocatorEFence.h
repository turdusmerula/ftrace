#ifndef __ALLOCATOREFENCE_H__
#define __ALLOCATOREFENCE_H__

#include "mem/Allocator.h"
#include "mem/Memory.h"

class AllocatorEFence : public Allocator
{
public:
	AllocatorEFence() {} ;
	virtual ~AllocatorEFence() {} ;
protected:

	/*
	 * Enum Mode indicates the status of a malloc buffer.
	 */
	enum _Mode
	{
		NOT_IN_USE = 0,		// Available to represent a malloc buffer.
		FREE,				// A free buffer.
		ALLOCATED,	// A buffer that is in use by efence
		PROTECTED,			// A freed buffer that can not be allocated again.
		INTERNAL_USE		// A buffer used internally by malloc().
	} ;
	typedef enum _Mode Mode ;

	/*
	 * Struct Slot contains all of the information about a malloc buffer except
	 * for the contents of its memory.
	 */
	struct _Slot
	{
		void*	userAddress ;
		void*	internalAddress ;
		size_t	userSize ;
		size_t	internalSize ;
		Mode	mode ;
	} ;
	typedef struct _Slot Slot ;


	/*
	 * allocationList points to the array of slot structures used to manage the
	 * malloc arena.
	 */
	static Slot* allocationList ;

	/*
	 * allocationListSize is the size of the allocation list. This will always
	 * be a multiple of the page size.
	 */
	static size_t allocationListSize ;

	/*
	 * slotCount is the number of Slot structures in allocationList.
	 */
	static size_t slotCount ;

	/*
	 * unUsedSlots is the number of Slot structures that are currently available
	 * to represent new malloc buffers. When this number gets too low, we will
	 * create new slots.
	 */
	static size_t unUsedSlots ;

	/*
	 * slotsPerPage is the number of slot structures that fit in a virtual
	 * memory page.
	 */
	static size_t slotsPerPage ;

	/*
	 * internalUse is set when allocating and freeing the allocator-internal
	 * data structures.
	 */
	static bool internalUse ;

	/*
	 * noAllocationListProtection is set to tell malloc() and free() not to
	 * manipulate the protection of the allocation list. This is only set in
	 * realloc(), which does it to save on slow system calls, and in
	 * allocateMoreSlots(), which does it because it changes the allocation list.
	 */
	static bool noAllocationListProtection ;

	/*
	 * bytesPerPage is set at run-time to the number of bytes per virtual-memory page
	 */
	static size_t bytesPerPage ;

	/*
	 * allocateMoreSlots is called when there are only enough slot structures
	 * left to support the allocation of a single malloc buffer.
	 */
	void allocateMoreSlots(void) ;

	/*
	 * Find the slot structure for a user address.
	 */
	Slot* slotForUserAddress(void* address) ;

	/*
	 * Find the slot structure for an internal address.
	 */
	Slot* slotForInternalAddress(void* address) ;

	/*
	 * Given the internal address of a buffer, find the buffer immediately
	 * before that buffer in the address space. This is used by free() to
	 * coalesce two free buffers into one.
	 */
	Slot* slotForInternalAddressPreviousTo(void* address) ;

	/*
	 * internalError is called for those "shouldn't happen" errors in the
	 * allocator.
	 */
	void internalError(void) ;

public:
	/*
	 * initialize sets up the memory allocation arena and the run-time
	 * configuration information.
	 */
	static void initialize(void) ;


	/*
	 * This is the memory allocator. When asked to allocate a buffer, allocate
	 * it in such a way that the end of the buffer is followed by an inaccessable
	 * memory page. If software overruns that buffer, it will touch the bad page
	 * and get an immediate segmentation fault. It's then easy to zero in on the
	 * offending code with a debugger.
	 *
	 * There are a few complications. If the user asks for an odd-sized buffer,
	 * we would have to have that buffer start on an odd address if the byte after
	 * the end of the buffer was to be on the inaccessable page. Unfortunately,
	 * there is lots of software that asks for odd-sized buffers and then
	 * requires that the returned address be word-aligned, or the size of the
	 * buffer be a multiple of the word size. An example are the string-processing
	 * functions on Sun systems, which do word references to the string memory
	 * and may refer to memory up to three bytes beyond the end of the string.
	 * For this reason, I take the alignment requests to memalign() and valloc()
	 * seriously, and
	 *
	 * Electric Fence wastes lots of memory. I do a best-fit allocator here
	 * so that it won't waste even more. It's slow, but thrashing because your
	 * working set is too big for a system's RAM is even slower.
	 */
	virtual void* memalign(size_t alignment, size_t userSize) ;

	virtual void free(void* address) ;

	virtual void* realloc(void* oldBuffer, size_t newSize) ;

	virtual void* malloc(size_t size) ;

	virtual void* calloc(size_t nelem, size_t elsize) ;

	virtual void* valloc (size_t size) ;


};


#endif
