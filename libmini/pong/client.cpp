//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to use a synchronous daytime client method
//

#include <iostream>

#include "client.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: client <host>" << std::endl;
    return 1;
  }

  boost::asio::io_service io_service;

  std::cout << client(io_service, argv[1], "daytime");

  return 0;
}
