#ifndef __PAGE_H__
#define __PAGE_H__

#include <sys/types.h>

class Page
{
protected:
	static const char* stringErrorReport(void)
		__attribute__((no_instrument_function)) ;

	static void mprotectFailed(void)
		__attribute__((no_instrument_function)) ;

public:
	static size_t getPageSize(void)
		__attribute__((no_instrument_function)) ;

	static void DeletePage(void* address, size_t size)
    	__attribute__((no_instrument_function)) ;

	static void* CreatePage(size_t size)
		__attribute__((no_instrument_function)) ;

	static void AllowAccessPage(void* address, size_t size)
    	__attribute__((no_instrument_function)) ;

	static void DenyAccessPage(void* address, size_t size)
    	__attribute__((no_instrument_function)) ;

} ;

#endif
