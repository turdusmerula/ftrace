#include <objects/vstring.h>
#include <mem/MemoryPool.h>

#include <string.h>

vstring::vstring()
{
	vstr = nullptr ;
	vsize = 0 ;
	msize = 0 ;
}

vstring::vstring(size_t size)
{
	vstr = MemoryPool::add<char>(size+1) ;
	vsize = size ;
	msize = size ;
}

vstring::vstring(const vstring &str)
{
	if(vstr==nullptr || msize<=str.size()+1)
	{
		vstr = MemoryPool::add<char>(str.size()+1) ;
		msize = str.size() ;
	}
	strcpy(vstr, str.c_str()) ;
	vsize = str.size() ;
}

vstring::vstring(const char* str)
{
	size_t len=strlen(str) ;
	if(vstr==nullptr || msize<=len+1)
	{
		vstr = MemoryPool::add<char>(len+1) ;
		msize = len ;
	}
	strncpy(vstr, str, len) ;
	vsize = len ;
}

const vstring &vstring::operator+=(char c)
{
	if(vsize+1>=msize)
	{
		char* nstr=MemoryPool::add<char>(msize+100) ;
		msize = msize+100 ;
		strcpy(nstr, vstr) ;
		vstr = nstr ;
	}
	vstr[vsize] = c ;
	vsize++ ;
	vstr[vsize] = '\0' ;

	return *this ;
}

const vstring &vstring::operator+=(const char* str)
{
	size_t len=strlen(str) ;
	if(vsize+len+1>=msize)
	{
		char* nstr=MemoryPool::add<char>(msize+len+1) ;
		msize = msize+100 ;
		strcpy(nstr, vstr) ;
		vstr = nstr ;
	}
	strcpy(vstr+vsize, str) ;
	vsize = vsize+len ;

	return *this ;
}

const vstring &vstring::operator+=(const vstring &str)
{
	if(vsize+str.size()+1>=msize)
	{
		char* nstr=MemoryPool::add<char>(msize+str.size()+1) ;
		msize = msize+100 ;
		strcpy(nstr, vstr) ;
		vstr = nstr ;
	}
	strcpy(vstr+vsize, str.c_str()) ;
	vsize = vsize+str.size() ;

	return *this ;
}

const vstring &vstring::operator=(char c)
{
	if(vstr==nullptr)
	{
		vstr = MemoryPool::add<char>(100) ;
		msize = 100 ;
	}
	vstr[0] = c ;
	vsize = 1 ;
	vstr[vsize] = '\0' ;
	return *this ;
}

const vstring &vstring::operator=(const char* str)
{
	size_t len=strlen(str) ;
	if(vstr==nullptr || len+1<=msize)
	{
		str = MemoryPool::add<char>(len+1) ;
		msize = len ;
	}
	strcpy(vstr, str) ;
	vsize = len ;
	return *this ;
}

const vstring &vstring::operator=(const vstring &str)
{
	size_t len=str.size() ;
	if(vstr==nullptr || len+1<=msize)
	{
		vstr = MemoryPool::add<char>(len+1) ;
		msize = len ;
	}
	strcpy(vstr, str.c_str()) ;
	vsize = len ;
	return *this ;
}

size_t vstring::rfind(char c, size_t rpos)
{
	if(vstr==nullptr)
		return npos ;

	if(rpos>=vsize)
		return npos ;

	for(size_t i=rpos ; i<vsize ; i++)
		if(vstr[i]==c)
			return i ;
	return npos ;
}

vstring& vstring::erase(size_t pos, size_t len)
{
	char* start=vstr ;
	size_t n=0 ;

	if(vstr==nullptr)
		return *this ;

	while(n<len && pos<=vsize)
	{
		*start = vstr[pos] ;
		start++ ;
		pos++ ;
		n++ ;
	}

	return *this ;
}

char* vstring::c_str() const
{
	return vstr ;
}
