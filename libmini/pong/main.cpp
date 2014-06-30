// (c) by Stefan Roettger, licensed under GPL 3.0

#define VERSION "v0.9 as of 30.June.2014"

#define LICENSE "licensed under GPL 3.0"
#define COPYRIGHT "(c) by Stefan Roettger 2014"
#define DISCLAIMER "no warranty, no responsibility, no nothing is granted for anything!"

#include <QApplication>

#include "sslserver.h"
#include "sslclient.h"

#include "serverui.h"
#include "clientui.h"

QString get_str(QString o)
{
   return(o.mid(o.indexOf("=")+1));
}

double get_opt(QString o)
{
   return(get_str(o).toDouble());
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
   std::cout << " " << app << " {options} {file}" << std::endl;
   std::cout << "where options are:" << std::endl;
   std::cout << " --server: start transmission server" << std::endl;
   std::cout << " --client: use drop box gui for transmissions" << std::endl;
   std::cout << " --transmit: transmit file" << std::endl;
   std::cout << " --dump: dump transmission database" << std::endl;
   std::cout << " --host=\"name\": specify host name" << std::endl;
   std::cout << " --port=n: specify tcp port n" << std::endl;
   std::cout << " --user=\"name\": specify user name" << std::endl;
   std::cout << " --verify-peer: verify integrity of peer" << std::endl;
   std::cout << " --self-certified: allow self-certified certificates" << std::endl;
   std::cout << " --compress: compress files" << std::endl;
   std::cout << " --no-gui: run without user interface" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "example server usage:" << std::endl;
   std::cout << " ./pong --no-gui &" << std::endl;
   std::cout << "example client usage:" << std::endl;
   std::cout << " ./ping --host=127.0.0.1" << std::endl;
   std::cout << "example transmission usage:" << std::endl;
   std::cout << " ./ping --transmit --host=pong.server.org --compress *.txt" << std::endl;
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
   QString host="";
   int port=10000;
   QString user="";
   bool verify=false;
   bool compress=false;
   bool gui=true;

#ifdef HAVE_SERVER
   server=true;
   client=false;
#endif

#ifdef HAVE_CLIENT
   server=false;
   client=true;
#endif

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="server") {server=true; client=transmit=dump=false;}
      else if (opt[i]=="client") {client=true; server=transmit=dump=false;}
      else if (opt[i]=="transmit") {transmit=true; server=client=dump=false;}
      else if (opt[i]=="dump") {dump=true; server=client=transmit=false;}
      else if (opt[i].startsWith("host=")) host=get_str(opt[i]);
      else if (opt[i].startsWith("port=")) port=(int)(get_opt(opt[i])+0.5);
      else if (opt[i].startsWith("user=")) user=get_str(opt[i]);
      else if (opt[i]=="verify-peer") verify=true;
      else if (opt[i]=="self-certified") verify=false;
      else if (opt[i]=="compress") compress=true;
      else if (opt[i]=="no-gui") gui=false;
      else if (opt[i]=="help") usage(argv[0]);
      else usage(argv[0]);

   // server mode
   if (server && arg.size()==0)
   {
      try
      {
         if (gui)
         {
            SSLTransmissionDatabaseServer server(port, "cert.pem", "key.pem", "/usr/share/pingpong");

            // start server on specified port (default 10000)
            server.start();

            // server gui
            ServerUI main(&server);
            main.show();

            return(app.exec());
         }
         else
         {
            SSLTransmissionDatabaseServer server(port, "cert.pem", "key.pem", "/usr/share/pingpong");

            // start server on specified port (default 10000)
            server.start();

            return(app.exec());
         }
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // client mode
   else if (client && arg.size()==0)
   {
      try
      {
         SSLTransmissionQueueClient client(host, port, user, verify, compress);

         // client gui
         ClientUI main(&client);
         main.show();

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // transmit mode
   else if (transmit && arg.size()>0)
   {
      try
      {
         SSLTransmissionDatabaseClient client(host, port, user, verify, compress);

         // transmit files
         for (int i=0; i<arg.size(); i++)
            if (!client.transmit(arg[i]))
               return(1);
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // dump mode
   else if (dump && arg.size()==0)
   {
      std::cout << "server db:" << std::endl;
      SSLTransmissionDatabase::dump();

      std::cout << "queue db:" << std::endl;
      SSLTransmissionDatabase::dump("queue");
   }
   // print usage
   else usage(argv[0]);

   return(0);
}
