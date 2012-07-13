// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "worker.h"

WorkerThread::WorkerThread(QObject *parent)
   : QThread(parent), grid_worker()
{}

WorkerThread::~WorkerThread()
{
   abort_jobs();
}

void WorkerThread::block_jobs()
{
   mutex.lock();
}

void WorkerThread::unblock_jobs()
{
   mutex.unlock();
}

void WorkerThread::start_jobs()
{
   if (!isRunning())
      start(LowPriority);
}

void WorkerThread::wait4jobs()
{
   wait();
}

void WorkerThread::run()
{
   grid_worker::run();
}

void WorkerThread::set_progress(double percentage)
{
   grid_worker::set_progress(percentage);

   emit reportProgress(percentage);
}

void WorkerThread::job_success(Job *job)
{
   emit finishedJob(job->get_id(), *job);
}

void WorkerThread::job_failure(Job *)
{}
