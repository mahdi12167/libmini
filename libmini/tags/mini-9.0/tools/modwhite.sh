#!/bin/tcsh -f

if ("$1" == "" || "$2" == "") then
   echo "usage: $0 input.tif output.tif"
   echo "   output.tif receives the geotag of input.tif"
   echo "   white (ff,ff,ff) is replaced with black (0,0,0)"
   echo "   requires the libgeotiff and ImageMagick tools to be installed"
endif

set in="$1"
set out="$2"

if ("$in:e" != "tif" && "$in:e" != "TIF") exit
if ("$out:e" != "tif" && "$out:e" != "TIF") exit

convert -fill "#000000" -opaque "#ffffff" "$in" "$out"
modgeotiff.sh "$in" "$out"
