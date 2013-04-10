//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to use a synchronous server class
//

#include "server.h"
#include "async_client.h" //!! async test

int main()
{
  boost::asio::io_service io_service;
  io_service.run();

  //!! async test
  http_client c(io_service, "www.gnu.org", "/licenses/gpl-3.0.txt");
  while (c.get_response()=="");
  std::cout << c.get_response().size() << std::endl;
  std::cout << c.get_response();

  server s(io_service);
  for (;;) s.respond();

  return 0;
}
