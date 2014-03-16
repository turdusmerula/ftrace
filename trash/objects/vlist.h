#ifndef _FVECTOR_H_
#define _FVECTOR_H_

#include <stdio.h>
#include <string.h>

template <class Type>
struct VListNode
{
	VListNode* next ;
	VListNode* prev ;
	Type data ;
} ;

//Template
template <class Type>
class vlist
{
public:
	vlist() ;

	bool push_back(const Type &value) ;

	inline size_t size() { return vsize ; }
protected:
	size_t vsize ;
} ;

template<typename T>
struct VListIterator
{
	VListNode<T>* node ;

	typedef VListIterator<_Tp>                _Self;
	typedef VListNode<_Tp>                    _Node;

	typedef ptrdiff_t                          difference_type;
	typedef std::bidirectional_iterator_tag    iterator_category;
	typedef _Tp                                value_type;
	typedef _Tp*                               pointer;
	typedef _Tp&                               reference;

	VListIterator()
	: node() { }

	explicit
	VListIterator(VListNode* __x)
	: node(__x) { }

	// Must downcast from _List_node_base to _List_node to get to _M_data.
	reference
	operator*() const
	{ return static_cast<_Node*>(node)->data ; }

	pointer
	operator->() const
	{ return std::__addressof(static_cast<_Node*>(_M_node)->data); }

	_Self&
	operator++()
	{
	_M_node = _M_node->_M_next;
	return *this;
	}

	_Self
	operator++(int)
	{
	_Self __tmp = *this;
	_M_node = _M_node->_M_next;
	return __tmp;
	}

	_Self&
	operator--()
	{
	_M_node = _M_node->_M_prev;
	return *this;
	}

	_Self
	operator--(int)
	{
	_Self __tmp = *this;
	_M_node = _M_node->_M_prev;
	return __tmp;
	}

	bool
	operator==(const _Self& __x) const
	{ return _M_node == __x._M_node; }

	bool
	operator!=(const _Self& __x) const
	{ return _M_node != __x._M_node; }

	// The only member points to the %list element.
};


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
