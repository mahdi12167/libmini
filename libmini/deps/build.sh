#!/bin/tcsh -f

# update curl:

cvs -d:pserver:anonymous:@cool.haxx.se:/cvsroot/curl login
cvs -d:pserver:anonymous@cool.haxx.se:/cvsroot/curl co curl

# update squish:

svn co http://libsquish.googlecode.com/svn/trunk squish-head

foreach file (squish-head/*.cpp squish-head/*.h squish-head/*.inl)
   cp -f $file $file:s/-head//
   ../mini/tabify.sh -u $file:s/-head//
end

# update GREYCstoration:

cvs -d:pserver:anonymous:@cimg.cvs.sourceforge.net:/cvsroot/cimg login
cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co CImg

foreach file (CImg.h plugins/greycstoration.h)
   cp -f CImg/$file greycstoration/$file
   ../mini/tabify.sh -u greycstoration/$file
end
