// (c) by Stefan Roettger

#include "jpegbase.h"
#include "pngbase.h"

#include "greycbase.h"

#include "convbase.h"

namespace convbase {

//! set conversion hook for external formats
void setconversion(MINI_CONVERSION_PARAMS *params)
   {databuf::setconversion(conversionhook,params);}

// conversion hook for external formats (JPEG/PNG)
int conversionhook(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,
                   unsigned char **newdata,unsigned int *newbytes,
                   databuf *obj,void *data)
   {
   MINI_CONVERSION_PARAMS *conversion_params=(MINI_CONVERSION_PARAMS *)data;

   switch (extformat)
      {
      case 1: // JPEG

         if (israwdata==0)
            {
            int width,height,components;

            *newdata=jpegbase::decompressJPEGimage(srcdata,bytes,&width,&height,&components);
            if ((unsigned int)width!=obj->xsize || (unsigned int)height!=obj->ysize) ERRORMSG();

            switch (components)
               {
               case 1: if (obj->type!=0) ERRORMSG(); break;
               case 3: if (obj->type!=3) ERRORMSG(); break;
               case 4: if (obj->type!=4) ERRORMSG(); break;
               default: ERRORMSG();
               }

            *newbytes=width*height*components;
            }
         else
            {
            int components;

            switch (obj->type)
               {
               case 0: components=1; break;
               case 3: components=3; break;
               case 4: components=4; break;
               default: return(0); // return failure
               }

            if (components==1 || components==3)
               if (conversion_params->usegreycstoration)
                  greycbase::denoiseGREYCimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->greyc_p,conversion_params->greyc_a);

            jpegbase::compressJPEGimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->jpeg_quality/100.0f,newdata,newbytes);
            }

         break;

      case 2: // PNG

         if (israwdata==0)
            {
            int width,height,components;

            *newdata=pngbase::decompressPNGimage(srcdata,bytes,&width,&height,&components);
            if ((unsigned int)width!=obj->xsize || (unsigned int)height!=obj->ysize) ERRORMSG();

            switch (components)
               {
               case 1: if (obj->type!=0) ERRORMSG(); break;
               case 2: if (obj->type!=1) ERRORMSG(); break;
               case 3: if (obj->type!=3) ERRORMSG(); break;
               case 4: if (obj->type!=4) ERRORMSG(); break;
               default: ERRORMSG();
               }

            *newbytes=width*height*components;
            }
         else
            {
            int components;

            switch (obj->type)
               {
               case 0: components=1; break;
               case 1: components=2; break;
               case 3: components=3; break;
               case 4: components=4; break;
               default: return(0); // return failure
               }

            if (components==1 || components==3)
               if (conversion_params->usegreycstoration)
                  greycbase::denoiseGREYCimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->greyc_p,conversion_params->greyc_a);

            pngbase::compressPNGimage(srcdata,obj->xsize,obj->ysize,components,newdata,newbytes);
            }

         break;

      default: ERRORMSG();
      }

   return(1); // return success
   }

}
