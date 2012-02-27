! Terrain Rendering with LibMini

Libmini is free software that displays geographic terrain data at
real-time. This is an example application of integrating libmini in
the QT user interface. The application, the libMini QTViewer,
primarily uses libmini viewer functions but provides additional camera
control. It is free software licensed under the GPL.

!! Prerequisites

The libMini QTViewer Requires the installation of:
 gnu/c++ or MSVC compiler
 svn/subversion tools
 qt/qmake

It is highly recommended to build and install QT from source!

For example grab the source tar ball from:
 ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz

!! Installation (Unix/Mac)

Type on the unix console in your QT source directory:
 ./configure && make && sudo make install

!! Installation (Windows):

Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Choose an appropriate platform to describe the version of MSVC:
 MSVC 8.0 -> SET PLATFORM=win32-msvc2005
 MSVC 9.0 -> SET PLATFORM=win32-msvc2008
 MSVC 10.0 -> SET PLATFORM=win32-msvc2010
Navigate to the QT source directory and type
 configure -platform %PLATFORM%
 nmake
Add the "bin" directory in your QT directory to your path:
 set PATH=%cd%\bin;%PATH%
Or add the "bin" directory to your PATH environment variable:
 System -> Advanced -> Environment Variables -> PATH

!! Libmini Dependencies (Unix/Mac)

Get the libMini dependencies from the libMini subversion repository.
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; ./build.sh)

!! Libmini Dependencies (Windows)

Precompiled static libraries for Windows MSVC 2005 are provided in the
libMini WIN32 directory. In your project directory get the following
sub-directory via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/WIN32

Additionally we need GDAL to be installed. In your project directory
get GDAL via Tortoise SVN:
 http://svn.osgeo.org/gdal/tags/1.8.1/gdal
In the GDAL source directory we have to tweak some variables in the nmake.opt file:
 MSVC_VER -> use 1600 to identify MSVC 10.0 for example, other versions accordingly
 GDAL_HOME -> point it to a directory below the qtviewer source package
  Usually this means that we point it to our project directory
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the GDAL source directory and type
 nmake /f makefile.vc staticlib
 set GDAL_HOME="same path as in nmake.opt"
 copy gdal.lib port\*.h gcore\*.h ogr\*.h %GDAL_HOME%

!! Checkout and Compilation (Unix/Mac)

Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/mini mini
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 (cd qtviewer; ./build.sh)

!! Checkout and Compilation (Windows)

Get the following subversion repositories via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/mini
 http://libmini.googlecode.com/svn/libmini/qtviewer
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the qtviewer source directory and type
 build.bat

!! Command-line Parameters

The application can be started without command line parameters:

 ./qtviewer

Then the application will show the entire earth on startup.

The application also accepts a list of urls as parameters.
Each url needs to point to a tileset directory.

Usage on the unix terminal:
 ./qtviewer { <tileset url> }

Then the application will zoom into the first specified tileset.

!! Tileset URLs

A tileset url can point to a regular libGrid or VTBuilder
(vterrain.org) tileset. The tileset directory layout of each tileset
(uri or url) has to be:

 elev/    - directory of elevation tiles
 elev.ini - ini file for elevation directory
 imag/    - directory of imagery tiles
 imag.ini - ini file for imagery directory

!! User Interface

The Qt viewer supports drag and drop of tileset urls.

!! Example Data

For example, you can drag&drop the tileset data directory of a libMini
demo into the viewer window. Download the Hawaii or Fraenkische demo
from stereofx.org/download and drag&drop the data/HawaiiTileset or
data/FrankischeTileset directory into the viewer
window. Double-clicking on the layer brings the respective tileset
into view.

!! Navigation

Navigation is self-explanatory via mouse or touch pad gestures:

 single click               - focus on target
 single click/drag          - rotate about focus
 double click               - focus and zoom on target
 mouse wheel scroll         - pan forward/backward
 touch pad scroll           - pan left/right and zoom in/out
 shift & single click drag  - heading and pitch
 shift & double click       - zoom out
 shift & touch pad scroll   - pan left/right and forward/backward

!! Extension

The QTViewer can be extended to display additional geometry (such as
buildings, GPS paths etc.) in a georeferenced ECEF context by deriving
a Renderer class from QTViewer's Viewer class. In that class, we
overload the build_ecef_geometry() method, which constructs a scene
graph using libMini's mininode class as a basis.

For example, we can add a cylinder as the north pole by specifying its
georeferenced position (mininode_coord), color (mininode_color) and
geometry (mininode_geometry_tube) and linking those node components
together as a simple scene graph. The so defined scene graph is
rendered on top of the earth and on top of any loaded libGrid or
VTBuilder tilesets:

 class Renderer: public Viewer
 {
    protected:

    mininode_group *Renderer::build_ecef_geometry()
    {
       mininode_group *scene =
          new mininode_coord(minicoord(miniv3d(0, 90*3600, 0), minicoord::MINICOORD_LLH))->
             append_child(new mininode_color(miniv3d(0.0, 0.0, 1.0)))->
             append_child(new mininode_geometry_tube(10000, 500000));

       return(scene);
    }
 };

More information about libMini's node classes can be found in the
mininodes.h header.

!! Postcard

If you found the software useful, please send a vacation postcard to:

 Prof. Dr. Stefan Roettger
 Wassertorstr. 10
 90489 Nuernberg
 Germany

Thanks!
