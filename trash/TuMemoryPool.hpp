#include <cppunit/extensions/HelperMacros.h>

#include <mem/MemoryPool.h>

#include <list>

class MemoryPoolTest : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(MemoryPoolTest) ;
    CPPUNIT_TEST(testCreateData) ;
    CPPUNIT_TEST(testAllocator) ;
//    CPPUNIT_TEST(testCreateBigData) ;
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp ()
    {
		std::cout << std::endl ;
    }

	void tearDown()
	{
	}


	void testCreateData()
	{
		for(int i=0 ; i<100 ; i++)
		{
			char* str=(char*)MemoryPool::add(100) ;
			if(str!=nullptr)
				for(int j=0 ; j<1024 ; j++)
					str[j] = 1 ;
		}

		displayPoolStats() ;
	}

//	void testCreateBigData()
//	{
//		for(int i=0 ; i<10 ; i++)
//		{
//			char* str=(char*)MemoryPool::add(999999) ;
//			for(int j=0 ; j<999999 ; j++)
//				str[j] = 1 ;
//		}
//
//		displayPoolStats() ;
//	}

	void testAllocator()
	{
		std::list<int, MemoryPoolAllocator<int>> l ;

		l.push_back(0) ;
		l.push_back(1) ;
		l.push_back(2) ;
		l.push_back(3) ;

		for(auto& n : l)
			std::cout << "Element: " << n << std::endl ;
		displayPoolStats() ;
	}

	void displayPoolStats()
	{
		std::cout << "Pool size: " << MemoryPool::getPoolSize() << std::endl ;
		std::cout << "Used size: " << MemoryPool::getUsedSize() << std::endl ;
	}
} ;

CPPUNIT_TEST_SUITE_REGISTRATION(MemoryPoolTest) ;
