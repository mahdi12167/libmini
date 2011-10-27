Libmini Integration in QT Application
=====================================
This is an example for integrating libmini into a QT application. This
sample application primarily uses mini viewer functions but provides
additional camera control. It is free software licensed under the GPL.

(0) Prerequisites
=================
Needs installation of:
 qt/qmake
 cmake/svn
 libjpg
 libpng
 zlib

It is highly recommended to build and install qt and cmake from source!
For example grab the source tar balls from:
 ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.zip
 http://www.cmake.org/files/v2.8/cmake-2.8.6.zip

(1) Libmini Dependencies (Unix)
===============================
The following additional dependencies are required by libMini:
 libcurl
 libsquish
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; ./build.sh)

(2A) Checkout and Compilation (Unix/Mac)
========================================
Type on the unix console in your qt source directory:
 ./configure && make && sudo make install
Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/mini mini
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 (cd qtviewer; ./build.sh)

(2B) Checkout and Compilation (Windows)
=======================================
Get the following subversion repositories via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/mini
 http://libmini.googlecode.com/svn/libmini/qtviewer
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the qt source directory and type
 configure -platform win32-msvc2005
 nmake
Navigate to the mini source directory and type
 cmake -G "NMake Makefiles" -DBUILD_MINI_SFX=ON -DCMAKE_BUILD_TYPE=RELEASE
 nmake
Navigate to the qtviewer source directory and type
 build.bat

(3) Command-line Parameters
===========================
Usage: qtviewer { <tileset url> }

(4) Tileset URLs
================
URLs can point to a regular libGrid or VTBuilder (vtterain.org) tileset.
The tileset directory layout of each tileset (uri or url) has to be:
 elev/    - directory of elevation tiles
 elev.ini - ini file for elevation directory
 imag/    - directory of imagery tiles
 imag.ini - ini file for imagery directory

(5) User Interface
==================
The Qt viewer supports drag and drop of tileset urls.

(6) Example Data
================
For example, you can drag&drop the tileset directory of a libMini demo
into the viewer window. Download the Hawaii or Fraenkische demo from
stereofx.org/download and drag&drop the data/HawaiiTileset or
data/FrankischeTileset directory. Double-clicking on the layer url
brings the respective tileset into view.

(7) Navigation
==============
Navigation is self-explanatory via mouse or touch pad gestures:
 single click               - focus on target
 single click/drag          - rotate about focus
 double click               - focus and zoom on target
 mouse wheel scroll         - pan forward/backward
 touch pad scroll           - pan left/right and zoom in/out
 shift & single click drag  - heading and pitch
 shift & double click       - zoom out
 shift & touch pad scroll   - pan left/right and forward/backward

(8) Retire
==========
If you found the software useful, please send a postcard to
 Prof. Dr. Stefan Roettger
 Hochstr. 33
 90429 Nuernberg
 Germany
