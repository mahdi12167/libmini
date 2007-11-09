// (c) by Stefan Roettger

#include <pthread.h>

#include "minitime.h"

#include "greycbase.h"

namespace greycbase {

#ifdef GREYCSTORATION

#define cimg_display_type 0
#define cimg_plugin "plugins/greycstoration.h"
#include "CImg.h"
using namespace cimg_library;

#endif

void denoiseGREYCimage(unsigned char *image,int width,int height,
                       float p,float a)
   {
#ifdef GREYCSTORATION

   const float amplitude      = 40.0f;
   const float sharpness      = p; // -p option of greycstoration
   const float anisotropy     = a; // -a option of greycstoration
   const float alpha          = 0.6f;
   const float sigma          = 1.1f;
   const float gfact          = 1.0f;
   const bool fast_approx     = true;
   const float gauss_prec     = 2.0f;
   const float dl             = 0.8f;
   const float da             = 30.0f;
   const unsigned int interp  = 0;
   const unsigned int tile    = 512;
   const unsigned int btile   = 4;
   const unsigned int threads = 4;

   CImg<unsigned char> img(image,width,height,1,3,false);

   img.greycstoration_run(amplitude,sharpness,anisotropy,alpha,sigma,gfact,dl,da,gauss_prec,interp,fast_approx,tile,btile,threads);

   while (img.greycstoration_is_running()) miniwaitfor(0.001f);

   memcpy(image,img.ptr(),3*width*height);

#endif
   }

}
