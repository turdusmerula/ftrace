#!/bin/bash

function usage()
{
	echo "$0 [<targets> ...]"
	echo "Available targets:"
	echo "    clean: clean sources" 
	echo "    depclean: clean sources and makefiles" 
	echo "    debug/release: build with debuging informations or not" 
	echo "    clang: build with clang instead of gcc" 
	echo "    notest: skip tests" 
	echo "    perfo: run performance tests" 
	echo "    gcov: add covering informations, imply debug" 
	echo "    valgrind: run valgrind tests, imply debug" 
	echo "    callgrind: run callgrind tests, imply debug" 
	echo "    cdash: send tests to cdash" 
	echo "    verbose: enable verbose mode" 
	echo "    help: output command help and quit" 
	exit 0
}

notest=0
callgrind=0
perfo=0

cmake_opts="-DCTEST_USE_LAUNCHERS=ON"
for arg in "$@"
do
	if [[ "_$arg" == "_clean" ]]
	then
		make clean
	fi
	
	if [[ "_$arg" == "_depclean" ]]
	then
		./clean.sh
	fi

	if [[ "_$arg" == "_debug" ]]
	then
		buildtype=Debug
	fi
	
	if [[ "_$arg" == "_release" ]]
	then
		buildtype=Release
	fi
	
	if [[ "_$arg" == "_clang" ]]
	then
		cmake_opts="$cmake_opts -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++"
	fi

	if [[ "_$arg" == "_notest" ]]
	then
		notest=1
	fi
	
	if [[ "_$arg" == "_perfo" ]]
	then
		perfo=1
	fi
	
	if [[ "_$arg" == "_gcov" ]]
	then
		buildtype=gcov
	fi

	if [[ "_$arg" == "_callgrind" ]]
	then
		cmake_opts="$cmake_opts -DPROFILE_TYPE=callgrind"
		callgrind=1
	fi

	if [[ "_$arg" == "_valgrind" ]]
	then
		cmake_opts="$cmake_opts -DPROFILE_TYPE=valgrind"
	fi

	if [[ "_$arg" == "_cdash" ]]
	then
		export cdash=1
	fi

	if [[ "_$arg" == "_verbose" ]]
	then
		export VERBOSE=1
	fi

	if [[ "_$arg" == "_help" ]]
	then
		usage
	fi

done

echo "Options: $cmake_opts"

if [[ "_$buildtype" != "_" ]]
then
	cmake_opts="$cmake_opts -DCMAKE_BUILD_TYPE=$buildtype"
fi

#All cmake files are purged when cleaning but this one is important for CDash
cat CTestConfig.txt > CTestConfig.cmake

echo "CMake options: $cmake_opts"
cmake . $cmake_opts

make
if [[ "_$notest" != "_1" ]]
then
	if [[ "_$callgrind" == "_1" ]]
	then
		if [[ "_$perfo" == "_1" ]]
		then
			cd tests/perfo
			/usr/bin/valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --callgrind-out-file=../../Testing/callgrind.out.%p Build/bin/core-odb-perfo
		else
			cd tests/tu
			/usr/bin/valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --callgrind-out-file=../../Testing/callgrind.out.%p Build/bin/core-odb-test
		#--dump-instr=yes --trace-jumps=yes Build/bin/core-odb-test
		fi
		kcachegrind $(ls -1 -t ../../Testing/callgrind.out.* | head -n1)			
	elif [[ "_${cdash}" == "_1" ]]
	then
		ctest -D Experimental -D NightlyMemCheck
	else
		ctest	
	fi
	
fi

