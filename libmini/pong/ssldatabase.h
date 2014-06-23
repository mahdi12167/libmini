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

protected:

   // create key/value table
   bool createTable();

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

#endif
