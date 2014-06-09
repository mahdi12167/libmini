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
void SSLServer::start(QString certPath, QString keyPath, quint16 port)
{
   certPath_ = certPath;
   keyPath_ = keyPath;

   bool listening = listen(QHostAddress::Any, port);

   if (listening)
      std::cout << "server " << serverAddress().toString().toStdString() << " is listening on " << serverPort() << std::endl;
   else
      std::cout << "server is unable to listen" << std::endl;
}

// handle new incoming connection
void SSLServer::incomingConnection(int socketDescriptor)
{
   std::cout << "incoming connection" << std::endl;

   // create new ssl server connection for each incoming connection
   SSLServerConnection *connection = new SSLServerConnection(socketDescriptor, certPath_, keyPath_, this);

   // initiate handshake
   connection->handshake();

   std::cout << "serving connection" << std::endl;
}

// ssl server connection ctor
SSLServerConnection::SSLServerConnection(int socketDescriptor, QString certPath, QString keyPath, QObject *parent)
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

// start reading after the connection is established
void SSLServerConnection::startReading()
{
   QByteArray data = socket_->readAll();

   std::cout << "incoming: \"" << QString(data).toStdString() << "\"" << std::endl;

   incomingData(data.constData(),data.size());
}

// handle incoming data
void SSLServerConnection::incomingData(const char *data,unsigned int size)
{
   static const char message[] = "message";

   if (size == strlen(message))
      if (strncmp(data, message, strlen(message)) == 0)
         std::cout << "success" << std::endl;
}

// ssl client ctor
SSLClient::SSLClient(QObject *parent)
   : QObject(parent)
{
   // start writing after the connection is encrypted
   connect(&socket_, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));
}

// ssl client dtor
SSLClient::~SSLClient()
{}

// start transmission
void SSLClient::start(QString hostName, quint16 port)
{
   socket_.setProtocol(QSsl::TlsV1);
   socket_.setPeerVerifyMode(QSslSocket::VerifyNone);
   socket_.connectToHostEncrypted(hostName, port);
   socket_.waitForDisconnected();
}

// start writing after the connection is established
void SSLClient::connectionEstablished()
{
   std::cout << "connection established" << std::endl;

   // start writing to the ssl socket
   startWriting();

   // close connection
   socket_.flush();
   socket_.close();

   std::cout << "connection closed" << std::endl;
}

// start writing through an established connection
void SSLClient::startWriting()
{
   char *data = outgoingData();

   std::cout << "outgoing: \"" << data << "\"" << std::endl;

   // write data to the ssl socket
   socket_.write(data, strlen(data));

   free(data);
}

// assemble outgoing data
char *SSLClient::outgoingData()
{
   static const char data[] = "message";

   return(strdup(data));
}
