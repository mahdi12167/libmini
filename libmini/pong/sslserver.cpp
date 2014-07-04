// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslserver.h"

// create a transmission response from the db
SSLTransmission *SSLTransmissionDatabaseResponder::respond(const SSLTransmission *t)
{
   if (t->getTID() != "")
      return(new SSLTransmission(db_->read(t->getTID(), t->getUID())));
   else
   {
      SSLTransmission *r = new SSLTransmission(*t);

      QString oldest = db_->oldest(t->getUID());

      if (oldest.size() == 0)
         r->setError();
      else
         *r = db_->read(oldest, t->getUID());

      return(r);
   }
}

// create a command response from the db
SSLTransmission *SSLTransmissionDatabaseResponder::command(const SSLTransmission *t)
{
   QString action = t->getData();

   SSLTransmission *r = new SSLTransmission(*t);

   r->append(":");

   if (action == "create_uid")
   {
      QString uid = db_->create_uid();

      if (uid.size() == 0)
         r->setError();
      else
         r->append(uid.toAscii());
   }
   else if (action == "pair_uid")
   {
      QString code = db_->create_code(t->getUID());

      if (code.size() == 0)
         r->setError();
      else
         r->append(code.toAscii());
   }
   else if (action.startsWith("pair_code:"))
   {
      QString code = action.mid(action.indexOf(":")+1);
      QString uid = db_->apply_code(code);

      if (uid.size() == 0)
         r->setError();
      else
         r->append(uid.toAscii());
   }
   else if (action == "oldest_tid")
   {
      QString oldest = db_->oldest(t->getUID());

      if (oldest.size() == 0)
         r->setError();
      else
         r->append(oldest.toAscii());
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

   // connect server with the connection factory transmitted signal
   connect(factory_, SIGNAL(transmitted(SSLTransmission)),
           this, SLOT(transmitted(SSLTransmission)));

   // connect server with the connection factory responded signal
   connect(factory_, SIGNAL(responded(SSLTransmission)),
           this, SLOT(responded(SSLTransmission)));

   // connect server status with the connection factory transmitted signal
   connect(factory_, SIGNAL(transmitted()),
           this, SLOT(receive()));

   // connect server status with the connection factory responded signal
   connect(factory_, SIGNAL(responded()),
           this, SLOT(send()));
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

// stop listening
void SSLTransmissionDatabaseServer::stop()
{
   server_->stop();
}

// store size
int SSLTransmissionDatabaseServer::size()
{
   return(db_->size());
}

// receive transmitted signal
void SSLTransmissionDatabaseServer::transmitted(SSLTransmission t)
{
   db_->write(t);
}

// receive responded signal
void SSLTransmissionDatabaseServer::responded(SSLTransmission t)
{
  db_->remove(t.getTID(), t.getUID());

  std::cout << "responded" << std::endl; //!! debug
}

// receive responded signal
void SSLTransmissionDatabaseServer::send()
{
   emit status_send(size());
}

// receive transmission signal
void SSLTransmissionDatabaseServer::receive()
{
   emit status_receive(size());
}
