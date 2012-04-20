#!/bin/tcsh -f

# this script pulls and installs the libMini dependencies
# the installation path can be supplied as an optional argument (default is ../..)

set homebin=~/bin
setenv PATH "$PATH"":""$homebin"
rehash

foreach app (wget git cmake)
   if (! -X $app) then
      ./get"$app".sh
   endif
end

set arg=$1

./pull.sh && ./install.sh $arg
