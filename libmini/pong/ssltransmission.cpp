// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{
   qRegisterMetaType<QByteArray>("QByteArray");
}

// create a new transmission server connection
SSLServerConnection *SSLTransmissionServerConnectionFactory::create(int socketDescriptor,
                                                                    QString certPath, QString keyPath,
                                                                    QObject *parent)
{
   SSLTransmissionServerConnection *connection =
      new SSLTransmissionServerConnection(socketDescriptor, certPath, keyPath, this, parent);

   // signal transmission
   connect(connection, SIGNAL(transmit(QByteArray)),
           this, SLOT(receive(QByteArray)), Qt::QueuedConnection);

   return(connection);
}

// receiver of transmitted data blocks
void SSLTransmissionServerConnectionFactory::receive(QByteArray data)
{
   emit transmitted(data);
   consume(data);
}

// consumer of transmitted data blocks
void SSLTransmissionServerConnectionFactory::consume(QByteArray data)
{}

SSLTransmissionServerConnection::SSLTransmissionServerConnection(int socketDescriptor,
                                                                 QString certPath, QString keyPath,
                                                                 SSLServerConnectionFactory *factory,
                                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent),
     transmitState_(true)
{}

// start reading from an established connection
void SSLTransmissionServerConnection::startReading(QSslSocket *socket)
{
   QDataStream in(socket);
   in.setVersion(QDataStream::Qt_4_0);

   if (transmitState_)
   {
      // check if entire header block has arrived
      if (socket->bytesAvailable() < (int)sizeof(header_)) return;

      // read data block size etc.
      in >> header_.size;
      in >> header_.compressed;

      transmitState_ = false;
   }

   // read data block from the ssl socket
   data_.append(socket->read(header_.size-data_.size()));

   // check if entire data block has arrived
   if (data_.size() < header_.size) return;

   // uncompress data block
   if (header_.compressed)
      data_ = qUncompress(data_);

   // signal transmission of data block
   emit transmit(data_);

   // disconnect the ssl socket
   socket->disconnect();
}

SSLTransmissionClient::SSLTransmissionClient(QObject *parent)
   : SSLClient(parent), compress_(false), compressed_(false)
{}

// enable compression
void SSLTransmissionClient::enableCompression(bool compress)
{
   compress_ = compress;
}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, QByteArray &data, bool verify)
{
   data_ = data;

   // compress upon request
   if (compress_)
      data_ = qCompress(data_, 3); // favor speed over compression ratio
   compressed_ = compress_;

   SSLClient::transmit(hostName, port, verify);

   return(true);
}

// start transmission
bool SSLTransmissionClient::transmitFile(QString hostName, quint16 port, QString fileName, bool verify)
{
   QByteArray data;
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
      return(false);

   data = file.readAll();

   return(transmit(hostName, port, data, verify));
}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   struct SSLTransmissionHeader header;

   QByteArray block;
   QDataStream out(&block, QIODevice::WriteOnly);

   // assemble header to contain data block size etc.
   header.size = data_.size();
   header.compressed = compressed_;

   // assemble header block
   out.setVersion(QDataStream::Qt_4_0);
   out << header.size;
   out << header.compressed;

   // write header block to the ssl socket
   socket->write(block);

   // write data block to the ssl socket
   socket->write(data_);

   // clear data block
   data_.clear();
}

// start non-blocking transmission
void SSLTransmissionClient::transmitFileNonBlocking(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
{
   SSLTransmissionThread::transmitFile(hostName, port, fileName, verify, compress);
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

   client.enableCompression(compress_);
   client.transmitFile(hostName_, port_, fileName_, verify_);
}

// non-blocking transmission
void SSLTransmissionThread::transmitFile(QString hostName, quint16 port, QString fileName, bool verify, bool compress)
{
   SSLTransmissionThread *thread;

   thread = new SSLTransmissionThread(hostName, port, fileName, verify, compress);
   thread->start();
}
