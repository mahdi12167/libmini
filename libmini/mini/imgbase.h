// (c) by Stefan Roettger

#ifndef IMGBASE_H
#define IMGBASE_H

#include "database.h"
#include "datacalc.h"

class imgbase
   {
   public:

   imgbase();
   ~imgbase();

   //! load image based on extension
   int loadimg(databuf &buf,char *filename,int doautodecompress=0);

   //! save image based on extension
   int saveimg(databuf &buf,char *filename,float jpgquality=0.0f);

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
                            unsigned char **s3tcdata,unsigned int *s3tcbytes,
                            databuf *obj,void *data);

   static void autodecompress(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                              unsigned char **rawdata,unsigned int *rawbytes,
                              databuf *obj,void *data);
   };

#endif
