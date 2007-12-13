// (c) by Stefan Roettger

#ifndef THREADBASE_H
#define THREADBASE_H

#include <pthread.h>

#include "datacloud.h"

class threadbase
   {
   public:

   threadbase();
   ~threadbase();

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

   MULTITHREAD_TYPE **MULTITHREAD;
   int MAXMULTITHREAD,NUMMULTITHREAD;

   static int INSTANCES;

   void threadinit_safe(int threads,int id);
   void threadexit_safe(int id);

   void startthread_safe(void *(*thread)(void *background),backarrayelem *background,int id);
   void jointhread_safe(backarrayelem *background,int id);

   void lock_cs_safe(int id);
   void unlock_cs_safe(int id);

   void lock_io_safe(int id);
   void unlock_io_safe(int id);

   void initmultithread(int id);
   void exitmultithread(int id);
   };

#endif
