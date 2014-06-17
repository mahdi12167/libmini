// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssldatabase.h"

SSLTransmissionDatabase::SSLTransmissionDatabase(QObject *parent)
   : QObject(parent)
{}

SSLTransmissionDatabase::~SSLTransmissionDatabase()
{}

// read a transmission from the db
SSLTransmission SSLTransmissionDatabase::read(QString tid, QString uid)
{
   SSLTransmission t;

   return(t);
}

// remove a transmission from the db
void SSLTransmissionDatabase::remove(QString tid, QString uid)
{}

// write a transmission to the db
void SSLTransmissionDatabase::write(SSLTransmission t)
{}
