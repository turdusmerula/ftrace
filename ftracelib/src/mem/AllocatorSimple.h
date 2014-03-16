#ifndef __ALLOCATORSIMPLE_H__
#define __ALLOCATORSIMPLE_H__

#include "mem/Allocator.h"
#include "mem/Memory.h"

/**
 * This is the most simple memory allocator (with very poor performances).
 * It consist in a simple list of slots (one ore more pages), each slot containing
 * a linked list of blocks.
 */
class AllocatorSimple : public Allocator
{
public:
	AllocatorSimple() {} ;
	virtual ~AllocatorSimple() {} ;

protected:
	struct _Slot ;

	/*
	 * Struct for a memory block inside a slot except the content of its memory
	 */
	struct _Block
	{
		_Slot*	slot ;
		_Block*	prevBlock ;
		_Block* nextBlock ;
		size_t	size ;
	} ;
	typedef struct _Block Block ;

	/*
	 * Struct Slot contains all of the information about a malloc buffer except
	 * for the contents of its memory.
	 */
	struct _Slot
	{
		//first block of the slot
		Block*	firstBlock ;

		//last block of the slot
		Block*	lastBlock ;

		size_t	size ;

		//indicates free space, thus memory can be fragmented and free space may not be contiguous
		size_t	freeSpace ;
	} ;
	typedef struct _Slot Slot ;

	static inline size_t sizeofBlock() { return sizeof(Block) ; }
	static inline size_t sizeofSlot() { return sizeof(Slot) ; }

	/*
	 * internalError is called for those "shouldn't happen" errors in the
	 * allocator.
	 */
	void internalError(void) ;

	/*
	 * allocationList points to the array of slot structures used to manage the
	 * malloc arena.
	 */
	static Slot** slotList ;

	/*
	 * allocationListSize is the size of the allocation list. This will always
	 * be a multiple of the page size.
	 */
	static size_t slotListSize ;

	/**
	 * Number of used slots
	 */
	static unsigned int slotCount ;

	/*
	 * bytesPerPage is set at run-time to the number of bytes per virtual-memory page
	 */
	static size_t bytesPerPage ;

	/*
	 * bytesPerPage is set at run-time to the number of bytes per slot, multiple of memory pages
	 */
	static size_t bytesPerSlot ;


	static size_t totalAlloc ;

	/**
	 * Search for free space inside a slot where the required user memory size can fit.
	 * return true if some space where found, in this case resBlock contains the
	 * address of the block preceeding the free space or NULL if free space at beginning
	 * of slot.
	 */
	bool queryBlock(Slot* slot, size_t size, Block* &resBlock) ;

	/**
	 * Creates a new block of the indicated user size after the indicated block of the slot.
	 * return the created block.
	 */
	Block* insertBlock(Slot* slot, Block* block, size_t size) ;

	/*
	 * Creates a new slot. A slot is a multiple of SlotSize sized to contain at least one blockSize.
	 * return the created slot.
	 */
	Slot* createSlot(size_t blockSize) ;

	/**
	 * fonction de débugage pour avoir un apperçu de l'état de la mémoire.
	 */
	static void showMemoryState() ;

public:
	/*
	 * initialize sets up the memory allocation arena and the run-time
	 * configuration information.
	 */
	static void initialize(void) ;


	virtual void* memalign(size_t alignment, size_t userSize) ;

	virtual void free(void* address) ;

	virtual void* realloc(void* oldBuffer, size_t newSize) ;

	virtual void* malloc(size_t size) ;

	virtual void* calloc(size_t nelem, size_t elsize) ;

	virtual void* valloc (size_t size) ;
};


#endif
