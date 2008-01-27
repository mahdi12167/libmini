#include "minibase.h"

#include "database.h"
#include "datacalc.h"
#include "squishbase.h"

// S3TC auto-compression hook
void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                  unsigned char **s3tcdata,unsigned int *s3tcbytes,
                  databuf *obj,void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::compressS3TC(isrgbadata,rawdata,bytes,
                            s3tcdata,s3tcbytes,obj->xsize,obj->ysize);
   }

int main(int argc,char *argv[])
   {
   databuf buf;

   datacalc calc;

   if (argc!=3)
      {
      printf("usage: %s <input.db> <output.db>\n",argv[0]);
      exit(1);
      }

   // register implicit calculator
   calc.doregister();

   // register auto-compression hook
   databuf::setautocompress(autocompress,NULL);

   // load buffer
   if (buf.loaddata(argv[1])==0)
      if (buf.loadPNMdata(argv[1])==0) exit(1);

   // compress to s3tc
   buf.autocompress();

   // save buffer
   buf.savedata(argv[2]);

   return(0);
   }
