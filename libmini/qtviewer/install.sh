#!/bin/tcsh -f
# unix install script

echo compiling libmini...
(cd mini; cmake -DBUILD_MINI_SFX=ON . && make -j 4)
if ($? != 0) exit($?)
echo compiling libgrid...
(cd grid; cmake . && make -j 4)
if ($? != 0) exit($?)
echo compiling libsquish...
(cd squish; cmake . && make -j 4)
if ($? != 0) exit($?)
echo compiling qtviewer...
cmake . && make -j 4
if ($? != 0) exit($?)
echo installing qtviewer...
echo enter root password when prompted
sudo make install
if ($? != 0) exit($?)
