#!/bin/tcsh -f

set boost = /usr/local/boost_*
g++ -I $boost client.cpp -L$boost/stage/lib -lboost_system -o client
