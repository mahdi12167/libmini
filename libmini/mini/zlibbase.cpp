// (c) by Stefan Roettger

#include <stdio.h>

extern "C"
   {
   #include <zlib.h>
   }

#include <mini/minibase.h>

#include "zlibbase.h"

namespace zlibbase {

// decompress with ZLIB
unsigned char *decompressZLIB(unsigned char *chunk,unsigned int chunklen,unsigned int *bytes)
   {
   //!! todo
   return(NULL);
   }

// compress with ZLIB
void compressZLIB(unsigned char *data,unsigned int bytes,unsigned char **chunk,unsigned int *chunklen,int level)
   {
   Bytef *mem;
   uLongf len;

   len=bytes+bytes/100+12;
   if ((mem=(Bytef *)malloc(len))==NULL) ERRORMSG();

   if (compress2(mem,&len,data,bytes,level)==Z_OK)
      {
      if (((*chunk)=(unsigned char *)realloc(mem,len))==NULL) ERRORMSG();
      *chunklen=len;
      return;
      }
   else
      {
      free(mem);

      len=2*bytes+12;
      if ((mem=(Bytef *)malloc(len))==NULL) ERRORMSG();

      if (compress2(mem,&len,data,bytes,level)==Z_OK)
         {
         if (((*chunk)=(unsigned char *)realloc(mem,len))==NULL) ERRORMSG();
         *chunklen=len;
         return;
         }
      else free(mem);
      }

   *chunk=NULL;
   *chunklen=0;
   }

}
