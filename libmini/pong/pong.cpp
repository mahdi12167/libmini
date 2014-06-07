class SSLServer: public QTcpServer
{
   Q_OBJECT

   public:

   SSLServer(QObject* parent = NULL)
      : QTcpServer(parent)
   {
      serverSocket = NULL;
   }

   ~SSLServer()
   {
      if (serverSocket)
         delete serverSocket;
   }

   void start(QString certPath, QString keyPath, quint16 port)
   {
      listen(QHostAddress::Any, port);

      this->certPath = certPath;
      this->keyPath = keyPath;
   }

   public slots:

   void readyToRead()
   {
      qDebug() << serverSocket->readAll();
   }

   void errorOccured(const QList &)
   {
      serverSocket->ignoreSslErrors();
   }

   protected:

   void incomingConnection(int socketDescriptor)
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

   protected:

   QSslSocket *serverSocket;

   QString certPath;
   QString keyPath;
};

class SSLClient: public QObject
{
   Q_OBJECT

   public:
      
   SSLClient(QObject* parent = NULL)
      : QObject(parent)
   {
      connect(&clientSocket, SIGNAL(encrypted()),
              this, SLOT(connectionEstablished()));

      connect(&clientSocket, SIGNAL(sslErrors(const QList<QSslError> &)),
              this, SLOT(errorOccured(const QList<QSslError> &)));
   }
      
   void start(QString hostName, quint16 port)
   {
      clientSocket.setProtocol(QSsl::TlsV1);
      clientSocket.connectToHostEncrypted(hostName, port);
   }

   public slots:

   // handle the signal of QSslSocket.encrypted()
   void connectionEstablished()
   {
      // get the peer's certificate
      QSslCertificate cert = clientSocket.peerCertificate();

      // write on the SSL connection
      clientSocket.write("hello, client\n", 14);
   }

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &error)
   {
      // simply ignore the errors
      clientSocket.ignoreSslErrors();
   }

   protected:

   QSslSocket clientSocket;
};

int main(int argc, char** argv)
{
   bool PONG_SERVER=true;

   QApplication app(argc, argv);

   if (PONG_SERVER)
   {
      SSLServer server;
      server.start("ca.cer", "ca.key", 8888);
   }
   else
   {
      SSLClient client;
      client.start("127.0.0.1", 8888);
   }

   return(app.exec());
}
