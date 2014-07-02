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

   unsigned int counter_;
   QLabel *counterLabel_;

   QLabel *lastLabel_;

   QLabel *nameLabel_;
   QLabel *userLabel_;
   QLabel *timeLabel_;

   QLabel *errorLabel_;

protected slots:

   void transmitted(SSLTransmission);
   void report(QString);
};

#endif
