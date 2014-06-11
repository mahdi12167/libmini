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
};

#endif
