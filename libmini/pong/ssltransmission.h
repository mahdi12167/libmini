// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLTRANSMISSION_H
#define SSLTRANSMISSION_H

#include <QThread>
#include <QSemaphore>

#include "ssldata.h"
#include "sslsocket.h"

// ssl transmission server connection factory class
class SSLTransmissionServerConnectionFactory: public SSLServerConnectionFactory
{
   Q_OBJECT

public:

   SSLTransmissionServerConnectionFactory(SSLTransmissionResponder *responder = NULL,
                                          QObject *parent = NULL);

   virtual ~SSLTransmissionServerConnectionFactory();

   // create a new transmission server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent);

   // get responder
   SSLTransmissionResponder *getResponder() const;

protected:

   SSLTransmissionResponder *responder_;

public slots:

   // receiver of transmitted data blocks
   void transmit(SSLTransmission);

   // receiver of respond data blocks
   void respond(SSLTransmission);

   // receiver of command data blocks
   void command(SSLTransmission);

   // receiver of transmission failures
   void failure(SSLTransmission);

signals:

   // signal transmission
   void transmitted(SSLTransmission);

   // signal response
   void responded(SSLTransmission);

   // signal command
   void executed(SSLTransmission);

   // signal error
   void error(SSLTransmission);
};

// ssl transmission server connection class
class SSLTransmissionServerConnection: public SSLServerConnection
{
   Q_OBJECT

public:

   SSLTransmissionServerConnection(int socketDescriptor,
                                   QString certPath, QString keyPath,
                                   SSLServerConnectionFactory *factory,
                                   SSLTransmissionResponder *responder = NULL,
                                   QObject *parent = NULL);

   virtual ~SSLTransmissionServerConnection();

protected:

   // start reading from ssl socket
   virtual bool startReading(QSslSocket *socket);

   SSLTransmission t_;

signals:

   // signal transmission of data block
   void transmit(SSLTransmission);

   // signal transmission of respond block
   void respond(SSLTransmission);

   // signal command data block
   void command(SSLTransmission);

   // signal transmission failures
   void failure(SSLTransmission);
};

// ssl transmission response receiver class
class SSLTransmissionResponseReceiver: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionResponseReceiver(QObject *parent = NULL);
   virtual ~SSLTransmissionResponseReceiver();

public slots:

   void pong(QString hostName, quint16 port, bool ack);
   void success(QString hostName, quint16 port, QString tid, QString uid, int command);
   void failure(QString hostName, quint16 port, QString tid, QString uid, int command);
   void response(SSLTransmission t);
   void result(SSLTransmission t);

signals:

   void onPong(QString hostName, quint16 port, bool ack);
   void onSuccess(QString hostName, quint16 port, QString tid, QString uid, int command);
   void onFailure(QString hostName, quint16 port, QString tid, QString uid, int command);
   void onResponse(SSLTransmission t);
   void onResult(SSLTransmission t);
};

// ssl transmission client class
class SSLTransmissionClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTransmissionClient(SSLTransmissionResponseReceiver *receiver = NULL,
                         int maxThreads=10,
                         QObject *parent = NULL);

   virtual ~SSLTransmissionClient();

   // start ping
   bool ping(QString hostName, quint16 port, bool verify=true);

   // start transmission
   bool transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify=true);

   // start file transmission
   bool transmit(QString hostName, quint16 port, QString fileName, QString uid,
                 bool verify=true, bool compress=false, SSLTransmission::CommandCode command = SSLTransmission::cc_transmit);

   // get transmission receiver
   SSLTransmissionResponseReceiver *getReceiver() const;

   // get transmission response
   SSLTransmission *getResponse() const;

   // finish non-blocking threads
   void finish();

protected:

   // start writing to ssl socket
   virtual bool startWriting(QSslSocket *socket);

   SSLTransmission t_;

   SSLTransmissionResponseReceiver *receiver_;

   QSemaphore threads_;
   int maxThreads_;

public slots:

   // start non-blocking ping
   void pingNonBlocking(QString hostName, quint16 port, bool verify=true);

   // start non-blocking transmission
   void transmitNonBlocking(QString hostName, quint16 port, const SSLTransmission &t, bool verify=true);

   // start non-blocking file transmission
   void transmitNonBlocking(QString hostName, quint16 port, QString fileName, QString uid,
                            bool verify=true, bool compress=false, SSLTransmission::CommandCode command = SSLTransmission::cc_transmit);

signals:

   void response(SSLTransmission);
   void result(SSLTransmission);
};

// ssl transmission thread class
class SSLTransmissionThread: public QThread
{
   Q_OBJECT

public:

   SSLTransmissionThread(QString hostName, quint16 port, const SSLTransmission &t, bool verify=true,
                         QSemaphore *threads = NULL,
                         QObject *parent = NULL);

   virtual ~SSLTransmissionThread();

protected:

   virtual void run();

   QString hostName_;
   quint16 port_;
   SSLTransmission t_;
   bool verify_;

   QSemaphore *threads_;

protected slots:

   void receive_response(SSLTransmission);
   void receive_result(SSLTransmission);

signals:

   void pong(QString, quint16, bool);
   void success(QString, quint16, QString, QString, int);
   void failure(QString, quint16, QString, QString, int);
   void response(SSLTransmission);
   void result(SSLTransmission);
};

#endif
