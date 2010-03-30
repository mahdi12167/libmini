#!/bin/tcsh -f

# this script installs the libMini dependencies locally

set prefix=$cwd/../..

./build.sh

# curl
(cd curl; ./buildconf; ./configure --prefix=$prefix/libcurl; make -j 2; make install)

# squish
(cd squish; cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish; make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   (cd freeglut; autoreconf --force --install; ./autogen.sh; ./configure --prefix=$prefix/freeglut; make -j 2; make install)
endif
