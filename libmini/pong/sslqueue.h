// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLQUEUE_H
#define SSLQUEUE_H

#include <QTimer>

#include "sslclient.h"

// ssl transmission queue client class
class SSLTransmissionQueueClient: public SSLTransmissionDatabaseClient
{
   Q_OBJECT

public:

   SSLTransmissionQueueClient(QString hostName, quint16 port = SSLTransmission::default_port,
                              QString uid="", bool verify=true, bool compress=false,
                              bool uploadMode=true,
                              int maxThreads=10,
                              QObject *parent = NULL);

   virtual ~SSLTransmissionQueueClient();

   // client mode (upload/download)
   bool uploadMode();

   // receive transmissions
   int receive();

   // is the queue empty?
   bool empty();

   // queue size
   int size();

   // total store size
   int total();

public slots:

   // start send queue
   void send();

   // start fetch queue
   void fetch();

   // start transmission queue
   void start();

   // stop transmission queue
   void stop();

   // clear transmissions
   void clear();

   // specify transmission host name
   void transmitHostName(QString hostName, quint16 port);

   // queue non-blocking transmission
   void transmitNonBlocking(const SSLTransmission &t);

   // queue non-blocking file transmission
   void transmitNonBlocking(QString fileName);

protected:

   SSLTransmissionDatabase *db_;

   bool uploadMode_;

   bool transmitting_;
   bool stopped_;

   QTimer *ping_timer_;
   static const int ping_interval = 10000; // ms

   SSLError e_;

protected slots:

   void pong(QString hostName, quint16 port, bool ack);
   void transmitted(QString hostName, quint16 port, QString tid, QString uid);
   void failed(QString hostName, quint16 port, QString tid, QString uid);
   void received(SSLTransmission t);

signals:

   void alive(QString hostName, quint16 port, bool ack);

   void status_send(int queued);
   void status_receive(int stored);
};

#endif
