// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "worker.h"

void WorkerThread::block()
{
   mutex.lock();
}

void WorkerThread::unblock()
{
   mutex.unlock();
}

void WorkerThread::start()
{
   if (!isRunning())
      QThread::start(LowPriority);
}

void WorkerThread::wait()
{
   QThread::wait();
}

void WorkerThread::run()
{
   grid_worker::run();
}
