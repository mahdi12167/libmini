#include <QApplication>
#include "sslsocket.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   if (argc<=1)
   {
      SSLServer server;
      server.start("ca.cer", "ca.key", 8888);
   }
   else
   {
      SSLClient client;
      client.start(argv[1], 8888);
   }

   return(app.exec());
}
