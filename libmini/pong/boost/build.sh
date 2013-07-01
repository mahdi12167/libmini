#!/bin/tcsh -f

set boost = /usr/local/boost_*/

foreach cpp (*.cpp)
  echo building $cpp
  if ($HOSTTYPE == "intel-mac" || $HOSTTYPE == "intel-pc") then
    g++ -I $boost $cpp -L$boost/stage/lib -lboost_system -lboost_thread -o $cpp:r
  else
    g++ -I $boost $cpp -L$boost/stage/lib -lboost_system -lboost_thread -lpthread -lrt -o $cpp:r
  endif
end
