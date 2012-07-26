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
 OpenGL (and GLUT)
 qt/qmake

The installation of OpenGL and GLUT is vendor specific: On MacOS X it
is already installed with the XCode development package, on Linux it
comes with the "mesa" and "free-glut3-dev" package whereas on Windows
it is usually installed with the MSVC IDE.

It is highly recommended to build and install QT from source!

For example grab the source tar ball from:
 ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz

!! Installation (Unix/Mac)

Type on the unix console in your QT source directory:
 ./configure -opensource && make && sudo make install

After the build process has finished (go get yourself a cup of coffee),
you will be asked to enter your root password for installation of qt.

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
To build GDAL we have to define some options (in the name.opt file):
 MSVC_VER -> use 1600 to identify MSVC 10.0, 1500 for MSVC 9.0, etc.
 GDAL_HOME -> needs to point to the GDAL installation directory
  Usually this means that we point it to our project directory
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate to the GDAL source directory and type
 set DEST="project directory"
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST%
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST% devinstall
 nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%DEST% staticlib
 copy gdal.lib %DEST%\lib
 copy port\*.h %DEST%\include
 copy gcore\*.h %DEST%\include
 copy ogr\*.h %DEST%\include

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
** Its appearance takes both slected color and dem layers into account.
** At least a single dem layer must be selected.
** Resampling takes place in a background thread.
*** Progress is shown in percent below the list view.
*** Resampling is a computationally expensive operation and can take several minutes to hours.
*** It produces a number of tiles that are stored in the proprietary libMini DB format.
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

A simple usage example in 5 steps - we produce a 3D tileset for the Island
of Oahu, Hawai'i, by resampling a dem and a color layer:

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

!! Postcard

If you found the software useful, please send a vacation postcard to:

 Prof. Dr. Stefan Roettger
 Wassertorstr. 10
 90489 Nuernberg
 Germany

Thanks!
