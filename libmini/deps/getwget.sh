#!/bin/tcsh -f

# this script pulls and installs wget

if (-X ftp) then
   echo warning: ftp is not installed!
   exit 1
endif

set wget=wget-1.13
set host=ftp://ftp.gnu.org/gnu/wget

ftp $host/$wget.tar.gz
tar zxf $wget.tar.gz

(cd $wget; ./configure --without-ssl --prefix=$HOME; make; make install)
