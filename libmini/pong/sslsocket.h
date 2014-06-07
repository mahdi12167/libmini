#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QTcpServer>
#include <QSslSocket>

class SSLServer: public QTcpServer
{
   Q_OBJECT

   public:

   SSLServer(QObject* parent = NULL);
   ~SSLServer();

   void start(QString certPath, QString keyPath, quint16 port);

   public slots:

   void readyToRead();

   protected:

   void incomingConnection(int socketDescriptor);

   QSslSocket *serverSocket;

   QString certPath;
   QString keyPath;
};

class SSLClient: public QObject
{
   Q_OBJECT

   public:

   SSLClient(QObject* parent = NULL);
   ~SSLClient();

   void start(QString hostName, quint16 port);

   public slots:

   // handle the signal of QSslSocket.encrypted()
   void connectionEstablished();

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &);

   protected:

   QSslSocket clientSocket;
};

#endif
