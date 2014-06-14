// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QFile>

#include "sslsocket.h"

// ssl server ctor
SSLServer::SSLServer(SSLServerConnectionFactory *factory, QObject *parent)
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

// ssl test server connection factory ctor
SSLTestServerConnectionFactory::SSLTestServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{}

// create a new test server connection
SSLServerConnection *SSLTestServerConnectionFactory::create(int socketDescriptor,
                                                            QString certPath, QString keyPath,
                                                            QObject *parent)
{
   return(new SSLTestServerConnection(socketDescriptor, certPath, keyPath, this, parent));
}

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
   socket_ = new QSslSocket(this);
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
      // self-termination after socket has disappeared
      connect(socket_, SIGNAL(destroyed()),
              this, SLOT(deleteLater()));

      delete socket_;
   }
}

// ssl server connection dtor
SSLServerConnection::~SSLServerConnection()
{
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
   // start reading from the ssl socket
   startReading(socket_);
}

// catch socket errors
void SSLServerConnection::error(QAbstractSocket::SocketError socketError)
{
   emit report(socket_->errorString());
}

// ssl test server connection ctor
SSLTestServerConnection::SSLTestServerConnection(int socketDescriptor,
                                                 QString certPath, QString keyPath,
                                                 SSLServerConnectionFactory *factory,
                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{}

// start reading from an established connection
void SSLTestServerConnection::startReading(QSslSocket *socket)
{
   static const QByteArray test("test");

   // check if entire block has arrived
   if (socket->bytesAvailable() < test.size()) return;

   // read data from the ssl socket
   QByteArray data = socket->readAll();

   // test output
   std::cout << "incoming: \"" << QString(data).toStdString() << "\"" << std::endl;

   // check wether or not test was successful
   if (data != test) throw e_;
}

// ssl client ctor
SSLClient::SSLClient(QObject *parent)
   : QObject(parent),
     e_("client")
{
   // start writing after connection is encrypted
   connect(&socket_, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));
}

// ssl client dtor
SSLClient::~SSLClient()
{}

// start transmission
bool SSLClient::transmit(QString hostName, quint16 port, bool verify)
{
   try
   {
      socket_.setProtocol(QSsl::TlsV1);
      if (!verify) socket_.setPeerVerifyMode(QSslSocket::VerifyNone);
      socket_.connectToHostEncrypted(hostName, port);

      // catch socket errors
      connect(&socket_, SIGNAL(error(QAbstractSocket::SocketError)),
              this, SLOT(error(QAbstractSocket::SocketError)));

      socket_.waitForDisconnected(-1); // no time-out
   }
   catch (SSLError &e)
   {
      return(false);
   }

   return(true);
}

// start writing after connection is established
void SSLClient::connectionEstablished()
{
   // start writing to the ssl socket
   startWriting(&socket_);

   // disconnect the ssl socket
   socket_.disconnectFromHost();
}

// catch socket errors
void SSLClient::error(QAbstractSocket::SocketError socketError)
{
   SSLError e(socket_.errorString().toStdString());
   throw e;
}

// ssl test client ctor
SSLTestClient::SSLTestClient(QObject *parent)
   : SSLClient(parent)
{}

// start writing through an established connection
void SSLTestClient::startWriting(QSslSocket *socket)
{
   static const QByteArray test("test");

   // test output
   std::cout << "outgoing: \"" << QString(test).toStdString() << "\"" << std::endl;

   // write data to the ssl socket
   socket->write(test);
}
