// (c) by Stefan Roettger

#include "minibase.h"

#include "pngbase.h"

namespace pngbase {

#include <stdio.h>

extern "C"
   {
   #include <png.h>
   }

unsigned char *decompressPNGimage(unsigned char *data,int bytes,int *width,int *height,int *components)
   {
   //!! not yet implemented
   return(NULL);
   }

void compressPNGimage(unsigned char *image,int width,int height,int components,unsigned char **data,unsigned int *bytes)
   {
   //!! not yet implemented
   }

}
