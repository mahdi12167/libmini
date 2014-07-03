// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLDATABASE_H
#define SSLDATABASE_H

#include <QStringList>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "ssltransmission.h"

// storage container for ssl transmissions
class SSLTransmissionDatabase: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionDatabase(QString name = "db",
                           QObject *parent = NULL);

   virtual ~SSLTransmissionDatabase();

   // open db connection
   bool openDB();

   // remove db
   bool removeDB();

   // get stored transmissions in the db
   int size();

   // create a unique user name
   QString create_uid(const int len=1024);

   // get all unique user names
   QStringList get_uids();

   // list user names in the db
   QStringList users();

   // list transmission names in the db
   QStringList list(QString uid);

   // retrieve oldest transmission name in the db
   QString oldest(QString uid);

   // check for a transmission name in the db
   bool exists(QString tid, QString uid);

   // hide a transmission name in the db
   bool hide(QString tid, QString uid, bool hidden=true);

   // read a transmission from the db
   SSLTransmission read(QString tid, QString uid);

   // remove a transmission from the db
   bool remove(QString tid, QString uid);

   // dump the db
   static void dump(QString name = "db");

protected:

   // create key/value tables
   bool createTables();

   QSqlDatabase *db_;
   QString path_;

   SSLError e_;

   public slots:

   // write a transmission to the db
   void write(SSLTransmission t);

   // remove a transmission from the db
   void remove(SSLTransmission t);

private:

   // generate a random string
   static QString random(const int len);
};

#endif
