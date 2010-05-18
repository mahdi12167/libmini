#!/bin/tcsh -f

# this script pulls and installs the libMini dependencies
# the installation path can be supplied as an optional argument (default is ../..)

set arg=$1

./pull.sh && ./install.sh $arg
