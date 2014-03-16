#ifndef _FVSTRING_H_
#define _FVSTRING_H_

#include <stdio.h>

static const size_t npos = -1;

//Template
class vstring
{
public:
	vstring() ;
	vstring(size_t size) ;
	vstring(const vstring& str) ;
	vstring(const char* str) ;

	const vstring &operator+=(char c) ;
	const vstring &operator+=(const char* str) ;
	const vstring &operator+=(const vstring &str) ;

	const vstring &operator=(char c) ;
	const vstring &operator=(const char* str) ;
	const vstring &operator=(const vstring &str) ;

	size_t rfind(char c, size_t rpos=0) ;
	vstring& erase(size_t pos=0, size_t len=npos) ;

	char* c_str() const ;
	inline size_t size() const { return vsize ; }
	inline size_t reserved() const { return msize ; }
protected:
	size_t msize ; //reserved size
	size_t vsize ; //used size
	char *vstr ;
} ;

#endif // _FVSTRING_H_
