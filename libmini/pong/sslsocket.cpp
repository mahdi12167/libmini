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
      std::cout << "server " << serverAddress().toString().toStdString() << " is listening on " << serverPort() << std::endl;
   else
      std::cout << "server cannot listen" << std::endl;

   certPath_ = certPath;
   keyPath_ = keyPath;
}

// handle new incoming connection
void SSLServer::incomingConnection(int socketDescriptor)
{
   std::cout << "incoming connection" << std::endl;

   // create new ssl socket for each incoming connection
   QSslSocket *socket = new QSslSocket(this);
   socket->setProtocol(QSsl::TlsV1);
   socket->setLocalCertificate(certPath_);
   socket->setPrivateKey(keyPath_);

   // create new self-terminating thread for each incoming connection
   if (socket->setSocketDescriptor(socketDescriptor))
   {
      ServerThread *thread = new ServerThread(socket, this);
      thread->start();

      std::cout << "serving connection" << std::endl;
   }
   else
   {
      std::cout << "connection disconnected" << std::endl;

      delete socket;
   }
}

// server thread ctor
ServerThread::ServerThread(QSslSocket *socket, QObject *parent)
   : QThread(parent)
{
   socket_ = socket;

   connect(socket, SIGNAL(readyRead()),
           this, SLOT(startReading()));

   connect(this, SIGNAL(finished()),
           this, SLOT(deleteAfter()));

   connect(this, SIGNAL(sslErrors(const QList &)),
           this, SLOT(errorOccured(const QList &)));
}

// server thread dtor
ServerThread::~ServerThread()
{
   delete socket_;
}

// server thread run method
void ServerThread::run()
{
   socket_->startServerEncryption();
   socket_->waitForDisconnected();
}

// start reading from the SSL socket after QSslSocket.readyRead() signal
void ServerThread::startReading()
{
   const char *data = socket_->readAll().constData();

   std::cout << "incoming: \"" << data << "\"" << std::endl;

   incomingData(data);
}

// handle incoming data
void ServerThread::incomingData(const char *data)
{
   if (strcmp(data, "message") == 0)
      std::cout << "success" << std::endl;
}

// handle the signal of QSslSocket.sslErrors()
void ServerThread::errorOccured(const QList<QSslError> &)
{
   std::cout << "ssl error" << std::endl;

   // simply ignore the errors
   socket_->ignoreSslErrors();
}

// client ctor
SSLClient::SSLClient(QObject* parent)
   : QObject(parent)
{
   connect(&clientSocket_, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));

   connect(&clientSocket_, SIGNAL(sslErrors(const QList<QSslError> &)),
           this, SLOT(errorOccured(const QList<QSslError> &)));
}

// client dtor
SSLClient::~SSLClient()
{}

// start transmission
void SSLClient::start(QString hostName, quint16 port)
{
   clientSocket_.setProtocol(QSsl::TlsV1);
   clientSocket_.connectToHostEncrypted(hostName, port);
}

// handle the signal of QSslSocket.encrypted()
void SSLClient::connectionEstablished()
{
   std::cout << "connection established" << std::endl;

   // get the peer's certificate
   QSslCertificate cert = clientSocket_.peerCertificate();

   // start writing to the socket
   startWriting();

   // close connection
   clientSocket_.close();

   std::cout << "connection closed" << std::endl;
}

// start writing through the established connection
void SSLClient::startWriting()
{
   char *data = outgoingData();

   std::cout << "outgoing: \"" << data << "\"" << std::endl;

   // write on the SSL connection
   clientSocket_.write(data, strlen(data));

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
   clientSocket_.ignoreSslErrors();
}
