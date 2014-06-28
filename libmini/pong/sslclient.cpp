// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QSettings>

#include "sslclient.h"

// ssl transmission database client ctor
SSLTransmissionDatabaseClient::SSLTransmissionDatabaseClient(QString hostName, quint16 port,
                                                             QString uid, bool verify, bool compress,
                                                             QObject *parent)
   : SSLTransmissionClient(parent),
     hostName__(hostName), port__(port),
     uid__(uid), verify__(verify), compress__(compress),
     autoselect__(true),
     receiver__(NULL)
{
   if (hostName__ != "" && uid__ != "")
      autoselect__ = false;

   receiver__ = new SSLTransmissionDatabaseResponseReceiver;
}

// ssl transmission database client dtor
SSLTransmissionDatabaseClient::~SSLTransmissionDatabaseClient()
{
   if (receiver__)
      delete receiver__;
}

// get host name
QString SSLTransmissionDatabaseClient::getHostName()
{
   if (!registerUID())
      return("");

   return(hostName__);
}

// get port
int SSLTransmissionDatabaseClient::getPort()
{
   if (!registerUID())
      return(-1);

   return(port__);
}

// get user name
QString SSLTransmissionDatabaseClient::getUID()
{
   if (!registerUID())
      return("");

   return(uid__);
}

// auto-select user name
bool SSLTransmissionDatabaseClient::autoselectUID(bool reset)
{
   if (!autoselect__)
      return(true);

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (settings.contains("hostName") && settings.contains("port") &&
       settings.contains("uid") && !reset)
   {
      hostName__ = settings.value("hostName").toString();

      port__ = settings.value("port").toInt();
      uid__ = settings.value("uid").toString();
   }
   else
   {
      SSLTransmissionClient client;
      SSLTransmission t("create_uid", "", QDateTime::currentDateTimeUtc(), SSLTransmission::cc_command);

      if (hostName__ == "")
         hostName__ = "localhost";

      if (!client.transmit(hostName__, port__, t, verify__))
         return(false);
      else
         if (!client.getResponse())
            return(false);
         else
            uid__ = client.getResponse()->getData();

      settings.setValue("hostName", hostName__);
      settings.setValue("port", port__);

      settings.setValue("uid", uid__);
   }

   return(true);
}

// get receiver
SSLTransmissionResponseReceiver *SSLTransmissionDatabaseClient::getReceiver()
{
   return(receiver__);
}

// start transmission
bool SSLTransmissionDatabaseClient::transmit(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      return(SSLTransmissionClient::transmit(hostName__, port__, fileName, uid, verify__, compress__));

   return(false);
}

// register user with server
bool SSLTransmissionDatabaseClient::registerUID()
{
   if (autoselectUID())
      return(true);

   if (receiver__)
      receiver__->onError("failed to register with server");

   return(false);
}

// reset user name
bool SSLTransmissionDatabaseClient::reset()
{
   if (autoselectUID(true))
      return(true);

   if (receiver__)
      receiver__->onError("failed to reset user name");

   return(false);
}

// specify transmission host name
void SSLTransmissionDatabaseClient::transmitHostName(QString hostName)
{
   if (hostName != hostName__)
   {
      hostName__ = hostName;
      uid__ = "";

      reset();
   }
}

// start non-blocking transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      SSLTransmissionClient::transmitNonBlocking(hostName__, port__, fileName, uid, verify__, compress__,
                                                 SSLTransmission::cc_transmit, receiver__);
   else
      if (receiver__)
         receiver__->onFailure(hostName__, port__, fileName, uid);
}
