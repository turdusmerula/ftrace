#include <objects/fmap.h>
#include <objects/fstring.h>

typedef fstring<100> Key ;

void testfmap()
{
	fmap<Key, int, 100> map ;

	map.push_back("toto", 10) ;
}
