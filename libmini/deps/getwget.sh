#!/bin/tcsh -f

# this script pulls and installs wget

set wget=wget-1.13
set host=http://ftp.gnu.org/gnu/wget/

wget $host/$wget.tar.gz
tar zxf $wget.tar.gz

(cd $wget; ./configure --without-ssl --prefix=$HOME; make; make install)
