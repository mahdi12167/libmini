// (c) by Stefan Roettger, licensed under GPL 3.0

#include <iostream>

#include "sslsocket.h"

// ssl server ctor
SSLServer::SSLServer(QObject *parent)
   : QTcpServer(parent)
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
      new SSLServerConnection(socketDescriptor, certPath_, keyPath_, this);

   // initiate handshake
   connection->handshake();
}

// ssl server connection ctor
SSLServerConnection::SSLServerConnection(int socketDescriptor,
                                         QString certPath, QString keyPath,
                                         QObject *parent)
   : QObject(parent)
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

// start reading from an established connection
void SSLServerConnection::startReading(QSslSocket *socket)
{
   // read data data from the ssl socket
   QByteArray data = socket->readAll();
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
   if (verify) socket_.setPeerVerifyMode(QSslSocket::VerifyNone);
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

// start writing through an established connection
void SSLClient::startWriting(QSslSocket *socket)
{
   // write data to the ssl socket
   socket->write("ping", 4);
}
