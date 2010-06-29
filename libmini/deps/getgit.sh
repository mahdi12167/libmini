#!/bin/tcsh -f

# this script pulls and installs git

set git=git-1.7.1

if (! -X git) then
   wget http://kernel.org/pub/software/scm/git/$git.tar.gz
   tar zxf $git.tar.gz

   (cd $git; ./configure --prefix=$HOME --without-iconv; make; make install)
end
