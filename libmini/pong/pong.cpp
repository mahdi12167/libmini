// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QApplication>

#include "ssltransmission.h"

int main(int argc, char **argv)
{
   QApplication app(argc, argv);

   if (argc <= 1)
   {
      SSLTransmissionServerConnectionFactory factory;
      SSLServer server(&factory);
      server.start("cert.pem", "key.pem", 10000);

      return(app.exec());
   }
   else
   {
      QByteArray data("transmission");
      SSLTransmissionClient client(data);
      client.start(argv[1], 10000, false);
   }

   return(0);
}
