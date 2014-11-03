#!/bin/tcsh -f

# this script pulls and installs the libMini dependencies
# the installation path can be supplied as an optional argument
# if the installation path is not specified, /usr/local will be used as path
# in the latter case, the script asks for the super user password
# following the installation path the install target can be explicitly specified
# for instance: ./build.sh /usr/local zlib
# the list of available install targets is shown with "./build.sh help"

set homebin=~/bin
setenv PATH "$PATH"":""$homebin"
rehash

foreach app (wget git cmake)
   if (! -X $app) then
      ./get"$app".sh
   endif
end

set arg=$1
set target=$2

if ($arg == "help") then
   ./install.sh /usr/local help
else
   ./pull.sh $target
   ./install.sh $arg $target
endif
