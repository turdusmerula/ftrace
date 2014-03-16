#ifndef _FSTACK_H_
#define _FSTACK_H_

#include <stdio.h>

//Template
template <class Type, size_t Size>
class fstack
{
public:
	fstack() ;

	bool push(const Type &value) ;
	Type& pop() ;

	Type& operator[](size_t index) ;

	inline size_t size() { return vsize ; }
protected:
	size_t vsize ;
	size_t vstart ;
	size_t vend ;
	Type values[Size+1] ;
} ;

template <class Type, size_t Size>
fstack<Type, Size>::fstack()
{
	vsize = 0 ;
	vstart = 0 ;
	vend = 0 ;
}

template <class Type, size_t Size>
bool fstack<Type, Size>::push(const Type &value)
{
	if(vsize<Size)
	{
		vend++ ;
		if(vend>=vsize)
			vend = 0 ;
		values[vend] = value ;
		vsize++ ;
		return true ;
	}
	return false ;
}

template <class Type, size_t Size>
Type& fstack<Type, Size>::pop()
{
	Type& ret=values[vstart] ;

	vstart++ ;
	if(vstart>=Size)
		vstart = 0 ;

	return ret ;
}

template <class Type, size_t Size>
Type& fstack<Type, Size>::operator[](size_t index)
{
	return values[index] ;
}

#endif // _FSTACK_H_
