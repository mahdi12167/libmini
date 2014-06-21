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

   ClientUI(QString hostName = "localhost", quint16 port = 10000, QString uid = "", bool verify=true, bool compress=false, QWidget *parent = NULL);
   virtual ~ClientUI();

protected:

   QString hostName_;
   quint16 port_;
   QString uid_;
   bool verify_;
   bool compress_;

   static QString normalizeFile(QString file);

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);

public:

   void dropEvent(QDropEvent *event);

signals:

   void transmit(QString hostName, quint16 port, QString fileName, QString uid, bool verify, bool compress);
};

#endif
