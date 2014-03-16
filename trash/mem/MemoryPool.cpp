#include <mem/MemoryPool.h>

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

int MemoryPool::shmid=-1 ;	//shared memory segment
char MemoryPool::shmname[50]="ftrace" ;
size_t MemoryPool::poolSize=0 ;
size_t MemoryPool::usedSize=0 ;
void* MemoryPool::pool=nullptr ;


MemoryPool::MemoryPool()
{
}

void MemoryPool::create()
{
	char spid[50]={0} ;

	//We can not be sure that sprintf wont use any malloc so here we have a custom way for writing the pid in the name
	char *sp=&spid[1] ;	//reversed string, 0 char is at the beginning
	char *dp=&shmname[6] ;
	__pid_t pid=getpid() ;
	while(pid>0)
	{
		puts("+") ;
		*sp = '0'+pid%10 ;
		pid /= 10 ;
		sp++ ;
	}
	sp-- ;
	while(*sp!='\0')
	{
		*dp = *sp ;
		dp++ ;
		sp-- ;
	}
	*dp = '\0' ;

	int fd=shm_open(shmname, O_RDWR | O_CREAT, 0777) ;
	if(fd==-1)
	{
		perror("SHM creation failed") ;
		return ;
	}

	//Set memory objects size
	if(ftruncate(fd, DefaultPoolSize)==-1)
	{
		perror("SHM sizing failed") ;
		return ;
	}

	pool = mmap(0, DefaultPoolSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0) ;
	if(pool==MAP_FAILED)
	{
		pool = nullptr ;
		perror("mmap failed") ;
	}
	else
		poolSize = DefaultPoolSize ;

	close(fd) ;
}

void MemoryPool::destroy()
{
	if (munmap(pool, poolSize)!=0)
		perror("munmap failed") ;

	if(shm_unlink(shmname)!=0)
		perror("unlink failed") ;
}

//void MemoryPool::augment()
//{
//	shmid_ds buf ;
//
//	if(pool==nullptr)
//		create() ;
//
//	buf.shm_segsz = poolSize+PoolSizeIncrement ;
//	if(shmctl(shmid, SHM_SIZE, &buf)<0)
//	{
//    	perror("SHM address can not be resized") ;
//        return ;
//	}
//
//    poolSize += PoolSizeIncrement ;
//}


void* MemoryPool::add(size_t size)
{
	uint8_t* address=nullptr ;
	size_t needed=size+size%8 ;	//align memory to blocks of 8 bytes

//	if(pool==nullptr || usedSize+needed>=poolSize)
//		augment() ;
	if(pool==nullptr || usedSize+needed>=poolSize)
		return nullptr ;	//No more memory could be allocated
	address = (uint8_t*)pool+usedSize ;

	usedSize += needed ;
	return address ;
}
