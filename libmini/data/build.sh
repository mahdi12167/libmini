#!/bin/tcsh -f

if ($1 == "" || $2 == "") exit

set day=$1
set ngt=$2

convert $day -geometry 512x256! EarthDay.png
convert $day -geometry 2048x1024! EarthDay-2K.png
convert $day -geometry 4096x2048! EarthDay-4K.png
convert $day -geometry 8192x4096! EarthDay-8K.png

convert $ngt -geometry 512x256! EarthNight.png
convert $ngt -geometry 2048x1024! EarthNight-2K.png
convert $ngt -geometry 4096x2048! EarthNight-4K.png
convert $ngt -geometry 8192x4096! EarthNight-8K.png

foreach image (Earth*.png)
   ../mini/tools/db2db $image $image:r.db
end
