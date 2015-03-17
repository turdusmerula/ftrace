#include <iostream>

void recursive()
{
	static int var=0 ;

	var++ ;
	if(var<20)
		recursive() ;
	new char[var] ;
}

void func1()
{
}

void func2()
{
	for(int i=0 ; i<100 ; i++)
		func1() ;
}

void func3()
{
	func2() ;
	for(int i=0 ; i<10 ; i++)
		func1() ;
}

void func4()
{
	func3() ;
	func2() ;
	func1() ;
	func3() ;
}

void func5()
{
	for(int i=0 ; i<5 ; i++)
		func4() ;
}

int main(int argc, char* argv[])
{
	std::cout << "Test" << std::endl ;

	recursive() ;

	func5() ;

	recursive() ;

	return 0 ;
}
