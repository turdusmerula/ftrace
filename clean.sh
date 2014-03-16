#!/bin/bash


find . -type d -name Build -exec rm -rf {} \;
find . -type d -name CMakeFiles -exec rm -rf {} \;
find . -type d -name _CPack_Packages -exec rm -rf {} \;
find . -type d -name Testing -exec rm -rf {} \;
find . -type f -name install_manifest.txt -exec rm -f {} \;
find . -type f -name Makefile -exec rm -f {} \;
find . -type f -name CMakeCache.txt -exec rm -f {} \;
find . -type f -name "*.cmake" -exec rm -f {} \;
find . -type f -name "*.db" -exec rm -f {} \;
find . -type f -name "*.defs" -exec rm -f {} \;
rm -rf lib*.deb Testing
rm -rf tmp



