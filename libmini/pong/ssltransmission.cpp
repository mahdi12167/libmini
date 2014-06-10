// (c) by Stefan Roettger, licensed under GPL 3.0

#include <iostream>

#include "ssltransmission.h"

SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{}

// create a new transmission server connection
SSLServerConnection *SSLTransmissionServerConnectionFactory::create(int socketDescriptor,
                                                                    QString certPath, QString keyPath,
                                                                    QObject *parent)
{
   SSLTransmissionServerConnection *connection =
      new SSLTransmissionServerConnection(socketDescriptor, certPath, keyPath, this, parent);

   // signal transmission
   connect(connection, SIGNAL(transmit(QByteArray &data)),
           this, SLOT(receive(QByteArray &data)), Qt::QueuedConnection);

   return(connection);
}

// receiver of transmitted data chunks
void SSLTransmissionServerConnectionFactory::receive(QByteArray &data)
{
   // transmit output
   std::cout << "transmitted: \"" << QString(data).toStdString() << "\"" << std::endl;
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

SSLTransmissionClient::SSLTransmissionClient(QObject *parent)
   : SSLClient(parent)
{}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   static const char data[] = "transmission";

   // transmission output
   std::cout << "transmitting: \"" << data << "\"" << std::endl;

   // write data to the ssl socket
   socket->write(data, strlen(data));
}
