// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QApplication>

#include "sslloop.h"

// ssl transmission database server loop ctor
SSLTransmissionDatabaseServerLoop::SSLTransmissionDatabaseServerLoop(quint16 port,
                                                                     QString certPath, QString keyPath,
                                                                     QString altPath,
                                                                     QObject *parent)
   : SSLTransmissionDatabaseServer(port,
                                   certPath, keyPath,
                                   altPath,
                                   parent)
{}

// ssl transmission database server loop ctor
SSLTransmissionDatabaseServerLoop::~SSLTransmissionDatabaseServerLoop()
{}

// keep transmission responses as backup
void SSLTransmissionDatabaseServerLoop::keepBackup()
{
   SSLTransmissionDatabaseServer::keepBackup();
}

// start the server loop
void SSLTransmissionDatabaseServerLoop::start()
{
   for (;;)
   {
      try
      {
         // start server on specified port (default 10000)
         SSLTransmissionDatabaseServer::start();

         // enter the event loop
         QApplication::exec();
      }
      catch (SSLError &e)
      {
         std::cout << "caught exception on " << QDateTime::currentDateTime().toString().toStdString() << std::endl;
         std::cout << " " << e.what() << std::endl;
      }
      catch (...)
      {
         std::cout << "caught exception on " << QDateTime::currentDateTime().toString().toStdString() << std::endl;
      }
   }
}
