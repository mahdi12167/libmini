#!/bin/tcsh -f

# the script requires two images as arguments
# the images are assumed to be day and night earth textures
# both need to be in LatLon coordinate layout
# such as the following BlueMarble images from visibleearth.nasa.gov:
#  day: land_ocean_ice_8192.tif or land_shallow_topo_8192.tif
#  night: land_ocean_ice_lights_8192.tif
if ($1 == "" || $2 == "") exit

set day=$1
set ngt=$2

# convert day image to required resolutions
convert $day -geometry 512x256! EarthDay.png
convert $day -geometry 1024x512! EarthDay-1K.png
convert $day -geometry 2048x1024! EarthDay-2K.png
convert $day -geometry 4096x2048! EarthDay-4K.png
convert $day -geometry 8192x4096! EarthDay-8K.png

# convert night image to required resolutions
convert $ngt -geometry 512x256! EarthNight.png
convert $ngt -geometry 1024x512! EarthNight-1K.png
convert $ngt -geometry 2048x1024! EarthNight-2K.png
convert $ngt -geometry 4096x2048! EarthNight-4K.png
convert $ngt -geometry 8192x4096! EarthNight-8K.png

# convert into db format /w s3tc and mipmap
foreach image (Earth*.png)
   ../mini/tools/db2db $image $image:r.db
   rm -f $image
end
