// (c) by Stefan Roettger, licensed under GPL 3.0

#include <iostream>

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
   queue_.push(data);
}

// check for received transmission
bool SSLTransmissionServerConnectionFactory::hasTransmission()
{
   return(!queue_.empty());
}

// get received transmission
QByteArray SSLTransmissionServerConnectionFactory::getTransmission()
{
   QByteArray t;

   if (!queue_.empty())
   {
      t=queue_.front();
      queue_.pop();
   }

   return(t);
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
   // read data from the ssl socket
   QByteArray data = socket->readAll();

   // signal transmission of data chunk
   emit transmit(data);
}

SSLTransmissionClient::SSLTransmissionClient(QByteArray &data, QObject *parent)
   : SSLClient(parent), data_(data)
{}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   // write data to the ssl socket
   socket->write(data_, strlen(data_));
}
