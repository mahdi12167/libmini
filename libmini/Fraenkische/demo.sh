#!/bin/tcsh -f

setenv PATH $PATH":".

if (-e SRTM) preprocess.sh

build.sh
Fraenkische $*
