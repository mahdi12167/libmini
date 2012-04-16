#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is ../..)

set arg=$1

if ($arg != "") then
   set prefix=$arg
else
   set prefix=$cwd/../..
endif

# libjpeg
(cd libjpeg; ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared; make -j 2; make install)

# libpng
(cd libpng; ./configure --prefix=$prefix/libpng --enable-static --disable-shared; make -j 2; make install)

# zlib
(cd zlib; ./configure --prefix=$prefix/zlib --enable-static --disable-shared; make -j 2; make install)

# curl
(cd curl; ./buildconf; ./configure --prefix=$prefix/libcurl --disable-static --enable-shared; make -j 2; make install)

# squish
(cd squish; cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish; make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   (cd freeglut; ./autogen.sh; ./autogen.sh; ./configure --prefix=$prefix/freeglut --enable-static --disable-shared; make -j 2; make install)
endif

# other libraries to install:

# gdal
(cd gdal; ./configure --prefix=$prefix/gdal --with-libtiff=internal --with-geotiff=internal --without-netcdf --without-sqlite3 --without-pg --enable-static --disable-shared; make -j 2; make install)
