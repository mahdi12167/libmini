//
// async_client.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by Stefan Roettger to provide an asynchronous thread-safe http client class
//

#ifndef BOOST_ASYNC_CLIENT_H
#define BOOST_ASYNC_CLIENT_H

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

class async_client
{
public:
  async_client(boost::asio::io_service& io_service,
               const std::string& host, const std::string& path)
    : resolver_(io_service),
      socket_(io_service),
      response_finished_(false),
      response_error_(false)
  {
    // lock wait mutex
    wait_mtx_.lock();

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    std::ostream request_stream(&request_);
    request_stream << "GET " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    tcp::resolver::query query(host, "http");
    resolver_.async_resolve(query,
                            boost::bind(&async_client::handle_resolve, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::iterator));
  }

  ~async_client()
  {
    // lock wait mutex
    wait_mtx_.lock();
  }

private:
  void handle_resolve(const boost::system::error_code& err,
                      tcp::resolver::iterator endpoint_iterator)
  {
    if (!err)
    {
      // Attempt a connection to each endpoint in the list until we
      // successfully establish a connection.
      boost::asio::async_connect(socket_, endpoint_iterator,
                                 boost::bind(&async_client::handle_connect, this,
                                             boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Error: " << err.message() << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  void handle_connect(const boost::system::error_code& err)
  {
    if (!err)
    {
      // The connection was successful. Send the request.
      boost::asio::async_write(socket_, request_,
                               boost::bind(&async_client::handle_write_request, this,
                                           boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Error: " << err.message() << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  void handle_write_request(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Read the response status line. The response_ streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      boost::asio::async_read_until(socket_, response_, "\r\n",
                                    boost::bind(&async_client::handle_read_status_line, this,
                                                boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Error: " << err.message() << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  void handle_read_status_line(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Check that response is OK.
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        std::cerr << "Invalid response\n";
        return;
      }
      if (status_code != 200)
      {
        std::cerr << "Response returned with status code ";
        std::cerr << status_code << "\n";
        return;
      }

      // Read the response headers, which are terminated by a blank line.
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
                                    boost::bind(&async_client::handle_read_headers, this,
                                                boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Error: " << err << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  void handle_read_headers(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Process the response headers.
      std::istream response_stream(&response_);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r") ;

      // Write whatever content we already have read.
      response_stream_ << &response_;

      // Start reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
                              boost::asio::transfer_at_least(1),
                              boost::bind(&async_client::handle_read_content, this,
                                          boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Error: " << err << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  void handle_read_content(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Write all of the data that has been read so far.
      response_stream_ << &response_;

      // Continue reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
                              boost::asio::transfer_at_least(1),
                              boost::bind(&async_client::handle_read_content, this,
                                          boost::asio::placeholders::error));
    }
    else if (err == boost::asio::error::eof)
    {
      // scoped access lock
      boost::mutex::scoped_lock lock(access_mtx_);

      // finished reading
      response_finished_ = true;
      response_time_ = time(0);

      // unlock wait mutex
      wait_mtx_.unlock();
    }
    else
    {
      std::cerr << "Error: " << err << "\n";
      response_finished_ = response_error_ = true;

      // unlock wait mutex
      wait_mtx_.unlock();
    }
  }

  tcp::resolver resolver_;
  tcp::socket socket_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;

  std::ostringstream response_stream_;
  bool response_finished_;
  time_t response_time_;
  bool response_error_;

  boost::mutex wait_mtx_;
  boost::mutex access_mtx_;

public:
  bool is_valid()
  {
    // scoped wait lock
    boost::mutex::scoped_lock wait(wait_mtx_);

    return(!response_error_);
  }

  bool is_finished()
  {
    // scoped access lock
    boost::mutex::scoped_lock lock(access_mtx_);

    return(response_finished_);
  }

  std::string get_response()
  {
    // scoped wait lock
    boost::mutex::scoped_lock wait(wait_mtx_);

    if (!response_error_)
      return response_stream_.str();
    else
      return "";
  }

  time_t get_response_time()
  {
    // scoped wait lock
    boost::mutex::scoped_lock wait(wait_mtx_);

    if (!response_error_)
      return response_time_;
    else
      return time(0);
  }

  boost::asio::io_service& get_io_service()
  {
    return(resolver_.get_io_service());
  }

};

#endif
