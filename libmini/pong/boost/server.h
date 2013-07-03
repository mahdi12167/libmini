//
// server.h
// ~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to provide a synchronous server class
//

#ifndef BOOST_SERVER_H
#define BOOST_SERVER_H

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service,
         unsigned int port = 13)
  {
    try
    {
      acceptor_ = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port));
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

    t_ = NULL;
  }

  ~server()
  {
    if (t_)
      delete t_;

    delete acceptor_;
  }

  void respond()
  {
    try
    {
      tcp::socket socket(acceptor_->get_io_service());
      acceptor_->accept(socket);

      std::string message = make_response_string();

      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

  void start()
  {
    // start server in new thread
    t_ = new boost::thread(boost::bind(&server::run, this));
  }

  void run()
  {
    while (true)
      respond();
  }

  boost::asio::io_service& get_io_service()
  {
    return(acceptor_->get_io_service());
  }

protected:
  tcp::acceptor *acceptor_;
  boost::thread *t_;

  virtual std::string make_response_string() = 0;
};

#endif
