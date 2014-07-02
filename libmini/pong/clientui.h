// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef CLIENTUI_H
#define CLIENTUI_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "sslqueue.h"

class ClientUI: public QWidget
{
   Q_OBJECT

public:

   ClientUI(SSLTransmissionQueueClient *client,
            QWidget *parent = NULL);

   virtual ~ClientUI();

protected:

   static QGroupBox *createEdit(QString name, QString value,
                                QLineEdit **lineEdit);

   static QString normalizeFile(QString file);

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);

   QString hostName_;
   QLineEdit *lineEdit_hostName;

   quint16 port_;

   unsigned int counter_;
   QLabel *counterLabel_;

   QLabel *queueLabel_;

   QLabel *errorLabel_;

public:

   void dropEvent(QDropEvent *event);

protected slots:

   void success(QString hostName, quint16 port, QString tid, QString uid);
   void failure(QString hostName, quint16 port, QString tid, QString uid);

   void registration();
   void error(QString e);

   void changed(int queued);
   void hostNameChanged();

signals:

   void host(QString hostName, quint16 port);
   void transmit(QString fileName);
};

#endif
