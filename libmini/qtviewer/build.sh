#!/bin/tcsh -f

# unix build script
# (c) by Stefan Roettger

if ($1 == "zip") then
   (cd ..; zip -qr9 qtviewer/QTVIEWER.zip qtviewer -x \*/.\?\?\* -x \*CVS/\*)
   exit
endif

if (! -X qmake) then
   echo qmake not installed!
   exit 1
endif

set options=""
if ($HOSTTYPE == "intel-pc") set options="-spec macx-g++"

qmake $options && make
