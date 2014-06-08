#!/bin/tcsh -f

# this script pulls and installs Qt from source
# installation directory is /usr/local/Trolltech/...

set qt4=ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz
set qt5=http://download.qt-project.org/official_releases/qt/5.3/5.3.0/single/qt-everywhere-opensource-src-5.3.0.tar.gz

set qt=$qt4

if (! -e $qt:t) wget $qt
tar zxf qt-*.tar.gz
cd qt-*/
./configure -opengl -release -no-webkit -no-svg -no-phonon -no-declarative -nomake examples -nomake demos -nomake tests -opensource -confirm-license
make -j 2
sudo make install
