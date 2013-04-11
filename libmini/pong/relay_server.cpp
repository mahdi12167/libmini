//
// relay_server.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Stefan Roettger (snroettg at googlemail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "relay_server.h"

int main()
{
  boost::asio::io_service io_service;

  relay_server s(io_service, "ws.geonames.org", "/findNearByWeatherJSON?lat=49.45&lng=11.08");
  for (;;) s.respond();

  return 0;
}
