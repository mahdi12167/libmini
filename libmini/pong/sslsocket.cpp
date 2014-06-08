#include <iostream>

#include "sslsocket.h"

SSLServer::SSLServer(QObject* parent)
   : QTcpServer(parent)
{
   serverSocket = new QSslSocket;
}

SSLServer::~SSLServer()
{
   delete serverSocket;
}

// start listening
void SSLServer::start(QString certPath, QString keyPath, quint16 port)
{
   listen(QHostAddress::Any, port);

   this->certPath = certPath;
   this->keyPath = keyPath;
}

// handle new incoming connection
void SSLServer::incomingConnection(int socketDescriptor)
{
   // pass the descriptor to the SSL socket
   if (serverSocket->setSocketDescriptor(socketDescriptor))
   {
      connect(serverSocket, SIGNAL(readyRead()),
              this, SLOT(startReading()));

      connect(serverSocket, SIGNAL(sslErrors(const QList &)),
              this, SLOT(errorOccured(const QList &)));

      serverSocket->setProtocol(QSsl::TlsV1);

      serverSocket->setPrivateKey(keyPath);
      serverSocket->setLocalCertificate(certPath);

      serverSocket->startServerEncryption();
   }
}

// start reading from the SSL socket after QSslSocket.readyRead() signal
void SSLServer::startReading()
{
   incomingData(serverSocket->readAll().constData());
}

// handle incoming data
void SSLServer::incomingData(const char *data)
{
   std::cout << data << std::endl;
}

// handle the signal of QSslSocket.sslErrors()
void SSLServer::errorOccured(const QList<QSslError> &)
{
   // simply ignore the errors
   serverSocket->ignoreSslErrors();
}

SSLClient::SSLClient(QObject* parent)
   : QObject(parent)
{
   connect(&clientSocket, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));

   connect(&clientSocket, SIGNAL(sslErrors(const QList<QSslError> &)),
           this, SLOT(errorOccured(const QList<QSslError> &)));
}

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
   // get the peer's certificate
   QSslCertificate cert = clientSocket.peerCertificate();

   // start writing to the socket
   startWriting();

   // close connection
   clientSocket.close();
}

// start writing through the established connection
void SSLClient::startWriting()
{
   char *data = outgoingData();

   // write on the SSL connection
   clientSocket.write(data, strlen(data));

   free(data);
}

// assemble outgoing data
char *SSLClient::outgoingData()
{
   char data[] = "hello";

   return(strdup(data));
}

// handle the signal of QSslSocket.sslErrors()
void SSLClient::errorOccured(const QList<QSslError> &)
{
   // simply ignore the errors
   clientSocket.ignoreSslErrors();
}
