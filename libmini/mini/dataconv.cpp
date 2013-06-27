// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/jpegbase.h>
#include <mini/pngbase.h>
#include <mini/zlibbase.h>

#ifdef USEGREYC
#include <mini/greycbase.h>
#endif

#include "dataconv.h"

namespace dataconv {

// set default conversion parameters for external formats
void setparameters(MINI_CONVERSION_PARAMS *params)
   {
   // parameter set for "better-quality" compressed images: quality=75 denoising=OFF -> ratio 1:20
   // parameter set for "less-size" compressed images: quality=50 denoising=ON -> ratio 1:40

   params->jpeg_quality=75.0f; // jpeg quality in percent

   params->usegreycstoration=FALSE; // use greycstoration for image denoising

   params->greyc_p=0.8f; // greycstoration sharpness, useful range=[0.7-0.9]
   params->greyc_a=0.4f; // greycstoration anisotropy, useful range=[0.1-0.5]

   params->png_gamma=0.0f; // png gamma (0.0=default 1.0=neutral)
   params->zlib_level=9; // zlib compression level (0=none 6=standard 9=highest)
   }

// set conversion hook for external formats
void setconversion(MINI_CONVERSION_PARAMS *params)
   {databuf::setconversion(conversionhook,params);}

// check conversion hook
int check_conversion()
   {return(databuf::check_conversion());}

// libMini conversion hook for external formats (JPEG/PNG/Z)
int conversionhook(int israwdata,unsigned char *srcdata,long long bytes,unsigned int extformat,
                   unsigned char **newdata,long long *newbytes,
                   databuf *obj,void *data)
   {
   unsigned int nbytes;

   MINI_CONVERSION_PARAMS *conversion_params=(MINI_CONVERSION_PARAMS *)data;

   if (conversion_params==NULL) return(0);

   switch (extformat)
      {
      case databuf::DATABUF_EXTFMT_JPEG:

         if (israwdata==0)
            {
            int width,height,components;

            *newdata=jpegbase::decompressJPEGimage(srcdata,bytes,&width,&height,&components);

            if (*newdata==NULL) return(0); // return failure

            switch (components)
               {
               case 1: if (obj->type!=0) return(0); break;
               case 3: if (obj->type!=3) return(0); break;
               case 4: if (obj->type!=4) return(0); break;
               default: return(0);
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

#ifdef USEGREYC

            if (components==1 || components==3)
               if (conversion_params->usegreycstoration)
                  greycbase::denoiseGREYCimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->greyc_p,conversion_params->greyc_a);

#endif

            jpegbase::compressJPEGimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->jpeg_quality/100.0f,newdata,&nbytes);

            if (*newdata==NULL) return(0); // return failure

            *newbytes=nbytes;
            }

         break;

      case databuf::DATABUF_EXTFMT_PNG:

         if (israwdata==0)
            {
            int width,height,components;

            *newdata=pngbase::decompressPNGimage(srcdata,bytes,&width,&height,&components);

            if (*newdata==NULL) return(0); // return failure

            switch (components)
               {
               case 1: if (obj->type!=0) return(0); break;
               case 2: if (obj->type!=1) return(0); break;
               case 3: if (obj->type!=3) return(0); break;
               case 4: if (obj->type!=4) return(0); break;
               default: return(0);
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

#ifdef USEGREYC

            if (components==3) // do not denoise elevation
               if (conversion_params->usegreycstoration)
                  greycbase::denoiseGREYCimage(srcdata,obj->xsize,obj->ysize,components,conversion_params->greyc_p,conversion_params->greyc_a);

#endif

            pngbase::compressPNGimage(srcdata,obj->xsize,obj->ysize,components,newdata,&nbytes,conversion_params->png_gamma,conversion_params->zlib_level);

            if (*newdata==NULL) return(0); // return failure

            *newbytes=nbytes;
            }

         break;

      case databuf::DATABUF_EXTFMT_Z:

         if (israwdata==0)
            {
            *newdata=zlibbase::decompressZLIB(srcdata,bytes,&nbytes);

            if (*newdata==NULL) return(0); // return failure

            *newbytes=nbytes;
            }
         else
            {
            zlibbase::compressZLIB(srcdata,bytes,newdata,&nbytes,conversion_params->zlib_level);

            if (*newdata==NULL) return(0); // return failure

            *newbytes=nbytes;
            }

         break;

      default: return(0);
      }

   return(1); // return success
   }

}
