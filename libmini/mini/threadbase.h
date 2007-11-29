// (c) by Stefan Roettger

#ifndef THREADBASE_H
#define THREADBASE_H

#include <pthread.h>

#include "datacloud.h"

class threadbase
   {
   public:

   static void threadinit(int threads,int id,void *data);
   static void threadexit(int id,void *data);

   static void startthread(void *(*thread)(void *background),backarrayelem *background,int id,void *data);
   static void jointhread(backarrayelem *background,int id,void *data);

   static void lock_cs(int id,void *data);
   static void unlock_cs(int id,void *data);

   static void lock_io(int id,void *data);
   static void unlock_io(int id,void *data);

   private:

   typedef pthread_t *PTHREADPTR;

   static int numthreads;

   static PTHREADPTR pthread;
   static pthread_mutex_t mutex,iomutex;
   static pthread_attr_t attr;
   };

#endif
