#!/bin/tcsh -f

# this script pulls the most recent tested versions of the libMini dependencies
# pthreads, libjpeg and libpng/libz are assumed to be installed on the system by default
# then the remaining dependencies are:

# curl
cvs -d:pserver:anonymous:@cool.haxx.se:/cvsroot/curl login
cvs -d:pserver:anonymous@cool.haxx.se:/cvsroot/curl co -r curl-7_19_4 curl

# squish
svn co -r 19 http://libsquish.googlecode.com/svn/trunk libsquish
foreach file (libsquish/*.cpp libsquish/*.h libsquish/*.inl libsquish/CMakeLists.txt)
   cp -f $file $file:s/libsquish/squish/
   ../mini/tabify.sh -u $file:s/libsquish/squish/
end

# GREYCstoration
cvs -d:pserver:anonymous:@cimg.cvs.sourceforge.net:/cvsroot/cimg login
cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co -r 1.174 CImg/CImg.h
cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co -r 1.3 CImg/plugins/greycstoration.h
foreach file (CImg.h plugins/greycstoration.h)
   cp -f CImg/$file greycstoration/$file
   ../mini/tabify.sh -u greycstoration/$file
end

# openthreads
svn up openthreads

# freeglut
svn up freeglut
