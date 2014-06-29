// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

// ssl transmission responder ctor
SSLTransmissionResponder::SSLTransmissionResponder()
{}

// ssl transmission responder dtor
SSLTransmissionResponder::~SSLTransmissionResponder()
{}

// ssl transmission server connection factory ctor
SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(SSLTransmissionResponder *responder,
                                                                               QObject *parent)
   : SSLServerConnectionFactory(parent),
     responder_(responder)
{
   qRegisterMetaType<SSLTransmission>("SSLTransmission");
}

// ssl transmission server connection factory dtor
SSLTransmissionServerConnectionFactory::~SSLTransmissionServerConnectionFactory()
{}

// create a new transmission server connection
SSLServerConnection *SSLTransmissionServerConnectionFactory::create(int socketDescriptor,
                                                                    QString certPath, QString keyPath,
                                                                    QObject *parent)
{
   SSLTransmissionServerConnection *connection =
      new SSLTransmissionServerConnection(socketDescriptor, certPath, keyPath, this, responder_, parent);

   // signal transmission
   connect(connection, SIGNAL(transmit(SSLTransmission)),
           this, SLOT(transmit(SSLTransmission)), Qt::QueuedConnection);

   // signal command
   connect(connection, SIGNAL(command(SSLTransmission)),
           this, SLOT(command(SSLTransmission)), Qt::QueuedConnection);

   // signal error
   connect(connection, SIGNAL(invalid(SSLTransmission)),
           this, SLOT(invalid(SSLTransmission)), Qt::QueuedConnection);

   return(connection);
}

// get responder
SSLTransmissionResponder *SSLTransmissionServerConnectionFactory::getResponder() const
{
   return(responder_);
}

// receiver of transmitted data blocks
void SSLTransmissionServerConnectionFactory::transmit(SSLTransmission t)
{
   emit transmitted(t);
}

// receiver of command data blocks
void SSLTransmissionServerConnectionFactory::command(SSLTransmission t)
{
   emit execute(t);
}

// receiver of invalid data blocks
void SSLTransmissionServerConnectionFactory::invalid(SSLTransmission t)
{
   emit error(t);
}

// ssl transmission server connection ctor
SSLTransmissionServerConnection::SSLTransmissionServerConnection(int socketDescriptor,
                                                                 QString certPath, QString keyPath,
                                                                 SSLServerConnectionFactory *factory,
                                                                 SSLTransmissionResponder *responder,
                                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{
   t_.setResponder(responder);
}

// ssl transmission server connection dtor
SSLTransmissionServerConnection::~SSLTransmissionServerConnection()
{}

// start reading from ssl socket
bool SSLTransmissionServerConnection::startReading(QSslSocket *socket)
{
   // read from the ssl socket
   if (!t_.read(socket)) return(false);

   // signal transmission of data block
   if (t_.valid())
   {
      if (t_.getCommand() == SSLTransmission::cc_transmit) emit transmit(t_);
      else if (t_.getCommand() == SSLTransmission::cc_command) emit command(t_);
   }
   else emit invalid(t_);

   return(true);
}

// ssl transmission client ctor
SSLTransmissionClient::SSLTransmissionClient(SSLTransmissionResponseReceiver *receiver,
                                             QObject *parent)
   : SSLClient(parent),
     receiver_(receiver)
{
   qRegisterMetaType<SSLTransmission>("SSLTransmission");
}

// ssl transmission client dtor
SSLTransmissionClient::~SSLTransmissionClient()
{}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify)
{
   bool success;

   t_ = t;

   // initiate transmission
   success = SSLClient::transmit(hostName, port, verify);

   // check for available response
   if (success)
      if (t_.getResponse())
         if (t_.getCommand() == SSLTransmission::cc_response)
            emit response(*(t_.getResponse()));
         else if (t_.getCommand() == SSLTransmission::cc_result)
            emit result(*(t_.getResponse()));

   return(success);
}

// start file transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, QString fileName, QString uid,
                                     bool verify, bool compress, SSLTransmission::CommandCode command)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
      return(false);

   SSLTransmission t(file, uid, command);

   if (compress)
      t.compress();

   return(transmit(hostName, port, t, verify));
}

// get transmission receiver
SSLTransmissionResponseReceiver *SSLTransmissionClient::getReceiver() const
{
   return(receiver_);
}

// get transmission response
SSLTransmission *SSLTransmissionClient::getResponse() const
{
   return(t_.getResponse());
}

// start writing to ssl socket
bool SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   // write to the ssl socket
   return(t_.write(socket));
}

// start non-blocking transmission
void SSLTransmissionClient::transmitNonBlocking(QString hostName, quint16 port, const SSLTransmission &t, bool verify)
{
   SSLTransmissionThread *thread;

   thread = new SSLTransmissionThread(hostName, port, t, verify);

   if (receiver_)
   {
      // signal successful transmission
      connect(thread, SIGNAL(success(QString, quint16, QString, QString)),
              receiver_, SLOT(success(QString, quint16, QString, QString)), Qt::QueuedConnection);

      // signal unsuccessful transmission
      connect(thread, SIGNAL(failure(QString, quint16, QString, QString)),
              receiver_, SLOT(failure(QString, quint16, QString, QString)), Qt::QueuedConnection);

      // signal transmission response
      connect(thread, SIGNAL(response(SSLTransmission)),
              receiver_, SLOT(response(SSLTransmission)), Qt::QueuedConnection);

      // signal command result
      connect(thread, SIGNAL(result(SSLTransmission)),
              receiver_, SLOT(result(SSLTransmission)), Qt::QueuedConnection);
   }

   thread->start();
}

// start non-blocking file transmission
void SSLTransmissionClient::transmitNonBlocking(QString hostName, quint16 port, QString fileName, QString uid,
                                                bool verify, bool compress, SSLTransmission::CommandCode command)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
      return;

   SSLTransmission t(file, uid, command);

   if (compress)
      t.compress();

   transmitNonBlocking(hostName, port, t, verify);
}

// ssl transmission thread ctor
SSLTransmissionThread::SSLTransmissionThread(QString hostName, quint16 port, const SSLTransmission &t, bool verify,
                                             QObject *parent)
   : QThread(parent),
     hostName_(hostName), port_(port), t_(t), verify_(verify)
{
   // self-termination after thread has finished
   connect(this, SIGNAL(finished()),
           this, SLOT(deleteLater()));
}

// ssl transmission thread dtor
SSLTransmissionThread::~SSLTransmissionThread()
{}

// thread run method
void SSLTransmissionThread::run()
{
   bool done;

   SSLTransmissionClient client;

   // receive response data block
   connect(&client, SIGNAL(response(SSLTransmission)),
           this, SLOT(receive_response(SSLTransmission)));

   // receive command result block
   connect(&client, SIGNAL(result(SSLTransmission)),
           this, SLOT(receive_result(SSLTransmission)));

   // transmit
   done = client.transmit(hostName_, port_, t_, verify_);

   // signal whether or not transmission was successful
   if (done)
      emit success(hostName_, port_, t_.getTID(), t_.getUID());
   else
      emit failure(hostName_, port_, t_.getTID(), t_.getUID());
}

// receive ssl transmission response
void SSLTransmissionThread::receive_response(SSLTransmission t)
{
   emit response(t);
}

// receive ssl command result
void SSLTransmissionThread::receive_result(SSLTransmission t)
{
   emit result(t);
}

// ssl transmission response receiver ctor
SSLTransmissionResponseReceiver::SSLTransmissionResponseReceiver(QObject *parent)
   : QObject(parent)
{}

// ssl transmission response receiver dtor
SSLTransmissionResponseReceiver::~SSLTransmissionResponseReceiver()
{}

// ssl transmission success
void SSLTransmissionResponseReceiver::success(QString hostName, quint16 port, QString tid, QString uid)
{
   emit onSuccess(hostName, port, tid, uid);
}

// ssl transmission failure
void SSLTransmissionResponseReceiver::failure(QString hostName, quint16 port, QString tid, QString uid)
{
   emit onFailure(hostName, port, tid, uid);
}

// ssl transmission response
void SSLTransmissionResponseReceiver::response(SSLTransmission t)
{
   emit onResponse(t);
}

// ssl transmission result
void SSLTransmissionResponseReceiver::result(SSLTransmission t)
{
   emit onResult(t);
}
