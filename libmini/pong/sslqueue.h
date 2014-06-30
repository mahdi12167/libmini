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

   // start transmission queue
   void start();

   // stop transmission queue
   void stop();

protected:

   SSLTransmissionDatabase *db_;

   bool stopped_;

   SSLError e_;

public slots:

   // specify transmission host name
   void transmitHostName(QString hostName, quint16 port);

   // queue non-blocking transmission
   void transmitNonBlocking(const SSLTransmission &t);

   // queue non-blocking file transmission
   void transmitNonBlocking(QString fileName);

protected slots:

   void transmitted(QString hostName, quint16 port, QString tid, QString uid);
};

#endif
