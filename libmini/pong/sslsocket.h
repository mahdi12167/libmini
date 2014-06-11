// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <string>
#include <exception>
#include <iostream>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>

class SSLServerConnectionFactory;

// ssl error class
class SSLError: public std::exception
{
public:

   SSLError(const std::string type="") throw() : std::exception(), type_(type) {}
   virtual ~SSLError() throw() {}

protected:

   virtual std::string what()
   {
      if (type_.empty())
         return("ssl socket error");
      else
         return("ssl socket error: "+type_);
   }

   std::string type_;
};

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(SSLServerConnectionFactory *factory, QObject *parent = NULL);
   virtual ~SSLServer();

   // start listening
   void start(QString certPath, QString keyPath, quint16 port = 0);

protected:

   // handle new incoming connection
   void incomingConnection(int socketDescriptor);

   SSLServerConnectionFactory *factory_;

   QString certPath_;
   QString keyPath_;

   SSLError e_;
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

   SSLServerConnectionFactory *factory_;

private:

   QSslSocket *socket_;

protected:

   SSLError e_;

protected slots:

   // start reading after connection is established
   void startReading();

   // catch socket errors
   void error(QAbstractSocket::SocketError socketError);
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

protected:

   SSLError e_;
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
   void start(QString hostName, quint16 port, bool verify=true);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket) = 0;

private:

   QSslSocket socket_;

protected:

   SSLError e_;

protected slots:

   // start writing after connection is established
   void connectionEstablished();

   // catch socket errors
   void error(QAbstractSocket::SocketError socketError);
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
