// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef WORKER_H
#define WORKER_H

#include <grid/grid_worker.h>

#include <QtCore/QThread>
#include <QtCore/QMutex>

//! background worker
class WorkerThread: public QThread, public grid_worker
{
   Q_OBJECT

   public:

   WorkerThread(QObject *parent = 0)
      : grid_worker(), QThread(parent)
   {}

   protected:

   virtual void block();
   virtual void unblock();
   virtual void start();
   virtual void wait();

   virtual void run();

   private:

   QMutex mutex;
};

#endif
