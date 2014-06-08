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

   // start listening
   void start(QString certPath, QString keyPath, quint16 port);

protected:

   // handle new incoming connection
   void incomingConnection(int socketDescriptor);

   // handle incoming data
   void incomingData(const char *data);

   QSslSocket *serverSocket;

   QString certPath;
   QString keyPath;

public slots:

   // handle the signal of QSslSocket.readyRead()
   void startReading();

   // handle the signal of QSslSocket.sslErrors()
   void errorOccured(const QList<QSslError> &);
};

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
