// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLSERVER_H
#define SSLSERVER_H

#include "ssldatabase.h"

// ssl transmission database responder class
class SSLTransmissionDatabaseResponder: public SSLTransmissionResponder
{
public:

   SSLTransmissionDatabaseResponder(SSLTransmissionDatabase *db)
      : SSLTransmissionResponder(), db_(db)
   {}

   virtual ~SSLTransmissionDatabaseResponder()
   {}

   // create a transmission response from the db
   virtual SSLTransmission *respond(const SSLTransmission *t);

   // create a command response from the db
   virtual SSLTransmission *command(const SSLTransmission *t);

protected:

   SSLTransmissionDatabase *db_;
};

// ssl transmission database server class
class SSLTransmissionDatabaseServer: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionDatabaseServer(quint16 port = SSLTransmission::default_port,
                                 QString certPath = "cert.pem", QString keyPath = "key.pem",
                                 QString altPath = "/usr/share/pong",
                                 QObject *parent = NULL);

   virtual ~SSLTransmissionDatabaseServer();

   // get port
   quint16 getPort();

   // get database
   SSLTransmissionDatabase *getDatabase();

   // get responder
   SSLTransmissionDatabaseResponder *getResponder();

   // get factory
   SSLTransmissionServerConnectionFactory *getFactory();

   // keep transmission responses as backup
   void keepBackup();

   // start listening
   void start();

   // stop listening
   void stop();

   // total store size
   int total();

protected:

   quint16 port_;
   QString certPath_, keyPath_;
   QString altPath_;

   SSLTransmissionDatabase *db_;
   SSLTransmissionDatabaseResponder *responder_;
   SSLTransmissionServerConnectionFactory *factory_;
   SSLServer *server_;

   bool keep_;

   SSLError e_;

protected slots:

   void transmitted(SSLTransmission t);
   void responded(SSLTransmission t);

signals:

   void status_send(int stored);
   void status_receive(int stored);
};

#endif
