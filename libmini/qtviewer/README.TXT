Libmini Integration in QT Application
=====================================
This is an example for integrating libmini into a QT application. This
sample application primarily uses mini viewer functions but provides
additional camera control.

Prerequisites
=============
Needs installation of:
 qt/qmake
 libmini
 libcurl
 libjpg
 libpng
 zlib

Compilation
===========
Type "./build.sh" on the unix console.

Command-line Parameters
=======================
Usage: qtviewer { <tileset url> }

Camera Controls
===============
WSAD - Move map up/down/left/right in screen space orientation.
Space - Move focus point to where mouse is pointing.
Q - Quit
