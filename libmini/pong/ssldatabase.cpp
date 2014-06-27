// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QDir>
#include <QSettings>

#include <QtSql/QSqlQuery>

#include "ssldatabase.h"

SSLTransmissionDatabase::SSLTransmissionDatabase(QObject *parent, QString name)
   : QObject(parent),
     e_("database")
{
   // determine db path
   QString path(QDir::home().path());
   path.append(QDir::separator()).append(name).append(".sqlite");
   path_ = QDir::toNativeSeparators(path);

   // seed random number generator
   srand(time(NULL));
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

   // create tables
   if (!exists)
      if (!createTables())
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

// create key/value tables
bool SSLTransmissionDatabase::createTables()
{
   bool success = false;

   // create tables
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

      QString create_users("CREATE TABLE users"
                           "("
                           "id INT,"
                           "uid TEXT NOT NULL,"
                           "PRIMARY KEY(uid)"
                           ")");

      QSqlQuery query;
      success = query.exec(create);
      success &= query.exec(create_users);
   }

   return(success);
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

   if (db_.isOpen())
   {
      do
      {
         uid = random(len);
         QString insert=QString("INSERT INTO users VALUES(NULL, '%1')").arg(uid);
         query.prepare(insert);
      }
      while (!query.exec());
   }

   return(uid);
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

// dump the db
void SSLTransmissionDatabase::dump()
{
   SSLTransmissionDatabase db;
   if (!db.openDB()) return;

   QStringList users = db.users();

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

// create a transmission response from the db
SSLTransmission *SSLTransmissionDatabaseResponder::create(const SSLTransmission *t)
{
   return(new SSLTransmission(db_->read(t->getTID(), t->getUID())));
}

// create a command response from the db
SSLTransmission *SSLTransmissionDatabaseResponder::command(const SSLTransmission *t)
{
   SSLTransmission *r = new SSLTransmission(t->getTID(), t->getUID());

   if (t->getTID() == "create_uid")
   {
      QString uid = db_->create_uid();

      if (uid.size() == 0)
         r->setError();
      else
         r->setData(uid.toAscii());
   }
   else if (t->getTID() == "oldest")
   {
      QString oldest = db_->oldest(t->getUID());

      if (oldest.size() == 0)
         r->setError();
      else
         r->setData(oldest.toAscii());
   }
   else
      r->setError();

   return(r);
}

// ssl transmission database server ctor
SSLTransmissionDatabaseServer::SSLTransmissionDatabaseServer(quint16 port,
                                                             QString certPath, QString keyPath,
                                                             QString altPath,
                                                             QObject *parent)
   : QObject(parent),
     port_(port), certPath_(certPath), keyPath_(keyPath), altPath_(altPath),
     db_(NULL), responder_(NULL), factory_(NULL), server_(NULL),
     e_("database server")
{
   // open transmission database
   db_ = new SSLTransmissionDatabase;
   if (!db_->openDB()) throw e_;

   // aggregate transmission database with server
   responder_ = new SSLTransmissionDatabaseResponder(db_);
   factory_ = new SSLTransmissionServerConnectionFactory(responder_);
   server_ = new SSLServer(factory_);

   // connect transmission database with the connection factory transmitted signal
   connect(factory_, SIGNAL(transmitted(SSLTransmission)),
           db_, SLOT(write(SSLTransmission)));
}

// ssl transmission database server dtor
SSLTransmissionDatabaseServer::~SSLTransmissionDatabaseServer()
{
   if (server_)
      delete server_;

   if (factory_)
      delete factory_;

   if (responder_)
      delete responder_;

   if (db_)
      delete db_;
}

// get port
quint16 SSLTransmissionDatabaseServer::getPort()
{
   return(port_);
}

// get database
SSLTransmissionDatabase *SSLTransmissionDatabaseServer::getDatabase()
{
   return(db_);
}

// get factory
SSLTransmissionServerConnectionFactory *SSLTransmissionDatabaseServer::getFactory()
{
   return(factory_);
}

// start listening
void SSLTransmissionDatabaseServer::start()
{
   // start server on specified port (default 10000)
   server_->start(certPath_, keyPath_, port_, altPath_);
}

// ssl transmission database client ctor
SSLTransmissionDatabaseClient::SSLTransmissionDatabaseClient(QString hostName, quint16 port,
                                                             QString uid, bool verify, bool compress,
                                                             QObject *parent)
   : SSLTransmissionClient(parent),
     _hostName_(hostName), _port_(port),
     _uid_(uid), _verify_(verify), _compress_(compress),
     _receiver_(NULL)
{
   _receiver_ = new SSLTransmissionDatabaseResponseReceiver;
}

// ssl transmission database client dtor
SSLTransmissionDatabaseClient::~SSLTransmissionDatabaseClient()
{
   if (_receiver_)
      delete _receiver_;
}

// get host name
QString SSLTransmissionDatabaseClient::getHostName()
{
   return(_hostName_);
}

// get port
quint32 SSLTransmissionDatabaseClient::getPort()
{
   return(_port_);
}

// get user name
QString SSLTransmissionDatabaseClient::getUID()
{
   return(_uid_);
}

// auto-select user name
bool SSLTransmissionDatabaseClient::autoselectUID(bool reset)
{
   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (settings.contains("uid") && !reset)
      _uid_ = settings.value("uid").toString();
   else
   {
      SSLTransmissionClient client;
      SSLTransmission t("create_uid", "", QDateTime::currentDateTimeUtc(), SSLTransmission::cc_command);

      if (!client.transmit(_hostName_, _port_, t, _verify_))
         return(false);
      else
         if (!client.getResponse())
            return(false);
         else
            _uid_ = client.getResponse()->getData();

      settings.setValue("uid", _uid_);
   }

   return(true);
}

// get receiver
SSLTransmissionResponseReceiver *SSLTransmissionDatabaseClient::getReceiver()
{
   return(_receiver_);
}

// start transmission
bool SSLTransmissionDatabaseClient::transmit(QString fileName)
{
   return(SSLTransmissionClient::transmit(_hostName_, _port_, fileName, _uid_, _verify_, _compress_));
}

// determine transmission host name
void SSLTransmissionDatabaseClient::transmitHostName(QString hostName)
{
   _hostName_ = hostName;
}

// start non-blocking transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(QString fileName)
{
   SSLTransmissionClient::transmitNonBlocking(_hostName_, _port_, fileName, _uid_, _verify_, _compress_,
                                              SSLTransmission::cc_transmit, _receiver_);
}
