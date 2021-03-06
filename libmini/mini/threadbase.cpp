// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "threadbase.h"

int threadbase::INSTANCES=0;

threadbase::MUTEX threadbase::iomutex;

threadbase::threadbase()
   {
   MULTITHREAD=NULL;

   MAXMULTITHREAD=0;
   NUMMULTITHREAD=0;

   INSTANCES++;

#ifndef USEOPENTH
#ifdef PTW32_STATIC_LIB
   if (INSTANCES==1) pthread_win32_process_attach_np();
#endif
   if (INSTANCES==1) pthread_mutex_init(&iomutex,NULL);
#else
   if (INSTANCES==1) OpenThreads::Thread::Init();
#endif
   }

threadbase::~threadbase()
   {
   INSTANCES--;

#ifndef USEOPENTH
   if (INSTANCES==1) pthread_mutex_destroy(&iomutex);
#ifdef PTW32_STATIC_LIB
   if (INSTANCES==0) pthread_win32_process_detach_np();
#endif
#endif
   }

void threadbase::threadinit(int threads,int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->threadinit_safe(threads,id);
   }

void threadbase::threadexit(int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->threadexit_safe(id);
   }

void threadbase::startthread(void *(*thread)(void *background),backarrayelem *background,int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->startthread_safe(thread,background,id);
   }

void threadbase::jointhread(backarrayelem *background,int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->jointhread_safe(background,id);
   }

void threadbase::lock_cs(int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->lock_cs_safe(id);
   }

void threadbase::unlock_cs(int id,void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->unlock_cs_safe(id);
   }

void threadbase::lock_io(void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->lock_io_safe();
   }

void threadbase::unlock_io(void *data)
   {
   threadbase *obj=(threadbase *)data;
   obj->unlock_io_safe();
   }

#ifndef USEOPENTH

void threadbase::threadinit_safe(int threads,int id)
   {
   initmultithread(id);

   MULTITHREAD[id]->numthreads=threads;

   MULTITHREAD[id]->thread=new pthread_t[threads];

   pthread_mutex_init(&MULTITHREAD[id]->mutex,NULL);

   pthread_attr_init(&MULTITHREAD[id]->attr);
   pthread_attr_setdetachstate(&MULTITHREAD[id]->attr,PTHREAD_CREATE_JOINABLE);
   }

void threadbase::threadexit_safe(int id)
   {
   pthread_mutex_destroy(&MULTITHREAD[id]->mutex);

   pthread_attr_destroy(&MULTITHREAD[id]->attr);

   delete[] MULTITHREAD[id]->thread;

   exitmultithread(id);
   }

void threadbase::startthread_safe(void *(*thread)(void *background),backarrayelem *background,int id)
   {pthread_create(&MULTITHREAD[id]->thread[background->background-1],&MULTITHREAD[id]->attr,thread,background);}

void threadbase::jointhread_safe(backarrayelem *background,int id)
   {
   void *status;
   pthread_join(MULTITHREAD[id]->thread[background->background-1],&status);
   }

void threadbase::lock_cs_safe(int id)
   {pthread_mutex_lock(&MULTITHREAD[id]->mutex);}

void threadbase::unlock_cs_safe(int id)
   {pthread_mutex_unlock(&MULTITHREAD[id]->mutex);}

void threadbase::lock_io_safe()
   {pthread_mutex_lock(&iomutex);}

void threadbase::unlock_io_safe()
   {pthread_mutex_unlock(&iomutex);}

#else

void threadbase::threadinit_safe(int threads,int id)
   {
   initmultithread(id);

   MULTITHREAD[id]->numthreads=threads;

   MULTITHREAD[id]->thread=new MyThread[threads];
   }

void threadbase::threadexit_safe(int id)
   {
   delete[] MULTITHREAD[id]->thread;

   exitmultithread(id);
   }

void threadbase::startthread_safe(void *(*thread)(void *background),backarrayelem *background,int id)
   {
   MULTITHREAD[id]->thread[background->background-1].setthread(thread,background);
   MULTITHREAD[id]->thread[background->background-1].start();
   }

void threadbase::jointhread_safe(backarrayelem *background,int id)
   {MULTITHREAD[id]->thread[background->background-1].join();}

void threadbase::lock_cs_safe(int id)
   {MULTITHREAD[id]->mutex.lock();}

void threadbase::unlock_cs_safe(int id)
   {MULTITHREAD[id]->mutex.unlock();}

void threadbase::lock_io_safe()
   {iomutex.lock();}

void threadbase::unlock_io_safe()
   {iomutex.unlock();}

#endif

void threadbase::initmultithread(int id)
   {
   if (MAXMULTITHREAD==0)
      {
      MAXMULTITHREAD=id+1;
      if ((MULTITHREAD=(MULTITHREAD_TYPE **)malloc(MAXMULTITHREAD*sizeof(MULTITHREAD_TYPE *)))==NULL) MEMERROR();
      }

   if (id>=MAXMULTITHREAD)
      {
      MAXMULTITHREAD=id+1;
      if ((MULTITHREAD=(MULTITHREAD_TYPE **)realloc(MULTITHREAD,MAXMULTITHREAD*sizeof(MULTITHREAD_TYPE *)))==NULL) MEMERROR();
      }

   MULTITHREAD[id]=new MULTITHREAD_TYPE;

   NUMMULTITHREAD++;
   }

void threadbase::exitmultithread(int id)
   {
   delete MULTITHREAD[id];

   NUMMULTITHREAD--;

   if (NUMMULTITHREAD==0)
      {
      free(MULTITHREAD);

      MULTITHREAD=NULL;
      MAXMULTITHREAD=0;
      }
   }
