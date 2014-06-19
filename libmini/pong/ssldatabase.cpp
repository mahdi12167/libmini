// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QDir>
#include <QSQLQuery>

#include "ssldatabase.h"

SSLTransmissionDatabase::SSLTransmissionDatabase(QObject *parent)
   : QObject(parent)
{}

SSLTransmissionDatabase::~SSLTransmissionDatabase()
{}

// open db connection
bool SSLTransmissionDatabase::openConnection()
{
   // open connection with SQLite driver
   db_ = QSqlDatabase::addDatabase("QSQLITE");

   // determine db path
   QString path(QDir::home().path());
   path.append(QDir::separator()).append("db.sqlite");
   path = QDir::toNativeSeparators(path);

   // set db name
   db_.setDatabaseName(path);
   path_ = path;

   // open db
   return db_.open();
}

bool SSLTransmissionDatabase::removeDB()
{
   // close database
   db_.close();

   // remove db file
   return(QFile::remove(path_));
}

// create key/value table
bool SSLTransmissionDatabase::createTable()
{
   bool success = false;

   // create table "transmissions"
   if (db_.isOpen())
   {
      QString create("CREATE TABLE transmissions"
                     "("
                     "tid TEXT PRIMARY KEY,"
                     "uid TEXT,"
                     "isotime varchar(19),"
                     "content BLOB,"
                     ")");

      QSqlQuery query;
      success = query.exec(create);
   }

   return(success);
}

// list transmissions in the db
QStringList SSLTransmissionDatabase::list(QString uid)
{
   QStringList list;

   if (db_.isOpen())
   {
      QString select = QString("SELECT t.tid FROM transmissions t"
                            "WHERE t.uid = '%1')").arg(uid);

      QSqlQuery query(select);

      while (query.next())
         list.append(query.value(0).toString());
   }

   return(list);
}

// read a transmission from the db
SSLTransmission SSLTransmissionDatabase::read(QString tid, QString uid)
{
   SSLTransmission t;

   if (db_.isOpen())
   {
      QString select = QString("SELECT t.content, t.isotime FROM transmissions t"
                               "WHERE (t.tid = '%1') AND (t.uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(select);
      query.next();

      t = SSLTransmission(query.value(0).toByteArray(), tid, uid,
                          QDateTime::fromString(query.value(1).toString(), Qt::ISODate));
   }

   return(t);
}

// remove a transmission from the db
void SSLTransmissionDatabase::remove(QString tid, QString uid)
{
   if (db_.isOpen())
   {
      QString remove = QString("DELETE FROM transmissions"
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(remove);
      query.exec();
   }
}

// write a transmission to the db
void SSLTransmissionDatabase::write(SSLTransmission t)
{
   if (db_.isOpen())
   {
      QString insert=QString("INSERT INTO transmissions VALUES('%1','%2','%3',?)")
                     .arg(t.getTID()).arg(t.getUID()).arg(t.getTime().toString(Qt::ISODate));

      QSqlQuery query;
      query.prepare(insert);
      query.addBindValue(t.getData());
      query.exec();
   }
}

// get last db error
QSqlError SSLTransmissionDatabase::lastError()
{
   return(db_.lastError());
}
