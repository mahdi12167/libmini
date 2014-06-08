#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QTcpServer>
#include <QSslSocket>

// ssl server class
class SSLServer: public QTcpServer
{
   Q_OBJECT

public:

   SSLServer(QObject* parent = NULL);
   virtual ~SSLServer();

   // start listening
   void start(QString certPath, QString keyPath, quint16 port);

protected:

   // handle new incoming connection
   virtual void incomingConnection(int socketDescriptor);

   QString certPath;
   QString keyPath;
};

// server ssl socket class
class ServerSocket: public QSslSocket
{
   Q_OBJECT

public:

   ServerSocket(QObject *parent = NULL);

protected:

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

   QSslSocket clientSocket;

public slots:

   // handle the signal of QSslSocket.encrypted()
   void connectionEstablished();

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &);
};

#endif
