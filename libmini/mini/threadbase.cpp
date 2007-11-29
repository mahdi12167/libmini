// (c) by Stefan Roettger

#include "threadbase.h"

threadbase::MULTITHREAD_TYPE **threadbase::MULTITHREAD=NULL;

int threadbase::MAXMULTITHREAD=0;
int threadbase::NUMMULTITHREAD=0;

void threadbase::threadinit(int threads,int id,void *data)
   {
   if (data!=NULL) ERRORMSG();

   initmultithread(id);

   MULTITHREAD[id]->numthreads=threads;

   MULTITHREAD[id]->pthread=new pthread_t[threads];

   pthread_mutex_init(&MULTITHREAD[id]->mutex,NULL);
   pthread_mutex_init(&MULTITHREAD[id]->iomutex,NULL);

   pthread_attr_init(&MULTITHREAD[id]->attr);
   pthread_attr_setdetachstate(&MULTITHREAD[id]->attr,PTHREAD_CREATE_JOINABLE);
   }

void threadbase::threadexit(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();

   pthread_mutex_destroy(&MULTITHREAD[id]->mutex);
   pthread_mutex_destroy(&MULTITHREAD[id]->iomutex);

   pthread_attr_destroy(&MULTITHREAD[id]->attr);

   delete[] MULTITHREAD[id]->pthread;

   exitmultithread(id);
   }

void threadbase::startthread(void *(*thread)(void *background),backarrayelem *background,int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_create(&MULTITHREAD[id]->pthread[background->background-1],&MULTITHREAD[id]->attr,thread,background);
   }

void threadbase::jointhread(backarrayelem *background,int id,void *data)
   {
   void *status;

   if (data!=NULL) ERRORMSG();
   pthread_join(MULTITHREAD[id]->pthread[background->background-1],&status);
   }

void threadbase::lock_cs(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_lock(&MULTITHREAD[id]->mutex);
   }

void threadbase::unlock_cs(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_unlock(&MULTITHREAD[id]->mutex);
   }

void threadbase::lock_io(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_lock(&MULTITHREAD[id]->iomutex);
   }

void threadbase::unlock_io(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_unlock(&MULTITHREAD[id]->iomutex);
   }

void threadbase::initmultithread(int id)
   {
   if (MAXMULTITHREAD==0)
      {
#ifdef PTW32_STATIC_LIB
      pthread_win32_process_attach_np();
#endif

      MAXMULTITHREAD=id+1;
      if ((MULTITHREAD=(MULTITHREAD_TYPE **)malloc(MAXMULTITHREAD*sizeof(MULTITHREAD_TYPE *)))==NULL) ERRORMSG();
      }

   if (id>=MAXMULTITHREAD)
      {
      MAXMULTITHREAD=id+1;
      if ((MULTITHREAD=(MULTITHREAD_TYPE **)realloc(MULTITHREAD,MAXMULTITHREAD*sizeof(MULTITHREAD_TYPE *)))==NULL) ERRORMSG();
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

#ifdef PTW32_STATIC_LIB
      pthread_win32_process_detach_np();
#endif
      }
   }
