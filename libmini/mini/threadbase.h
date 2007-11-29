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

   struct MULTITHREAD_STRUCT
      {
      int numthreads;

      PTHREADPTR pthread;
      pthread_mutex_t mutex,iomutex;
      pthread_attr_t attr;
      };

   typedef MULTITHREAD_STRUCT MULTITHREAD_TYPE;

   static MULTITHREAD_TYPE **MULTITHREAD;
   static int MAXMULTITHREAD,NUMMULTITHREAD;

   static void initmultithread(int id);
   static void exitmultithread(int id);
   };

#endif
