// (c) by Stefan Roettger, licensed under GPL 3.0

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "ssltransmission.h"

#include "serverui.h"
#include "clientui.h"

double get_opt(QString o)
{
   o=o.mid(o.indexOf("=")+1);
   return(o.toDouble());
}

void usage(const char *prog)
{
   QString name(prog);
   std::cout << "usage:" << std::endl;
   std::cout << " " << name.mid(name.lastIndexOf("/")+1).toStdString() << " {options} [ip-address [file]]" << std::endl;
   std::cout << "where options are:" << std::endl;
   std::cout << " --server: start pong server" << std::endl;
   std::cout << " --client: use drop box gui for transmissions" << std::endl;
   std::cout << " --transmit: transmit file" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "example server usage:" << std::endl;
   std::cout << " ./pong --server" << std::endl;
   std::cout << "example client usage:" << std::endl;
   std::cout << " ./pong --client localhost" << std::endl;
   exit(1);
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   setlocale(LC_NUMERIC, "C");

   // get argument list
   QStringList args = QCoreApplication::arguments();

   // scan for arguments and options
   QStringList arg,opt;
   for (int i=1; i<args.size(); i++)
      if (args[i].startsWith("--")) opt.push_back(args[i].mid(2));
      else if (args[i].startsWith("-")) opt.push_back(args[i].mid(1));
      else arg.push_back(args[i]);

   bool server=false;
   bool client=false;
   bool transmit=false;

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="server") server=true;
      else if (opt[i]=="client") client=true;
      else if (opt[i]=="transmit") transmit=true;
      else if (opt[i]=="help") usage(argv[0]);
      else usage(argv[0]);

   // server mode
   if (server && arg.size()==0)
   {
      try
      {
         ServerUI main;

         main.show();

         SSLTransmissionServerConnectionFactory factory;
         SSLServer server(&factory);

         // connect server gui with connection factory transmitted signal
         QObject::connect(&factory, SIGNAL(transmitted(QByteArray)),
                          &main, SLOT(transmitted(QByteArray)));

         // connect server gui with connection factory report signal
         QObject::connect(&factory, SIGNAL(report(QString)),
                          &main, SLOT(report(QString)));

         // start server on port 10000
         server.start("cert.pem", "key.pem", 10000);

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }
   // client mode
   else if (client && arg.size()==1)
   {
      try
      {
         ClientUI main(arg[0], 10000, false);

         main.show();

         SSLTransmissionClient client;

         // connect server gui with client
         QObject::connect(&main, SIGNAL(transmitFile(QString, quint16, QString, bool, bool)),
                          &client, SLOT(transmitFileNonBlocking(QString, quint16, QString, bool, bool)));

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }
   // transmit mode
   else if (transmit && arg.size()==2)
   {
      try
      {
         SSLTransmissionClient client;

         if (!client.transmitFile(arg[0], 10000, arg[1], false))
            return(1);
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }
   // print usage
   else usage(argv[0]);

   return(0);
}
