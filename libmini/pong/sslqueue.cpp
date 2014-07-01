// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslqueue.h"

// ssl transmission queue client ctor
SSLTransmissionQueueClient::SSLTransmissionQueueClient(QString hostName, quint16 port,
                                                       QString uid, bool verify, bool compress,
                                                       QObject *parent)
   : SSLTransmissionDatabaseClient(hostName, port, uid, verify, compress, parent),
     transmitting_(false), stopped_(true),
     e_("queue client")
{
   // open transmission database
   db_ = new SSLTransmissionDatabase("queue");
   if (!db_->openDB()) throw e_;

   // signal ssl transmission pong
   connect(getReceiver(), SIGNAL(onPong(QString, quint16)),
           this, SLOT(alive(QString, quint16)));

   // signal ssl transmission success
   connect(getReceiver(), SIGNAL(onSuccess(QString, quint16, QString, QString)),
           this, SLOT(transmitted(QString, quint16, QString, QString)));

   // signal ssl transmission failure
   connect(getReceiver(), SIGNAL(onFailure(QString, quint16, QString, QString)),
           this, SLOT(failed(QString, quint16, QString, QString)));

   timer_ = new QTimer(this);
   connect(timer_, SIGNAL(timeout()), this, SLOT(pingNonBlocking()));
   timer_->start(10000); // ms
}

// ssl transmission queue client dtor
SSLTransmissionQueueClient::~SSLTransmissionQueueClient()
{
   delete db_;
   delete timer_;
}

// start transmission queue
void SSLTransmissionQueueClient::start()
{
   stopped_ = false;

   if (!transmitting_)
   {
      QString uid = getUID();
      QString tid = db_->oldest(uid);

      if (tid.size()>0)
      {
         SSLTransmission t = db_->read(tid, uid);
         SSLTransmissionDatabaseClient::transmitNonBlocking(t);
         transmitting_ = true;
      }
   }

   emit changed(size());
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

void SSLTransmissionQueueClient::alive(QString hostName, quint16 port)
{
   start();
}

void SSLTransmissionQueueClient::transmitted(QString hostName, quint16 port, QString tid, QString uid)
{
   db_->remove(tid, uid);
   transmitting_ = false;

   emit changed(size());

   if (!stopped_)
      start();
}

void SSLTransmissionQueueClient::failed(QString hostName, quint16 port, QString tid, QString uid)
{
   transmitting_ = false;
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

   emit changed(size());

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
