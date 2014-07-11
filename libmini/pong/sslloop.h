// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLLOOP_H
#define SSLLOOP_H

#include "sslserver.h"

// ssl transmission database server loop class
class SSLTransmissionDatabaseServerLoop: protected SSLTransmissionDatabaseServer
{
   Q_OBJECT

public:

   SSLTransmissionDatabaseServerLoop(quint16 port = SSLTransmission::default_port,
                                     QString certPath = "cert.pem", QString keyPath = "key.pem",
                                     QString altPath = "/usr/share/pong",
                                     QObject *parent = NULL);

   virtual ~SSLTransmissionDatabaseServerLoop();

   // keep transmission responses as backup
   void keepBackup();

   // start the server loop
   void start();
};

#endif
