// (c) by Stefan Roettger

#include "minibase.h"

#include "squishbase.h"

#include <squish.h> // do not put the header file into /usr/include on MacOS X

namespace squishbase {

MODE_ENUM MODE=MODE_FAST; // we strive to compress as fast as possible

void compressS3TC(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                  unsigned char **s3tcdata,unsigned int *s3tcbytes,
                  int width,int height)
   {
   int i;

   unsigned char *rgbadata;

   int mode;

   static const int modefast=squish::kDxt1 | squish::kColourRangeFit; // fast but produces artifacts
   static const int modegood=squish::kDxt1 | squish::kColourClusterFit; // almost no artifacts though much slower
   static const int modeslow=squish::kDxt1 | squish::kColourIterativeClusterFit; // no artifacts but really sluggish

   switch (MODE)
      {
      default:
      case MODE_FAST: mode=modefast; break;
      case MODE_GOOD: mode=modegood; break;
      case MODE_SLOW: mode=modeslow; break;
      }

   if (isrgbadata==0)
      {
      rgbadata=(unsigned char *)malloc(4*width*height);
      if (rgbadata==NULL) ERRORMSG();

      for (i=0; i<width*height; i++)
         {
         rgbadata[4*i]=rawdata[3*i];
         rgbadata[4*i+1]=rawdata[3*i+1];
         rgbadata[4*i+2]=rawdata[3*i+2];
         rgbadata[4*i+3]=255;
         }

      rawdata=rgbadata;
      }

   *s3tcbytes=squish::GetStorageRequirements(width,height,mode);
   *s3tcdata=(unsigned char *)malloc(*s3tcbytes);
   if (*s3tcdata==NULL) ERRORMSG();

   squish::CompressImage(rawdata,width,height,*s3tcdata,mode);

   if (isrgbadata==0) free(rawdata);
   }

// mode selector
void setmode(MODE_ENUM mode)
   {MODE=mode;}

}
