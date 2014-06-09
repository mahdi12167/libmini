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

   // create new ssl server socket for each incoming connection
   ServerSocket *socket = new ServerSocket(socketDescriptor, certPath_, keyPath_, this);

   // initiate handshake
   socket->handshake();

   std::cout << "serving connection" << std::endl;
}

// ssl server socket ctor
ServerSocket::ServerSocket(int socketDescriptor, QString certPath, QString keyPath, QObject *parent)
   : QSslSocket(parent)
{
   // attach ssl socket to incoming connection
   setSocketDescriptor(socketDescriptor);

   // configure ssl socket
   setProtocol(QSsl::TlsV1);
   setLocalCertificate(certPath);
   setPrivateKey(keyPath);

   // start reading from an established connection
   connect(this, SIGNAL(readyRead()),
           this, SLOT(startReading()));

   // self-termination after socket has disconnected
   connect(this, SIGNAL(disconnected()),
           this, SLOT(deleteLater()));
}

// ssl server socket dtor
ServerSocket::~ServerSocket()
{}

// start ssl handshake
void ServerSocket::handshake()
{
   startServerEncryption();
}

// start reading after the connection is established
void ServerSocket::startReading()
{
   QByteArray data = readAll();

   std::cout << "incoming: " << data.size() << "bytes" << std::endl;

   incomingData(data.constData(),data.size());
}

// handle incoming data
void ServerSocket::incomingData(const char *data,unsigned int size)
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

// start writing after the connection is established
void SSLClient::connectionEstablished()
{
   std::cout << "connection established" << std::endl;

   // get the peer's certificate
   QSslCertificate cert = socket_.peerCertificate();

   // start writing to the ssl socket
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
