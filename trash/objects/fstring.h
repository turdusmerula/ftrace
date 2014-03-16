#ifndef _FSTRING_H_
#define _FSTRING_H_

#include <stdio.h>
#include <string.h>

static const size_t npos = -1;

//Template
template <size_t Size>
class fstring
{
public:
	fstring() ;

	template <size_t Size2>
	fstring(const fstring<Size2>& str) ;

	fstring(const char* str) ;

	const fstring<Size> &operator+=(char c) ;
	const fstring<Size> &operator+=(const char* str) ;
	template <size_t Size2>	const fstring<Size> &operator+=(const fstring<Size2> &str) ;

	const fstring<Size> &operator=(char c) ;
	const fstring<Size> &operator=(const char* str) ;
	template <size_t Size2>	const fstring<Size> &operator=(const fstring<Size2> &str) ;

	size_t rfind(char c, size_t rpos=0) ;
	fstring<Size>& erase(size_t pos=0, size_t len=npos) ;

	char* c_str() const ;
protected:
	size_t size ;
	char str[Size+1] ;
} ;

template <size_t Size>
fstring<Size>::fstring()
{
	str[0] = '\0' ;
	size = 0 ;
}

template <size_t Size>
template <size_t Size2>
fstring<Size>::fstring(const fstring<Size2> &str)
{
	strncpy(this->str, str.c_str(), Size) ;
	size = strlen(this->str) ;
}

template <size_t Size>
fstring<Size>::fstring(const char* str)
{
	strncpy(this->str, str, Size) ;
	size = strlen(this->str) ;
}

template <size_t Size>
const fstring<Size> &fstring<Size>::operator+=(char c)
{
	if(size<Size)
	{
		str[size] = c ;
		size++ ;
		str[size] = '\0' ;
	}
	return *this ;
}

template <size_t Size>
const fstring<Size> &fstring<Size>::operator+=(const char* str)
{
	if(size<Size)
	{
		strncpy(&this->str[size], str, Size-size) ;
		size = strlen(this->str) ;
	}
	return *this ;
}

template <size_t Size>
template <size_t Size2>
const fstring<Size> &fstring<Size>::operator+=(const fstring<Size2> &str)
{
	if(size<Size)
	{
		strncpy(&this->str[size], str.c_str(), Size-size) ;
		size = strlen(this->str) ;
	}
	return *this ;
}



template <size_t Size>
const fstring<Size> &fstring<Size>::operator=(char c)
{
	str[0] = c ;
	size++ ;
	str[size] = '\0' ;
	return *this ;
}

template <size_t Size>
const fstring<Size> &fstring<Size>::operator=(const char* str)
{
	strncpy(this->str, str, Size-size) ;
	size = strlen(this->str) ;
	return *this ;
}

template <size_t Size>
template <size_t Size2>
const fstring<Size> &fstring<Size>::operator=(const fstring<Size2> &str)
{
	strncpy(this->str, str.c_str(), Size) ;
	size = strlen(this->str) ;
	return *this ;
}

template <size_t Size>
size_t fstring<Size>::rfind(char c, size_t rpos)
{
	if(rpos>=size)
		return npos ;

	for(int i=rpos ; i<size ; i++)
		if(str[i]==c)
			return i ;
	return npos ;
}

template <size_t Size>
fstring<Size>& fstring<Size>::erase(size_t pos, size_t len)
{
	char* start=str ;
	int n=0 ;

	while(n<len && pos<=size)
	{
		*start = str[pos] ;
		start++ ;
		pos++ ;
		n++ ;
	}

	return *this ;
}

template <size_t Size>
char* fstring<Size>::c_str() const
{
	return (char*)&str[0] ;
}

#endif // _FSTRING_H_
