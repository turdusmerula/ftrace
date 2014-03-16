#ifndef _FMAP_H_
#define _FMAP_H_

#include <stdio.h>
#include <utility>

//Template
template <class Key, class Type, size_t Size>
class fmap
{
public:
	fmap() ;

	bool push_back(const std::pair<Key, Type> &value) ;
	bool push_back(const Key &key, const Type &value) ;

	size_t find(const Key &key) ;
	std::pair<Key, Type>& operator[](size_t index) ;

	inline size_t size() { return vsize ; }
protected:
	size_t vsize ;
	std::pair<Key, Type> values[Size+1] ;
} ;

template <class Key, class Type, size_t Size>
fmap<Key, Type, Size>::fmap()
{
	vsize = 0 ;
}

template <class Key, class Type, size_t Size>
bool fmap<Key, Type, Size>::push_back(const std::pair<Key, Type> &value)
{
	if(vsize<Size)
	{
		values[vsize] = value ;
		vsize++ ;
		return true ;
	}
	return false ;
}

template <class Key, class Type, size_t Size>
bool fmap<Key, Type, Size>::push_back(const Key &key, const Type &value)
{
	if(vsize<Size)
	{
		values[vsize] = std::pair<Key, Type>(key, value) ;
		vsize++ ;
		return true ;
	}
	return false ;
}

template <class Key, class Type, size_t Size>
size_t fmap<Key, Type, Size>::find(const Key &key)
{
	for(size_t i=0 ; i<size ; i++)
		if(values[i].first==key)
			return i ;
	return -1 ;
}

template <class Key, class Type, size_t Size>
std::pair<Key, Type>& fmap<Key, Type, Size>::operator[](size_t index)
{
	return values[index] ;
}

#endif // _FMAP_H_
