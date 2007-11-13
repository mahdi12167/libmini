#!/bin/tcsh -f

if ($1 == "clean") then
   \rm -rf SRTM/*.pgm
   \rm -rf GTOPO30/*.pgm
   \rm -rf LandSat-ETM/*.ppm
   \rm -rf LandSat-NDVI/*.pgm
   \rm -rf LandSat-MSS/*.ppm
   \rm -rf Vegetation/*.p?m
   \rm -rf Soil/*.ppm
   exit
endif

if ($1 == "tidy") then
   preprocess.sh clean
   \rm -rf LandSat-ETM/*_7t*.png
   \rm -rf LandSat-MSS/*.png
   \rm -f ETC/Cities.txt
   exit
endif

cd SRTM

foreach file (*.zip)
   echo processing $file
   if (! -e $file:r:r.pgm) then
      if (-e $file:r:r.txt) then
         unzip $file
         \cp -f $file:r:r.txt $file:r:r.pgm
         cat $file:r >>$file:r:r.pgm
         \rm -f $file:r
      endif
   endif
end

cd ..

cd GTOPO30

foreach file (*.png)
   echo processing $file
   if (! -e $file:r.pgm) then
      if (-e $file:r.txt) then
         convert $file -depth 16 'gray:'$file:r.raw
         \cp -f $file:r.txt $file:r.pgm
         cat $file:r.raw >>$file:r.pgm
         \rm -f $file:r.raw
      endif
   endif
end

cd ..

cd LandSat-ETM

foreach file (*.met)
   set output=$file:r.png
   if (! -e $output) then
      etm2nat.sh $output nat
   endif
end

foreach file (*.png)
   echo processing $file
   if (! -e $file:r.ppm) then
      if (-e $file:r.txt) then
         convert $file -depth 8 'rgb:'$file:r.raw
         \cp -f $file:r.txt $file:r.ppm
         cat $file:r.raw >>$file:r.ppm
         \rm -f $file:r.raw
      endif
   endif
end

cd ..

cd LandSat-NDVI

foreach file (*.png)
   echo processing $file
   if (! -e $file:r.pgm) then
      if (-e $file:r.txt) then
         convert $file -depth 8 'gray:'$file:r.raw
         \cp -f $file:r.txt $file:r.pgm
         cat $file:r.raw >>$file:r.pgm
         \rm -f $file:r.raw
      endif
   endif
end

cd ..

cd LandSat-MSS

foreach file (*.ip3)
   set output=$file:r.png
   if (! -e $output) then
      mss2nat.sh $output nat
   endif
end

foreach file (*.png)
   echo processing $file
   if (! -e $file:r.ppm) then
      if (-e $file:r.txt) then
         convert $file -depth 8 'rgb:'$file:r.raw
         \cp -f $file:r.txt $file:r.ppm
         cat $file:r.raw >>$file:r.ppm
         \rm -f $file:r.raw
      endif
   endif
end

cd ..

cd Vegetation

foreach file (*.png)
   echo processing $file
   convert $file $file:r"RGB".ppm
   convert $file $file:r.matte
   convert $file:r.matte $file:r"A".pgm
   rm -f $file:r.matte
end

if (! -e ../TreesRGB.ppm) then
   set tree1=ohia4_v7_256
   set tree2=ohia5_v3_256
   set tree3=mamane1_v2_256
   set tree4=koa1_v2_256

   foreach file ($tree1*.p?m $tree2*.p?m $tree3*.p?m $tree4*.p?m)
      convert $file -geometry 256x256! _$file
   end

   montage +frame +shadow +label -tile 4x1 -geometry 256x256 _$tree1"RGB".ppm _$tree2"RGB".ppm _$tree3"RGB".ppm _$tree4"RGB".ppm -depth 8 ../TreesRGB.ppm
   montage +frame +shadow +label -tile 4x1 -geometry 256x256 _$tree1"A".pgm _$tree2"A".pgm _$tree3"A".pgm _$tree4"A".pgm -depth 8 ../TreesA.pgm
endif

cd ..

cd Soil

if (! -e ../Soil.pvm) then
   set tex1=stone.png
   set tex2=mud.png
   set tex3=grass1.png
   set tex4=grass2.png
   set tex5=grass3.png
   set tex6=grass4.png
   set tex7=forest1.png
   set tex8=forest2.png
   foreach file ($tex1 $tex2 $tex3 $tex4 $tex5 $tex6 $tex7 $tex8)
      convert $file -geometry 512x512! $file:r.ppm
      ../tools/pnm2pvm $file:r.ppm ../Soil.pvm
   end
endif

cd ..

cd ETC

if (! -e Cities.txt) then
   geo2txt.sh
endif

cd ..
