// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslserver.h"

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
   else if (t->getTID() == "oldest_tid")
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

// get responder
SSLTransmissionDatabaseResponder *SSLTransmissionDatabaseServer::getResponder()
{
   return(responder_);
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
