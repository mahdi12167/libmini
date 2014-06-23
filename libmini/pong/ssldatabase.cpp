// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QDir>
#include <QSQLQuery>

#include "ssldatabase.h"

SSLTransmissionDatabase::SSLTransmissionDatabase(QObject *parent, QString name)
   : QObject(parent),
     e_("database")
{
   // determine db path
   QString path(QDir::home().path());
   path.append(QDir::separator()).append(name).append(".sqlite");
   path_ = QDir::toNativeSeparators(path);
}

SSLTransmissionDatabase::~SSLTransmissionDatabase()
{}

// open db connection
bool SSLTransmissionDatabase::openDB()
{
   // check for existing database
   bool exists = QFileInfo(path_).exists();

   // open default connection with SQLite driver
   db_ = QSqlDatabase::addDatabase("QSQLITE");

   // open db
   db_.setDatabaseName(path_);
   bool success = db_.open();

   // create table
   if (!exists)
      if (!createTable())
         success = false;

   return(success);
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
                     "id INT,"
                     "tid TEXT NOT NULL,"
                     "uid TEXT NOT NULL,"
                     "isotime VARCHAR(19),"
                     "content BLOB,"
                     "compressed BIT,"
                     "PRIMARY KEY(tid, uid)"
                     ")");

      QSqlQuery query;
      success = query.exec(create);
   }

   return(success);
}

// list user names in the db
QStringList SSLTransmissionDatabase::users()
{
   QStringList list;

   if (db_.isOpen())
   {
      QString select = QString("SELECT DISTINCT uid FROM transmissions "
                               "ORDER BY uid ASC");

      QSqlQuery query(select);

      while (query.next())
         list.append(query.value(0).toString());
   }

   return(list);
}

// list transmission names in the db
QStringList SSLTransmissionDatabase::list(QString uid)
{
   QStringList list;

   if (db_.isOpen())
   {
      QString select = QString("SELECT tid FROM transmissions "
                               "WHERE uid = '%1' "
                               "ORDER BY id ASC").arg(uid);

      QSqlQuery query(select);

      while (query.next())
         list.append(query.value(0).toString());
   }

   return(list);
}

// retrieve oldest transmission name in the db
QString SSLTransmissionDatabase::oldest(QString uid)
{
   QString tid;

   if (db_.isOpen())
   {
      QString select = QString("SELECT tid, MIN(id) FROM transmissions "
                               "WHERE uid = '%1'").arg(uid);

      QSqlQuery query(select);

      if (query.next())
         tid = query.value(0).toString();
   }

   return(tid);
}

// check for a transmission name in the db
bool SSLTransmissionDatabase::exists(QString tid, QString uid)
{
   if (db_.isOpen())
   {
      QString select = QString("SELECT id FROM transmissions "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(select);
      if (query.next()) return(true);
   }

   return(false);
}

// read a transmission from the db
SSLTransmission SSLTransmissionDatabase::read(QString tid, QString uid)
{
   SSLTransmission t;
   t.setError();

   if (db_.isOpen())
   {
      QString select = QString("SELECT content, isotime, compressed FROM transmissions "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(select);

      if (query.next())
         t = SSLTransmission(query.value(0).toByteArray(), tid, uid,
                             QDateTime::fromString(query.value(1).toString(), Qt::ISODate),
                             query.value(2).toInt());
   }

   return(t);
}

// write a transmission to the db
void SSLTransmissionDatabase::write(SSLTransmission t)
{
   if (db_.isOpen())
   {
      QString insert=QString("INSERT OR REPLACE INTO transmissions VALUES(NULL, '%1', '%2', '%3', ?, %4)")
                     .arg(t.getTID()).arg(t.getUID()).arg(t.getTime().toString(Qt::ISODate)).arg(t.compressed());

      QSqlQuery query;
      query.prepare(insert);
      query.addBindValue(t.getData());
      if (!query.exec()) throw e_;
   }
}

// remove a transmission from the db
bool SSLTransmissionDatabase::remove(QString tid, QString uid)
{
   if (db_.isOpen())
   {
      QString remove = QString("DELETE FROM transmissions "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(remove);
      if (query.exec()) return(true);
   }

   return(false);
}

// create a transmission response from the db
SSLTransmission *SSLTransmissionDatabaseResponder::create(const SSLTransmission *t)
{
   return(new SSLTransmission(db_->read(t->getTID(), t->getUID())));
}