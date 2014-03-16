#include <objects/fstack.h>


void testfstack()
{
	fstack<int, 100> stack ;

	stack.push(10) ;

	stack.pop() ;

}
