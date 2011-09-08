// (c) by Stefan Roettger

#include "minibase.h"

#include "miniio.h"

namespace miniio {

// check a file
int checkfile(const char *filename)
   {
   FILE *file;

   if ((file=fopen(filename,"rb"))==NULL) return(0);
   fclose(file);

   return(1);
   }

// write a raw file
void writefile(const char *filename,unsigned char *data,unsigned int bytes)
   {
   FILE *file;

   if (bytes<1) ERRORMSG();

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   if (fwrite(data,1,bytes,file)!=bytes) IOERROR();

   fclose(file);
   }

// read a raw file
unsigned char *readfile(const char *filename,unsigned int *bytes)
   {
   const unsigned int BLOCKSIZE=1<<20;

   FILE *file;

   unsigned char *data;
   unsigned int cnt,blkcnt;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   data=NULL;
   cnt=0;

   do
      {
      if (data==NULL)
         {if ((data=(unsigned char *)malloc(BLOCKSIZE))==NULL) MEMERROR();}
      else
         if ((data=(unsigned char *)realloc(data,cnt+BLOCKSIZE))==NULL) MEMERROR();

      blkcnt=fread(&data[cnt],1,BLOCKSIZE,file);
      cnt+=blkcnt;
      }
   while (blkcnt==BLOCKSIZE);

   if (cnt==0)
      {
      free(data);
      return(NULL);
      }

   if ((data=(unsigned char *)realloc(data,cnt))==NULL) MEMERROR();

   fclose(file);

   *bytes=cnt;

   return(data);
   }

// read a raw file as string
char *readstring(const char *filename)
   {
   char *data;
   unsigned int bytes;

   data=(char *)readfile(filename,&bytes);

   if (data==NULL) return(NULL);

   if ((data=(char *)realloc(data,bytes+1))==NULL) MEMERROR();
   data[bytes]='\0';

   return(data);
   }

// read a raw file and compute signature
unsigned int signature(const char *filename)
   {
   unsigned int sig;

   unsigned char *data;
   unsigned int bytes;

   sig=0;

   if ((data=readfile(filename,&bytes))!=NULL)
      {
      sig=signature(data,bytes);
      free(data);
      }

   return(sig);
   }

// compute a signature
unsigned int signature(const unsigned char *data,unsigned int bytes)
   {
   unsigned int i;

   unsigned int sig;

   const unsigned char *ptr;

   sig=0;
   ptr=data;

   for (i=0; i<bytes; i++) sig=((271*sig)^34+(*ptr++));

   return(sig);
   }

}
