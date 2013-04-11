//
// daytime_server.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Stefan Roettger (snroettg at googlemail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>

#include "server.h"

class daytime_server: public server
{
public:
  daytime_server(boost::asio::io_service& io_service,
                 unsigned int port=13)
     : server(io_service, port)
    {}

protected:
  virtual std::string make_response_string()
  {
    using namespace std; // For time_t, time and ctime;

    std::string response;

    time_t now = time(0);
    response.append(ctime(&now));

    return response;
  }

};
