#!/bin/tcsh -f

# this script installs the libMini dependencies locally

set prefix=$cwd/../..

./build.sh

# curl
(cd curl; ./buildconf; ./configure --prefix=$prefix/libcurl; make; make install)

# squish
(cd squish; cmake .; make)

# freeglut
(cd freeglut; autoreconf --force --install; ./autogen.sh; ./configure --prefix=$prefix/freeglut; make; make install)
