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
echo BUILDING LIBJPEG
(cd libjpeg; ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared; make -j 2; make install)

# libpng
echo BUILDING LIBPNG
(cd libpng; ./configure --prefix=$prefix/libpng --enable-static --disable-shared; make -j 2; make install)

# zlib
echo BUILDING ZLIB
(cd zlib; ./configure --prefix=$prefix/zlib; make -j 2; make install)

# curl
echo BUILDING CURL
(cd curl;\
 ./buildconf;\
 ./configure --prefix=$prefix/libcurl\
             --without-ssl --disable-ldap --disable-ldaps\
             --enable-static --disable-shared;\
 make -j 2; make install)

# squish
echo BUILDING SQUISH
(cd squish; cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish; make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   echo BUILDING FREEGLUT
   (cd freeglut; ./autogen.sh; ./autogen.sh; ./configure --prefix=$prefix/freeglut --enable-static --disable-shared; make -j 2; make install)
endif

# other libraries to install:

# gdal
echo BUILDING GDAL
(cd gdal;\
 ./configure --prefix=$prefix/gdal\
             --with-libtiff=internal --with-geotiff=internal\
             --with-rename-internal-libtiff-symbols=yes --with-rename-internal-libgeotiff-symbols=yes\
             --with-png=internal --with-jpeg=internal\
             --without-threads --without-curl --without-netcdf\
             --without-sqlite3 --without-pg\
             --without-ld-shared\
             --enable-static --disable-shared;\
 make -j 2; make install)
