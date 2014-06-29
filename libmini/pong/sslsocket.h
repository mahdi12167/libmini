// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <string>
#include <exception>
#include <iostream>

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslSocket>
#include <QThread>

class SSLServerConnectionFactory;

// ssl error class
class SSLError: public std::exception
{
public:

   SSLError(const std::string type="") throw() : std::exception(), type_(type) {}
   virtual ~SSLError() throw() {}

   virtual const char *what() const throw()
   {
      if (type_.empty())
         return("ssl socket error");
      else
         return(("ssl socket error: "+type_).c_str());
   }

protected:

   std::string type_;
};

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(SSLServerConnectionFactory *factory,
             QObject *parent = NULL);

   virtual ~SSLServer();

   // start listening
   void start(QString certPath, QString keyPath, quint16 port = 0, QString altPath = "");

   // get factory
   SSLServerConnectionFactory *getFactory() const;

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

   // start reading from ssl socket
   virtual bool startReading(QSslSocket *socket) = 0;

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

signals:

   // report errors
   void report(QString);
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

public slots:

   // receive error report
   void receiveReport(QString);

signals:

   // report errors
   void report(QString);
};

// ssl client base class
class SSLClient: public QThread
{
   Q_OBJECT

public:

   SSLClient(QObject *parent = NULL);
   virtual ~SSLClient();

   // start transmission
   bool transmit(QString hostName, quint16 port, bool verify=true);

protected:

   // thread run method
   virtual void run();

   // start writing after connection is established
   bool startWriting();

   // start writing to ssl socket
   virtual bool startWriting(QSslSocket *socket) = 0;

private:

   QSslSocket *socket_;

   QString hostName_;
   quint16 port_;
   bool verify_;

   bool success_;

protected:

   SSLError e_;
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

   virtual ~SSLTestServerConnection();

protected:

   // start reading from ssl socket
   virtual bool startReading(QSslSocket *socket);
};

// ssl test server connection factory class
class SSLTestServerConnectionFactory: public SSLServerConnectionFactory
{
public:

   SSLTestServerConnectionFactory(QObject *parent = NULL);
   virtual ~SSLTestServerConnectionFactory();

   // create a new test server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent);

};

// ssl test client class
class SSLTestClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTestClient(QObject *parent = NULL);
   virtual ~SSLTestClient();

protected:

   // start writing to ssl socket
   virtual bool startWriting(QSslSocket *socket);
};

#endif
