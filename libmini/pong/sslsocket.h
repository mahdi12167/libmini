#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QTcpServer>
#include <QSslSocket>
#include <QThread>

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(QObject* parent = NULL);
   virtual ~SSLServer();

   // start listening
   void start(QString certPath, QString keyPath, quint16 port = 0);

protected:

   // handle new incoming connection
   virtual void incomingConnection(int socketDescriptor);

   QString certPath_;
   QString keyPath_;
};

// server thread class serving a ssl socket
class ServerThread: public QThread
{
   Q_OBJECT

public:

   ServerThread(QSslSocket *socket, QObject *parent = NULL);
   virtual ~ServerThread();

protected:

   QSslSocket *socket_;

   virtual void run();

   // handle incoming data
   void incomingData(const char *data);

public slots:

   // handle the signal of QSslSocket.readyRead()
   void startReading();

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &);
};

// ssl client class
class SSLClient: public QObject
{
   Q_OBJECT

public:

   SSLClient(QObject* parent = NULL);
   ~SSLClient();

   // start transmission
   void start(QString hostName, quint16 port);

protected:

   // start writing
   void startWriting();

   // assemble outgoing data
   char *outgoingData();

   QSslSocket clientSocket_;

public slots:

   // handle the signal of QSslSocket.encrypted()
   void connectionEstablished();

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &);
};

#endif
