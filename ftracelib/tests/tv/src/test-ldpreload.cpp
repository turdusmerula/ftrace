#include <iostream>
#include <vector>
#include <thread>
#include <memory>

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

static std::vector<std::shared_ptr<std::thread>> threads ;

int main(int argc, char* argv[])
{
	std::cout << "Test" << std::endl ;

	recursive() ;

	func5() ;

	recursive() ;

	for(int i=0 ; i<20 ; i++)
	{
		threads.push_back(std::make_shared<std::thread>(
			[]() {
				func5() ;
			}
		)) ;
	}
	for(auto& thread : threads)
		thread->join() ;


	return 0 ;
}
