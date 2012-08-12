// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#else
#include <windows.h>
#define stat _stat
#endif

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

// write a RAW file
void writefile(const char *filename,unsigned char *data,unsigned int bytes)
   {
   FILE *file;

   if (bytes<1) ERRORMSG();

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   if (fwrite(data,1,bytes,file)!=bytes) IOERROR();

   fclose(file);
   }

// read a RAW file
unsigned char *readfile(const char *filename,unsigned int *bytes)
   {
   FILE *file;

   unsigned char *data;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   data=readfiled(file,bytes);

   fclose(file);

   return(data);
   }

// read from a RAW file
unsigned char *readfiled(FILE *file,unsigned int *bytes)
   {
   const unsigned int BLOCKSIZE=1<<20;

   unsigned char *data;
   unsigned int cnt,blkcnt;

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

   *bytes=cnt;

   return(data);
   }

// write a string as RAW file
void writestring(const char *filename,const char *cstr)
   {writefile(filename,(unsigned char *)cstr,strlen(cstr));}

// read a RAW file as string
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

// read a RAW file and compute signature
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

// get file modification year
int getmodyear(const char *filename)
   {
   struct tm* clock;
   struct stat attrib;

   stat(filename,&attrib);

   clock = gmtime(&(attrib.st_mtime));

   return(clock->tm_year);
   }

// get file modification time relative to year
long long int getmodtime(const char *filename,int year)
   {
   struct tm* clock;
   struct stat attrib;

   stat(filename,&attrib);

   clock = gmtime(&(attrib.st_mtime));

   return(clock->tm_sec+
          60*(clock->tm_min+
              60*(clock->tm_hour+
                  24*(clock->tm_mday+
                      31*(clock->tm_mon+
                          365*clock->tm_year-(long long int)year)))));
   }

}
