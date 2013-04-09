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
#include "async_client.h" //!! test

int main()
{
  boost::asio::io_service io_service;

  http_client c(io_service, "boost.org", "/LICENSE_1_0.txt"); //!! test
  server s(io_service);

  return 0;
}
