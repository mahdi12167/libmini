#!/bin/tcsh -f

# this script pulls and installs Qt from source
# installation directory is /usr/local/Trolltech/...

set qt4=ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz
set qt5=http://download.qt-project.org/official_releases/qt/5.3/5.3.0/single/qt-everywhere-opensource-src-5.3.0.tar.gz

set qt=$qt4

set SQLITE=/usr/local

if (! -e $qt:t) wget $qt
tar zxf qt-*.tar.gz
cd qt-*/

./configure -opengl -openssl -plugin-sql-sqlite -release -no-webkit -no-svg -no-phonon -no-declarative -nomake examples -nomake tests -opensource -confirm-license

make -j 2
sudo make install

if (! -e $SQLITE/lib/libsqlite3.a) then
   wget http://www.sqlite.org/2014/sqlite-autoconf-3080500.tar.gz
   tar zxf sqlite-autoconf-3080500.tar.gz
   (cd sqlite-autoconf-3080500;\
    ./configure --prefix=$SQLITE --disable-shared --enable-static;\
    make -j 2; sudo make install)
endif

if (-e $SQLITE/lib/libsqlite3.a) then
   (cd src/plugins/sqldrivers/sqlite;\
    qmake "INCLUDEPATH+=$SQLITE/include" "LIBS+=$SQLITE/lib/libsqlite3.a" && make clean && make && sudo make install)
endif
