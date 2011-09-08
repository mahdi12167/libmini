Libmini Integration in QT Application
=====================================
This is an example for integrating libmini into a QT application. This
sample application primarily uses mini viewer functions but provides
additional camera control.

(0) Prerequisites
=================
Needs installation of:
 qt/qmake
 cmake/svn
 libjpg
 libpng
 zlib

(1) Libmini Dependencies
========================
The following additional dependencies are required by libMini:
 libcurl
 libsquish
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; ./build.sh)

(2) Checkout and Compilation
============================
Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/mini mini
 (cd mini; cmake -DBUILD_MINI_SFX . && make)
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 (cd qtviewer; ./build.sh)

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

(6) Navigation
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

(7) Retire
==========
If you found the software useful, please send postcard to
 Prof. Dr. Stefan Roettger
 Hochstr. 33
 90429 Nuernberg
 Germany
