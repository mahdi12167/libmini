// (c) by Stefan Roettger

#ifndef THREADBASE_H
#define THREADBASE_H

#include <pthread.h>

#include "datacloud.h"

class threadbase
   {
   public:

   static void threadinit(int threads=1);
   static void threadexit();

   static void startthread(void *(*thread)(void *background),backarrayelem *background,void *data);
   static void jointhread(backarrayelem *background,void *data);

   static void lock_cs(void *data);
   static void unlock_cs(void *data);

   static void lock_io(void *data);
   static void unlock_io(void *data);

   private:

   typedef pthread_t *PTHREADPTR;

   static int numthreads;

   static PTHREADPTR pthread;
   static pthread_mutex_t mutex,iomutex;
   static pthread_attr_t attr;
   };

#endif
