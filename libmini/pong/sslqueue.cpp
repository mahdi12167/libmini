// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslqueue.h"

// ssl transmission queue client ctor
SSLTransmissionQueueClient::SSLTransmissionQueueClient(QString hostName, quint16 port,
                                                       QString uid, bool verify, bool compress,
                                                       bool uploadMode,
                                                       int maxThreads,
                                                       QObject *parent)
   : SSLTransmissionDatabaseClient(hostName, port, uid, verify, compress, maxThreads, parent),
     uploadMode_(uploadMode),
     transmitting_(false), stopped_(true),
     e_("queue client")
{
   // open transmission database
   db_ = new SSLTransmissionDatabase(uploadMode_?"queue":"eueuq");
   if (!db_->openDB()) throw e_;

   // signal ssl transmission pong
   connect(this, SIGNAL(pong(QString, quint16, bool)),
           this, SLOT(alive(QString, quint16, bool)));

   // signal ssl transmission success
   connect(this, SIGNAL(success(QString, quint16, QString, QString)),
           this, SLOT(transmitted(QString, quint16, QString, QString)));

   // signal ssl transmission failure
   connect(this, SIGNAL(failure(QString, quint16, QString, QString)),
           this, SLOT(failed(QString, quint16, QString, QString)));

   // signal ssl transmission response
   connect(this, SIGNAL(response(SSLTransmission)),
           this, SLOT(received(SSLTransmission)));

   // establish timer
   timer_ = new QTimer(this);
   connect(timer_, SIGNAL(timeout()), this, SLOT(pingNonBlocking()));
}

// ssl transmission queue client dtor
SSLTransmissionQueueClient::~SSLTransmissionQueueClient()
{
   delete db_;
   delete timer_;
}

// client mode (upload/download)
bool SSLTransmissionQueueClient::uploadMode()
{
   return(uploadMode_);
}

// start transmission queue
void SSLTransmissionQueueClient::send()
{
   if (!uploadMode_) throw e_;

   stopped_ = false;

   timer_->start(10000); // ms

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

   emit status_send(size());
}

// start transmission queue
void SSLTransmissionQueueClient::receive()
{
   if (uploadMode_) throw e_;

   stopped_ = false;

   timer_->start(10000); // ms

   if (!transmitting_)
   {
      QString uid = getUID();
      SSLTransmission t = SSLTransmission::ssl_respond(uid);
      SSLTransmissionDatabaseClient::transmitNonBlocking(t);
      transmitting_ = true;
   }

   emit status_receive(size());
}

// stop transmission queue
void SSLTransmissionQueueClient::stop()
{
   stopped_ = true;

   timer_->stop();
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

// ssl transmission pong
void SSLTransmissionQueueClient::alive(QString hostName, quint16 port, bool ack)
{
   if (ack)
      if (uploadMode_)
         send();
      else
         receive();
   else
      emit error("cannot ping host");
}

// ssl transmission success
void SSLTransmissionQueueClient::transmitted(QString hostName, quint16 port, QString tid, QString uid)
{
   transmitting_ = false;

   db_->remove(tid, uid);

   emit status_send(size());

   if (!stopped_)
      send();
}

// ssl transmission failure
void SSLTransmissionQueueClient::failed(QString hostName, quint16 port, QString tid, QString uid)
{
   transmitting_ = false;
}

// ssl transmission response
void SSLTransmissionQueueClient::received(SSLTransmission t)
{
   transmitting_ = false;

   if (!t.valid())
      return;

   db_->write(t);

   emit status_receive(size());

   if (!stopped_)
      receive();
}

// specify transmission host name
void SSLTransmissionQueueClient::transmitHostName(QString hostName, quint16 port)
{
   SSLTransmissionDatabaseClient::transmitHostName(hostName, port);

   if (uploadMode_)
      emit status_send(size());
   else
      emit status_receive(size());
}

// queue non-blocking transmission
void SSLTransmissionQueueClient::transmitNonBlocking(const SSLTransmission &t)
{
   if (!uploadMode_) throw e_;

   db_->write(t);

   emit status_send(size());

   if (!stopped_)
      send();
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
