// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QTcpServer>
#include <QSslSocket>

class SSLServerConnectionFactory;

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(SSLServerConnectionFactory *factory, QObject *parent = NULL);
   virtual ~SSLServer();

   // start listening
   bool start(QString certPath, QString keyPath, quint16 port = 0);

protected:

   // handle new incoming connection
   void incomingConnection(int socketDescriptor);

   SSLServerConnectionFactory *factory_;

   QString certPath_;
   QString keyPath_;
};

// ssl server connection base class
class SSLServerConnection: public QObject
{
   Q_OBJECT

public:

   SSLServerConnection(int socketDescriptor,
                       QString certPath, QString keyPath,
                       SSLServerConnectionFactory *factory,
                       QObject *parent = NULL);

   virtual ~SSLServerConnection();

   // start ssl handshake
   void handshake();

protected:

   // start reading from an established connection
   virtual void startReading(QSslSocket *socket) = 0;

private:

   SSLServerConnectionFactory *factory_;
   QSslSocket *socket_;

protected slots:

   // start reading after connection is established
   void startReading();
};

// ssl test server connection class
class SSLTestServerConnection: public SSLServerConnection
{
   Q_OBJECT

public:

   SSLTestServerConnection(int socketDescriptor,
                           QString certPath, QString keyPath,
                           SSLServerConnectionFactory *factory,
                           QObject *parent = NULL);

protected:

   // start reading from an established connection
   virtual void startReading(QSslSocket *socket);
};

// ssl server connection factory base class
class SSLServerConnectionFactory: public QObject
{
   Q_OBJECT

public:

   SSLServerConnectionFactory(QObject *parent = NULL);
   virtual ~SSLServerConnectionFactory();

   // create a new server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent) = 0;

};

// ssl test server connection factory class
class SSLTestServerConnectionFactory: public SSLServerConnectionFactory
{
public:

   SSLTestServerConnectionFactory(QObject *parent = NULL);

   // create a new test server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent);

};

// ssl client base class
class SSLClient: public QObject
{
   Q_OBJECT

public:

   SSLClient(QObject *parent = NULL);
   virtual ~SSLClient();

   // start transmission
   bool start(QString hostName, quint16 port, bool verify=true);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket) = 0;

private:

   QSslSocket socket_;

protected slots:

   // start writing after connection is established
   void connectionEstablished();
};

// ssl test client class
class SSLTestClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTestClient(QObject *parent = NULL);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket);
};

#endif
