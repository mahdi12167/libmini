// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef CLIENTUI_H
#define CLIENTUI_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "sslclient.h"

class ClientUI: public QWidget
{
   Q_OBJECT

public:

   ClientUI(SSLTransmissionDatabaseClient *client,
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

   unsigned int counter_;
   QLabel *counterLabel_;

   QLabel *errorLabel_;

public:

   void dropEvent(QDropEvent *event);

public slots:

   void success(QString hostName, quint16 port, QString fileName, QString uid);
   void error(QString e);

protected slots:

   void hostNameChanged();

signals:

   void host(QString hostName);
   void transmit(QString fileName);
};

#endif
