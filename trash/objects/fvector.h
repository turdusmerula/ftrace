#ifndef _FVECTOR_H_
#define _FVECTOR_H_

#include <stdio.h>
#include <string.h>

//Template
template <class Type, size_t Size>
class fvector
{
public:
	fvector() ;

	bool push_back(const Type &value) ;

	Type& operator[](size_t index) ;

	inline size_t size() { return vsize ; }
protected:
	size_t vsize ;
	Type values[Size+1] ;
} ;

template <class Type, size_t Size>
fvector<Type, Size>::fvector()
{
	vsize = 0 ;
}

template <class Type, size_t Size>
bool fvector<Type, Size>::push_back(const Type &value)
{
	if(vsize<Size)
	{
		values[vsize] = value ;
		vsize++ ;
		return true ;
	}
	return false ;
}

template <class Type, size_t Size>
Type& fvector<Type, Size>::operator[](size_t index)
{
	return values[index] ;
}

#endif // _FVECTOR_H_
