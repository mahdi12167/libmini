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

   SSLTransmissionQueueClient *client_;

   QString hostName_;
   QLineEdit *lineEdit_hostName_;

   quint16 port_;

   bool uploadMode_;

   QLabel *codeLabel_;
   QLineEdit *lineEdit_pairCode_;

   unsigned int counter_;
   QLabel *counterLabel_;

   QLabel *queueLabel_;

   QLabel *errorLabel_;

public:

   void dropEvent(QDropEvent *event);

public slots:

   void success(QString hostName, quint16 port, QString tid, QString uid);
   void failure(QString hostName, quint16 port, QString tid, QString uid);

   void registration();
   void gotPairCode(QString code);
   void gotPairUID(QString uid);
   void error(QString e);

   void status_send(int queued);
   void status_receive(int stored);

   void hostNameChanged();
   void pairCodeChanged();

signals:

   void host(QString hostName, quint16 port);
   void code(QString code);
   void transmit(QString fileName);
};

#endif
