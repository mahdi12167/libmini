#include <iostream>

#include <QApplication>

#include "sslsocket.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   if (argc <= 1)
   {
      std::cout << "starting server" << std::endl;

      SSLServer server;
      server.start("cert.pem", "key.pem", 10000);
   }
   else
   {
      std::cout << "starting client" << std::endl;

      SSLClient client;
      client.start(argv[1], 10000);
   }

   return(app.exec());
}
