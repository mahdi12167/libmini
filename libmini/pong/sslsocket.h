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

// ssl server socket class
class ServerSocket: public QSslSocket
{
   Q_OBJECT

public:

   ServerSocket(int socketDescriptor, QString certPath, QString keyPath, QObject *parent = NULL);
   virtual ~ServerSocket();

   // start ssl handshake
   void handshake();

protected:

   // handle incoming data
   void incomingData(const char *data, unsigned int size);

protected slots:

   // start reading after the connection is established
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

   // start writing after the connection is established
   void connectionEstablished();
};

#endif
