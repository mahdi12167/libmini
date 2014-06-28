// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLDATABASE_H
#define SSLDATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "ssltransmission.h"

// storage container for ssl transmissions
class SSLTransmissionDatabase: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionDatabase(QObject *parent = NULL, QString name = "db");
   virtual ~SSLTransmissionDatabase();

   // open db connection
   bool openDB();

   // remove db
   bool removeDB();

   // create a unique user name
   QString create_uid(const int len=1024);

   // list user names in the db
   QStringList users();

   // list transmission names in the db
   QStringList list(QString uid);

   // retrieve oldest transmission name in the db
   QString oldest(QString uid);

   // check for a transmission name in the db
   bool exists(QString tid, QString uid);

   // read a transmission from the db
   SSLTransmission read(QString tid, QString uid);

   // remove a transmission from the db
   bool remove(QString tid, QString uid);

   // dump the db
   static void dump();

protected:

   // create key/value tables
   bool createTables();

   QSqlDatabase db_;
   QString path_;

   SSLError e_;

   public slots:

   // write a transmission to the db
   void write(SSLTransmission);

private:

   // generate a random string
   static QString random(const int len);
};

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
   virtual SSLTransmission *create(const SSLTransmission *t);

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

   SSLTransmissionDatabaseServer(quint16 port = 10000,
                                 QString certPath = "cert.pem", QString keyPath = "key.pem",
                                 QString altPath = "/usr/share/pong",
                                 QObject *parent = NULL);

   virtual ~SSLTransmissionDatabaseServer();

   // get port
   quint16 getPort();

   // get database
   SSLTransmissionDatabase *getDatabase();

   // get factory
   SSLTransmissionServerConnectionFactory *getFactory();

   // start listening
   void start();

protected:

   quint16 port_;
   QString certPath_, keyPath_;
   QString altPath_;

   SSLTransmissionDatabase *db_;
   SSLTransmissionDatabaseResponder *responder_;
   SSLTransmissionServerConnectionFactory *factory_;
   SSLServer *server_;

   SSLError e_;
};

// ssl transmission database response receiver class
class SSLTransmissionDatabaseResponseReceiver: public SSLTransmissionResponseReceiver
{
   Q_OBJECT

public:

   SSLTransmissionDatabaseResponseReceiver(QObject *parent = NULL) : SSLTransmissionResponseReceiver(parent) {}
   virtual ~SSLTransmissionDatabaseResponseReceiver() {}

   virtual void onSuccess(QString hostName, quint16 port, QString fileName, QString uid) {}
   virtual void onFailure(QString hostName, quint16 port, QString fileName, QString uid) {}
   virtual void onResponse(SSLTransmission t) {}
   virtual void onResult(SSLTransmission t) {}
   virtual void onError(QString e) {}
};

// ssl transmission database client class
class SSLTransmissionDatabaseClient: public SSLTransmissionClient
{
   Q_OBJECT

public:

   SSLTransmissionDatabaseClient(QString hostName, quint16 port=10000,
                                 QString uid="", bool verify=true, bool compress=false,
                                 QObject *parent = NULL);

   virtual ~SSLTransmissionDatabaseClient();

   // get host name
   QString getHostName();

   // get port
   int getPort();

   // get user name
   QString getUID();

   // get receiver
   SSLTransmissionResponseReceiver *getReceiver();

   // start transmission
   bool transmit(QString fileName);

   // reset user name
   bool reset();

protected:

   QString hostName__;
   quint16 port__;
   QString uid__;
   bool verify__;
   bool compress__;

   bool autoselect__;

   SSLTransmissionResponseReceiver *receiver__;

   // register user with server
   bool registerUID();

   // auto-select user name
   bool autoselectUID(bool reset=false);

public slots:

   // determine transmission host name
   void transmitHostName(QString hostName);

   // start non-blocking transmission
   void transmitNonBlocking(QString fileName);
};

#endif
