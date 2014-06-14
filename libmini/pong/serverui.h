// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SERVERUI_H
#define SERVERUI_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

class ServerUI: public QWidget
{
   Q_OBJECT

public:

   ServerUI(QWidget *parent = NULL);
   virtual ~ServerUI();

protected:

   unsigned int counter_;
   QLabel *counterLabel_;

   QLabel *lastLabel_;
   QLabel *modifiedLabel_;

public slots:

   void transmitted(QByteArray, qint64);
   void report(QString);
};

#endif
