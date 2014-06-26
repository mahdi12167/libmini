// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QFile>

#include "sslsocket.h"

// ssl server ctor
SSLServer::SSLServer(SSLServerConnectionFactory *factory,
                     QObject *parent)
   : QTcpServer(parent),
     factory_(factory),
     certPath_(""), keyPath_(""),
     e_("server")
{}

// ssl server dtor
SSLServer::~SSLServer()
{}

// start listening
void SSLServer::start(QString certPath, QString keyPath, quint16 port, QString altPath)
{
   certPath_ = certPath;
   keyPath_ = keyPath;

   if (!QFile(certPath_).exists())
   {
      certPath_ = altPath+"/"+certPath;
      if (!QFile(certPath_).exists()) throw e_;
   }

   if (!QFile(keyPath_).exists())
   {
      keyPath_ = altPath+"/"+keyPath;
      if (!QFile(keyPath_).exists()) throw e_;
   }

   // listen on port
   if (!listen(QHostAddress::Any, port)) throw e_;
}

// handle new incoming connection
void SSLServer::incomingConnection(int socketDescriptor)
{
   // create new ssl server connection for each incoming connection
   SSLServerConnection *connection =
      factory_->create(socketDescriptor, certPath_, keyPath_, this);

   // initiate handshake
   connection->handshake();
}

// ssl server connection factory ctor
SSLServerConnectionFactory::SSLServerConnectionFactory(QObject *parent)
   : QObject(parent),
     e_("server connection factory")
{}

// ssl server connection factory dtor
SSLServerConnectionFactory::~SSLServerConnectionFactory()
{}

// receive error report
void SSLServerConnectionFactory::receiveReport(QString error)
{
   emit report(error);
}

// ssl server connection ctor
SSLServerConnection::SSLServerConnection(int socketDescriptor,
                                         QString certPath, QString keyPath,
                                         SSLServerConnectionFactory *factory,
                                         QObject *parent)
   : QObject(parent), factory_(factory),
     e_("server connection")
{
   // create new ssl socket for each incoming connection
   socket_ = new QSslSocket;

   // assign descriptor to ssl socket
   if (socket_->setSocketDescriptor(socketDescriptor))
   {
      // catch socket errors
      connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)),
              this, SLOT(error(QAbstractSocket::SocketError)));

      // connect error report signal
      connect(this, SIGNAL(report(QString)),
              factory_, SLOT(receiveReport(QString)));

      // configure ssl socket
      socket_->setProtocol(QSsl::TlsV1);
      socket_->setLocalCertificate(certPath);
      socket_->setPrivateKey(keyPath);

      // start reading from an established connection
      connect(socket_, SIGNAL(readyRead()),
              this, SLOT(startReading()));

      // self-termination after socket has disconnected
      connect(socket_, SIGNAL(disconnected()),
              this, SLOT(deleteLater()));
   }
   else
   {
      // self-termination
      deleteLater();
   }
}

// ssl server connection dtor
SSLServerConnection::~SSLServerConnection()
{
   // delete ssl socket
   delete socket_;
}

// start ssl handshake
void SSLServerConnection::handshake()
{
   socket_->startServerEncryption();
}

// start reading after connection is established
void SSLServerConnection::startReading()
{
   // start reading from ssl socket
   if (startReading(socket_))
   {
      // disconnect ssl socket
      socket_->disconnectFromHost();
   }
}

// catch socket errors
void SSLServerConnection::error(QAbstractSocket::SocketError)
{
   emit report(socket_->errorString());
}

// ssl client ctor
SSLClient::SSLClient(QObject *parent)
   : QThread(parent),
     e_("client")
{}

// ssl client dtor
SSLClient::~SSLClient()
{}

// start transmission
bool SSLClient::transmit(QString hostName, quint16 port, bool verify)
{
   if (hostName == "")
      return(false);

   hostName_ = hostName;
   port_ = port;
   verify_ = verify;

   // start thread
   success_ = false;
   start();

   // wait until thread has finished
   wait();

   return(success_);
}

// thread run method
void SSLClient::run()
{
   // create new ssl socket for each outgoing transmission
   socket_ = new QSslSocket;

   // configure ssl socket
   socket_->setProtocol(QSsl::TlsV1);
   if (!verify_) socket_->setPeerVerifyMode(QSslSocket::VerifyNone);

   // connect ssl socket
   socket_->connectToHostEncrypted(hostName_, port_);

   // wait for encryption to be established
   if (socket_->waitForEncrypted())
      success_ = startWriting();

   // delete ssl socket
   delete socket_;
}

// start writing after connection is established
bool SSLClient::startWriting()
{
   bool success;

   // start writing to ssl socket
   success = startWriting(socket_);

   // disconnect ssl socket
   socket_->disconnectFromHost();

   // wait until ssl socket has disconnected
   if (socket_->state() != QTcpSocket::UnconnectedState)
      if (!socket_->waitForDisconnected())
         success = false;

   return(success);
}

// ssl test server connection factory ctor
SSLTestServerConnectionFactory::SSLTestServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{}

// ssl test server connection factory dtor
SSLTestServerConnectionFactory::~SSLTestServerConnectionFactory()
{}

// create a new test server connection
SSLServerConnection *SSLTestServerConnectionFactory::create(int socketDescriptor,
                                                            QString certPath, QString keyPath,
                                                            QObject *parent)
{
   return(new SSLTestServerConnection(socketDescriptor, certPath, keyPath, this, parent));
}

// ssl test server connection ctor
SSLTestServerConnection::SSLTestServerConnection(int socketDescriptor,
                                                 QString certPath, QString keyPath,
                                                 SSLServerConnectionFactory *factory,
                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{}

// ssl test server connection dtor
SSLTestServerConnection::~SSLTestServerConnection()
{}

// start reading from ssl socket
bool SSLTestServerConnection::startReading(QSslSocket *socket)
{
   static const QByteArray test("test");

   // check if entire data block has arrived
   if (socket->bytesAvailable() < test.size()) return(false);

   // read data block from ssl socket
   QByteArray data = socket->readAll();

   // test output
   std::cout << "incoming: \"" << QString(data).toStdString() << "\"" << std::endl;

   // check wether or not test was successful
   if (data != test) throw e_;

   return(true);
}

// ssl test client ctor
SSLTestClient::SSLTestClient(QObject *parent)
   : SSLClient(parent)
{}

// ssl test client dtor
SSLTestClient::~SSLTestClient()
{}

// start writing to ssl socket
bool SSLTestClient::startWriting(QSslSocket *socket)
{
   static const QByteArray test("test");

   // test output
   std::cout << "outgoing: \"" << QString(test).toStdString() << "\"" << std::endl;

   // write data block to ssl socket
   socket->write(test);

   // wait until entire data block has been written
   return(socket->waitForBytesWritten());
}
