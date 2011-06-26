Libmini Integration in QT Application
=====================================
This is an example for integrating libmini into QT application. This
sample application primarily uses mini viewer functions but provides
additional camera control.

Prerequisites
=============
Needs installation of:
 qt/qmake
 libmini
 libcurl
 libjpg
 libpng > 1.4.0
 zlib

Compilation
===========
Type "./build.sh" on the unix console.

Command-line Parameters
=======================
Same as the basic libmini viewer.

Usage: landscape <baseurl> <baseid> <basepath1> <basepath2>
Example: landscape Hawaii/data HawaiiTileset/ tiles landsat

Camera Controls
===============
WSAD - Move map up/down/left/right in screen space orientation.
Hold middle mouse button & move left/right - Camera orbiting around focus point horizontally
Hold middle mouse button & move up/down - Camera orbiting around focus point vertically
Left + Right mouse button click - Reset map orientation to north up
Space - Move focus point to where mouse is pointing
R - Move focus point to where mouse is pointing and reset camera to north up and look straight down
