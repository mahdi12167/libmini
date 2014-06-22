// (c) by Stefan Roettger, licensed under GPL 3.0

#include "serverui.h"

ServerUI::ServerUI(QWidget *parent)
   : QWidget(parent)
{
   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);

   layout->addWidget(new QLabel("Pong Server"));

   counter_ = 0;
   counterLabel_ = new QLabel("Transmissions:");
   layout->addWidget(counterLabel_);

   lastLabel_ = new QLabel("none");
   layout->addWidget(lastLabel_);

   timeLabel_ = new QLabel;
   layout->addWidget(timeLabel_);

   nameLabel_ = new QLabel;
   layout->addWidget(nameLabel_);

   userLabel_ = new QLabel;
   layout->addWidget(userLabel_);

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
   nameLabel_->setText("File name: "+t.getTID());
   userLabel_->setText("User name: "+t.getUID());
}

void ServerUI::report(QString error)
{
   lastLabel_->setText("Last request @ "+
                       QDateTime::currentDateTimeUtc().toString()+": "+error);
}
