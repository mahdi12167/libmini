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

// receiver of transmitted data chunks
void SSLTransmissionServerConnectionFactory::receive(QByteArray data)
{
   emit transmitted(data);
}

SSLTransmissionServerConnection::SSLTransmissionServerConnection(int socketDescriptor,
                                                                 QString certPath, QString keyPath,
                                                                 SSLServerConnectionFactory *factory,
                                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{}

// start reading from an established connection
void SSLTransmissionServerConnection::startReading(QSslSocket *socket)
{
   qint64 block;

   QDataStream in(socket);
   in.setVersion(QDataStream::Qt_4_0);

   // check if header has arrived
   if (socket->bytesAvailable() < (int)sizeof(block)) return;

   // read data block size
   in >> block;

   // check if entire data block has arrived
   if (socket->bytesAvailable() < block) return;

   // read data from the ssl socket
   QByteArray data = socket->read(block);

   // signal transmission of data chunk
   emit transmit(data);
}

SSLTransmissionClient::SSLTransmissionClient(QByteArray &data, QObject *parent)
   : SSLClient(parent), data_(data)
{}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   QByteArray header;
   QDataStream out(&header, QIODevice::WriteOnly);

   // assemble header to contain data block length
   out.setVersion(QDataStream::Qt_4_0);
   out << (qint64)data_.size();

   // write header to the ssl socket
   socket->write(header, header.size());

   // write data block to the ssl socket
   socket->write(data_, data_.size());
}
