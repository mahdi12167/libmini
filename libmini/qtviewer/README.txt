! Terrain Rendering with LibMini

Libmini is free software that displays geographic terrain data at
real-time. This is an example application of integrating libmini in
the Qt user interface. The application, the libMini QTViewer,
primarily uses libmini viewer functions but provides additional camera
control. It is free software licensed under the GPL.

Tested platforms are:
* MacOS X 10.5 and 10.6
* Debian
* Ubuntu 11.04 and 12.04
* Linux Mint 13
* OpenSuSe 11.4
* Windows XP and Windows 7
Other platforms may work, but are untested.

!! Prerequisites

The compilation of the libMini QTViewer requires the installation of:
 tcsh, autotools and cmake (unix)
 gnu/c++ (unix) or MSVC compiler (windows)
 svn and git (unix) or Tortoise SVN (windows)
 OpenGL (and GLUT)
 qt/qmake

The installation of OpenGL and GLUT is vendor specific: On MacOS X it
is already installed with the XCode development package, on Linux it
comes with the "mesa-dev", "X11-dev" and "free-glut3-dev" development
packages whereas on Windows it is usually installed with the MSVC IDE.

On MacOS X and Windows, it is recommended to build and install Qt from source!
On Linux, it is mostly sufficient to install a recent Qt binary package.

If you install Qt from source, grab the source tar ball from:
 ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz

!! Qt Installation (Unix/Mac)

Type on the unix console in your Qt source directory:
 ./configure -opensource && make && sudo make install

After the build process has finished (go get yourself a cup of coffee),
you will be asked to enter your root password for installation of Qt.

!! Qt Installation (Windows):

Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Choose an appropriate platform to describe the version of MSVC:
 MSVC 8.0 -> SET PLATFORM=win32-msvc2005
 MSVC 9.0 -> SET PLATFORM=win32-msvc2008
 MSVC 10.0 -> SET PLATFORM=win32-msvc2010
Navigate into the Qt source directory and type
 configure -platform %PLATFORM% -opensource -nomake examples -nomake demos
 nmake
Add the "bin" directory in your Qt directory to your path:
 set PATH=%cd%\bin;%PATH%
Or add the "bin" directory to your PATH environment variable:
 System -> Advanced -> Environment Variables -> PATH

At your option, you can configure Qt to produce static libraries with
 configure -platform %PLATFORM% -opensource -release -static -no-libpng -no-sql-sqlite -nomake examples -nomake demos

!! Installation of Libmini Dependencies (Unix/Mac)

Get the libMini dependencies from the libMini subversion repository.
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; sudo ./build.sh)

!! Installation of Libmini Dependencies (Windows)

Precompiled static libraries for Windows MSVC 2005 are provided in the
libMini WIN32 directory. In your project directory, get the following
sub-directory via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/WIN32

Additionally we need GDAL to be installed. In your project directory,
get GDAL via Tortoise SVN:
 http://svn.osgeo.org/gdal/tags/1.8.1/gdal
To build GDAL we have to define some options (in the nmake.opt file):
 MSVC_VER -> use 1600 to identify MSVC 10.0, 1500 for MSVC 9.0, etc.
 GDAL_HOME -> needs to point to the GDAL installation directory
  Usually this means that we point it to our project directory
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate into the GDAL source directory and type
 set DEST="project directory"
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST%
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST% devinstall
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST% staticlib
 copy gdal.lib %DEST%\lib
 copy port\*.h %DEST%\include
 copy gcore\*.h %DEST%\include
 copy ogr\*.h %DEST%\include

!! QTViewer Checkout and Compilation (Unix/Mac)

Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 with cmake: (cd qtviewer; cmake . && make)
 with qmake: (cd qtviewer; ./build.sh)

!! QTViewer Checkout and Compilation (Windows)

In your project directory, get the following subversion repository
via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/qtviewer
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate into the qtviewer source directory and type
 build.bat

!! Usage

The application can be started without command line parameters:

 ./qtviewer

Then the application will show the entire earth on startup.

The application also accepts a list of urls as parameters.
Each url needs to point to a geotiff image layer or a tileset directory.

Usage on the unix terminal:
 ./qtviewer { <image.tif> | <tileset url> }

Then the application will zoom into the first specified argument.

!! Image Layers

An image layer can contain imagery (color layer) or elevation
information (dem layer).

The usual file format for color and dem layers is geotiff. See
http://www.gdal.org for more information on that format and additional
tools to process geotiff images.

!! Tileset URLs

A tileset url can point to a regular libGrid or VTBuilder
(vterrain.org) tileset. The tileset directory layout of each tileset
(uri or url) has to be:

 elev/    - directory of elevation tiles
 elev.ini - ini file for elevation directory
 imag/    - directory of imagery tiles
 imag.ini - ini file for imagery directory

The elev and image directories contain the tiles of the tileset stored
in the proprietary libMini DB format:
* The DB format provides mip-mapped s3tc-compressed image data.
* The DB format also provides lzw-compressed floating point elevation data.

!! User Interface

The QTViewer user interface supports drag and drop of geo-referenced
images and tileset urls.

The images are displayed as 3D thumbnails at the geo-referenced
position, whereas the tilesets are displayed in full 3D. The images
and tilesets are also displayed in a list view on the right.

Right-clicking at the list view triggers a context menu with a variety
of available operations:
* A single layer can be opened.
** It is displayed as flat geo-referenced thumbnail.
* A single layer can be inspected.
** Info about its size and geo-reference is shown.
* A single dem layer can be shaded.
** A diffuse noon lighting technique is applied.
* Multiple images can be selected or unselected.
* Multiple selected images can be resampled to a produce a tileset.
** A tileset is displayed in full 3D.
** Its appearance takes both selected color and dem layers into account.
** At least a single dem layer must be selected.
** Resampling takes place in a background thread.
*** Progress is shown in percent below the list view.
*** Resampling is a computationally expensive operation and can take several minutes to hours.
*** It produces a number of tiles that are stored in the proprietary libMini DB format.
*** For more information about resampling, please see the README of the libGrid library.
** After resampling, the original flat layers are hidden and replaced by the 3D resampled tileset.
*** The tiles of the tileset are paged in and out on demand.
*** Therefore, a tileset is in principle not limited in size or extent.
* A single layer or multiple selected layers can be removed, finally.

!! Example Data

For example, you can drag&drop a tileset data directory into the
viewer window:

Download the Hawaii or Fraenkische demo from stereofx.org/download and
drag&drop the data/HawaiiTileset or data/FrankischeTileset directory
into the viewer window. Double-clicking on the layer brings the
respective tileset into view.

You can also drag in layers from the libGrid data directory:
 http://code.google.com/p/libgrid/source/browse/libgrid/data

The best way to get that data directory is to check it out via svn:
 svn checkout http://libgrid.googlecode.com/svn/libgrid/data data

!! Usage Example

A simple usage example in 5 steps - we produce and visualize a 3D tileset
for the Island of Oahu, Hawai'i, by resampling a dem and a color layer:

* We assume that we have the libgrid data directory available.
* Step 1) Drag the "Oahu-10.tif" dem layer from the "elev" directory into the qtviewer.
* Step 2) Drag the "Oahu-25.tif" color layer from the "imag" directory into the qtviewer.
* Step 3) Right click at the list view and choose "resample all" from the context menu.
* Step 4) Wait several minutes while the resampling takes place.
* Step 5) Then zoom into the produced tileset with the middle mouse wheel to see some 3D details.

If this takes too long for you impatient guys, we can do the same with
a small island off the east coast of Oahu, Manana Island, for a total
of just 3 simple steps:

* We assume that we have the libgrid data directory available.
* Step 1) Drag the "MananaIsland.tif" dem layer from the "elev/Oahu-Islands" directory into the qtviewer.
* Step 2) Right click at the list view and choose "resample all" from the context menu.
* Step 3) Zoom into Manana Island to see some 3D details.

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

!! Programming Example

In this programming example we are going to put a building on a place
on earth.

The place on earth is defined as a local coordinate system at a
geo-referenced position. Possible geographic reference systems are
Lat/Lon, UTM, Mercator or ECEF. The z-axis of the local coordinate
system points upwards and the y-axis points north. The units of the
local coordinate system are meters.

The building is modeled in the local coordinate system as a scene
graph that contains two simple geometric objects, a cube as the base
and a prism as the roof.

The scene graph is constructed from a set of graph nodes as follows:

* derive from '''Viewer''' class
* overwrite build_ecef_geometry method to describe the scene as a graph
* construct scene graph to contain a single house in a local coordinate system
** at position Lat/Lon = 49 degrees latitude, 11 degrees longitude
*** via appending a coord node to the scene graph
** and define the local scale of the building
*** via appending a scale node
** and define the appearance of the building's components
*** via appending color nodes
** and define the proper positions of the components in the local coordinate system
*** via adppending translate nodes
** and add the geometry of the house to the scene graph
*** via appending cube and prism nodes

All graph nodes are derived from libMini's mininode base class.

 class Renderer: public Viewer
 {
    protected:

    mininode_group *Renderer::build_ecef_geometry()
    {
       // define local coordinate system at Lat/Lon=49/11
       mininode *house = new mininode_coord(minicoord(miniv3d(11*3600, 49*3600, 0),
                                            minicoord::MINICOORD_LLH));

       // scale up by a factor of 10000
       mininode *scale = house->append_child(new mininode_scale(10000));

       // white cube with base size 10x10 and height 5 meters
       // origin of the cube is its barycenter
       // translate up 2.5 meters
       scale->append_child(new mininode_color(miniv3d(1.0, 1.0, 1.0)))->
          append_child(new mininode_translate(miniv3d(0.0, 0.0, 2.5)))->
          append_child(new mininode_geometry_cube(10.0, 10.0, 5.0));

       // grey prism as roof with base size 10x10
       // origin of the prism is the center of its base plane
       // translate up 5 meters
       scale->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
          append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
          append_child(new mininode_geometry_prism(10.0, 10.0, 2.5));

       return(house);
    }
 };

!! CMake Example

The procedure of adding ecef geometry, as explained in the previous
sections, is illustrated in the CMake example (in the example.cmake
folder). It shows a rotating earth with an additional pole axis
(mininode_geometry_tube) and an equator ring (mininode_geometry_band).

This example is a good starting point to code your own geographic
applications. For example, you can show a tileset that has been
resampled with the qtviewer (or vtbuilder) just by issuing the
following commands in the MainWindow::timerEvent method:

 static bool loaded=false;
 if (!loaded)
    {
    viewerWindow->getViewer()->loadMap("url or file path to tileset");
    loaded=true;
    }
 viewerWindow->repaint();

Then you can navigate to a particular view point given in a geografic
coordinate system (Lat/Lon, UTM, Mercator, etc.):

 // define eye coordinates
 minicoord eye;
 double lat=49.5,lon=11.05, height=1000.0; // 1 km above Nuremberg airport
 eye.set_llh(lat,lon);

 // set camera to eye coordinates looking down by default
 viewerWindow->getViewer()->getCamera()->set_eye(eye);
 viewerWindow->repaint();

Or just let the earth rotate:

 // rotate camera left so that earth rotates right
 viewerWindow->getViewer()->getCamera()->move_left(10000.0);
 viewerWindow->repaint();

!! Most Simple CMake Example

The most simple usage of the qt viewer is to just derive from the
QtViewer class provided in the <mini/qt_viewer.h> header and override
the timer event and the ecef geometry construction pure virtual
methods. Source code for this use case is given in the simple.cmake
folder. To start, just have a look at the qt_viewer.h header, which
contains the complete definition of the QtViewer base class that can
be subclassed as follows:

 #include <mini/qt_viewer.h>

 class MyQtViewer: public QtViewer
 {
 public:

    MyQtViewer() {}
    ~MyQtViewer() {}

 protected:

    virtual mininode_group *build_ecef_geometry()
    {
       // return empty group node
       // append nodes at your pleasure
       mininode_group *group = new mininode_group();
       return(group);
    }

    virtual void timerEvent(QTimerEvent *)
    {
       // rotate camera left so that earth rotates right
       getCamera()->move_left(10000.0);
       repaint();
    }
 };

That's it!

!! Postcard

If you found the software useful, please send a vacation postcard to:

 Prof. Dr. Stefan Roettger
 Wassertorstr. 10
 90489 Nuernberg
 Germany

Thanks!
