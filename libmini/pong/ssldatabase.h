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

   SSLTransmissionDatabase(QObject *parent = NULL);
   virtual ~SSLTransmissionDatabase();

   // open db connection
   bool openConnection();

   // remove db
   bool removeDB();

   // create key/value table
   bool createTable();

   // read a transmission from the db
   SSLTransmission read(QString tid, QString uid);

   // remove a transmission from the db
   void remove(QString tid, QString uid);

   // get last db error
   //  if opening database has failed
   //  user can ask for error description by QSqlError::text()
   QSqlError lastError();

protected:

   QSqlDatabase db_;
   QString path_;

public slots:

   // write a transmission to the db
   void write(SSLTransmission);
};

#endif
