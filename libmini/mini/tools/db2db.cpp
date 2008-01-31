#include "minibase.h"

#include "miniio.h"

#include "database.h"
#include "datacalc.h"

#include "squishbase.h"

#include "jpegbase.h"
#include "pngbase.h"

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
      FILE_TYPE_JPG,
      FILE_TYPE_PNG,
      FILE_TYPE_ETC
      };

   FILE_TYPE src,dst;
   char *src_ext,*dst_ext;

   unsigned char *jpgdata;
   unsigned int jpgbytes;

   int jpgwidth,jpgheight,jpgcomponents;

   float jpgquality=0.9f;

   unsigned char *pngdata;
   unsigned int pngbytes;

   int pngwidth,pngheight,pngcomponents;

   unsigned char *rawdata;

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
      else if (strcmp(src_ext,".jpg")==0) src=FILE_TYPE_JPG;
      else if (strcmp(src_ext,".png")==0) src=FILE_TYPE_PNG;

   dst=FILE_TYPE_ETC;

   if (dst_ext!=NULL)
      if (strcmp(dst_ext,".db")==0) dst=FILE_TYPE_DB;
      else if (strcmp(dst_ext,".pgm")==0) dst=FILE_TYPE_PNM;
      else if (strcmp(dst_ext,".ppm")==0) dst=FILE_TYPE_PNM;
      else if (strcmp(dst_ext,".pvm")==0) dst=FILE_TYPE_PVM;
      else if (strcmp(dst_ext,".jpg")==0) dst=FILE_TYPE_JPG;
      else if (strcmp(dst_ext,".png")==0) dst=FILE_TYPE_PNG;

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
   else if (src==FILE_TYPE_JPG)
      {
      jpgdata=readfile(argv[1],&jpgbytes);
      rawdata=decompressJPEGimage(jpgdata,jpgbytes,&jpgwidth,&jpgheight,&jpgcomponents);

      if (jpgcomponents==1) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,0);
      else if (jpgcomponents==3) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,3);
      else if (jpgcomponents==4) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,4);
      }
   else if (src==FILE_TYPE_PNG)
      {
      pngdata=readfile(argv[1],&pngbytes);
      rawdata=decompressPNGimage(pngdata,pngbytes,&pngwidth,&pngheight,&pngcomponents);

      if (pngcomponents==1) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,0);
      else if (pngcomponents==2) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,1); //!! MSB?
      else if (pngcomponents==3) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,3);
      else if (pngcomponents==4) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,4);
      }

   // compress to or decompress from s3tc
   if (dst==FILE_TYPE_DB) buf.autocompress();
   else buf.autodecompress();

   // save buffer
   if (dst==FILE_TYPE_DB) buf.savedata(argv[2]);
   else if (dst==FILE_TYPE_PNM) buf.savePNMdata(argv[2]);
   else if (dst==FILE_TYPE_PVM) buf.savePVMdata(argv[2]);
   else if (dst==FILE_TYPE_JPG && buf.zsize==1 && buf.tsteps==1)
      {
      if (buf.type==0) compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,1,jpgquality,&jpgdata,&jpgbytes);
      else if (buf.type==3) compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,3,jpgquality,&jpgdata,&jpgbytes);
      else if (buf.type==4) compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,4,jpgquality,&jpgdata,&jpgbytes);

      writefile(argv[2],jpgdata,jpgbytes);
      }
   else if (dst==FILE_TYPE_PNG && buf.zsize==1 && buf.tsteps==1)
      {
      if (buf.type==0) compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,1,&pngdata,&pngbytes);
      else if (buf.type==1) compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,2,&pngdata,&pngbytes); //!! MSB?
      else if (buf.type==3) compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,3,&pngdata,&pngbytes);
      else if (buf.type==4) compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,4,&pngdata,&pngbytes);

      writefile(argv[2],pngdata,pngbytes);
      }

   // release buffer
   buf.release();

   return(0);
   }
