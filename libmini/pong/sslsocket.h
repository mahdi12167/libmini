// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QTcpServer>
#include <QSslSocket>

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(QObject *parent = NULL);
   virtual ~SSLServer();

   // start listening
   void start(QString certPath, QString keyPath, quint16 port = 0);

protected:

   // handle new incoming connection
   virtual void incomingConnection(int socketDescriptor);

   QString certPath_;
   QString keyPath_;
};

// ssl server connection class
class SSLServerConnection: public QObject
{
   Q_OBJECT

public:

   SSLServerConnection(int socketDescriptor, QString certPath, QString keyPath, QObject *parent = NULL);
   virtual ~SSLServerConnection();

   // start ssl handshake
   void handshake();

protected:

   // handle incoming data
   void incomingData(const char *data, unsigned int size);

   QSslSocket *socket_;

protected slots:

   // connection established
   void connectionEstablished();

   // start reading after connection is established
   void startReading();
};

// ssl client class
class SSLClient: public QObject
{
   Q_OBJECT

public:

   SSLClient(QObject *parent = NULL);
   virtual ~SSLClient();

   // start transmission
   void start(QString hostName, quint16 port);

protected:

   // start writing through an established connection
   void startWriting();

   // assemble outgoing data
   char *outgoingData();

   QSslSocket socket_;

protected slots:

   // start writing after connection is established
   void connectionEstablished();
};

#endif
