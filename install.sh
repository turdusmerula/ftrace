#!/bin/bash

rm -f *.deb
make package
sudo dpkg -r libftrace
sudo dpkg -i libftrace*.deb
