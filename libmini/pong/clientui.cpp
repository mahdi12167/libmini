// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

#include "clientui.h"

ClientUI::ClientUI(QWidget *parent)
   : QWidget(parent)
{
   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);

   layout->addWidget(new QLabel("Ping Client"));
   layout->addWidget(new QLabel("Drag and Drop files to transmit"));

   QPushButton *quitButton = new QPushButton("Quit");
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   layout->addWidget(quitButton);

   // accept drag and drop
   setAcceptDrops(true);
}

ClientUI::~ClientUI()
{}

QString ClientUI::normalizeFile(QString file)
{
   file.remove("file://");
   file.replace('\\', '/');
   if (file.contains(QRegExp("^/[A-Z]:"))) file.remove(0,1);

   return(file);
}

void ClientUI::dragEnterEvent(QDragEnterEvent *event)
{
   event->acceptProposedAction();
}

void ClientUI::dragMoveEvent(QDragMoveEvent *event)
{
   event->acceptProposedAction();
}

void ClientUI::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls())
   {
      event->acceptProposedAction();

      QList<QUrl> urlList = mimeData->urls();

      for (int i=0; i<urlList.size(); i++)
      {
         QUrl qurl = urlList.at(i);
         QString url = qurl.toString();

         //!! connect to client to use non-blocking transmit
         SSLTransmissionThread::transmitFile("localhost", 10000, normalizeFile(url), false);
      }
   }
}

void ClientUI::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}
