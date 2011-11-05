Libmini Integration in QT Application
=====================================
This is a sample application for integrating libmini into QT. This
example primarily uses mini viewer functions but provides additional
camera control. It is free software licensed under the GPL.

(0) Prerequisites
=================
Requires installation of:
 qt/qmake
 cmake/svn
 libjpg
 libpng
 zlib

It is highly recommended to build and install QT from source!
For example grab the source tar ball from:
 ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz

(1A) Installation (Unix/Mac)
============================
Type on the unix console in your QT source directory:
 ./configure && make sub-tools && sudo make install

(1B) Installation (Windows):
============================
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the QT source directory and type
 configure -platform win32-msvc2005
 nmake sub-tools

(2A) Libmini Dependencies (Unix/Mac)
====================================
Get the libMini dependencies from the libMini subversion repository.
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; ./build.sh)

(2B) Libmini Dependencies (Windows)
===================================
Precompiled static libraries for Windows MSVC 2005 are provided in the
libMini WIN32 directory. In your project directory get the following
sub-directory via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/WIN32

(3A) Checkout and Compilation (Unix/Mac)
========================================
Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/mini mini
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 (cd qtviewer; ./build.sh)

(3B) Checkout and Compilation (Windows)
=======================================
Get the following subversion repositories via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/mini
 http://libmini.googlecode.com/svn/libmini/qtviewer
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the qtviewer source directory and type
 build.bat

(4) Command-line Parameters
===========================
Usage: qtviewer { <tileset url> }

(5) Tileset URLs
================
URLs can point to a regular libGrid or VTBuilder (vterrain.org) tileset.
The tileset directory layout of each tileset (uri or url) has to be:
 elev/    - directory of elevation tiles
 elev.ini - ini file for elevation directory
 imag/    - directory of imagery tiles
 imag.ini - ini file for imagery directory

(6) User Interface
==================
The Qt viewer supports drag and drop of tileset urls.

(7) Example Data
================
For example, you can drag&drop the tileset directory of a libMini demo
into the viewer window. Download the Hawaii or Fraenkische demo from
stereofx.org/download and drag&drop the data/HawaiiTileset or
data/FrankischeTileset directory. Double-clicking on the layer brings
the respective tileset into view.

(8) Navigation
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

(9) Extension
=============
The QTViewer can be extended to display additional geometry (such as
buildings, GPS paths etc.) in a georeferenced ECEF context by deriving
a Renderer class from QTViewer's Viewer class. In that class, we
overload the build_ecef_geometry() method, which constructs a scene
graph using libMini's mininode class as a basis.

For example, we can add a cylinder as a axis at the north pole by
specifying its georeferenced position (mininode_coord), color
(mininode_color) and geometry (mininode_geometry_tube) and linking
those node components together as a simple scene graph. The so defined
scene graph is rendered on top of the earth and on top of any loaded
libGrid or VTBuilder tilesets:

class Renderer: public Viewer
{
   protected:

   mininode *Renderer::build_ecef_geometry()
   {
      mininode *scene =
         new mininode_coord(minicoord(miniv3d(0,90*3600,0),minicoord::MINICOORD_LLH))->
            append_child(new mininode_color(miniv3d(0.0,0.0,1.0)))->
            append_child(new mininode_geometry_tube(10000,500000));

      return(scene);
   }
};

More information about libMini's node classes can be found in the mininodes.h header.

(10) Retire
===========
If you found the software useful, please send a vacation postcard to
 Prof. Dr. Stefan Roettger
 Hochstr. 33
 90429 Nuernberg
 Germany
