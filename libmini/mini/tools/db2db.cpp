// (c) by Stefan Roettger

#include <mini/minibase.h>

#include <mini/database.h>
#include <mini/imgbase.h>

static const int fillin_radius=5;

int main(int argc,char *argv[])
   {
   databuf buf;

   float jpgquality=0.9f;
   int pnglevel=9;

   if (argc!=3)
      {
      printf("usage: %s <input.db> <output.db>\n",argv[0]);
      exit(1);
      }

   // load buffer
   if (imgbase::loadimg(buf,argv[1])!=0)
      {
      // fill-in no-data values
      if (buf.type!=databuf::DATABUF_TYPE_RGB) buf.fillnodata(fillin_radius);

      // save buffer
      imgbase::saveimg(buf,argv[2],jpgquality,pnglevel);

      // release buffer
      buf.release();
      }

   return(0);
   }
