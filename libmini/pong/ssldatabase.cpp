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

   // create table "files"
   if (db_.isOpen())
   {
      QSqlQuery query;
      success = query.exec("CREATE TABLE files"
                           "("
                           "filename TEXT PRIMARY KEY,"
                           "content BLOB"
                           ")");
   }

   return(success);
}

// read a transmission from the db
SSLTransmission SSLTransmissionDatabase::read(QString tid, QString uid)
{
   SSLTransmission t;

   return(t);
}

// remove a transmission from the db
void SSLTransmissionDatabase::remove(QString tid, QString uid)
{}

// write a transmission to the db
void SSLTransmissionDatabase::write(SSLTransmission t)
{}

// get last db error
QSqlError SSLTransmissionDatabase::lastError()
{
   return(db_.lastError());
}
