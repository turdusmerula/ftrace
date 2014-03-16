#include "mem/Page.h"

#include <log/Print.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//Source code from Electric Fence
//$NetBSD: page.c,v 1.2 2003/12/04 16:23:34 drochner Exp $	*/

/*
 * Lots of systems are missing the definition of PROT_NONE.
 */
#ifndef	PROT_NONE
#define	PROT_NONE	0
#endif

/*
 * 386 BSD has MAP_ANON instead of MAP_ANONYMOUS.
 */
#if ( !defined(MAP_ANONYMOUS) && defined(MAP_ANON) )
#define	MAP_ANONYMOUS	MAP_ANON
#endif

/*
 * For some reason, I can't find mprotect() in any of the headers on
 * IRIX or SunOS 4.1.2
 */
/* extern C_LINKAGE int mprotect(void * addr, size_t len, int prot); */

static caddr_t	startAddr = (caddr_t) 0;

#if ( !defined(sgi) && !defined(_AIX) )
extern int	sys_nerr;
/*extern char *	sys_errlist[];*/
#endif

const char* Page::stringErrorReport(void)
{
#	if(defined(sgi))
		return strerror(oserror()) ;
#	elif(defined(_AIX))
		return strerror(errno) ;
#	else
		if(errno>0 && errno<sys_nerr)
			return sys_errlist[errno] ;
		else
			return "Unknown error.\n" ;
#	endif
}

/*
 * Create memory.
 */
void* Page::CreatePage(size_t size)
{
	caddr_t		allocation;

	/*
	 * In this version, "startAddr" is a _hint_, not a demand.
	 * When the memory I map here is contiguous with other
	 * mappings, the allocator can coalesce the memory from two
	 * or more mappings into one large contiguous chunk, and thus
	 * might be able to find a fit that would not otherwise have
	 * been possible. I could _force_ it to be contiguous by using
	 * the MMAP_FIXED flag, but I don't want to stomp on memory mappings
	 * generated by other software, etc.
	 */
	allocation = (caddr_t) mmap(
	 NULL
	,size
	,PROT_READ|PROT_WRITE
	,MAP_SHARED|MAP_ANONYMOUS
	,-1
	,0);

	if ( allocation == (caddr_t)-1 )
		Exit("mmap() failed: %s", stringErrorReport());

	return (void *)allocation;
}

void Page::mprotectFailed(void)
{
	Exit("mprotect() failed: %s", stringErrorReport());
}

void Page::AllowAccessPage(void* address, size_t size)
{
	if ( mprotect((caddr_t)address, size, PROT_READ|PROT_WRITE) < 0 )
		mprotectFailed();
}

void Page::DenyAccessPage(void* address, size_t size)
{
	if ( mprotect((caddr_t)address, size, PROT_NONE) < 0 )
		mprotectFailed();
}

void Page::DeletePage(void* address, size_t size)
{
	DenyAccessPage(address, size);
}

size_t Page::getPageSize(void)
{
	return getpagesize();
}