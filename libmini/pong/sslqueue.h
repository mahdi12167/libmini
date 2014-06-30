// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLQUEUE_H
#define SSLQUEUE_H

#include "sslclient.h"

// ssl transmission queue client class
class SSLTransmissionQueueClient: public SSLTransmissionDatabaseClient
{
   Q_OBJECT

public:

   SSLTransmissionQueueClient(QString hostName, quint16 port=10000,
                              QString uid="", bool verify=true, bool compress=false,
                              QObject *parent = NULL);

   virtual ~SSLTransmissionQueueClient();

   // queue transmission
   bool transmit(QString fileName);

   // queue file transmission
   bool transmit(const SSLTransmission &t);

public slots:

   // specify transmission host name
   void transmitHostName(QString hostName, quint16 port);

   // start non-blocking transmission
   void transmitNonBlocking(const SSLTransmission &t);

   // start non-blocking file transmission
   void transmitNonBlocking(QString fileName);
};

#endif
