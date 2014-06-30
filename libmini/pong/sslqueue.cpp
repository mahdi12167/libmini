// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslqueue.h"

// ssl transmission queue client ctor
SSLTransmissionQueueClient::SSLTransmissionQueueClient(QString hostName, quint16 port,
                                                       QString uid, bool verify, bool compress,
                                                       QObject *parent)
   : SSLTransmissionDatabaseClient(hostName, port, uid, verify, compress, parent),
     stopped_(true),
     e_("queue client")
{
   // open transmission database
   db_ = new SSLTransmissionDatabase("queue");
   if (!db_->openDB()) throw e_;

   connect(this, SIGNAL(success(QString, quint16, QString, QString)),
           this, SLOT(transmitted(QString, quint16, QString, QString)));
}

// ssl transmission queue client dtor
SSLTransmissionQueueClient::~SSLTransmissionQueueClient()
{
   if (db_)
      delete db_;
}

// start transmission queue
void SSLTransmissionQueueClient::start()
{
   stopped_ = false;

   QString tid = db_->oldest(uid_);

   if (tid.size()>0)
   {
      SSLTransmission t = db_->read(tid, uid_);
      SSLTransmissionDatabaseClient::transmitNonBlocking(t);
   }
}

// stop transmission queue
void SSLTransmissionQueueClient::stop()
{
   stopped_ = true;
}

// is the queue empty?
bool SSLTransmissionQueueClient::empty()
{
   return(db_->oldest(uid_).size()==0);
}

void SSLTransmissionQueueClient::transmitted(QString hostName, quint16 port, QString tid, QString uid)
{
   db_->remove(tid, uid);

   if (!stopped_)
      start();
}

// specify transmission host name
void SSLTransmissionQueueClient::transmitHostName(QString hostName, quint16 port)
{
   if (!empty())
      return;

   SSLTransmissionDatabaseClient::transmitHostName(hostName, port);
}

// queue non-blocking transmission
void SSLTransmissionQueueClient::transmitNonBlocking(const SSLTransmission &t)
{
   db_->write(t);
   start();
}

// queue non-blocking file transmission
void SSLTransmissionQueueClient::transmitNonBlocking(QString fileName)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly)) throw e_;

   SSLTransmission t(file, uid_);

   if (compress_)
      t.compress();

   transmitNonBlocking(t);
}
