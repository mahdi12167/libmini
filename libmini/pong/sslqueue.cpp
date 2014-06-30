// (c) by Stefan Roettger, licensed under GPL 3.0

#include "sslqueue.h"

// ssl transmission queue client ctor
SSLTransmissionQueueClient::SSLTransmissionQueueClient(QString hostName, quint16 port,
                                                       QString uid, bool verify, bool compress,
                                                       QObject *parent)
   : SSLTransmissionDatabaseClient(hostName, port, uid, verify, compress, parent)
{}

// ssl transmission queue client dtor
SSLTransmissionQueueClient::~SSLTransmissionQueueClient()
{}

// queue transmission
bool SSLTransmissionQueueClient::transmit(QString fileName)
{
   return(SSLTransmissionDatabaseClient::transmit(fileName));
}

// specify transmission host name
void SSLTransmissionQueueClient::transmitHostName(QString hostName, quint16 port)
{
   SSLTransmissionQueueClient::transmitHostName(hostName, port);
}

// start non-blocking transmission
void SSLTransmissionQueueClient::transmitNonBlocking(QString fileName)
{
   SSLTransmissionQueueClient::transmitNonBlocking(fileName);
}
