#!/bin/tcsh -f

# this script pulls and installs qt

if (! -X wget) then
   echo warning: wget is not installed!
   exit 1
endif

set qt=qt-everywhere-opensource-src-4.7.4
set host=ftp://ftp.qt.nokia.com/qt/source

wget $host/$qt.tar.gz
tar zxf $qt.tar.gz

(cd $qt; ./configure && make && sudo make install)
