// (c) by Stefan Roettger

#ifdef USEGREYC

#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

#endif

#include <mini/minibase.h>

#include "greycbase.h"

namespace greycbase {

void denoiseGREYCimage(unsigned char *image,int width,int height,int components,
                       float p,float a)
   {
   if (image==NULL || width<1 || height<1 || components<1) return;
   if (p<0.0f || a<0.0f) return;

#ifdef USEGREYC

   const float amplitude   = 40.0f;
   const float sharpness   = p; // smoothing parameter
   const float anisotropy  = a; // denoising parameter
   const float alpha       = 0.6f;
   const float sigma       = 1.1f;
   const float dl          = 0.8f;
   const float da          = 30.0f;
   const float gauss_prec  = 2.0f;
   const int   interp      = 0;
   const bool  fast_approx = true;

   if (components!=1 && components!=3) return;

   CImg<unsigned char> img(image,width,height,1,components,false);

   img.blur_anisotropic(amplitude,sharpness,anisotropy,alpha,sigma,dl,da,gauss_prec,interp,fast_approx);

   memcpy(image,img,width*height*components);

#endif
   }

}
