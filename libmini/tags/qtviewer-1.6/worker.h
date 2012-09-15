// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef WORKER_H
#define WORKER_H

#include <mini/mini_generic.h>
#include <grid/grid_workers.h>

#include <QtCore/QThread>
#include <QtCore/QMutex>

//! background worker
class WorkerThread: public QThread, public grid_worker
{
   Q_OBJECT

   public:

   WorkerThread(QObject *parent = 0);
   virtual ~WorkerThread();

   void run_job(Job *job);
   void abort_jobs();

   protected:

   virtual void block_jobs();
   virtual void unblock_jobs();
   virtual void start_jobs();
   virtual void wait4jobs();

   virtual void run();

   virtual void set_progress(double percentage);

   virtual void job_success(Job *job);
   virtual void job_failure(Job *job, int errorcode);

   private:

   QMutex mutex;

   signals:

   void reportProgress(double percentage, const ministring &job);
   void finishedJob(const ministring &job, const ministrings &args);
   void failedJob(const ministring &job, const ministrings &args, int errorcode);
};

#endif
