// (c) by Stefan Roettger, licensed under GPL 3.0

#define VERSION "v0.2 as of 20.June.2014"

#define LICENSE "licensed under GPL 3.0"
#define COPYRIGHT "(c) by Stefan Roettger 2014"
#define DISCLAIMER "no warranty, no responsibility, no nothing is granted for anything!"

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "ssltransmission.h"
#include "ssldatabase.h"

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
   std::string app(name.mid(name.lastIndexOf("/")+1).toStdString());
   std::cout << app << " " << VERSION << std::endl;
   std::cout << " " << LICENSE << std::endl;
   std::cout << " " << COPYRIGHT << std::endl;
   std::cout << " " << DISCLAIMER << std::endl;
   std::cout << "usage:" << std::endl;
   std::cout << " " << app << " {options} [ip-address [file]]" << std::endl;
   std::cout << "where options are:" << std::endl;
   std::cout << " --server: start pong server" << std::endl;
   std::cout << " --client: use drop box gui for transmissions" << std::endl;
   std::cout << " --transmit: transmit file" << std::endl;
   std::cout << " --dump: dump database contents" << std::endl;
   std::cout << " --port=n: use tcp port n" << std::endl;
   std::cout << " --compress: compress files" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "example server usage:" << std::endl;
   std::cout << " ./pong --server" << std::endl;
   std::cout << "example client usage:" << std::endl;
   std::cout << " ./pong --client 127.0.0.1" << std::endl;
   std::cout << "example transmission usage:" << std::endl;
   std::cout << " ./pong --transmit pong.server.org file.txt" << std::endl;
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

   bool server=true;
   bool client=false;
   bool transmit=false;
   bool dump=false;
   int port=10000;
   bool compress=false;

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="server") {server=true; client=transmit=dump=false;}
      else if (opt[i]=="client") {client=true; server=transmit=dump=false;}
      else if (opt[i]=="transmit") {transmit=true; server=client=dump=false;}
      else if (opt[i]=="dump") {dump=true; server=client=transmit=false;}
      else if (opt[i].startsWith("port=")) port=(int)get_opt(opt[i]);
      else if (opt[i]=="compress") compress=true;
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

         SSLTransmissionDatabase db;
         if (!db.openDB()) return(1);

         // connect server gui with the connection factory transmitted signal
         QObject::connect(&factory, SIGNAL(transmitted(SSLTransmission)),
                          &main, SLOT(transmitted(SSLTransmission)));

         // connect transmission database with the connection factory transmitted signal
         QObject::connect(&factory, SIGNAL(transmitted(SSLTransmission)),
                          &db, SLOT(write(SSLTransmission)));

         // connect server gui with the connection factory report signal
         QObject::connect(&factory, SIGNAL(report(QString)),
                          &main, SLOT(report(QString)));

         // start server on specified port (default 10000)
         server.start("cert.pem", "key.pem", port, "/usr/share/pong");

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
         ClientUI main(arg[0], port, false, compress);

         main.show();

         SSLTransmissionClient client;

         // connect server gui with client
         QObject::connect(&main, SIGNAL(transmit(QString, quint16, QString, bool, bool)),
                          &client, SLOT(transmitNonBlocking(QString, quint16, QString, bool, bool)));

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

         if (!client.transmit(arg[0], port, arg[1], false, compress))
            return(1);
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
   }
   // dump mode
   else if (dump && arg.size()==0)
   {
      SSLTransmissionDatabase db;
      if (!db.openDB()) return(1);

      QStringList users = db.users();

      for (int i=0; i<users.size(); i++)
      {
         std::cout << "user \"" << users[i].toStdString() << "\":" << std::endl;

         QStringList list = db.list(users[i]);

         for (int j=0; j<list.size(); j++)
         {
            SSLTransmission t = db.read(list[j], users[i]);
            std::cout << " " << t << std::endl;
         }
      }
   }
   // print usage
   else usage(argv[0]);

   return(0);
}
