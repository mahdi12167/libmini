#!/bin/tcsh -f

# this script pulls and installs git

set git=git-1.7.8
set host=http://git-core.googlecode.com/files/

wget $host/$git.tar.gz
tar zxf $git.tar.gz

(cd $git; ./configure --prefix=$HOME --without-iconv; make; make install)
