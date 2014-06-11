// (c) by Stefan Roettger, licensed under GPL 3.0

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "ssltransmission.h"
#include "serverui.h"

int main(int argc, char **argv)
{
   QApplication app(argc, argv);

   if (argc <= 1)
   {
      try
      {
         ServerUI main;

         main.show();

         SSLTransmissionServerConnectionFactory factory;
         SSLServer server(&factory);
         server.start("cert.pem", "key.pem", 10000);

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }
   else
   {
      try
      {
         QByteArray data("transmission");
         SSLTransmissionClient client(data);
         client.start(argv[1], 10000, false);
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }

   return(0);
}
