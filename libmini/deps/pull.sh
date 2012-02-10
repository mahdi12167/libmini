#!/bin/tcsh -f

# this script pulls the most recent tested versions of the libMini dependencies

# check for installation of version control systems used for pulling:

foreach app (cvs svn git wget cmake)
   if (! -X $app) then
      echo warning: $app is not installed!
      if ($app == "git") echo "solution: install git with the getgit.sh script"
      if ($app == "wget") echo "solution: install cmake with the getwget.sh script"
      if ($app == "cmake") echo "solution: install cmake with the getcmake.sh script"
   endif
end

# the pthreads and libpng/zlib libraries are assumed to be installed
# then the remaining dependencies to pull are:

# libjpeg v8d
if (-X wget) then
   if (! -e libjpeg) then
      wget http://www.ijg.org/files/jpegsrc.v8d.tar.gz
      tar zxf jpegsrc.v8d.tar.gz
      mv jpeg-8d libjpeg
   endif
endif

# curl 7.21.2
if (-X git) then
   if (! -e curl) then
      git clone git://curl.haxx.se/curl
   endif
   (cd curl; git checkout curl-7_21_2)
else
   if (-X wget) then
      if (! -e curl) then
         wget http://curl.haxx.se/download/curl-7.21.2.tar.gz
         tar zxf curl-7.21.2.tar.gz
         mv curl-7.21.2 curl
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

# openthreads 2.3.0
if (-X svn) then
   svn up openthreads
endif

# freeglut 2.7.0
if (-X svn) then
   svn up freeglut -r 855
endif

# other libraries to pull:

# gdal 1.8.1
if (-X svn) then
   svn up gdal
endif
