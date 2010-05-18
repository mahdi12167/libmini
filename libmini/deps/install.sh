#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is ../..)

set arg=$1

if ($arg != "") then
   set prefix=$arg
else
   set prefix=$cwd/../..
endif

# curl
(cd curl; ./buildconf; ./configure --prefix=$prefix/libcurl; make -j 2; make install)

# squish
(cd squish; cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish; make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   (cd freeglut; ./autogen.sh; ./autogen.sh; ./configure --prefix=$prefix/freeglut; make -j 2; make install)
endif

# other libraries to install:

# gdal
(cd gdal; ./configure --prefix=$prefix/gdal --with-libtiff=internal --with-geotiff=internal; make -j 2; make install)
