#!/bin/tcsh -f

# unix build script
# (c) by Stefan Roettger

if ($1 == "clean") then
   make clean
   rm -rf Makefile* qtviewer qtviewer_debug qtviewer*.app mocs* objs* rccs* uics*
   exit
endif

if ($1 == "zip") then
   (cd ..; zip -qr9 qtviewer/QTVIEWER.zip qtviewer -x \*/.\?\?\* -x \*CVS/\*)
   exit
endif

if (! -X qmake) then
   echo error: qmake is not installed!
   echo suggest: add your qmake binary path to your search path
   echo for example using tcsh: setenv PATH $PATH":/usr/local/Trolltech/Qt-4.7.4/bin"
   exit 1
endif

set qoptions=""
if ($HOSTTYPE == "intel-pc") set qoptions="-spec macx-g++"

set moptions=""
if ($1 == "") set moptions="$1"

qmake $qoptions && make $moptions -j4
