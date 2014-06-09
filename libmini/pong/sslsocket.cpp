#include <iostream>

#include "sslsocket.h"

// server ctor
SSLServer::SSLServer(QObject *parent)
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

   // attach descriptor of new connection to ssl socket
   if (socket->setSocketDescriptor(socketDescriptor))
   {
      // create new server thread for each incoming connection
      ServerThread *thread = new ServerThread(socket, this);

      // execute thread run method
      thread->start();

      std::cout << "serving connection" << std::endl;
   }
   else
   {
      std::cout << "connection failure" << std::endl;

      delete socket;
   }
}

// server thread ctor
ServerThread::ServerThread(QSslSocket *socket, QObject *parent)
   : QThread(parent)
{
   socket_ = socket;

   // start reading when ready
   connect(socket_, SIGNAL(readyRead()),
           this, SLOT(startReading()));

   // self-termination
   connect(this, SIGNAL(finished()),
           this, SLOT(deleteLater()));
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

// client ctor
SSLClient::SSLClient(QObject *parent)
   : QObject(parent)
{
   connect(&clientSocket_, SIGNAL(encrypted()),
           this, SLOT(connectionEstablished()));
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
