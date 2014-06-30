// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLTRANSMISSION_H
#define SSLTRANSMISSION_H

#include <algorithm>

#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include "sslsocket.h"

class SSLTransmission;

// ssl transmission responder base class
class SSLTransmissionResponder
{
public:

   SSLTransmissionResponder();
   virtual ~SSLTransmissionResponder();

   // create a transmission response
   virtual SSLTransmission *create(const SSLTransmission *t) = 0;

   // create a command response
   virtual SSLTransmission *command(const SSLTransmission *t) = 0;
};

// ssl transmission class
class SSLTransmission
{
public:

   // ssl transmission header structure
   struct SSLTransmissionHeader
   {
      qint32 magic; // magic number
      qint8 command; // command number
      qint64 size; // size of data block
      qint8 compressed; // is the data block compressed?
      qint64 time; // time stamp of data block in epoch respresentation
      quint16 tid_size; // size of transmission id block
      quint16 uid_size; // size of user id block
      qint8 valid; // is the data block valid
   };

   static const int header_size =
      sizeof(qint32) +
      sizeof(qint8) +
      sizeof(qint64) +
      sizeof(qint8) +
      sizeof(qint64) +
      sizeof(quint16) +
      sizeof(quint16) +
      sizeof(qint8);

   static const qint32 magic_number = 0x02071971;
   static const qint8 response_ok = 1;

   enum CommandCode
   {
      cc_transmit = 0,
      cc_respond  = 1,
      cc_response = 2,
      cc_command = 3,
      cc_result = 4
   };

   SSLTransmission(const QString tid="", const QString uid="",
                   const QDateTime time = QDateTime::currentDateTimeUtc(),
                   const CommandCode command = cc_transmit)
      : data_(),
        tid_(tid.toAscii()), uid_(uid.toAscii()),
        transmitState_(0), response_(NULL), responder_(NULL)
   {
      header_.magic = magic_number;
      header_.command = command;
      header_.size = 0;
      header_.compressed = false;
      header_.time = time.toUTC().toMSecsSinceEpoch();
      header_.tid_size = std::min(tid_.size(), 65535);
      header_.uid_size = std::min(uid_.size(), 65535);
      header_.valid = true;
   }

   SSLTransmission(const QByteArray &data, const QString tid="", const QString uid="",
                   const QDateTime time = QDateTime::currentDateTimeUtc(), bool compressed=false,
                   const CommandCode command = cc_transmit)
      : data_(data),
        tid_(tid.toAscii()), uid_(uid.toAscii()),
        transmitState_(0), response_(NULL), responder_(NULL)
   {
      header_.magic = magic_number;
      header_.command = command;
      header_.size = data_.size();
      header_.compressed = compressed;
      header_.time = time.toUTC().toMSecsSinceEpoch();
      header_.tid_size = std::min(tid_.size(), 65535);
      header_.uid_size = std::min(uid_.size(), 65535);
      header_.valid = true;
   }

   SSLTransmission(QFile &file, const QString uid="",
                   const CommandCode command = cc_transmit)
      : data_(file.readAll()),
        tid_(file.fileName().toAscii()), uid_(uid.toAscii()),
        transmitState_(0), response_(NULL), responder_(NULL)
   {
      QFileInfo fileInfo(file);

      header_.magic = magic_number;
      header_.command = command;
      header_.size = data_.size();
      header_.compressed = false;
      header_.time = fileInfo.lastModified().toUTC().toMSecsSinceEpoch();
      header_.tid_size = std::min(tid_.size(), 65535);
      header_.uid_size = std::min(uid_.size(), 65535);
      header_.valid = true;
   }

   SSLTransmission(const SSLTransmission &t)
      : header_(t.header_), data_(t.data_),
        tid_(t.tid_), uid_(t.uid_),
        transmitState_(t.transmitState_), response_(NULL), responder_(t.responder_)
   {
      if (t.response_)
         response_ = new SSLTransmission(*t.response_);
   }

   SSLTransmission& operator = (const SSLTransmission &t)
   {
      header_ = t.header_;
      data_ = t.data_;

      tid_ = t.tid_;
      uid_ = t.uid_;

      transmitState_ = t.transmitState_;
      response_ = NULL;
      responder_ = t.responder_;

      if (t.response_)
         response_ = new SSLTransmission(*t.response_);

      return(*this);
   }

   ~SSLTransmission()
   {
      if (response_)
         delete response_;
   }

   bool empty() const
   {
      return(header_.size == 0);
   }

   void setData(const QByteArray &data, bool compressed=false)
   {
      data_ =  data;
      header_.size = data_.size();
      header_.compressed = compressed;
   }

   QByteArray getData() const
   {
      return(data_);
   }

   bool compressed() const
   {
      return(header_.compressed);
   }

   qint64 getSize() const
   {
      return(header_.size);
   }

   QDateTime getTime() const
   {
      QDateTime t;

      t.setMSecsSinceEpoch(header_.time);

      return(t.toUTC());
   }

   void setTime(QDateTime time)
   {
      header_.time = time.toUTC().toMSecsSinceEpoch();
   }

   QString getTID() const
   {
      return(tid_);
   }

   QString getShortTID(const int len=50) const
   {
      QString tid(tid_);

      if (tid.size() > len)
         tid = tid.mid(0, len-1) + "...";

      return(tid);
   }

   void setTID(QString tid)
   {
      tid_ = tid.toAscii();
      header_.tid_size = std::min(tid_.size(), 65535);
   }

   QString getUID() const
   {
      return(uid_);
   }

   QString getShortUID(const int len=50) const
   {
      QString uid(uid_);

      if (uid.size() > len)
         uid = uid.mid(0, len-1) + "...";

      return(uid);
   }

   void setUID(QString uid)
   {
      uid_ = uid.toAscii();
      header_.uid_size = std::min(uid_.size(), 65535);
   }

   CommandCode getCommand() const
   {
      return((CommandCode)header_.command);
   }

   void setCommand(CommandCode command)
   {
      header_.command = command;
   }

   SSLTransmissionResponder *getResponder()
   {
      return(responder_);
   }

   void setResponder(SSLTransmissionResponder *responder)
   {
      responder_ = responder;
   }

   SSLTransmission *getResponse() const
   {
      return(response_);
   }

   void setError()
   {
      header_.valid = false;
   }

   bool valid() const
   {
      return(header_.valid);
   }

   void append(const QByteArray &data)
   {
      if (!header_.compressed)
      {
         data_.append(data);
         header_.size = data_.size();
      }
   }

   void compress()
   {
      if (!header_.compressed)
      {
         data_ = qCompress(data_, 3); // favor speed over compression ratio
         header_.size = data_.size();
         header_.compressed = true;
      }
   }

   void uncompress()
   {
      if (header_.compressed)
      {
         data_ = qUncompress(data_);
         header_.size = data_.size();
         header_.compressed = false;
      }
   }

   bool write(QSslSocket *socket, bool ack=true)
   {
      if (transmitState_ == 0)
      {
         QByteArray block;
         QDataStream out(&block, QIODevice::WriteOnly);

         // assemble header block
         out.setVersion(QDataStream::Qt_4_0);
         out << header_.magic;
         out << header_.command;
         out << header_.size;
         out << header_.compressed;
         out << header_.time;
         out << header_.tid_size;
         out << header_.uid_size;
         out << header_.valid;

         // write header block to the ssl socket
         socket->write(block);

         // write tid block to the ssl socket
         if (header_.tid_size > 0)
            socket->write(tid_);

         // write uid block to the ssl socket
         if (header_.uid_size > 0)
            socket->write(uid_);

         // write data block to the ssl socket
         if (header_.size > 0)
            socket->write(data_);

         // wait until entire data block has been written
         if (!socket->waitForBytesWritten(-1)) // no time-out
            return(false);

         // clear data block
         data_.clear();

         transmitState_++;
      }

      if (transmitState_ == 1)
      {
         if (ack)
         {
            if (header_.command == cc_transmit ||
                header_.command == cc_response ||
                header_.command == cc_result)
            {
               char code;

               // check if response code has arrived
               if (socket->bytesAvailable() < 1)
                  if (!socket->waitForReadyRead(-1)) // no time-out
                     return(false);

               // read response code from the ssl socket
               socket->read(&code, 1);

               // check for correct response code
               if (code != response_ok)
                  return(false);
            }
            else if (header_.command == cc_respond ||
                     header_.command == cc_command)
            {
               // allocate transmission response
               response_ = new SSLTransmission();

               // check if entire response block has arrived
               while (!response_->read(socket))
                  if (!socket->waitForReadyRead())
                     return(false);

               // check for correct response block
               if (!response_->valid())
                  return(false);
            }
         }

         transmitState_++;
      }

      return(true);
   }

   bool read(QSslSocket *socket)
   {
      QDataStream in(socket);
      in.setVersion(QDataStream::Qt_4_0);

      if (transmitState_ == 0)
      {
         // check if entire header block has arrived
         if (socket->bytesAvailable() < header_size) return(false);

         // read magic number
         in >> header_.magic;
         if (header_.magic != magic_number)
         {
            setError();
            return(true);
         }

         // read header block
         in >> header_.command;
         in >> header_.size;
         in >> header_.compressed;
         in >> header_.time;
         in >> header_.tid_size;
         in >> header_.uid_size;
         in >> header_.valid;

         transmitState_++;
      }

      if (transmitState_ == 1)
      {
         if (header_.tid_size > 0)
         {
            // read tid block from the ssl socket
            tid_.append(socket->read(header_.tid_size-tid_.size()));

            // check if entire tid block has arrived
            if (tid_.size() < header_.tid_size) return(false);
         }

         transmitState_++;
      }

      if (transmitState_ == 2)
      {
         if (header_.uid_size > 0)
         {
            // read uid block from the ssl socket
            uid_.append(socket->read(header_.uid_size-uid_.size()));

            // check if entire uid block has arrived
            if (uid_.size() < header_.uid_size) return(false);
         }

         transmitState_++;
      }

      if (transmitState_ == 3)
      {
         if (header_.size > 0)
         {
            // read data block from the ssl socket
            data_.append(socket->read(header_.size-data_.size()));

            // check if entire data block has arrived
            if (data_.size() < header_.size) return(false);
         }

         transmitState_++;
      }

      if (transmitState_ == 4)
      {
         if (header_.command == cc_transmit ||
             header_.command == cc_response ||
             header_.command == cc_result)
         {
            char code = response_ok;

            // write response code to the ssl socket
            socket->write(&code, 1);

            // wait until response code has been written
            if (!socket->waitForBytesWritten())
            {
               setError();
               return(true);
            }
         }
         else if (header_.command == cc_respond ||
                  header_.command == cc_command)
         {
            // ask transmission responder to respond
            if (responder_)
            {
               // create transmission response
               if (header_.command == cc_respond)
               {
                  response_ = responder_->create(this);
                  response_->setCommand(cc_response);
               }
               else
               {
                  response_ = responder_->command(this);
                  response_->setCommand(cc_result);
               }

               // write transmission response to the ssl socket
               if (!response_->write(socket, false))
               {
                  setError();
                  return(true);
               }

               transmitState_++;
               return(false);
            }
            else
            {
               setError();
               return(true);
            }
         }

         transmitState_++;
      }

      if (transmitState_ == 5)
      {
         if (header_.command == cc_respond ||
             header_.command == cc_command)
         {
            char code;

            // check if response code has arrived
            if (socket->bytesAvailable() < 1)
               if (!socket->waitForReadyRead())
               {
                  setError();
                  return(true);
               }

            // read response code from the ssl socket
            socket->read(&code, 1);

            // check for correct response code
            if (code != response_ok)
            {
               setError();
               return(true);
            }
         }

         transmitState_++;
      }

      return(true);
   }

protected:

   struct SSLTransmissionHeader header_; // transmission header
   QByteArray data_; // transmission data block
   QByteArray tid_; // transmission id
   QByteArray uid_; // user id

   int transmitState_; // actual state of transmission
   SSLTransmission *response_; // received transmission response
   SSLTransmissionResponder *responder_; // transmission responder
};

// stream output
inline std::ostream& operator << (std::ostream &out, const SSLTransmission &t)
{
   out << "SSLTransmission(";

   out << "\"" << t.getShortTID().toStdString() << "\", "
       << "\"" << t.getShortUID().toStdString() << "\", "
       << t.getTime().toString(Qt::ISODate).toStdString() << ", "
       << t.getSize();

   if (t.compressed())
      out << ", COMPRESSED";

   if (!t.valid())
      out << ", INVALID";

   out << ")";

   return(out);
}

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

   // receiver of command data blocks
   void command(SSLTransmission);

   // receiver of invalid data blocks
   void invalid(SSLTransmission);

signals:

   // signal transmission
   void transmitted(SSLTransmission);

   // signal command
   void execute(SSLTransmission);

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

   // signal command data block
   void command(SSLTransmission);

   // signal invalid data block
   void invalid(SSLTransmission);
};

// ssl transmission response receiver class
class SSLTransmissionResponseReceiver: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionResponseReceiver(QObject *parent = NULL);
   virtual ~SSLTransmissionResponseReceiver();

public slots:

   void success(QString hostName, quint16 port, QString tid, QString uid);
   void failure(QString hostName, quint16 port, QString tid, QString uid);
   void response(SSLTransmission t);
   void result(SSLTransmission t);

signals:

   void onSuccess(QString hostName, quint16 port, QString tid, QString uid);
   void onFailure(QString hostName, quint16 port, QString tid, QString uid);
   void onResponse(SSLTransmission t);
   void onResult(SSLTransmission t);
};

// ssl transmission client class
class SSLTransmissionClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTransmissionClient(SSLTransmissionResponseReceiver *receiver = NULL,
                         QObject *parent = NULL);

   virtual ~SSLTransmissionClient();

   // start transmission
   bool transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify=true);

   // start file transmission
   bool transmit(QString hostName, quint16 port, QString fileName, QString uid,
                 bool verify=true, bool compress=false, SSLTransmission::CommandCode command = SSLTransmission::cc_transmit);

   // get transmission receiver
   SSLTransmissionResponseReceiver *getReceiver() const;

   // get transmission response
   SSLTransmission *getResponse() const;

protected:

   // start writing to ssl socket
   virtual bool startWriting(QSslSocket *socket);

   SSLTransmission t_;

   SSLTransmissionResponseReceiver *receiver_;

public slots:

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
                         QObject *parent = NULL);

   virtual ~SSLTransmissionThread();

protected:

   virtual void run();

   QString hostName_;
   quint16 port_;
   SSLTransmission t_;
   bool verify_;

protected slots:

   void receive_response(SSLTransmission);
   void receive_result(SSLTransmission);

signals:

   void success(QString, quint16, QString, QString);
   void failure(QString, quint16, QString, QString);
   void response(SSLTransmission);
   void result(SSLTransmission);
};

#endif
