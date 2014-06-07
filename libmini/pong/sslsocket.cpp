#include "sslsocket.h"

SSLServer::SSLServer(QObject* parent)
   : QTcpServer(parent)
{
   serverSocket = NULL;
}

SSLServer::~SSLServer()
{
   if (serverSocket)
      delete serverSocket;
}

void SSLServer::start(QString certPath, QString keyPath, quint16 port)
{
   listen(QHostAddress::Any, port);

   this->certPath = certPath;
   this->keyPath = keyPath;
}

void SSLServer::readyToRead()
{
   qDebug() << serverSocket->readAll();
}

void SSLServer::incomingConnection(int socketDescriptor)
{
   if (!serverSocket)
   {
      serverSocket = new QSslSocket;

      if (serverSocket->setSocketDescriptor(socketDescriptor))
      {
         connect(serverSocket, SIGNAL(readyRead()),
                 this, SLOT(readyToRead()));

         connect(serverSocket, SIGNAL(sslErrors(const QList &)),
                 this, SLOT(errorOccured(const QList &)));

         serverSocket->setProtocol(QSsl::TlsV1);
         serverSocket->setPrivateKey(keyPath);
         serverSocket->setLocalCertificate(certPath);
         serverSocket->startServerEncryption();
      }
      else
      {
         delete serverSocket;
      }
   }
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

   // write on the SSL connection
   clientSocket.write("hello, client\n", 14);
}

// handle the signal of QSslSocket.sslErrors()
void SSLClient::errorOccured(const QList<QSslError> &)
{
   // simply ignore the errors
   clientSocket.ignoreSslErrors();
}
