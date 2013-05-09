//
// client.h
// ~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to provide a synchronous client class
//

#include <iostream>

#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class client
{
public:
  client(boost::asio::io_service& io_service,
         const std::string &host, const std::string &service)
    : valid_(false)
  {
    try
    {
      tcp::resolver resolver(io_service);
      tcp::resolver::query query(host, service);
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      tcp::socket socket(io_service);
      boost::asio::connect(socket, endpoint_iterator);

      for (;;)
      {
        boost::array<char, 128> buf;
        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (error == boost::asio::error::eof)
          break; // Connection closed cleanly by peer.
        else if (error)
          throw boost::system::system_error(error); // Some other error.

        response_.append(buf.data(), len);
      }

      valid_ = true;
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

protected:
  std::string response_;
  bool valid_;

public:
  bool is_valid()
  {
    return(valid_);
  }

  std::string get_response()
  {
    if (valid_)
      return(response_);
    else
      return("");
  }

};
