#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is /usr/local)
# if the installation path begins with /usr, the script asks for the super user password
# following the installation path the install target can be explicitly specified
# for instance: ./install.sh /usr/local zlib
# the list of available install targets is shown by specifying the install target "help"

set arg=$1
set target=$2

if ($arg != "") then
   set prefix=$arg
else
   set prefix=/usr/local
endif

if ($target == "help") then
   echo "available install targets:"
   echo " zlib libjpeg libpng"
   echo " curl squish freeglut"
   echo " proj libiconv gdal"
   echo " dcmtk sqlite exiv2"
   exit
endif

echo "installing to $prefix"

set sudo=""
if ($prefix:h == "/usr") then
   if ($USER != "root") set sudo="sudo"
endif

# zlib
if ($target == "" || $target == "zlib") then
   if (-e zlib) then
      if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip zlib on MacOS X
         echo BUILDING ZLIB
         (cd zlib;\
          ./configure --prefix=$prefix/zlib;\
          make -j 2; $sudo make install)
      endif
   endif
endif

# libjpeg
if ($target == "" || $target == "libjpeg") then
   if (-e libjpeg) then
      if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip libjpeg on MacOS X
         echo BUILDING LIBJPEG
         (cd libjpeg;\
          ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared;\
          make -j 2; $sudo make install)
      endif
   endif
endif

# libpng
if ($target == "" || $target == "libpng") then
   if (-e libpng) then
      echo BUILDING LIBPNG
      (cd libpng;\
       ./configure --prefix=$prefix/libpng --enable-static --disable-shared;\
       make -j 2; $sudo make install)
   endif
endif

# curl
if ($target == "" || $target == "curl") then
   if (-e libcurl) then
      echo BUILDING CURL
      (cd libcurl;\
       ./buildconf;\
       ./configure --prefix=$prefix/libcurl\
                   --without-ssl --disable-ldap --disable-ldaps\
                   --enable-static --disable-shared;\
       make -j 2; $sudo make install)
   endif
endif

# squish
if ($target == "" || $target == "squish") then
   if (-e squish) then
      echo BUILDING SQUISH
      (cd squish;\
       cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish;\
       make -j 2; $sudo make install)
   endif
endif

# freeglut
if ($target == "" || $target == "freeglut") then
   if (-e freeglut) then
      if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
         echo BUILDING FREEGLUT
         (cd freeglut;\
          cmake -DCMAKE_INSTALL_PREFIX=$prefix/freeglut;\
          make -j 2; $sudo make install)
      endif
   endif
endif

# other libraries to install:

# proj (required by gdal)
if ($target == "" || $target == "proj") then
   if (-e proj) then
      echo BUILDING PROJ.4
      (cd proj;\
       ./configure --prefix=$prefix;\
       make -j 2; $sudo make install)
   endif
endif

# libiconv (required by gdal 1.9+)
if ($target == "" || $target == "libiconv") then
   if (-e libiconv) then
      echo BUILDING LIBICONV
      (cd libiconv;\
       ./configure --prefix=$prefix/libiconv --enable-static --disable-shared;\
       make -j 2; $sudo make install)
   endif
endif

# gdal
if ($target == "" || $target == "gdal") then
   if (-e gdal) then
      echo BUILDING GDAL
      (cd gdal;\
       ./configure --prefix=$prefix/gdal\
                   --with-libtiff=internal --with-geotiff=internal\
                   --with-png=internal --with-jpeg=internal\
                   --without-curl --without-netcdf\
                   --without-sqlite3 --without-pg\
                   --without-ld-shared\
                   --enable-static --disable-shared;\
       make -j 2; $sudo make install)
   endif
endif

# dcmtk
if ($target == "" || $target == "dcmtk") then
   if (-e dcmtk) then
      echo BUILDING DCMTK
      if ($HOSTTYPE != "intel-mac" && $HOSTTYPE != "intel-pc") then
         (cd dcmtk;\
          ./configure --prefix=$prefix/dcmtk CXXFLAGS="-g -O2 -fpermissive";\
          make -j 2; $sudo make install-lib)
      else
         (cd dcmtk;\
          ./configure --prefix=$prefix/dcmtk;\
          make -j 2; $sudo make install-lib)
      endif
   endif
endif

# sqlite amalgamation
if ($target == "") then
   if (-e sqlite-amalgamation) then
      echo "BUILDING SQLite (amalgamation)"
      (cd sqlite-amalgamation;\
       gcc -DSQLITE_ENABLE_RTREE=1 shell.c sqlite3.c -lpthread -ldl -o sqlite3)
   endif
endif

# sqlite autoconf (built with r-tree module by default)
if ($target == "" || $target == "sqlite") then
   if (-e sqlite-autoconf) then
      echo "BUILDING SQLite (autoconf)"
      (cd sqlite-autoconf;\
       ./configure --prefix=$prefix --disable-shared --enable-static;\
       make -j 2; $sudo make install)
   endif
endif

# exiv2
if ($target == "" || $target == "exiv2") then
   if (-e exiv2) then
      echo "BUILDING exiv2"
      (cd exiv2;\
       ./configure --prefix=$prefix --disable-shared --enable-static --disable-lensdata --disable-xmp;\
       make -j 2; $sudo make install)
   endif
endif
