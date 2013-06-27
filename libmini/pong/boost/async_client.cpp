//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to use an asynchronous http client class
//

#include <iostream>

#include "async_client.h"

int main(int argc, char* argv[])
{
  if (argc!=2 && argc!=3)
  {
    std::cerr << "Usage: async_client <host> [<path>]" << std::endl;
    return 1;
  }

  boost::asio::io_service io_service;

  std::string host = argv[1];
  std::string path = (argc>2)?argv[2]:"/index.html";

  async_client c(io_service, host, path);
  io_service.run();
  std::cout << c.get_response() << std::endl;

  return 0;
}
