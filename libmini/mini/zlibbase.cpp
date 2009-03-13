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
   //!! todo
   *chunk=NULL;
   *chunklen=0;
   }

}
