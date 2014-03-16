#include "mem/AllocatorSimple.h"
#include "mem/Page.h"
#include "log/Print.h"
#include "com/Config.h"

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

AllocatorSimple::Slot** AllocatorSimple::slotList=NULL ;
size_t AllocatorSimple::slotListSize=0 ;
size_t AllocatorSimple::bytesPerPage=0 ;
size_t AllocatorSimple::bytesPerSlot=0 ;
unsigned int AllocatorSimple::slotCount=0 ;
size_t AllocatorSimple::totalAlloc=0 ;

void AllocatorSimple::initialize(void)
{
	//Get the run-time configuration of the virtual memory page size.
 	bytesPerPage = Page::getPageSize() ;
	//Get the run-time configuration of the slot size.
 	bytesPerSlot = bytesPerPage*Config::PAGE_SLOT_SIZE ;

 	//Create slot list
 	slotListSize = bytesPerPage*Config::PAGE_SLOT_SIZE ;
 	slotList = (Slot**)Page::CreatePage(slotListSize) ;
	memset((char*)slotList, 0, slotListSize) ;

	//Allocate first slot
	slotList[0] = (Slot*)Page::CreatePage(bytesPerSlot) ;
	slotList[0]->firstBlock = NULL ;
	slotList[0]->lastBlock = NULL ;
	slotList[0]->size = bytesPerSlot ;
	slotList[0]->freeSpace = slotList[0]->size-sizeofSlot() ;

	//One slot used
	slotCount = 1 ;

//	showMemoryState() ;
}


void AllocatorSimple::internalError(void)
{
	Abort("Internal error in allocator.") ;
}

bool AllocatorSimple::queryBlock(AllocatorSimple::Slot* slot, size_t size, AllocatorSimple::Block* &resBlock)
{
	size_t realSize=size+sizeofBlock() ;

	//Print("--queryBlock need %l--\n", size) ;
	//Check if free space is enough
	if(realSize>slot->freeSpace)
		return false ;

	//Slot is empty and large enough
	if(slot->firstBlock==NULL)
	{
		resBlock = NULL ;
		return true ;
	}

	//Slot not empty, check if free space after the last block can fit
	size_t freeEndSpace=(size_t)(slot)+slot->size-(size_t)(slot->lastBlock)-(slot->lastBlock->size+sizeofBlock()) ;
	if(realSize<=freeEndSpace)
	{
		//Space can fit, return the last block
		resBlock = slot->lastBlock ;
		return true ;
	}

	//Check if there's a chance to have enough free space between two blocks because we know that the space at the
	//end is too small for the required space
	if(realSize>slot->freeSpace-freeEndSpace)
		return false ;

	//Check space between blocks, there is maybe enough somewhere
	Block* block=slot->firstBlock ;
	while(block!=NULL && block->nextBlock!=NULL)
	{
		//Check usable free space between two blocks
		size_t freeSpace=(size_t)(block->nextBlock)-(size_t)(block)-(block->size+sizeofBlock()) ;
		//Print("freeSpace: %d\n", freeSpace) ;
		if(freeSpace>=realSize)
		{
			resBlock = block ;
			return true ;
		}

		block = block->nextBlock ;
	}

	//Not enough space found, memory is fragmented :/
	return false ;
}

AllocatorSimple::Block* AllocatorSimple::insertBlock(AllocatorSimple::Slot* slot, AllocatorSimple::Block* block, size_t size)
{
	//Warning, no test made here to check if there's free space enough for performance

	Block* newBlock ;

	if(block==NULL)
	{
		//Inserting first block
		newBlock = (Block*)((char*)slot+sizeofSlot()) ;
		newBlock->prevBlock = NULL ;
		newBlock->nextBlock = NULL ;
		newBlock->size = size ;
		newBlock->slot = slot ;

		slot->firstBlock = newBlock ;
		slot->lastBlock = newBlock ;
	}
	else
	{
		//Inserting block after another block
		newBlock = (Block*)((char*)block+block->size+sizeofBlock()) ;
		newBlock->prevBlock = block ;
		newBlock->nextBlock = block->nextBlock ;
		newBlock->size = size ;
		newBlock->slot = slot ;

		//Modify previous block
		if(block->nextBlock!=NULL)
			block->nextBlock->prevBlock = newBlock ;
		else
		{
			//Insert at the last block
			slot->lastBlock = newBlock ;
		}
		block->nextBlock = newBlock ;
	}

	slot->freeSpace -= sizeofBlock()+newBlock->size ;
	totalAlloc += newBlock->size ;

	return newBlock ;
}

AllocatorSimple::Slot* AllocatorSimple::createSlot(size_t blockSize)
{
	size_t slotSize ;

	//Print("--createSlot needed:%d--\n", blockSize) ;

	slotSize = bytesPerSlot ;
	//If the needed size is greater than a slot size then fit to a multiple of slot size
	if(blockSize+sizeofBlock()+sizeofSlot()>bytesPerSlot) //Size of the first block+size of the slot to be created
		slotSize = bytesPerSlot*((blockSize+sizeofBlock()+sizeofSlot())/bytesPerSlot+1) ;

	if(slotCount>=slotListSize/sizeof(Slot))
	{
		//List of slots too small, need to extend it
		size_t newSlotSize=slotListSize+bytesPerSlot ;
		Slot** newSlotList=(Slot**)Page::CreatePage(newSlotSize) ;
		memcpy(newSlotList, slotList, slotListSize) ;
		memset(&(((char *)newSlotList)[slotListSize]), 0, bytesPerSlot) ;

		Page::DeletePage(slotList, slotListSize) ;
		slotList = newSlotList ;
		slotListSize = newSlotSize ;
	}

	//Create slot
	Slot* slot=(Slot*)Page::CreatePage(slotSize) ;
	slotList[slotCount] = slot ;
	slotCount++ ;

	slot->firstBlock = NULL ;
	slot->lastBlock = NULL ;
	slot->size = slotSize ;
	slot->freeSpace = slotSize-sizeofSlot() ;

	return slot ;
}

void* AllocatorSimple::memalign(size_t alignment, size_t userSize)
{
	size_t neededSize ;

	if(userSize==0 && !Config::EF_ALLOW_MALLOC_0)
		Abort("Allocating 0 bytes, probably a bug.") ;

	//Compute needed free space of aligned user data
	size_t slack=userSize%alignment ;
	neededSize = userSize ;
	if(slack!=0)
		neededSize += alignment-slack ;

	//Search for a slot with enough free space
	Block* block=NULL ;
	Slot* slot=NULL ;
	for(unsigned int s=0 ; s<slotCount ; s++)
	{
		if(queryBlock(slotList[s], neededSize, block)!=false)
		{
			//Allocate block
			block = insertBlock(slotList[s], block, neededSize) ;
			slot = slotList[s] ;
			//Print("insertBlock: %x\n", block) ;
			break ;
		}
	}

	//If no block created then a new slot is needed
	if(block==NULL)
	{
		slot = createSlot(neededSize) ;
		//Create a block in the slot
		block = insertBlock(slot, slot->firstBlock, neededSize) ;
	}

	return (char*)block+sizeofBlock() ;
}

void AllocatorSimple::free(void* address)
{
	Block* block ;
	Slot* slot ;

	//Convert address to block address
	block = (Block*)((char*)address-sizeofBlock()) ;
	slot = block->slot ;

	if(block->nextBlock!=NULL)
	{
		block->nextBlock->prevBlock = block->prevBlock ;
	}
	if(block->prevBlock!=NULL)
	{
		block->prevBlock->nextBlock = block->nextBlock ;
	}

	//Update of counters
	slot->freeSpace += block->size+sizeofBlock() ;
	totalAlloc -= block->size ;

	//showMemoryState() ;
}

void* AllocatorSimple::realloc(void* oldBuffer, size_t newSize)
{
	void* newBuffer=this->malloc(newSize) ;
//	Block* newBlock=(Block*)((char*)newBuffer-sizeofBlock()) ;

	if(oldBuffer)
	{
		size_t size ;
		Block* oldBlock=(Block*)((char*)oldBuffer-sizeofBlock()) ;

		size = oldBlock->size ;
		if(newSize<size)
			size = newSize ;

		if(size>0)
			memcpy(newBuffer, oldBuffer, size) ;

		this->free(oldBuffer) ;
	}

	//showMemoryState() ;

	return newBuffer ;
}

void* AllocatorSimple::malloc(size_t size)
{
	void* allocation ;

	allocation = memalign(Config::EF_ALIGNMENT, size);

	//showMemoryState() ;

	return allocation;
}

void* AllocatorSimple::calloc(size_t nelem, size_t elsize)
{
	size_t size=nelem*elsize ;
	void* allocation ;

	allocation = malloc(size) ;
	memset(allocation, 0, size) ;

	//showMemoryState() ;

	return allocation ;
}

void* AllocatorSimple::valloc (size_t size)
{
	void* allocation ;

	allocation = memalign(bytesPerPage, size) ;

	//showMemoryState() ;

	return allocation ;
}

void AllocatorSimple::showMemoryState()
{
	Print("--Memory state-- \n") ;
	Print("Slot size:%d\n", sizeofSlot()) ;
	Print("Block size:%d\n", sizeofBlock()) ;
	Print("Total alloc:%l\n", totalAlloc) ;
	for(unsigned int islot=0 ; islot<slotCount ; islot++)
	{
		Slot* slot=slotList[islot] ;
		size_t freeSpace ;

		Print("+ Slot %d(%x): free=%d, size=%d\n", islot, (void*)slot, slot->freeSpace, slot->size) ;

		if(slot->firstBlock==NULL)
			freeSpace = slot->freeSpace ;
		else
			freeSpace = (size_t)(slot->firstBlock)-(size_t)(slot)-sizeofSlot() ;

		if(freeSpace>0)
			Print(" \t+ free space=%l\n", freeSpace) ;


		Block* block=slot->firstBlock ;
		unsigned int iblock=0 ;
		while(block!=NULL)
		{
			Print("\t+ Block %d(%x): size=%d, <--%x - %x-->, slot=%x\n", iblock, (void*)block, block->size, (void*)block->prevBlock, (void*)block->nextBlock, (void*)block->slot) ;

			if(block->nextBlock!=NULL)
				freeSpace = (size_t)(block->nextBlock)-(size_t)(block)-(block->size+sizeofBlock()) ;
			else
				freeSpace = (size_t)(slot)+slot->size-(size_t)(slot->lastBlock)-(slot->lastBlock->size+sizeofBlock()) ;

			if(freeSpace>0)
				Print("\t+ free space=%l\n", freeSpace) ;

			block = block->nextBlock ;
			iblock++ ;
		}
	}
}

