#!/bin/tcsh -f

# this script pulls and installs Qt from source
# installation directory is /usr/local/Trolltech/...

set qt4=http://download.qt-project.org/official_releases/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.tar.gz
set qt5=http://download.qt-project.org/official_releases/qt/5.2/5.2.1/single/qt-everywhere-opensource-src-5.2.1.tar.gz

set qt=$qt4

wget $qt
tar zxf qt-*.tar.gz
cd qt-*/
./configure -opensource
make
sudo make install
