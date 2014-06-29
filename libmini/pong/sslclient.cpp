// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QSettings>

#include "sslclient.h"

// ssl transmission database response receiver ctor
SSLTransmissionDatabaseResponseReceiver::SSLTransmissionDatabaseResponseReceiver(QObject *parent)
   : SSLTransmissionResponseReceiver(parent)
{}

// ssl transmission database response receiver dtor
SSLTransmissionDatabaseResponseReceiver::~SSLTransmissionDatabaseResponseReceiver()
{}

// successful transmission
void SSLTransmissionDatabaseResponseReceiver::onSuccess(QString hostName, quint16 port, QString fileName, QString uid)
{}

// unsuccessful transmission
void SSLTransmissionDatabaseResponseReceiver::onFailure(QString hostName, quint16 port, QString fileName, QString uid)
{}

// transmission response
void SSLTransmissionDatabaseResponseReceiver::onResponse(SSLTransmission t)
{}

// command result
void SSLTransmissionDatabaseResponseReceiver::onResult(SSLTransmission t)
{}

// general error
void SSLTransmissionDatabaseResponseReceiver::onError(QString e)
{}

// ssl transmission database client ctor
SSLTransmissionDatabaseClient::SSLTransmissionDatabaseClient(QString hostName, quint16 port,
                                                             QString uid, bool verify, bool compress,
                                                             QObject *parent)
   : QObject(parent),
     hostName_(hostName), port_(port),
     uid_(uid), verify_(verify), compress_(compress),
     autoselect_(true),
     receiver_(NULL),
     client_(NULL)
{
   if (hostName_ != "" && uid_ != "")
      autoselect_ = false;

   receiver_ = new SSLTransmissionDatabaseResponseReceiver(parent);
   client_ = new SSLTransmissionClient(receiver_, parent);
}

// ssl transmission database client dtor
SSLTransmissionDatabaseClient::~SSLTransmissionDatabaseClient()
{
   if (receiver_)
      delete receiver_;

   if (client_)
      delete client_;
}

// get host name
QString SSLTransmissionDatabaseClient::getHostName()
{
   if (!registerUID())
      return("");

   return(hostName_);
}

// get port
int SSLTransmissionDatabaseClient::getPort()
{
   if (!registerUID())
      return(-1);

   return(port_);
}

// get user name
QString SSLTransmissionDatabaseClient::getUID()
{
   if (!registerUID())
      return("");

   return(uid_);
}

// auto-select user name
bool SSLTransmissionDatabaseClient::autoselectUID(bool reset)
{
   if (!autoselect_)
      return(true);

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (settings.contains("hostName") && settings.contains("port") &&
       settings.contains("uid") && !reset)
   {
      hostName_ = settings.value("hostName").toString();

      port_ = settings.value("port").toInt();
      uid_ = settings.value("uid").toString();
   }
   else
   {
      SSLTransmission t("create_uid", "", QDateTime::currentDateTimeUtc(), SSLTransmission::cc_command);

      if (hostName_ == "")
         hostName_ = "localhost";

      if (!client_->transmit(hostName_, port_, t, verify_))
         return(false);
      else
         if (!client_->getResponse())
            return(false);
         else
            uid_ = client_->getResponse()->getData();

      settings.setValue("hostName", hostName_);
      settings.setValue("port", port_);

      settings.setValue("uid", uid_);
   }

   return(true);
}

// get receiver
SSLTransmissionResponseReceiver *SSLTransmissionDatabaseClient::getReceiver()
{
   return(receiver_);
}

// start transmission
bool SSLTransmissionDatabaseClient::transmit(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      return(client_->transmit(hostName_, port_, fileName, uid, verify_, compress_));

   return(false);
}

// register user with server
bool SSLTransmissionDatabaseClient::registerUID()
{
   if (autoselectUID())
      return(true);

   if (receiver_)
      receiver_->onError("failed to register with server");

   return(false);
}

// reset user name
bool SSLTransmissionDatabaseClient::reset()
{
   if (autoselectUID(true))
      return(true);

   if (receiver_)
      receiver_->onError("failed to reset user name");

   return(false);
}

// specify transmission host name
void SSLTransmissionDatabaseClient::transmitHostName(QString hostName)
{
   if (hostName != hostName_)
   {
      hostName_ = hostName;
      uid_ = "";

      reset();
   }
}

// start non-blocking transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      client_->transmitNonBlocking(hostName_, port_, fileName, uid, verify_, compress_,
                                   SSLTransmission::cc_transmit);
   else
      if (receiver_)
         receiver_->onFailure(hostName_, port_, fileName, uid);
}
