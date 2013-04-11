//
// relay_server.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Stefan Roettger (snroettg at googlemail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <unistd.h>

#include "server.h"
#include "async_client.h"

class relay_server: public server
{
public:
  relay_server(boost::asio::io_service& io_service,
               const std::string& host, const std::string& path,
               unsigned int port=13)
     : server(io_service, port)
    {
      host_=host;
      path_=path;

      c1_ = new async_client(io_service, host_, path_);
      c2_ = new async_client(io_service, host_, path_);

      io_service.run();
      while (!c1_->is_valid()) sleep(1);
    }

  ~relay_server()
  {
    delete c1_;
    delete c2_;
  }

  void update_response()
  {
    if (c2_->is_valid())
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

    response.append(c1_->get_response());

    if (response.length()>0)
       if (*(response.end()-1) != '\n') response.push_back('\n');

    return response;
  }

private:
  std::string host_;
  std::string path_;

  async_client *c1_, *c2_;
};
