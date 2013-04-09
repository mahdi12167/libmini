//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to provide a synchronous server class
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service,
         unsigned int port=13)
  {
    try
    {
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

      for (;;)
      {
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        std::string message = make_response_string();

        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      }
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

protected:
  virtual std::string make_response_string()
  {
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
  }

};
