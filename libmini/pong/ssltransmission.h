// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLTRANSMISSION_H
#define SSLTRANSMISSION_H

#include "sslsocket.h"

// ssl transmission server connection factory class
class SSLTransmissionServerConnectionFactory: public SSLServerConnectionFactory
{
   Q_OBJECT

public:

   SSLTransmissionServerConnectionFactory(QObject *parent = NULL);

   // create a new transmission server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent);

protected:

public slots:

   // receiver of transmitted data chunks
   void receive(QByteArray);

signals:

   // signal transmission
   void transmitted(QByteArray);
};

// ssl transmission server connection class
class SSLTransmissionServerConnection: public SSLServerConnection
{
   Q_OBJECT

public:

   SSLTransmissionServerConnection(int socketDescriptor,
                                   QString certPath, QString keyPath,
                                   SSLServerConnectionFactory *factory,
                                   QObject *parent = NULL);

protected:

   // start reading from an established connection
   virtual void startReading(QSslSocket *socket);

signals:

   // signal transmission of data chunk
   void transmit(QByteArray);
};

// ssl transmission client class
class SSLTransmissionClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTransmissionClient(QObject *parent = NULL);

   // start transmission
   void transmit(QString hostName, quint16 port, QByteArray &data, bool verify=true);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket);

   QByteArray data_;
};

#endif
