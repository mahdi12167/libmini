// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIIMG_H
#define MINIIMG_H

#include <mini/database.h>
#include <mini/datacalc.h>

class miniimg
   {
   public:

   //! load image based on extension
   static int loadimg(databuf &buf,char *filename);

   //! save image based on extension
   static int saveimg(databuf &buf,char *filename,float jpgquality=0.0f,int pnglevel=0);

   private:

   enum FILE_TYPE
      {
      FILE_TYPE_DB,
      FILE_TYPE_PNM,
      FILE_TYPE_PVM,
      FILE_TYPE_JPG,
      FILE_TYPE_PNG,
      FILE_TYPE_ETC
      };

   static datacalc CALC;

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                            unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,
                            void *data);

   static void autodecompress(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                              unsigned char **rawdata,unsigned int *rawbytes,int width,int height,
                              void *data);
   };

#endif
