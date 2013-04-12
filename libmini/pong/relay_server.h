//
// relay_server.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Stefan Roettger (snroettg at googlemail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/thread.hpp>

#include "server.h"
#include "async_client.h"

class relay_server: public server
{
public:
  relay_server(boost::asio::io_service& io_service,
               const std::string& host, const std::string& path,
               double lifetime = 60.0,
               unsigned int port=13)
     : server(io_service, port)
    {
      host_ = host;
      path_ = path;

      lifetime_ = lifetime;

      // create new async clients
      c1_ = new async_client(io_service, host_, path_);
      c2_ = new async_client(io_service, host_, path_);

      // start io service handling async clients in new thread
      t_ = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
    }

  ~relay_server()
  {
    // delete async clients
    delete c1_;
    delete c2_;

    // delete io thread
    delete t_;
  }

  void update_response()
  {
    // if async request is finished start next
    if (c2_->is_finished())
    {
      delete c1_;
      c1_ = c2_;
      c2_ = new async_client(c1_->get_io_service(), host_, path_);
    }

    // if latest response is too old start next
    if (difftime(time(0), c1_->get_response_time()) > lifetime_)
    {
      delete c1_;
      c1_ = c2_;
      c2_ = new async_client(c1_->get_io_service(), host_, path_);
    }
  }

protected:
  virtual std::string make_response_string()
  {
    std::string response;

    update_response();

    if (c1_->is_valid())
       response.append(c1_->get_response());
    else
       response.append("relay error");

    if (response.length()>0)
       if (*(response.end()-1) != '\n') response.push_back('\n');

    return response;
  }

private:
  std::string host_;
  std::string path_;

  double lifetime_;

  async_client *c1_, *c2_;

  boost::thread *t_;
};
