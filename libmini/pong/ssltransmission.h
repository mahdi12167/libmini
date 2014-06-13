// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLTRANSMISSION_H
#define SSLTRANSMISSION_H

#include <QFile>
#include <QThread>

#include "sslsocket.h"

// ssl transmission header structure
struct SSLTransmissionHeader
{
   qint64 size;
   qint8 compressed;
};

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

   // consumer of transmitted data blocks
   virtual void consume(QByteArray data);

public slots:

   // receiver of transmitted data blocks
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

   bool state_;
   struct SSLTransmissionHeader header_;

signals:

   // signal transmission of data block
   void transmit(QByteArray);
};

// ssl transmission client class
class SSLTransmissionClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTransmissionClient(QObject *parent = NULL);

   // start transmission
   bool transmit(QString hostName, quint16 port, QByteArray &data, bool verify=true);
   bool transmitFile(QString hostName, quint16 port, QString fileName, bool verify=true);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket, bool compress=false);

   QByteArray data_;

public slots:

   // start non-blocking transmission
   void transmitFileNonBlocking(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);
};

// ssl transmission thread class
class SSLTransmissionThread: public QThread
{
   Q_OBJECT

public:

   SSLTransmissionThread(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);

   virtual ~SSLTransmissionThread();

   // non-blocking transmission
   static void transmitFile(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);

protected:

   virtual void run();

   QString hostName_;
   quint16 port_;
   QString fileName_;
   bool verify_;
   bool compress_;
};

#endif
