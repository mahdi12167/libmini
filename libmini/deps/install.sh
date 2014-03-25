#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is /usr/local)
# if the installation path is the default, this script must be run as super user (sudo ./install.sh)

set arg=$1

if ($arg != "") then
   set prefix=$arg
else
   set prefix=/usr/local
endif

echo "installing to $prefix"

if ($prefix == "usr/local") then
   if ($USER != "root") then
      echo "this script must be run as super user (sudo ./install.sh)"
      exit 1
   endif
endif

# zlib
if (-e zlib) then
   if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip zlib on MacOS X
      echo BUILDING ZLIB
      (cd zlib;\
       ./configure --prefix=$prefix/zlib;\
       make -j 2; make install)
   endif
endif

# libjpeg
if (-e libjpeg) then
   if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip libjpeg on MacOS X
      echo BUILDING LIBJPEG
      (cd libjpeg;\
       ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared;\
       make -j 2; make install)
   endif
endif

# libpng
if (-e libpng) then
   echo BUILDING LIBPNG
   (cd libpng;\
    ./configure --prefix=$prefix/libpng --enable-static --disable-shared;\
    make -j 2; make install)
endif

# curl
if (-e libcurl) then
   echo BUILDING CURL
   (cd libcurl;\
    ./buildconf;\
    ./configure --prefix=$prefix/libcurl\
                --without-ssl --disable-ldap --disable-ldaps\
                --enable-static --disable-shared;\
    make -j 2; make install)
endif

# squish
echo BUILDING SQUISH
(cd squish;\
 cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish;\
 make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   echo BUILDING FREEGLUT
   (cd freeglut;\
    cmake -DCMAKE_INSTALL_PREFIX=$prefix/freeglut;\
    make -j 2; make install)
endif

# other libraries to install:

# proj (required by gdal)
if (-e proj) then
   echo BUILDING PROJ.4
   (cd proj;\
    ./configure --prefix=$prefix;\
    make -j 2; make install)
endif

# libiconv (required by gdal 1.9+)
if (-e libiconv) then
   echo BUILDING LIBICONV
   (cd libiconv;\
    ./configure --prefix=$prefix/libiconv --enable-static --disable-shared;\
    make -j 2; make install)
endif

# gdal
echo BUILDING GDAL
(cd gdal;\
 ./configure --prefix=$prefix/gdal\
             --with-libtiff=internal --with-geotiff=internal\
             --with-png=internal --with-jpeg=internal\
             --without-curl --without-netcdf\
             --without-sqlite3 --without-pg\
             --without-ld-shared\
             --enable-static --disable-shared;\
 make -j 2; make install)

# dcmtk
echo BUILDING DCMTK
if ($HOSTTYPE != "intel-mac" && $HOSTTYPE != "intel-pc") then
   (cd dcmtk;\
    ./configure --prefix=$prefix/dcmtk CXXFLAGS="-g -O2 -fpermissive";\
    make -j 2; make install-lib)
else
   (cd dcmtk;\
    ./configure --prefix=$prefix/dcmtk;\
    make -j 2; make install-lib)
endif
