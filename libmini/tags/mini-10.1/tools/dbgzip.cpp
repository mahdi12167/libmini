// (c) by Stefan Roettger

#include <mini/minibase.h>

#include <mini/database.h>
#include <mini/dataconv.h>

int main(int argc,char *argv[])
   {
   databuf buf;
   BOOLINT extfmt_z;
   int success;

   dataconv::MINI_CONVERSION_PARAMS conversion_params;

   if (argc!=3)
      {
      printf("usage: %s <input.db> <output.db>\n",argv[0]);
      printf(" compress/decompress db files with zlib.\n");
      exit(1);
      }

   // register libMini conversion hook
   dataconv::setparameters(&conversion_params);
   dataconv::setconversion(&conversion_params);

   // load stub
   if (buf.loaddata(argv[1],1)!=0)
      {
      extfmt_z=buf.extformat==databuf::DATABUF_EXTFMT_Z;

      // load buffer
      if (buf.loaddata(argv[1])!=0)
         {
         // save buffer
         if (extfmt_z) success=buf.savedata(argv[2]);
         else success=buf.savedata(argv[2],databuf::DATABUF_EXTFMT_Z);
         if (success==0) fprintf(stderr,"write error\n");

         // release buffer
         buf.release();
         }
      else fprintf(stderr,"read error\n");
      }
   else fprintf(stderr,"read error\n");

   return(0);
   }
