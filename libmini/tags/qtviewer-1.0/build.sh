#!/bin/tcsh -f

# unix build script
# (c) by Stefan Roettger

if (! -X qmake) then
   echo qmake not installed!
   exit 1
endif

set options=""
if ($HOSTTYPE == "intel-pc") set options="-spec macx-g++"

qmake $options && make
