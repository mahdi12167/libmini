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

   // create new server thread for each incoming connection
   ServerThread *thread = new ServerThread(socketDescriptor, certPath_, keyPath_, this);

   // execute thread run method
   thread->start();

   std::cout << "serving connection" << std::endl;
}

// ssl server thread ctor
ServerThread::ServerThread(int socketDescriptor, QString certPath, QString keyPath, QObject *parent)
   : QThread(parent)
{
   // create new ssl socket for each incoming connection
   socket_ = new QSslSocket(this);
   socket_->setSocketDescriptor(socketDescriptor);

   // configure ssl socket
   socket_->setProtocol(QSsl::TlsV1);
   socket_->setLocalCertificate(certPath);
   socket_->setPrivateKey(keyPath);

   // start reading when ready signal is emitted
   connect(socket_, SIGNAL(readyRead()),
           this, SLOT(startReading()));

   // self-termination after thread has finished
   connect(this, SIGNAL(finished()),
           this, SLOT(deleteLater()));
}

// ssl server thread dtor
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

// ssl client ctor
SSLClient::SSLClient(QObject *parent)
   : QObject(parent)
{
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
   socket_.connectToHostEncrypted(hostName, port);
}

// start wrting to the ssl socket after the QSslSocket.encrypted() signal
void SSLClient::connectionEstablished()
{
   std::cout << "connection established" << std::endl;

   // get the peer's certificate
   QSslCertificate cert = socket_.peerCertificate();

   // start writing to the socket
   startWriting();

   // close connection
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
   char data[] = "message";

   return(strdup(data));
}
