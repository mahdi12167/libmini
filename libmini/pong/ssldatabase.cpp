// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QDir>

#include <QtSql/QSqlQuery>

#include "ssldatabase.h"

// ssl database ctor
SSLTransmissionDatabase::SSLTransmissionDatabase(QString name,
                                                 QObject *parent)
   : QObject(parent),
     e_("database")
{
   // determine db path
   QString path(QDir::home().path());
   path.append(QDir::separator()).append(name).append(".sqlite");
   path_ = QDir::toNativeSeparators(path);

   // seed random number generator
   srand(time(NULL));

   // create db
   db_ = new QSqlDatabase;
}

// ssl database dtor
SSLTransmissionDatabase::~SSLTransmissionDatabase()
{
   // close db
   db_->close();
   delete db_;

   // remove db connection
   QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

// open db connection
bool SSLTransmissionDatabase::openDB()
{
   // check for existing db
   bool exists = QFileInfo(path_).exists();

   // open default connection with SQLite driver
   *db_ = QSqlDatabase::addDatabase("QSQLITE");

   // open db
   db_->setDatabaseName(path_);
   bool success = db_->open();

   // create tables
   if (!exists)
      if (!createTables())
         success = false;

   return(success);
}

// remove db
bool SSLTransmissionDatabase::removeDB()
{
   // close db
   db_->close();

   // remove db file
   return(QFile::remove(path_));
}

// create key/value tables
bool SSLTransmissionDatabase::createTables()
{
   bool success = false;

   // create tables
   if (db_->isOpen())
   {
      QString create("CREATE TABLE transmissions"
                     "("
                     "id INT,"
                     "tid TEXT NOT NULL,"
                     "uid TEXT NOT NULL,"
                     "isotime VARCHAR(19),"
                     "content BLOB,"
                     "compressed BIT,"
                     "hidden BIT,"
                     "PRIMARY KEY(tid, uid)"
                     ")");

      QString create_users("CREATE TABLE users"
                           "("
                           "id INT,"
                           "uid TEXT NOT NULL,"
                           "PRIMARY KEY(uid)"
                           ")");

      QString create_codes("CREATE TABLE codes"
                           "("
                           "id INT,"
                           "code TEXT NOT NULL,"
                           "uid TEXT NOT NULL,"
                           "PRIMARY KEY(code)"
                           ")");

      QSqlQuery query;
      success = query.exec(create);
      success &= query.exec(create_users);
      success &= query.exec(create_codes);
   }

   return(success);
}

// get stored transmissions in the db
int SSLTransmissionDatabase::size()
{
   if (db_->isOpen())
   {
      QString count = QString("SELECT COUNT(*) FROM transmissions");

      QSqlQuery query(count);

      if (query.next())
         return(query.value(0).toInt());
   }

   return(0);
}

// generate a random string
QString SSLTransmissionDatabase::random(const int len)
{
   static const char alphanum[] =
       "0123456789"
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
       "abcdefghijklmnopqrstuvwxyz";

   QString s;

   for (int i=0; i<len; i++)
      s += alphanum[rand() % (sizeof(alphanum)-1)];

   return(s);
}

// create a unique user name
QString SSLTransmissionDatabase::create_uid(const int len)
{
   QString uid;

   QSqlQuery query;

   if (db_->isOpen())
   {
      do
      {
         uid = random(len);
         QString insert = QString("INSERT INTO users VALUES(NULL, '%1')").arg(uid);
         query.prepare(insert);
      }
      while (!query.exec());
   }

   return(uid);
}

// create code to pair user name
QString SSLTransmissionDatabase::create_code(QString uid, int len)
{
   QString code;

   QSqlQuery query;

   if (db_->isOpen())
   {
      do
      {
         code = random(len);
         QString insert = QString("INSERT INTO codes VALUES(NULL, '%1', '%2')").arg(code).arg(uid);
         query.prepare(insert);
      }
      while (!query.exec());
   }

   // return unique pair code for uid
   return(code);
}

// apply code to pair user name
QString SSLTransmissionDatabase::apply_code(QString code)
{
   QString uid;

   if (db_->isOpen())
   {
      QString select = QString("SELECT uid FROM codes "
                               "WHERE code = '%1'").arg(code);

      QSqlQuery query(select);

      if (query.next())
         uid = query.value(0).toString();
   }

   // return uid for unique pair code
   return(uid);
}

// remove code to pair user name
bool SSLTransmissionDatabase::remove_code(QString code)
{
   if (db_->isOpen())
   {
      QString remove = QString("DELETE FROM codes "
                               "WHERE code = '%1'").arg(code);

      QSqlQuery query(remove);
      if (query.exec()) return(true);
   }

   return(false);
}

// get all unique user names
QStringList SSLTransmissionDatabase::get_uids()
{
   QStringList list;

   if (db_->isOpen())
   {
      QString select = QString("SELECT DISTINCT uid FROM users "
                               "ORDER BY uid ASC");

      QSqlQuery query(select);

      while (query.next())
         list.append(query.value(0).toString());
   }

   return(list);
}

// list user names in the db
QStringList SSLTransmissionDatabase::users()
{
   QStringList list;

   if (db_->isOpen())
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

   if (db_->isOpen())
   {
      QString select = QString("SELECT tid FROM transmissions "
                               "WHERE (uid = '%1') AND (hidden = 0) "
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

   if (db_->isOpen())
   {
      QString select = QString("SELECT tid FROM transmissions "
                               "WHERE (uid = '%1') AND (hidden = 0) "
                               "ORDER BY id ASC LIMIT 1").arg(uid);

      QSqlQuery query(select);

      if (query.next())
         tid = query.value(0).toString();
   }

   return(tid);
}

// check for a transmission name in the db
bool SSLTransmissionDatabase::exists(QString tid, QString uid)
{
   if (db_->isOpen())
   {
      QString select = QString("SELECT id FROM transmissions "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(select);
      if (query.next()) return(true);
   }

   return(false);
}

// hide a transmission name in the db
bool SSLTransmissionDatabase::hide(QString tid, QString uid, bool hidden)
{
   if (db_->isOpen())
   {
      QString select = QString("UPDATE transmissions "
                               "SET hidden = %3 "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid).arg((int)hidden);

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

   if (db_->isOpen())
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
   if (db_->isOpen())
   {
      QString insert=QString("INSERT OR REPLACE INTO transmissions VALUES(NULL, '%1', '%2', '%3', ?, %4, 0)")
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
   if (db_->isOpen())
   {
      QString remove = QString("DELETE FROM transmissions "
                               "WHERE (tid = '%1') AND (uid = '%2')").arg(tid).arg(uid);

      QSqlQuery query(remove);
      if (query.exec()) return(true);
   }

   return(false);
}

// dump the db
void SSLTransmissionDatabase::dump(QString name)
{
   SSLTransmissionDatabase db(name);
   if (!db.openDB()) return;

   QStringList uids = db.get_uids();

   if (uids.size()>0)
      std::cout << "registered users: " << uids.size() << std::endl;

   QStringList users = db.users();

   if (users.size()==0)
      std::cout << "no transmissions" << std::endl;
   else
      for (int i=0; i<users.size(); i++)
      {
         std::cout << "user \"" << users[i].toStdString() << "\":" << std::endl;

         QStringList list = db.list(users[i]);

         for (int j=0; j<list.size(); j++)
         {
            SSLTransmission t = db.read(list[j], users[i]);
            std::cout << " " << t << std::endl;
         }
      }
}
