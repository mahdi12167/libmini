//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to use a synchronous client class
//

#include <iostream>

#include "client.h"

int main(int argc, char* argv[])
{
  if (argc != 2 && argc!=3)
  {
    std::cerr << "Usage: client <host> [<port>]" << std::endl;
    return 1;
  }

  boost::asio::io_service io_service;

  std::string host = argv[1];
  std::string port = "daytime";
  if (argc > 2) port = argv[2];

  client c(io_service, host, port);
  std::cout << c.get_response();

  return 0;
}
