#!/bin/tcsh -f

if ("$1" == "" || "$2" == "") then
   echo "usage: $0 input.tif output.tif"
   echo "   output.tif receives the geotag of input.tif"
   echo "   requires the libgeotiff tools to be installed"
endif

set in="$1"
set out="$2"

set compr="-c zip"

if ("$in:e" != "tif" && "$in:e" != "TIF") exit
if ("$out:e" != "tif" && "$out:e" != "TIF") exit

listgeo -no_norm "$in" >"$in:r.geo"

if (! -e "$out") cp "$in" "$out"

geotifcp $compr -g "$in:r.geo" "$out" "$out:r_tmp.tif"

mv -f "$out:r_tmp.tif" "$out"

rm -f "$in:r.geo"
