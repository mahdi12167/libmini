// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{
   qRegisterMetaType<SSLTransmission>("SSLTransmission");
}

// create a new transmission server connection
SSLServerConnection *SSLTransmissionServerConnectionFactory::create(int socketDescriptor,
                                                                    QString certPath, QString keyPath,
                                                                    QObject *parent)
{
   SSLTransmissionServerConnection *connection =
      new SSLTransmissionServerConnection(socketDescriptor, certPath, keyPath, this, parent);

   // signal transmission
   connect(connection, SIGNAL(transmit(SSLTransmission)),
           this, SLOT(receive(SSLTransmission)), Qt::QueuedConnection);

   return(connection);
}

// receiver of transmitted data blocks
void SSLTransmissionServerConnectionFactory::receive(SSLTransmission t)
{
   emit transmitted(t);
   consume(t);
}

// consumer of transmitted data blocks
void SSLTransmissionServerConnectionFactory::consume(SSLTransmission &)
{}

SSLTransmissionServerConnection::SSLTransmissionServerConnection(int socketDescriptor,
                                                                 QString certPath, QString keyPath,
                                                                 SSLServerConnectionFactory *factory,
                                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{}

// start reading from an established connection
void SSLTransmissionServerConnection::startReading(QSslSocket *socket)
{
   // read from the ssl socket
   if (!t_.read(socket)) return;

   // signal transmission of data block
   emit transmit(t_);

   // disconnect the ssl socket
   socket->disconnect();
}

SSLTransmissionClient::SSLTransmissionClient(QObject *parent)
   : SSLClient(parent)
{}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify)
{
   t_ = t;

   return(SSLClient::transmit(hostName, port, verify));
}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
      return(false);

   SSLTransmission t(file);

   if (compress)
      t.compress();

   return(transmit(hostName, port, t, verify));
}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   // write to the ssl socket
   t_.write(socket);
}

// start non-blocking transmission
void SSLTransmissionClient::transmitNonBlocking(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
{
   SSLTransmissionThread::transmit(hostName, port, fileName, verify, compress);
}

SSLTransmissionThread::SSLTransmissionThread(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
   : QThread(), hostName_(hostName), port_(port), fileName_(fileName), verify_(verify), compress_(compress)
{
   // self-termination after thread has finished
   connect(this, SIGNAL(finished()),
           this, SLOT(deleteLater()));
}

SSLTransmissionThread::~SSLTransmissionThread()
{}

// thread run method
void SSLTransmissionThread::run()
{
   SSLTransmissionClient client;

   client.transmit(hostName_, port_, fileName_, verify_, compress_);
}

// non-blocking transmission
void SSLTransmissionThread::transmit(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
{
   SSLTransmissionThread *thread;

   thread = new SSLTransmissionThread(hostName, port, fileName, verify, compress);
   thread->start();
}
