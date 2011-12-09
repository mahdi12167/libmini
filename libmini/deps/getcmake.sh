#!/bin/tcsh -f

# this script pulls and installs cmake

set cmake=cmake-2.8.5
set host=http://www.cmake.org/files/v2.8

wget $host/$cmake.tar.gz
tar zxf $cmake.tar.gz

(cd $cmake; ./configure --prefix=$HOME; make; make install)
