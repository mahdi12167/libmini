// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QTimer>

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

   // signal ssl transmission success
   connect(getReceiver(), SIGNAL(onSuccess(QString, quint16, QString, QString)),
           this, SLOT(transmitted(QString, quint16, QString, QString)));

   // signal ssl transmission failure
   connect(getReceiver(), SIGNAL(onFailure(QString, quint16, QString, QString)),
           this, SLOT(failed(QString, quint16, QString, QString)));
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

   QString uid = getUID();
   QString tid = db_->oldest(uid);

   if (tid.size()>0)
   {
      db_->hide(tid, uid);

      SSLTransmission t = db_->read(tid, uid);
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
   return(db_->oldest(getUID()).size()==0);
}

// queue size
int SSLTransmissionQueueClient::size()
{
   return(db_->list(getUID()).size());
}

void SSLTransmissionQueueClient::transmitted(QString hostName, quint16 port, QString tid, QString uid)
{
   db_->remove(tid, uid);

   emit changed();

   if (!stopped_)
      start();
}

void SSLTransmissionQueueClient::failed(QString hostName, quint16 port, QString tid, QString uid)
{
   db_->hide(tid, uid, false);

   if (!stopped_)
      QTimer::singleShot(1000, this, SLOT(start()));
}

// specify transmission host name
void SSLTransmissionQueueClient::transmitHostName(QString hostName, quint16 port)
{
   if (!empty())
   {
      emit error("queue not empty");
      return;
   }

   SSLTransmissionDatabaseClient::transmitHostName(hostName, port);
}

// queue non-blocking transmission
void SSLTransmissionQueueClient::transmitNonBlocking(const SSLTransmission &t)
{
   db_->write(t);

   emit changed();

   start();
}

// queue non-blocking file transmission
void SSLTransmissionQueueClient::transmitNonBlocking(QString fileName)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
   {
      emit error("unable to open file");
      return;
   }

   SSLTransmission t(file, uid_);

   if (compress_)
      t.compress();

   transmitNonBlocking(t);
}
