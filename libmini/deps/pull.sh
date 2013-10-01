#!/bin/tcsh -f

# this script pulls the most recent tested versions of the libMini dependencies

# check for installation of version control systems used for pulling:

foreach app (wget cvs svn git cmake)
   if (! -X $app) then
      echo warning: $app is not installed!
   endif
end

# the pthreads and libpng/zlib libraries are assumed to be installed
# then the remaining dependencies to pull are:

# libjpeg v8d
if (! -e /usr/include/jpeglib.h) then
   if (-X wget) then
      if (! -e libjpeg) then
         wget http://www.ijg.org/files/jpegsrc.v8d.tar.gz
         tar zxf jpegsrc.v8d.tar.gz
         mv jpeg-8d libjpeg
      endif
   endif
endif

# libpng 1.5.14
if (! -e /usr/include/png.h) then
   if (-X git) then
      if (! -e libpng) then
         git clone git://libpng.git.sourceforge.net/gitroot/libpng/libpng
      endif
      (cd libpng; git checkout v1.5.14)
   endif
endif

# zlib 1.2.7
if (! -e /usr/include/zlib.h) then
   if (-X git) then
      if (! -e zlib) then
         git clone git://github.com/madler/zlib
      endif
      (cd zlib; git checkout v1.2.7)
   endif
endif

# curl 7.21.2
if (! -e /usr/include/curl/curl.h) then
   if (-X git) then
      if (! -e libcurl) then
         git clone git://github.com/bagder/curl libcurl
      endif
      (cd libcurl; git checkout curl-7_21_2)
   else
      if (-X wget) then
         if (! -e libcurl) then
            wget http://curl.haxx.se/download/curl-7.21.2.tar.gz
            tar zxf curl-7.21.2.tar.gz
            mv curl-7.21.2 libcurl
         endif
      endif
   endif
endif

# squish 1.11 /w cmake
if (-X ../mini/tabify.sh && -X svn) then
   svn co -r 37 http://libsquish.googlecode.com/svn/trunk libsquish
   foreach file (libsquish/*.cpp libsquish/*.h libsquish/*.inl libsquish/CMakeLists.txt libsquish/*.pro)
      cp -f $file $file:s/libsquish/squish/
      ../mini/tabify.sh -x $file:s/libsquish/squish/
   end
endif

# CImg/GREYCstoration 1.5.0
if (-X ../mini/tabify.sh && -X cvs) then
   cvs -d:pserver:anonymous:@cimg.cvs.sourceforge.net:/cvsroot/cimg login
   cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co -r 1.996 CImg/CImg.h
   foreach file (CImg.h)
      cp -f CImg/$file greycstoration/$file
      ../mini/tabify.sh -x greycstoration/$file
   end
endif

# openthreads 2.6.0
# note: currently disabled due to svn problems with the osg server
# svn co http://www.openscenegraph.org/svn/osg/OpenThreads/tags/OpenThreads-2.6.0

# freeglut 2.8.0+
if (-X svn) then
   svn up freeglut -r 1504 # svn external
endif

# other libraries to pull:

# gdal 1.9.2
if (-X svn) then
   svn up gdal # svn external
endif

# proj 4.8 (required by gdal)
if (-X svn) then
   svn up proj # svn external
endif

# libiconv 1.14 (required by gdal 1.9+)
if (! -e /usr/include/iconv.h || ! -e /usr/lib/libiconv.a) then
   if (-X wget) then
      if (! -e libiconv) then
         wget http://ftp.gnu.org/gnu/libiconv/libiconv-1.14.tar.gz
         tar zxf libiconv-1.14.tar.gz
         mv libiconv-1.14 libiconv
      endif
   endif
endif

# dcmtk
if (-X wget) then
   if (! -e dcmtk) then
      wget dicom.offis.de/download/dcmtk/dcmtk360/dcmtk-3.6.0.tar.gz
      tar zxf dcmtk-3.6.0.tar.gz
      mv dcmtk-3.6.0 dcmtk
   endif
endif
