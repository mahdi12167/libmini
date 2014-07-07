// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLDATA_H
#define SSLDATA_H

#include <algorithm>

#include <QFile>
#include <QFileInfo>
#include <QDateTime>

#include <QSslSocket>

class SSLTransmission;

// ssl transmission responder base class
class SSLTransmissionResponder
{
public:

   SSLTransmissionResponder() {}
   virtual ~SSLTransmissionResponder() {}

   // create a transmission response
   virtual SSLTransmission *respond(const SSLTransmission *t) = 0;

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
      cc_ping = 0,
      cc_transmit = 1,
      cc_respond  = 2,
      cc_response = 3,
      cc_command = 4,
      cc_result = 5
   };

   SSLTransmission(const CommandCode command,
                   const QDateTime time = QDateTime::currentDateTimeUtc())
      : data_(),
        tid_(), uid_(),
        transmitState_(0), transmitFailure_(false),
        response_(NULL), responder_(NULL)
   {
      header_.magic = magic_number;
      header_.command = command;
      header_.size = 0;
      header_.compressed = false;
      header_.time = time.toUTC().toMSecsSinceEpoch();
      header_.tid_size = 0;
      header_.uid_size = 0;
      header_.valid = true;
   }

   SSLTransmission(const QString tid="", const QString uid="",
                   const QDateTime time = QDateTime::currentDateTimeUtc(),
                   const CommandCode command = cc_transmit)
      : data_(),
        tid_(tid.toAscii()), uid_(uid.toAscii()),
        transmitState_(0), transmitFailure_(false),
        response_(NULL), responder_(NULL)
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
        transmitState_(0), transmitFailure_(false),
        response_(NULL), responder_(NULL)
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
        transmitState_(0), transmitFailure_(false),
        response_(NULL), responder_(NULL)
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
        transmitState_(0), transmitFailure_(false),
        response_(NULL), responder_(t.responder_)
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

      transmitState_ = 0;
      transmitFailure_ = false;

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

protected:

   void setFailure()
   {
      transmitFailure_ = true;
   }

public:

   bool success() const
   {
      return(!transmitFailure_);
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
         {
            setFailure();
            return(false);
         }

         // clear data block
         data_.clear();

         transmitState_++;
      }

      if (transmitState_ == 1)
      {
         if (ack)
         {
            if (header_.command == cc_ping ||
                header_.command == cc_transmit ||
                header_.command == cc_response ||
                header_.command == cc_result)
            {
               char code;

               // check if response code has arrived
               if (socket->bytesAvailable() < 1)
                  if (!socket->waitForReadyRead(-1)) // no time-out
                  {
                     setFailure();
                     return(false);
                  }

               // read response code from the ssl socket
               socket->read(&code, 1);

               // check for correct response code
               if (code != response_ok)
               {
                  setFailure();
                  return(false);
               }
            }
            else if (header_.command == cc_respond ||
                     header_.command == cc_command)
            {
               // allocate transmission response
               response_ = new SSLTransmission();

               // check if entire response block has arrived
               while (!response_->read(socket))
                  if (!socket->waitForReadyRead())
                  {
                     setFailure();
                     return(false);
                  }

               // check for correct transmission of response block
               if (!response_->success())
               {
                  setFailure();
                  return(false);
               }
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
            setFailure();
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
         if (header_.command == cc_ping ||
             header_.command == cc_transmit ||
             header_.command == cc_response ||
             header_.command == cc_result)
         {
            char code = response_ok;

            // write response code to the ssl socket
            socket->write(&code, 1);

            // wait until response code has been written
            if (!socket->waitForBytesWritten())
            {
               setFailure();
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
                  response_ = responder_->respond(this);
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
                  setFailure();
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
                  setFailure();
                  return(true);
               }

            // read response code from the ssl socket
            socket->read(&code, 1);

            // check for correct response code
            if (code != response_ok)
            {
               setFailure();
               return(true);
            }
         }

         transmitState_++;
      }

      return(true);
   }

   static SSLTransmission ssl_ping()
   {
      return(SSLTransmission(SSLTransmission::cc_ping));
   }

   static SSLTransmission ssl_transmission(const QString tid, const QString uid, const QByteArray &a, bool compressed=false)
   {
      return(SSLTransmission(a, tid, uid, QDateTime::currentDateTimeUtc(), compressed));
   }

   static SSLTransmission ssl_file(const QString fileName, const QString uid, bool compress=false)
   {
      SSLTransmission t;

      QFile file(fileName);

      if (!file.open(QIODevice::ReadOnly))
         t.setError();
      else
      {
         t = SSLTransmission(file, uid);

         if (compress)
            t.compress();
      }

      return(t);
   }

   static SSLTransmission ssl_respond(const QString uid)
   {
      return(ssl_respond("", uid));
   }

   static SSLTransmission ssl_respond(const QString tid, const QString uid)
   {
      return(SSLTransmission(tid, uid, QDateTime::currentDateTimeUtc(), cc_respond));
   }

   static SSLTransmission ssl_command(const QString action, const QString tid="", const QString uid="")
   {
      QByteArray a(action.toAscii());
      return(SSLTransmission(a, tid, uid, QDateTime::currentDateTimeUtc(), false, cc_command));
   }

protected:

   struct SSLTransmissionHeader header_; // transmission header
   QByteArray data_; // transmission data block
   QByteArray tid_; // transmission id
   QByteArray uid_; // user id

   int transmitState_; // actual state of transmission
   bool transmitFailure_; // actual failure status of transmission

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

   switch (t.getCommand())
   {
      case SSLTransmission::cc_ping: out << ", PING"; break;
      case SSLTransmission::cc_transmit: out << ", TRANSMIT"; break;
      case SSLTransmission::cc_respond: out << ", RESPOND"; break;
      case SSLTransmission::cc_response: out << ", RESPONSE"; break;
      case SSLTransmission::cc_command: out << ", COMMAND"; break;
      case SSLTransmission::cc_result: out << ", RESULT"; break;
   }

   if (!t.valid())
      out << ", INVALID";

   out << ")";

   return(out);
}

#endif
