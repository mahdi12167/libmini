// (c) by Stefan Roettger, licensed under GPL 3.0

#include <iostream>

#include "sslsocket.h"

// ssl server ctor
SSLServer::SSLServer(SSLServerConnectionFactory *factory, QObject *parent)
   : QTcpServer(parent),
     factory_(factory),
     certPath_(""), keyPath_("")
{}

// ssl server dtor
SSLServer::~SSLServer()
{}

// start listening
bool SSLServer::start(QString certPath, QString keyPath, quint16 port)
{
   certPath_ = certPath;
   keyPath_ = keyPath;

   return(listen(QHostAddress::Any, port));
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
   : QObject(parent)
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

// ssl server connection ctor
SSLServerConnection::SSLServerConnection(int socketDescriptor,
                                         QString certPath, QString keyPath,
                                         SSLServerConnectionFactory *factory,
                                         QObject *parent)
   : QObject(parent), factory_(factory)
{
   // create new ssl socket for each incoming connection
   socket_ = new QSslSocket(this);
   socket_->setSocketDescriptor(socketDescriptor);

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
   startReading(socket_);
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
   // read data from the ssl socket
   QByteArray data = socket->readAll();

   // test output
   std::cout << "incoming: \"" << QString(data).toStdString() << "\"" << std::endl;
}

// ssl client ctor
SSLClient::SSLClient(QObject *parent)
   : QObject(parent)
{
   // start writing after connection is encrypted
   connect(&socket_, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));
}

// ssl client dtor
SSLClient::~SSLClient()
{}

// start transmission
bool SSLClient::start(QString hostName, quint16 port, bool verify)
{
   socket_.setProtocol(QSsl::TlsV1);
   if (!verify) socket_.setPeerVerifyMode(QSslSocket::VerifyNone);
   socket_.connectToHostEncrypted(hostName, port);

   return(!socket_.waitForDisconnected());
}

// start writing after connection is established
void SSLClient::connectionEstablished()
{
   // start writing to the ssl socket
   startWriting(&socket_);

   // close connection
   socket_.flush();
   socket_.close();
}

// ssl test client ctor
SSLTestClient::SSLTestClient(QObject *parent)
   : SSLClient(parent)
{}

// start writing through an established connection
void SSLTestClient::startWriting(QSslSocket *socket)
{
   static const char data[] = "test";

   // test output
   std::cout << "outgoing: \"" << data << "\"" << std::endl;

   // write data to the ssl socket
   socket->write(data, strlen(data));
}
