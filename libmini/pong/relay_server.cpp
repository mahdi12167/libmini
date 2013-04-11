//
// relay_server.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Stefan Roettger (snroettg at googlemail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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
    }

  ~relay_server()
  {
    delete c1_;
    delete c2_;
  }

  virtual std::string make_response_string()
  {
    using namespace std; // For time_t, time and ctime;

    std::string response;

    time_t now = time(0);
    response.append(ctime(&now));

    response.append(c1_->get_response());

    if (c2_->is_valid())
    {
      delete c1_;
      c1_ = c2_;
      c2_ = new async_client(c1_->get_io_service(), host_, path_);
    }

    return response;
  }

private:
  std::string host_;
  std::string path_;

  async_client *c1_, *c2_;
};

int main()
{
  boost::asio::io_service io_service;

  relay_server s(io_service, "ws.geonames.org", "/findNearByWeatherJSON?lat=49.45&lng=11.08");
  for (;;) s.respond();

  return 0;
}
