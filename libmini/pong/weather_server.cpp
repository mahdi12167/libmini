//
// weather_server.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to provide a weather service
//

#include "server.h"
#include "async_client.h"

int main()
{
  boost::asio::io_service io_service;

  server s(io_service);
  async_client c(io_service, "http://ws.geonames.org", "/findNearByWeatherJSON?lat=49.45&lng=11.08");

  for (;;)
  {
    s.respond();
  }

  return 0;
}
