// (c) by Stefan Roettger, licensed under GPL 3.0

#include "serverui.h"

ServerUI::ServerUI(QWidget *parent)
   : QWidget(parent)
{
   // set main inherited style sheet
   QString css("QGroupBox { background-color: #eeeeee; border: 2px solid #999999; border-radius: 5px; margin: 3px; padding-top: 16px; }"
               "QGroupBox::title { subcontrol-origin: padding; subcontrol-position: top left; padding-left: 8px; padding-top: 3px; }");
   setStyleSheet(css);

   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);

   layout->addWidget(new QLabel("Pong Server"));

   QGroupBox *infoBox = new QGroupBox("Server status");
   QVBoxLayout *infoBoxLayout = new QVBoxLayout;
   layout->addWidget(infoBox);
   infoBox->setLayout(infoBoxLayout);

   counter_ = 0;
   counterLabel_ = new QLabel("Transmissions:");
   infoBoxLayout->addWidget(counterLabel_);

   lastLabel_ = new QLabel("none");
   infoBoxLayout->addWidget(lastLabel_);

   timeLabel_ = new QLabel;
   infoBoxLayout->addWidget(timeLabel_);

   nameLabel_ = new QLabel;
   infoBoxLayout->addWidget(nameLabel_);

   userLabel_ = new QLabel;
   infoBoxLayout->addWidget(userLabel_);

   QPushButton *quitButton = new QPushButton("Quit");
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   layout->addWidget(quitButton);
}

ServerUI::~ServerUI()
{}

void ServerUI::transmitted(SSLTransmission t)
{
   counter_++;
   counterLabel_->setText("Transmissions: "+QString::number(counter_));

   QString transmission;

   if (t.getSize() < 1024)
      transmission = QString::number(t.getSize())+" bytes";
   else if (t.getSize() < 1024*1024)
      transmission = QString::number((double)t.getSize()/1024, 'g', 3)+" kbytes";
   else
      transmission = QString::number((double)t.getSize()/(1024*1024), 'g', 3)+" mbytes";

   lastLabel_->setText("Last request @ "+
                       QDateTime::currentDateTimeUtc().toString()+": "+transmission);

   timeLabel_->setText("Time stamp: "+t.getTime().toString());
   nameLabel_->setText("File name: \""+t.getShortTID()+"\"");
   userLabel_->setText("User name: \""+t.getShortUID()+"\"");
}

void ServerUI::report(QString error)
{
   lastLabel_->setText("Last request @ "+
                       QDateTime::currentDateTimeUtc().toString()+": "+error);
}
