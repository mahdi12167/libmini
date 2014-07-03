// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SERVERUI_H
#define SERVERUI_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "sslserver.h"

class ServerUI: public QWidget
{
   Q_OBJECT

public:

   ServerUI(SSLTransmissionDatabaseServer *server,
            QWidget *parent = NULL);

   virtual ~ServerUI();

protected:

   unsigned int counterIn_;
   QLabel *counterInLabel_;

   unsigned int counterOut_;
   QLabel *counterOutLabel_;

   QLabel *queueLabel_;

   QLabel *lastLabel_;

   QLabel *nameLabel_;
   QLabel *userLabel_;
   QLabel *timeLabel_;

   QLabel *errorLabel_;

public slots:

   void transmitted(SSLTransmission);
   void responded(SSLTransmission);
   void report(QString);

   void status_send(int stored);
   void status_receive(int stored);
};

#endif
