// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/database.h>
#include <mini/miniimg.h>

static const int fillin_radius=5;

int main(int argc,char *argv[])
   {
   databuf buf;

   float jpgquality=0.9f;
   int pnglevel=9;

   if (argc!=2 && argc!=3)
      {
      printf("usage: %s <input.ext> [<output.ext>]\n",argv[0]);
      printf(" convert images with ext = db/pnm/pgm/ppm/pvm/jpg/png\n");
      printf(" or output information about image.\n");
      exit(1);
      }

   // load buffer
   if (miniimg::loadimg(buf,argv[1])!=0)
      {
      // just output image info
      if (argc==2) buf.print_info();
      else
         {
         // fill-in no-data values
         if (buf.type!=databuf::DATABUF_TYPE_RGB) buf.fillnodata(fillin_radius);

         // save buffer
         if (miniimg::saveimg(buf,argv[2],jpgquality,pnglevel)==0) fprintf(stderr,"write error\n");
         }

      // release buffer
      buf.release();
      }
   else fprintf(stderr,"read error\n");

   return(0);
   }
