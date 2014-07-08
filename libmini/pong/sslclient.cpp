// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QSettings>

#include "sslclient.h"

// ssl transmission database client ctor
SSLTransmissionDatabaseClient::SSLTransmissionDatabaseClient(QString hostName, quint16 port,
                                                             QString uid, bool verify, bool compress,
                                                             int maxThreads,
                                                             QObject *parent)
   : QObject(parent),
     hostName_(hostName), port_(port),
     uid_(uid), verify_(verify), compress_(compress),
     autoselect_(true), autoselecting_(false), pairing_(false),
     receiver_(NULL),
     client_(NULL)
{
   if (hostName_ != "" && uid_ != "")
      autoselect_ = false;

   receiver_ = new SSLTransmissionResponseReceiver(parent);
   client_ = new SSLTransmissionClient(receiver_, maxThreads, parent);

   // signal ssl transmission pong
   connect(receiver_, SIGNAL(onPong(QString, quint16, bool)),
           this, SLOT(onPong(QString, quint16, bool)));

   // signal ssl transmission success
   connect(receiver_, SIGNAL(onSuccess(QString, quint16, QString, QString, int)),
           this, SLOT(onSuccess(QString, quint16, QString, QString, int)));

   // signal ssl transmission failure
   connect(receiver_, SIGNAL(onFailure(QString, quint16, QString, QString, int)),
           this, SLOT(onFailure(QString, quint16, QString, QString, int)));

   // signal ssl transmission response
   connect(receiver_, SIGNAL(onResponse(SSLTransmission)),
           this, SLOT(onResponse(SSLTransmission)));

   // signal ssl transmission result
   connect(receiver_, SIGNAL(onResult(SSLTransmission)),
           this, SLOT(onResult(SSLTransmission)));
}

// ssl transmission database client dtor
SSLTransmissionDatabaseClient::~SSLTransmissionDatabaseClient()
{
   if (client_)
      delete client_;

   if (receiver_)
      delete receiver_;
}

// get host name
QString SSLTransmissionDatabaseClient::getHostName(bool blocking)
{
   if (!autoselectUID(blocking))
      return("");

   return(hostName_);
}

// get port
quint16 SSLTransmissionDatabaseClient::getPort(bool blocking)
{
   if (!autoselectUID(blocking))
      return(0);

   return(port_);
}

// get user name
QString SSLTransmissionDatabaseClient::getUID()
{
   if (!autoselectUID())
      return("");

   return(uid_);
}

// get short user name
QString SSLTransmissionDatabaseClient::getShortUID(int len)
{
   QString uid = getUID();

   if (uid.size() > len)
      uid = uid.mid(0, len-1) + "...";

   return(uid);
}

// auto-select user name
bool SSLTransmissionDatabaseClient::autoselectUID(bool blocking)
{
   if (!autoselect_)
      return(true);

   if (autoselecting_)
      return(false);

   if (port_ == 0)
      port_ = SSLTransmission::default_port;

   QString hostName = hostName_;
   quint16 port = port_;
   QString uid = uid_;

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (settings.contains("hostName") &&
       settings.contains("port") &&
       settings.contains("uid"))
   {
      hostName = settings.value("hostName").toString();
      port = settings.value("port").toInt();
      uid = settings.value("uid").toString();

      if (port == 0)
         port = SSLTransmission::default_port;
   }

   if (hostName_ == hostName && port_ == port)
   {
      hostName_ = hostName;
      port_ = port;
      uid_ = uid;
   }
   else
      uid_ = "";

   if (hostName_ == "")
      return(false);

   if (uid_ == "")
   {
      settings.setValue("hostName", hostName_);
      settings.setValue("port", port_);
      settings.setValue("uid", uid_);

      SSLTransmission t = SSLTransmission::ssl_command("create_uid");

      if (blocking)
      {
         if (!client_->transmit(hostName_, port_, t, verify_))
            return(false);
         else
            if (!client_->getResponse())
               return(false);
            else
            {
               QString response = client_->getResponse()->getData();

               if (!response.startsWith("create_uid:"))
                  return(false);
               else
               {
                  uid_ = response.mid(response.indexOf(":")+1);

                  settings.setValue("uid", uid_);

                  emit registration();
               }
            }
      }
      else
      {
         client_->transmitNonBlocking(hostName_, port_, t, verify_);
         autoselecting_ = true;

         return(false);
      }
   }

   return(true);
}

// pair user name by sending uid and receiving code
void SSLTransmissionDatabaseClient::pairUID()
{
   if (!pairing_)
   {
      SSLTransmission t = SSLTransmission::ssl_command("pair_uid", "", getUID());

      client_->transmitNonBlocking(hostName_, port_, t, verify_);
      pairing_ = true;
   }
}

// sync user name by sending code and receiving uid
void SSLTransmissionDatabaseClient::pairCode(QString code)
{
   if (!pairing_)
   {
      SSLTransmission t = SSLTransmission::ssl_command("pair_code:");
      t.append(code.toAscii());

      client_->transmitNonBlocking(hostName_, port_, t, verify_);
      pairing_ = true;
   }
}

// get receiver
SSLTransmissionResponseReceiver *SSLTransmissionDatabaseClient::getReceiver()
{
   return(receiver_);
}

// start ping
bool SSLTransmissionDatabaseClient::ping()
{
   QString hostName = getHostName();

   if (hostName != "")
      return(client_->ping(hostName, port_, verify_));

   return(false);
}

// start transmission
bool SSLTransmissionDatabaseClient::transmit(SSLTransmission t)
{
   QString uid = getUID();

   if (uid != "")
   {
      t.setUID(uid);
      return(client_->transmit(hostName_, port_, t, verify_));
   }

   return(false);
}

// start file transmission
bool SSLTransmissionDatabaseClient::transmit(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      return(client_->transmit(hostName_, port_,
                               fileName, uid,
                               verify_, compress_));

   return(false);
}

// finish non-blocking threads
void SSLTransmissionDatabaseClient::finish()
{
   client_->finish();
}

// specify transmission host name
void SSLTransmissionDatabaseClient::transmitHostName(QString hostName, quint16 port)
{
   hostName_ = hostName;
   port_ = port;

   autoselectUID(false);
}

// send transmission pair uid
void SSLTransmissionDatabaseClient::transmitPairUID()
{
   pairUID();
}

// send transmission pair code
void SSLTransmissionDatabaseClient::transmitPairCode(QString code)
{
   pairCode(code);
}

// start non-blocking ping
void SSLTransmissionDatabaseClient::pingNonBlocking()
{
   QString hostName = getHostName(false);

   if (hostName != "")
      client_->pingNonBlocking(hostName, port_, verify_);
}

// start non-blocking transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(SSLTransmission t)
{
   QString uid = getUID();

   if (uid != "")
   {
      t.setUID(uid);
      client_->transmitNonBlocking(hostName_, port_, t, verify_);
   }
}

// start non-blocking file transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
   {
      QFileInfo fileInfo(fileName);

      if (fileInfo.isReadable())
         client_->transmitNonBlocking(hostName_, port_,
                                      fileName, uid,
                                      verify_, compress_);
      else
         emit error("unable to read file");
   }
   else
      emit error("failed to register with host");
}

// ssl transmission pong
void SSLTransmissionDatabaseClient::onPong(QString hostName, quint16 port, bool ack)
{
   emit pong(hostName, port, ack);
}

// ssl transmission success
void SSLTransmissionDatabaseClient::onSuccess(QString hostName, quint16 port, QString tid, QString uid, int command)
{
   if (command == SSLTransmission::cc_transmit)
      emit success(hostName, port, tid, uid);
}

// ssl transmission failure
void SSLTransmissionDatabaseClient::onFailure(QString hostName, quint16 port, QString tid, QString uid, int command)
{
   if (command == SSLTransmission::cc_transmit ||
       command == SSLTransmission::cc_respond)
      emit failure(hostName, port, tid, uid);
   else if (command == SSLTransmission::cc_command)
   {
      autoselecting_ = false;
      pairing_ = false;

      emit error("cannot contact host");
   }
}

// ssl transmission response
void SSLTransmissionDatabaseClient::onResponse(SSLTransmission t)
{
   emit response(t);
}

// ssl transmission result
void SSLTransmissionDatabaseClient::onResult(SSLTransmission t)
{
   QString response = t.getData();

   if (response.startsWith("create_uid:"))
   {
      if (t.valid())
      {
         uid_ = response.mid(response.indexOf(":")+1);

         QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

         settings.setValue("uid", uid_);

         emit registration();
      }

      autoselecting_ = false;
   }
   else if (response.startsWith("pair_code:"))
   {
      if (t.valid())
      {
         QString code = response.mid(response.indexOf(":")+1);
         uid_ = code.mid(code.indexOf(":")+1);

         QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

         settings.setValue("uid", uid_);

         emit gotPairUID(uid_);
      }

      pairing_ = false;
   }
   else if (response.startsWith("pair_uid:"))
   {
      if (t.valid())
      {
         QString code = response.mid(response.indexOf(":")+1);

         emit gotPairCode(code);
      }
      else
         emit error("pairing failed");

      pairing_ = false;
   }
}
