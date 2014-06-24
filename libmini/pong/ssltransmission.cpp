// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

// ssl transmission server connection factory ctor
SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(SSLTransmissionResponder *responder,
                                                                               QObject *parent)
   : SSLServerConnectionFactory(parent), responder_(responder)
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
           this, SLOT(receive(SSLTransmission)), Qt::QueuedConnection);

   return(connection);
}

// receiver of transmitted data blocks
void SSLTransmissionServerConnectionFactory::receive(SSLTransmission t)
{
   emit transmitted(t);
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
      else if (t_.getCommand() == SSLTransmission::cc_respond) emit respond(t_);
      else if (t_.getCommand() == SSLTransmission::cc_command) emit command(t_);
   }
   else emit invalid(t_);

   return(true);
}

// ssl transmission client ctor
SSLTransmissionClient::SSLTransmissionClient(QObject *parent)
   : SSLClient(parent)
{}

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

// start transmission
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
void SSLTransmissionClient::transmitNonBlocking(QString hostName, quint16 port, QString fileName, QString uid,
                                                bool verify, bool compress, SSLTransmission::CommandCode command,
                                                SSLTransmissionResponseReceiver *receiver)
{
   SSLTransmissionThread *thread;

   thread = new SSLTransmissionThread(hostName, port, fileName, uid, verify, compress, command);

   if (receiver != NULL)
      {
         // signal successful transmission
         connect(thread, SIGNAL(success(QString, quint16, QString, QString)),
                 receiver, SLOT(success(QString, quint16, QString, QString)), Qt::QueuedConnection);

         // signal unsuccessful transmission
         connect(thread, SIGNAL(failure(QString, quint16, QString, QString)),
                 receiver, SLOT(failure(QString, quint16, QString, QString)), Qt::QueuedConnection);

         // signal transmission response
         connect(thread, SIGNAL(response(SSLTransmission)),
                 receiver, SLOT(response(SSLTransmission)), Qt::QueuedConnection);

         // signal command result
         connect(thread, SIGNAL(result(SSLTransmission)),
                 receiver, SLOT(result(SSLTransmission)), Qt::QueuedConnection);
         }

   thread->start();
}

// ssl transmission thread ctor
SSLTransmissionThread::SSLTransmissionThread(QString hostName, quint16 port, QString fileName, QString uid,
                                             bool verify, bool compress, SSLTransmission::CommandCode command,
                                             QObject *parent)
   : QThread(parent), hostName_(hostName), port_(port), fileName_(fileName),
     uid_(uid), verify_(verify), compress_(compress), command_(command)
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
   bool ok;

   SSLTransmissionClient client;

   // receive response data block
   connect(&client, SIGNAL(response(SSLTransmission)),
           this, SLOT(receive_response(SSLTransmission)));

   // receive command result block
   connect(&client, SIGNAL(result(SSLTransmission)),
           this, SLOT(receive_result(SSLTransmission)));

   // transmit
   ok = client.transmit(hostName_, port_, fileName_, uid_, verify_, compress_, command_);

   // signal whether or not transmission was successful
   if (ok)
      emit success(hostName_, port_, fileName_, uid_);
   else
      emit failure(hostName_, port_, fileName_, uid_);
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

// successful transmission
void SSLTransmissionResponseReceiver::success(QString hostName, quint16 port, QString fileName, QString uid)
{
   onSuccess(hostName, port, fileName, uid);
}

// unsuccessful transmission
void SSLTransmissionResponseReceiver::failure(QString hostName, quint16 port, QString fileName, QString uid)
{
   onFailure(hostName, port, fileName, uid);
}

// transmission response
void SSLTransmissionResponseReceiver::response(SSLTransmission t)
{
   onResponse(t);
}

// transmission response
void SSLTransmissionResponseReceiver::result(SSLTransmission t)
{
   onResult(t);
}
