// (c) by Stefan Roettger

#include <mini/minibase.h>

#include <mini/database.h>
#include <mini/convbase.h>

int main(int argc,char *argv[])
   {
   databuf buf;

   convbase::MINI_CONVERSION_PARAMS conversion_params;

   if (argc!=3)
      {
      printf("usage: %s <input.db> <output.db>\n",argv[0]);
      exit(1);
      }

   // register libMini conversion hook
   convbase::setparameters(&conversion_params);
   convbase::setconversion(&conversion_params);

   // load buffer
   if (buf.loaddata(argv[1])!=0)
      {
      // save buffer
      buf.savedata(argv[2],databuf::DATABUF_EXTFMT_Z);

      // release buffer
      buf.release();
      }

   return(0);
   }
