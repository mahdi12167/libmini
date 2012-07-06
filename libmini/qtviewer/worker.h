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

   WorkerThread(QObject *parent = 0);
   virtual ~WorkerThread();

   protected:

   virtual void block_jobs();
   virtual void unblock_jobs();
   virtual void start_jobs();
   virtual void wait4jobs();

   virtual void run();

   virtual void job_success(Job *job);
   virtual void job_failure(Job *job);

   private:

   QMutex mutex;

   signals:

   void finishedJob(const ministrings &job);
};

#endif
