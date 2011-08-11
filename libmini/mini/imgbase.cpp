// (c) by Stefan Roettger

#include <mini/minibase.h>

#include <mini/miniio.h>

#include <mini/database.h>
#include <mini/datacalc.h>

#include <mini/squishbase.h>

#include <mini/jpegbase.h>
#include <mini/pngbase.h>

#include <mini/convbase.h>

#include "imgbase.h"

datacalc imgbase::CALC;

// S3TC auto-compression hook
void imgbase::autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                           unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,
                           void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::compressS3TC(isrgbadata,rawdata,bytes,
                            s3tcdata,s3tcbytes,width,height,
                            squishbase::SQUISHMODE_GOOD);
   }

// S3TC auto-decompression hook
void imgbase::autodecompress(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,
                             unsigned char **rawdata,unsigned int *rawbytes,int width,int height,
                             void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::decompressS3TC(isrgbadata,s3tcdata,bytes,
                              rawdata,rawbytes,width,height);
   }

// load image based on extension
int imgbase::loadimg(databuf &buf,char *filename)
   {
   FILE_TYPE type;
   char *ext;

   BOOLINT succeed;

   unsigned char *jpgdata;
   unsigned int jpgbytes;

   int jpgwidth,jpgheight,jpgcomponents;

   unsigned char *pngdata;
   unsigned int pngbytes;

   int pngwidth,pngheight,pngcomponents;

   unsigned char *rawdata;

   convbase::MINI_CONVERSION_PARAMS conversion_params;

   if (checkfile(filename)==0) return(0);

   ext=strrchr(filename,'.');

   type=FILE_TYPE_ETC;

   if (ext!=NULL)
      if (strstr(ext,".db")!=0) type=FILE_TYPE_DB;
      else if (strstr(ext,".pnm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".pgm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".ppm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".pvm")!=0) type=FILE_TYPE_PVM;
      else if (strcmp(ext,".jpg")==0) type=FILE_TYPE_JPG;
      else if (strcmp(ext,".png")==0) type=FILE_TYPE_PNG;

   // register libMini conversion hook
   if (convbase::check_conversion()==0)
      {
      convbase::setparameters(&conversion_params);
      convbase::setconversion(&conversion_params);
      }

   // register implicit calculator
   if (databuf::check_interpreter()==0) CALC.doregister();

   // load buffer
   succeed=1;
   if (type==FILE_TYPE_DB) succeed=buf.loaddata(filename);
   else if (type==FILE_TYPE_PNM) succeed=buf.loadPNMdata(filename);
   else if (type==FILE_TYPE_PVM) succeed=buf.loadPVMdata(filename);
   else if (type==FILE_TYPE_JPG)
      {
      jpgdata=readfile(filename,&jpgbytes);
      rawdata=jpegbase::decompressJPEGimage(jpgdata,jpgbytes,&jpgwidth,&jpgheight,&jpgcomponents);
      free(jpgdata);

      if (rawdata==NULL) return(0);

      if (jpgcomponents==1) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,0);
      else if (jpgcomponents==3) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,3);
      else if (jpgcomponents==4) buf.set(rawdata,jpgwidth*jpgheight*jpgcomponents,jpgwidth,jpgheight,1,1,4);
      }
   else if (type==FILE_TYPE_PNG)
      {
      pngdata=readfile(filename,&pngbytes);
      rawdata=pngbase::decompressPNGimage(pngdata,pngbytes,&pngwidth,&pngheight,&pngcomponents);
      free(pngdata);

      if (rawdata==NULL) return(0);

      if (pngcomponents==1) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,0);
      else if (pngcomponents==2)
         {
         buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,1);
         buf.swap2();
         }
      else if (pngcomponents==3) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,3);
      else if (pngcomponents==4) buf.set(rawdata,pngwidth*pngheight*pngcomponents,pngwidth,pngheight,1,1,4);
      }
   else succeed=0;

   return(succeed);
   }

// save image based on extension
int imgbase::saveimg(databuf &buf,char *filename,float jpgquality,int pnglevel)
   {
   FILE_TYPE type;
   char *ext;

   unsigned char *jpgdata;
   unsigned int jpgbytes;

   unsigned char *pngdata;
   unsigned int pngbytes;

   ext=strrchr(filename,'.');

   type=FILE_TYPE_ETC;

   if (ext!=NULL)
      if (strstr(ext,".db")!=0) type=FILE_TYPE_DB;
      else if (strstr(ext,".pnm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".pgm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".ppm")!=0) type=FILE_TYPE_PNM;
      else if (strstr(ext,".pvm")!=0) type=FILE_TYPE_PVM;
      else if (strcmp(ext,".jpg")==0) type=FILE_TYPE_JPG;
      else if (strcmp(ext,".png")==0) type=FILE_TYPE_PNG;

   // automatic mip-mapping
   if (type==FILE_TYPE_DB) buf.automipmap();

   // register auto-compression hook
   if (databuf::check_autocompress()==0) databuf::setautocompress(autocompress,NULL);

   // register auto-decompression hook
   if (databuf::check_autodecompress()==0) databuf::setautodecompress(autodecompress,NULL);

   // drop mipmaps
   if (type!=FILE_TYPE_DB) buf.drop_mipmap();

   // compress to or decompress from s3tc
   if (type==FILE_TYPE_DB) buf.autocompress();
   else buf.autodecompress();

   // save buffer
   if (type==FILE_TYPE_DB) buf.savedata(filename);
   else if (type==FILE_TYPE_PNM) buf.savePNMdata(filename);
   else if (type==FILE_TYPE_PVM) buf.savePVMdata(filename);
   else if (type==FILE_TYPE_JPG && buf.zsize==1 && buf.tsteps==1)
      {
      if (buf.type==databuf::DATABUF_TYPE_BYTE) jpegbase::compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,1,jpgquality,&jpgdata,&jpgbytes);
      else if (buf.type==databuf::DATABUF_TYPE_RGB || buf.type==databuf::DATABUF_TYPE_RGB_MM) jpegbase::compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,3,jpgquality,&jpgdata,&jpgbytes);
      else if (buf.type==databuf::DATABUF_TYPE_RGBA || buf.type==databuf::DATABUF_TYPE_RGBA_MM) jpegbase::compressJPEGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,4,jpgquality,&jpgdata,&jpgbytes);
      else return(0);

      if (jpgdata==NULL) return(0);

      writefile(filename,jpgdata,jpgbytes);
      }
   else if (type==FILE_TYPE_PNG && buf.zsize==1 && buf.tsteps==1)
      {
      if (buf.type==databuf::DATABUF_TYPE_BYTE) pngbase::compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,1,&pngdata,&pngbytes,0.0f,pnglevel);
      else if (buf.type==databuf::DATABUF_TYPE_SHORT)
         {
         buf.swap2();
         pngbase::compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,2,&pngdata,&pngbytes,0.0f,pnglevel);
         }
      else if (buf.type==databuf::DATABUF_TYPE_RGB || buf.type==databuf::DATABUF_TYPE_RGB_MM) pngbase::compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,3,&pngdata,&pngbytes,0.0f,pnglevel);
      else if (buf.type==databuf::DATABUF_TYPE_RGBA || buf.type==databuf::DATABUF_TYPE_RGBA_MM) pngbase::compressPNGimage((unsigned char *)buf.data,buf.xsize,buf.ysize,4,&pngdata,&pngbytes,0.0f,pnglevel);
      else return(0);

      if (pngdata==NULL) return(0);

      writefile(filename,pngdata,pngbytes);
      }
   else return(0);

   return(1);
   }
