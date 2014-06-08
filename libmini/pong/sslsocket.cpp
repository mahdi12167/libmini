#include <iostream>

#include "sslsocket.h"

// server ctor
SSLServer::SSLServer(QObject* parent)
   : QTcpServer(parent)
{}

// server dtor
SSLServer::~SSLServer()
{}

// start listening
void SSLServer::start(QString certPath, QString keyPath, quint16 port)
{
   bool listening = listen(QHostAddress::Any, port);

   if (listening)
      std::cout << "server " << serverAddress().toString().toStdString() << " is listening on " << port << std::endl;
   else
      std::cout << "server cannot listen" << std::endl;

   this->certPath = certPath;
   this->keyPath = keyPath;
}

// handle new incoming connection
void SSLServer::incomingConnection(int socketDescriptor)
{
   std::cout << "incoming connection" << std::endl;

   // create new self-terminating socket for each incoming connection
   ServerSocket *socket = new ServerSocket(this);

   // pass the descriptor to the SSL socket
   if (socket->setSocketDescriptor(socketDescriptor))
   {
      socket->setProtocol(QSsl::TlsV1);

      socket->setPrivateKey(keyPath);
      socket->setLocalCertificate(certPath);

      socket->startServerEncryption();

      std::cout << "encrypting connection" << std::endl;
   }
}

// socket ctor
ServerSocket::ServerSocket(QObject *parent)
   : QSslSocket(parent)
{
   connect(this, SIGNAL(readyRead()),
           this, SLOT(startReading()));

   connect(this, SIGNAL(sslErrors(const QList &)),
           this, SLOT(errorOccured(const QList &)));
}

// start reading from the SSL socket after QSslSocket.readyRead() signal
void ServerSocket::startReading()
{
   const char *data = readAll().constData();

   std::cout << "incoming: \"" << data << "\"" << std::endl;

   incomingData(data);
}

// handle incoming data
void ServerSocket::incomingData(const char *data)
{
   if (strcmp(data, "message") == 0)
      std::cout << "success" << std::endl;
}

// handle the signal of QSslSocket.sslErrors()
void ServerSocket::errorOccured(const QList<QSslError> &)
{
   std::cout << "ssl error" << std::endl;

   // simply ignore the errors
   ignoreSslErrors();
}

// client ctor
SSLClient::SSLClient(QObject* parent)
   : QObject(parent)
{
   connect(&clientSocket, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));

   connect(&clientSocket, SIGNAL(sslErrors(const QList<QSslError> &)),
           this, SLOT(errorOccured(const QList<QSslError> &)));
}

// client dtor
SSLClient::~SSLClient()
{}

// start transmission
void SSLClient::start(QString hostName, quint16 port)
{
   clientSocket.setProtocol(QSsl::TlsV1);
   clientSocket.connectToHostEncrypted(hostName, port);
}

// handle the signal of QSslSocket.encrypted()
void SSLClient::connectionEstablished()
{
   std::cout << "connection established" << std::endl;

   // get the peer's certificate
   QSslCertificate cert = clientSocket.peerCertificate();

   // start writing to the socket
   startWriting();

   // close connection
   clientSocket.close();

   std::cout << "connection closed" << std::endl;
}

// start writing through the established connection
void SSLClient::startWriting()
{
   char *data = outgoingData();

   std::cout << "outgoing: \"" << data << "\"" << std::endl;

   // write on the SSL connection
   clientSocket.write(data, strlen(data));

   free(data);
}

// assemble outgoing data
char *SSLClient::outgoingData()
{
   char data[] = "message";

   return(strdup(data));
}

// handle the signal of QSslSocket.sslErrors()
void SSLClient::errorOccured(const QList<QSslError> &)
{
   std::cout << "ssl error" << std::endl;

   // simply ignore the errors
   clientSocket.ignoreSslErrors();
}
