#!/bin/tcsh -f

# unix build script
# (c) by Stefan Roettger

set options=""
if ($HOSTTYPE == "intel-pc") set options="-spec macx-g++"

qmake $options && make
