#include <objects/fstring.h>


void testfstring()
{
	fstring<100> str1="toto" ;
	fstring<10> str2(str1) ;
	fstring<100> str3 ;
	fstring<100> str4 ;

	str3 += str2 ;
	str3 += 't' ;
	str3 += "tata" ;

	str4 = '1' ;
	str4 = "yes" ;
	str4 = str3 ;
}
