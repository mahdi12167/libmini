// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef CLIENTUI_H
#define CLIENTUI_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

class ClientUI: public QWidget
{
   Q_OBJECT

public:

   ClientUI(QWidget *parent = NULL);
   virtual ~ClientUI();

protected:

   static QString normalizeFile(QString file);

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);

public:

   void dropEvent(QDropEvent *event);

signals:

   void transmit(QString fileName);
};

#endif
