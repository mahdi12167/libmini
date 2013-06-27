#!/bin/tcsh -f

set boost = /usr/local/boost_*/

foreach cpp (*.cpp)
  echo building $cpp
  g++ -I $boost $cpp -L$boost/stage/lib -lboost_system -lboost_thread -lpthread -lrt -o $cpp:r
end
