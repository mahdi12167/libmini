#!/bin/tcsh -f

set boost = /usr/local/boost_*

foreach cpp (*.cpp)
  echo building $cpp
  g++ -I $boost $cpp -L$boost/stage/lib -lboost_system -o $cpp:r
end