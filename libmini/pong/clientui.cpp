// (c) by Stefan Roettger, licensed under GPL 3.0

#include "clientui.h"

ClientUI::ClientUI(SSLTransmissionQueueClient *client,
                   QWidget *parent)
   : QWidget(parent),
     client_(client),
     hostName_(""), port_(0)
{
   // get client mode
   uploadMode_ = client_->uploadMode();

   // set main inherited style sheet
   QString css("QGroupBox { background-color: #eeeeee; border: 2px solid #999999; border-radius: 5px; margin: 3px; padding-top: 16px; }"
               "QGroupBox::title { subcontrol-origin: padding; subcontrol-position: top left; padding-left: 8px; padding-top: 3px; }");
   setStyleSheet(css);

   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);
   layout->addWidget(new QLabel("Ping Client"));

   if (uploadMode_)
   {
      QGroupBox *dropBox = new QGroupBox("To transmit files");
      QVBoxLayout *dropBoxLayout = new QVBoxLayout;
      layout->addWidget(dropBox);
      dropBox->setLayout(dropBoxLayout);
      QLabel *dropText = new QLabel("drag&drop them here!");
      dropBoxLayout->addWidget(dropText);
   }

   QGroupBox *lineEditGroup_hostName = createEdit(uploadMode_?"Transmit to host":"Receive from host", "", &lineEdit_hostName_);
   connect(lineEdit_hostName_, SIGNAL(editingFinished()), this, SLOT(hostNameChanged()));
   layout->addWidget(lineEditGroup_hostName);

   if (uploadMode_)
   {
      QGroupBox *lineEditGroup_pairCode = createEdit("Enter pair code", "", &lineEdit_pairCode_);
      connect(lineEdit_pairCode_, SIGNAL(editingFinished()), this, SLOT(pairCodeChanged()));
      layout->addWidget(lineEditGroup_pairCode);
   }

   QGroupBox *infoBox = new QGroupBox("Client status");
   QVBoxLayout *infoBoxLayout = new QVBoxLayout;
   layout->addWidget(infoBox);
   infoBox->setLayout(infoBoxLayout);

   counter_ = 0;
   counterLabel_ = new QLabel(uploadMode_?"Outgoing: none":"Incoming: none");
   infoBoxLayout->addWidget(counterLabel_);

   queueLabel_ = new QLabel;
   infoBoxLayout->addWidget(queueLabel_);

   if (!uploadMode_)
   {
      codeLabel_ = new QLabel;
      infoBoxLayout->addWidget(codeLabel_);
   }

   errorLabel_ = new QLabel;
   infoBoxLayout->addWidget(errorLabel_);

   if (!uploadMode_)
   {
      QPushButton *pairButton = new QPushButton("Pair Client");
      connect(pairButton, SIGNAL(pressed()), client, SLOT(transmitPairUID()));
      layout->addWidget(pairButton);
   }

   QPushButton *quitButton = new QPushButton("Quit");
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   layout->addWidget(quitButton);

   // accept drag and drop
   if (uploadMode_)
      setAcceptDrops(true);

   // connect gui with host slot
   QObject::connect(this, SIGNAL(host(QString, quint16)),
                    client, SLOT(transmitHostName(QString, quint16)));

   // connect gui with code slot
   QObject::connect(this, SIGNAL(code(QString)),
                    client, SLOT(transmitPairCode(QString)));

   // connect gui with transmit slot
   QObject::connect(this, SIGNAL(transmit(QString)),
                    client, SLOT(transmitNonBlocking(QString)));

   // connect alive signal with gui
   QObject::connect(client, SIGNAL(alive(QString, quint16, bool)),
                    this, SLOT(alive(QString, quint16, bool)));

   // connect success signal with gui
   QObject::connect(client, SIGNAL(success(QString, quint16, QString, QString)),
                    this, SLOT(transmitted(QString, quint16, QString, QString)));

   // connect failure signal with gui
   QObject::connect(client, SIGNAL(failure(QString, quint16, QString, QString)),
                    this, SLOT(failed(QString, quint16, QString, QString)));

   // connect response signal with gui
   QObject::connect(client, SIGNAL(response(SSLTransmission)),
                    this, SLOT(received(SSLTransmission)));

   // connect registration signal with gui
   QObject::connect(client, SIGNAL(registration()),
                    this, SLOT(registration()));

   // connect pair code signal with gui
   QObject::connect(client, SIGNAL(gotPairCode(QString)),
                    this, SLOT(gotPairCode(QString)));

   // connect pair uid signal with gui
   QObject::connect(client, SIGNAL(gotPairUID(QString)),
                    this, SLOT(gotPairUID(QString)));

   // connect error signal with gui
   QObject::connect(client, SIGNAL(error(QString)),
                    this, SLOT(error(QString)));

   // connect send status signal with gui
   QObject::connect(client, SIGNAL(status_send(int)),
                    this, SLOT(status_send(int)));

   // connect receive status signal with gui
   QObject::connect(client, SIGNAL(status_receive(int)),
                    this, SLOT(status_receive(int)));

   // start transmission queue
   if (uploadMode_)
      client->send();
   else
      client->receive();
}

ClientUI::~ClientUI()
{
   client_->finish();
}

QGroupBox *ClientUI::createEdit(QString name, QString value,
                                QLineEdit **lineEdit)
{
   QGroupBox *lineEditGroup = new QGroupBox(name);
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value);
   lineEditLayout->addWidget(*lineEdit);
   return(lineEditGroup);
}

void ClientUI::hostNameChanged()
{
   hostName_ = lineEdit_hostName_->text();

   emit host(hostName_, port_);
}

void ClientUI::pairCodeChanged()
{
   emit code(lineEdit_pairCode_->text());
}

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
         QString fileName = normalizeFile(qurl.toString());

         emit transmit(fileName);
      }
   }
}

void ClientUI::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}

void ClientUI::alive(QString hostName, quint16 port, bool ack)
{
   if (ack)
   {
      hostName_ = hostName;
      port_ = port;

      lineEdit_hostName_->setText(hostName);
   }
   else
      errorLabel_->setText("cannot connect to host");
}

void ClientUI::transmitted(QString hostName, quint16 port, QString tid, QString uid)
{
   counter_++;
   counterLabel_->setText("Outgoing: "+QString::number(counter_));

   errorLabel_->setText("ok");
}

void ClientUI::failed(QString hostName, quint16 port, QString tid, QString uid)
{
   errorLabel_->setText("transmission failure");
}

void ClientUI::received(SSLTransmission t)
{
   if (!t.valid())
      return;

   counter_++;
   counterLabel_->setText("Incoming: "+QString::number(counter_));

   errorLabel_->setText("ok");
}

void ClientUI::registration()
{
   errorLabel_->setText("registered with host");
}

void ClientUI::gotPairCode(QString code)
{
   codeLabel_->setText("Pair code: "+code);
   errorLabel_->setText("ready for pairing");
}

void ClientUI::gotPairUID(QString uid)
{
   lineEdit_pairCode_->setText("");
   errorLabel_->setText("client paired");
}

void ClientUI::error(QString e)
{
   errorLabel_->setText(e);
}

void ClientUI::status_send(int queued)
{
   queueLabel_->setText("Queued: "+QString::number(queued));
}

void ClientUI::status_receive(int stored)
{
   queueLabel_->setText("Stored: "+QString::number(stored));
}
