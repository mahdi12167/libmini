// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QApplication>

#include "sslsocket.h"

int main(int argc, char **argv)
{
   QApplication app(argc, argv);

   if (argc <= 1)
   {
      SSLServer server;
      server.start("cert.pem", "key.pem", 10000);

      return(app.exec());
   }
   else
   {
      SSLClient client;
      client.start(argv[1], 10000);
   }

   return(0);
}
