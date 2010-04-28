#!/bin/tcsh -f

# this script pulls the most recent tested versions of the libMini dependencies

# check for installation of version control systems used for pulling:

foreach app (cvs svn git)
   if (! -X $app) then
      echo error: $app is not installed!
      exit 1
   endif
end

# the pthreads, libjpeg and libpng/libz libraries are assumed to be installed
# then the remaining dependencies to pull are:

# curl 7.20.1
if (! -e curl) then
   git clone git://curl.haxx.se/curl
endif
(cd curl; git checkout curl-7_20_1)

# squish 1.10 /w cmake
svn co -r 26 http://libsquish.googlecode.com/svn/trunk libsquish
foreach file (libsquish/*.cpp libsquish/*.h libsquish/*.inl libsquish/CMakeLists.txt)
   cp -f $file $file:s/libsquish/squish/
   ../mini/tabify.sh -x $file:s/libsquish/squish/
end

# GREYCstoration 1.3.4
cvs -d:pserver:anonymous:@cimg.cvs.sourceforge.net:/cvsroot/cimg login
cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co -r 1.627 CImg/CImg.h
foreach file (CImg.h)
   cp -f CImg/$file greycstoration/$file
   ../mini/tabify.sh -x greycstoration/$file
end

# openthreads 2.3.0
svn up openthreads

# freeglut 2.7.0
svn up freeglut -r 855

# other libraries to pull:

# gdal 1.7.1
svn up gdal
