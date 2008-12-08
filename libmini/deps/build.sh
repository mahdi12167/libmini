#!/bin/tcsh -f

cvs -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg login
cvs -z3 -d:pserver:anonymous@cimg.cvs.sourceforge.net:/cvsroot/cimg co -P CImg

foreach file (CImg.h plugins/greycstoration.h)
   cp -f CImg/$file greycstoration/$file
   ../mini/tabify.sh -u greycstoration/$file
end

foreach file (squish-head/*.cpp squish-head/*.h squish-head/*.inl)
   cp -f $file $file:s/-head//
   ../mini/tabify.sh -u $file:s/-head//
end

pushd squish >&/dev/null
patch -u -p1 <../squish.diff
popd >&/dev/null
