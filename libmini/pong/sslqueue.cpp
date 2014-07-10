// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslqueue.h"

// ssl transmission queue client ctor
SSLTransmissionQueueClient::SSLTransmissionQueueClient(QString hostName, quint16 port,
                                                       QString uid, bool verify, bool compress,
                                                       bool uploadMode,
                                                       int maxThreads,
                                                       QObject *parent)
   : SSLTransmissionDatabaseClient(hostName, port, uid, verify, compress,
                                   "open-terrain.org", uploadMode?"Upload":"Download",
                                   maxThreads, parent),
     uploadMode_(uploadMode),
     transmitting_(false), stopped_(true),
     e_("queue client")
{
   // open transmission database
   db_ = new SSLTransmissionDatabase(uploadMode_?"upload":"download");
   if (!db_->openDB()) throw e_;

   // signal ssl transmission pong
   connect(this, SIGNAL(pong(QString, quint16, bool)),
           this, SLOT(pong(QString, quint16, bool)));

   // signal ssl transmission success
   connect(this, SIGNAL(success(QString, quint16, QString, QString)),
           this, SLOT(transmitted(QString, quint16, QString, QString)));

   // signal ssl transmission failure
   connect(this, SIGNAL(failure(QString, quint16, QString, QString)),
           this, SLOT(failed(QString, quint16, QString, QString)));

   // signal ssl transmission response
   connect(this, SIGNAL(response(SSLTransmission)),
           this, SLOT(received(SSLTransmission)));

   // establish ping timer
   ping_timer_ = new QTimer(this);
   connect(ping_timer_, SIGNAL(timeout()), this, SLOT(pingNonBlocking()));
}

// ssl transmission queue client dtor
SSLTransmissionQueueClient::~SSLTransmissionQueueClient()
{
   delete db_;
   delete ping_timer_;
}

// client mode (upload/download)
bool SSLTransmissionQueueClient::uploadMode()
{
   return(uploadMode_);
}

// receive transmissions
int SSLTransmissionQueueClient::receive()
{
   int count = 0;

   QString uid = getUID();

   if (uid != "")
   {
      SSLTransmission t = SSLTransmission::ssl_respond(uid);

      while (transmit(t))
         if (getResponse())
         {
            SSLTransmission t(*getResponse());

            if (t.valid())
            {
               db_->write(t);
               count++;
            }
            else
               break;
         }
   }

   return(count);
}

// start send queue
void SSLTransmissionQueueClient::send()
{
   if (!uploadMode_) throw e_;

   if (stopped_)
      pingNonBlocking();

   stopped_ = false;

   ping_timer_->start(ping_interval);

   if (!transmitting_)
   {
      QString uid = getUID();

      if (uid != "")
      {
         QString tid = db_->oldest(uid);

         if (tid != "")
         {
            SSLTransmission t = db_->read(tid, uid);
            SSLTransmissionDatabaseClient::transmitNonBlocking(t);
            transmitting_ = true;
         }
      }
   }

   emit status_send(size());
}

// start fetch queue
void SSLTransmissionQueueClient::fetch()
{
   if (uploadMode_) throw e_;

   if (stopped_)
      pingNonBlocking();

   stopped_ = false;

   ping_timer_->start(ping_interval);

   if (!transmitting_)
   {
      QString uid = getUID();

      if (uid != "")
      {
         SSLTransmission t = SSLTransmission::ssl_respond(uid);
         SSLTransmissionDatabaseClient::transmitNonBlocking(t);
         transmitting_ = true;
      }
   }

   emit status_receive(total());
}

// start transmission queue
void SSLTransmissionQueueClient::start()
{
   if (uploadMode_)
      send();
   else
      fetch();
}

// stop transmission queue
void SSLTransmissionQueueClient::stop()
{
   stopped_ = true;

   ping_timer_->stop();
}

// is the queue empty?
bool SSLTransmissionQueueClient::empty()
{
   QString uid = getUID();

   if (uid != "")
      return(db_->oldest(uid) == "");

   return(true);
}

// queue size
int SSLTransmissionQueueClient::size()
{
   QString uid = getUID();

   if (uid != "")
      return(db_->list(uid).size());

   return(0);
}

// total store size
int SSLTransmissionQueueClient::total()
{
   return(db_->size());
}

// clear transmissions
void SSLTransmissionQueueClient::clear()
{
   if (uploadMode_)
   {
      QString uid = getUID();

      if (uid != "")
         db_->clear(uid);

      emit status_send(size());
   }
   else
   {
      db_->removeDB();

      emit status_receive(total());
   }
}

// ssl transmission pong
void SSLTransmissionQueueClient::pong(QString hostName, quint16 port, bool ack)
{
   if (ack)
      start();

   emit alive(hostName, port, ack);
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

   emit status_receive(total());

   if (!stopped_)
      fetch();
}

// specify transmission host name
void SSLTransmissionQueueClient::transmitHostName(QString hostName, quint16 port)
{
   if (uploadMode_)
      clear();

   SSLTransmissionDatabaseClient::transmitHostName(hostName, port);

   if (uploadMode_)
      emit status_send(size());
   else
      emit status_receive(total());
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
