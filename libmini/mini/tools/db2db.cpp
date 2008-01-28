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

// S3TC auto-decompression hook
void autodecompress(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                    unsigned char **rawdata,unsigned int *rawbytes,
                    databuf *obj,void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::decompressS3TC(isrgbadata,s3tcdata,bytes,
                              rawdata,rawbytes,obj->xsize,obj->ysize);
   }

int main(int argc,char *argv[])
   {
   databuf buf;
   datacalc calc;

   enum FILE_TYPE
      {
      FILE_TYPE_DB,
      FILE_TYPE_PNM,
      FILE_TYPE_PVM,
      FILE_TYPE_ETC
      };

   FILE_TYPE src,dst;
   char *src_ext,*dst_ext;

   if (argc!=3)
      {
      printf("usage: %s <input.db> <output.db>\n",argv[0]);
      exit(1);
      }

   src_ext=strrchr(argv[1],'.');
   dst_ext=strrchr(argv[2],'.');

   src=FILE_TYPE_ETC;

   if (src_ext!=NULL)
      if (strcmp(src_ext,".db")==0) src=FILE_TYPE_DB;
      else if (strcmp(src_ext,".pgm")==0) src=FILE_TYPE_PNM;
      else if (strcmp(src_ext,".ppm")==0) src=FILE_TYPE_PNM;
      else if (strcmp(src_ext,".pvm")==0) src=FILE_TYPE_PVM;

   dst=FILE_TYPE_ETC;

   if (dst_ext!=NULL)
      if (strcmp(dst_ext,".db")==0) dst=FILE_TYPE_DB;
      else if (strcmp(dst_ext,".pgm")==0) dst=FILE_TYPE_PNM;
      else if (strcmp(dst_ext,".ppm")==0) dst=FILE_TYPE_PNM;
      else if (strcmp(dst_ext,".pvm")==0) dst=FILE_TYPE_PVM;

   // register implicit calculator
   calc.doregister();

   // register auto-compression hook
   databuf::setautocompress(autocompress,NULL);

   // register auto-decompression hook
   databuf::setautodecompress(autodecompress,NULL);

   // load buffer
   if (src==FILE_TYPE_DB) buf.loaddata(argv[1]);
   else if (src==FILE_TYPE_PNM) buf.loadPNMdata(argv[1]);
   else if (src==FILE_TYPE_PVM) buf.loadPVMdata(argv[1]);

   // compress to or decompress from s3tc
   if (dst==FILE_TYPE_DB) buf.autocompress();
   else buf.autodecompress();

   // save buffer
   if (dst==FILE_TYPE_DB) buf.savedata(argv[2]);
   else if (dst==FILE_TYPE_PNM) buf.savePNMdata(argv[2]);
   else if (dst==FILE_TYPE_PVM) buf.savePVMdata(argv[2]);

   // release buffer
   buf.release();

   return(0);
   }
