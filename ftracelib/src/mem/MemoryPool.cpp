#include <mem/MemoryPool.h>

#include <mem/Page.h>

MemoryPool::Pool* MemoryPool::first=nullptr ;
MemoryPool::Pool* MemoryPool::current=nullptr ;


MemoryPool::MemoryPool()
{
}

void MemoryPool::createPool(size_t minsize)
{
	Pool* pool ;

	size_t rawsize ;
	if(minsize<Page::getPageSize()*DefaultPoolPagesNum)
	{
		rawsize = Page::getPageSize()*DefaultPoolPagesNum ;
		pool = (Pool*)Page::CreatePage(rawsize) ;
	}
	else
	{
		rawsize = Page::getPageSize()*(minsize/Page::getPageSize()+1) ;
		pool = (Pool*)Page::CreatePage(rawsize) ;
	}

	pool->next = nullptr ;
	pool->prev = nullptr ;
	pool->usedSize = 0 ;
	pool->poolSize = rawsize-sizeof(Pool) ;
	pool->values = (uint8_t*)(pool)+sizeof(Pool) ;

	Pool* prev=current ;
	current = pool ;
	current->prev = prev ;
	if(current->prev!=nullptr)
		current->prev->next = current ;

	if(first==nullptr)
		first = current ;
}

const MemoryPool::Pool* MemoryPool::getFirst()
{
	return first ;
}

const MemoryPool::Pool* MemoryPool::getCurrent()
{
	return current ;
}
