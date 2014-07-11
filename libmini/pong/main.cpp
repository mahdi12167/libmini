// (c) by Stefan Roettger, licensed under GPL 3.0

#define VERSION "v1.0.1 as of 11.July.2014"

#define LICENSE "licensed under GPL 3.0"
#define COPYRIGHT "(c) by Stefan Roettger 2014"
#define DISCLAIMER "no warranty, no responsibility, no nothing is granted for anything!"

#include <QApplication>

#include "sslloop.h"
#include "sslqueue.h"

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
   std::cout << " --up: start client with drop box gui to upload transmission to server" << std::endl;
   std::cout << " --down: start client to download transmissions from server" << std::endl;
   std::cout << " --ping: ping server" << std::endl;
   std::cout << " --transmit: transmit files to server" << std::endl;
   std::cout << " --receive: receive files from server" << std::endl;
   std::cout << " --dump: dump transmission database" << std::endl;
   std::cout << " --host=\"name\": specify host name" << std::endl;
   std::cout << " --port=n: specify tcp port n" << std::endl;
   std::cout << " --user=\"name\": specify user name" << std::endl;
   std::cout << " --pair=\"code\": specify pair code" << std::endl;
   std::cout << " --verify-peer: verify integrity of peer" << std::endl;
   std::cout << " --self-certified: allow self-certified certificates" << std::endl;
   std::cout << " --compress: compress files" << std::endl;
   std::cout << " --quiet: run silently" << std::endl;
   std::cout << " --no-gui: run without user interface" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "example server usage:" << std::endl;
   std::cout << " ./pong &" << std::endl;
   std::cout << "example client upload usage:" << std::endl;
   std::cout << " ./ping --up --host=127.0.0.1" << std::endl;
   std::cout << "example client download usage:" << std::endl;
   std::cout << " ./poing --down --host=127.0.0.1" << std::endl;
   std::cout << "example transmission use case:" << std::endl;
   std::cout << " at server.org:" << std::endl;
   std::cout << "  ./pong --no-gui &" << std::endl;
   std::cout << " at home:" << std::endl;
   std::cout << "  ./poing --pair --code=\"pair code\" --host=server.org" << std::endl;
   std::cout << " while abroad:" << std::endl;
   std::cout << " ./ping --pair --code=\"pair code\" --host=server.org" << std::endl;
   std::cout << " ./ping --transmit --compress *.txt" << std::endl;
   std::cout << " back home:" << std::endl;
   std::cout << " ./poing --receive" << std::endl;
   std::cout << " ./poing --dump" << std::endl;
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
   bool client_up=false;
   bool client_down=false;
   bool ping=false;
   bool transmit=false;
   bool receive=false;
   bool dump=false;
   QString host="";
   int port=SSLTransmission::default_port;
   QString user="";
   bool pair=false;
   QString code="";
   bool verify=false;
   bool compress=false;
   bool quiet=false;
   bool gui=true;

#ifdef HAVE_SERVER
   server=true;
   client_up=false;
   client_down=false;
#endif

#ifdef HAVE_CLIENT_UP
   server=false;
   client_up=true;
   client_down=false;
#endif

#ifdef HAVE_CLIENT_DOWN
   server=false;
   client_up=false;
   client_down=true;
#endif

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="server") {server=true; client_up=client_down=ping=transmit=receive=pair=false;}
      else if (opt[i]=="up") {client_up=true; client_down=false; server=ping=transmit=receive=pair=false;}
      else if (opt[i]=="down") {client_down=true; client_up=false; server=ping=transmit=receive=pair=false;}
      else if (opt[i]=="ping") {ping=true; server=client_up=client_down=transmit=receive=dump=pair=false;}
      else if (opt[i]=="transmit") {transmit=true; server=client_up=client_down=ping=receive=dump=pair=false;}
      else if (opt[i]=="receive") {receive=true; server=client_up=client_down=ping=transmit=dump=pair=false;}
      else if (opt[i]=="dump") {dump=true; ping=transmit=receive=pair=false;}
      else if (opt[i].startsWith("host=")) host=get_str(opt[i]);
      else if (opt[i].startsWith("port=")) port=(int)(get_opt(opt[i])+0.5);
      else if (opt[i].startsWith("user=")) user=get_str(opt[i]);
      else if (opt[i].startsWith("pair")) {pair=true; server=ping=transmit=receive=dump=false;}
      else if (opt[i].startsWith("code=")) code=get_str(opt[i]);
      else if (opt[i]=="verify-peer") verify=true;
      else if (opt[i]=="self-certified") verify=false;
      else if (opt[i]=="compress") compress=true;
      else if (opt[i]=="quiet") quiet=true;
      else if (opt[i]=="no-gui") gui=false;
      else if (opt[i]=="help") usage(argv[0]);
      else usage(argv[0]);

   // dump mode
   if (dump && arg.size()==0)
   {
      if (server)
         SSLTransmissionDatabase::dump();
      else if (client_up)
         SSLTransmissionDatabase::dump("upload");
      else
         SSLTransmissionDatabase::dump("download");
   }
   // server mode
   else if (server && arg.size()==0)
   {
      if (gui)
      {
         try
         {
            SSLTransmissionDatabaseServer server(port, "cert.pem", "key.pem", "/usr/share/pingpong");

            // server gui
            ServerUI main(&server);
            main.show();

            // start server on specified port (default 10000)
            server.start();

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
      else
      {
         SSLTransmissionDatabaseServerLoop server(port, "cert.pem", "key.pem", "/usr/share/pingpong");

         // start server on specified port (default 10000)
         server.start(); // does not return from event loop
      }
   }
   // ping mode
   else if (ping && arg.size()==0)
   {
      try
      {
         SSLTransmissionQueueClient client(host, port, user, verify, compress, client_up);

         // ping server
         if (!client.ping())
         {
            if (!quiet)
               std::cout << "cannot ping host \"" << client.getHostName().toStdString() << "\"" << std::endl;

            return(1);
         }
         else
            if (!quiet)
            {
               std::cout << "connection established: " << std::endl;
               std::cout << " host: " << client.getHostName().toStdString() << std::endl;
               std::cout << " port: " << client.getPort() << std::endl;
               std::cout << " uid: " << client.getShortUID().toStdString() << std::endl;
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
   // pair mode
   else if (pair && arg.size()==0)
   {
      try
      {
         SSLTransmissionQueueClient client(host, port, user, verify, compress, client_up);

         if (!client_up)
         {
            // pair download client
            QString result = client.pairUID(code);

            if (result == "")
            {
               if (!quiet)
                  std::cout << "cannot pair client" << std::endl;

               return(1);
            }
            else
            {
               if (!quiet)
                  std::cout << "pairing client" << std::endl;

               if (code == "")
                  if (!quiet)
                     std::cout << "pair code: \"" << result.toStdString() << "\"" << std::endl;
            }
         }
         else
         {
            if (code == "")
            {
               if (!quiet)
                  std::cout << "please specify a pair code" << std::endl;

               return(1);
            }
            else
            {
               // pair upload client
               QString result = client.pairCode(code);

               if (result == "")
               {
                  if (!quiet)
                     std::cout << "cannot pair client" << std::endl;

                  return(1);
               }
               else
               {
                  if (!quiet)
                     std::cout << "paired client" << std::endl;
               }
            }
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
   // receive mode
   else if (receive && arg.size()==0)
   {
      try
      {
         SSLTransmissionQueueClient client(host, port, user, verify, compress, false);

         // receive files
         int count = client.receive();

         if (!quiet)
            std::cout << "received files: " << count << std::endl;
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
         SSLTransmissionQueueClient client(host, port, user, verify, compress, true);

         // transmit files
         for (int i=0; i<arg.size(); i++)
            if (!client.transmit(arg[i]))
            {
               if (!quiet)
                  std::cout << "cannot transmit file: " << arg[i].toStdString() << std::endl;

               return(1);
            }
            else
               if (!quiet)
                  std::cout << "transmitted file: " << arg[i].toStdString() << std::endl;
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
   else if ((client_up || client_down) && arg.size()==0)
   {
      try
      {
         SSLTransmissionQueueClient client(host, port, user, verify, compress, client_up);

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
   // print usage
   else usage(argv[0]);

   return(0);
}
