// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "miniio.h"
#include "minicrs.h"

#include "pnmbase.h"
#include "pnmsample.h"

#include "miniOGL.h"

#include "datafill.h"

#include "database.h"

// magic identifier
unsigned int databuf::MAGIC1=12640; // original magic identifier of DB version 1
unsigned int databuf::MAGIC2=13048; // backwards compatibility for DB version 2
unsigned int databuf::MAGIC3=13091; // backwards compatibility for DB version 3
unsigned int databuf::MAGIC4=13269; // backwards compatibility for DB version 4
unsigned int databuf::MAGIC5=13398; // backwards compatibility for DB version 5
unsigned int databuf::MAGIC6=13761; // actual magic identifier of DB version 6

// helper variable for LSB vs. MSB check
unsigned short int databuf::INTEL_CHECK=1;

// global flag for automatic mip-mapping
int databuf::AUTOS3TCMIPMAP=0;

// static hook for conversion from and to an external format
int (*databuf::CONVERSION_HOOK)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data)=NULL;
void *databuf::CONVERSION_DATA=NULL;

// static hook for automatic s3tc compression
void (*databuf::AUTOCOMPRESS_HOOK)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,void *data)=NULL;
void *databuf::AUTOCOMPRESS_DATA=NULL;

// static hook for automatic s3tc decompression
void (*databuf::AUTODECOMPRESS_HOOK)(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,unsigned char **rawdata,unsigned int *rawbytes,int width,int height,void *data)=NULL;
void *databuf::AUTODECOMPRESS_DATA=NULL;

// static hooks for conversion from an implicit format
void (*databuf::INTERPRETER_INIT)(unsigned int implformat,const char *code,int bytes,const char *path,databuf *obj,void *data)=NULL;
void (*databuf::INTERPRETER_HOOK)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data)=NULL;
void *databuf::INTERPRETER_DATA=NULL;

// default constructor
databuf::databuf()
   {
   xsize=ysize=zsize=0;
   tsteps=0;

   type=DATABUF_TYPE_BYTE;

   comment=NULL;

   swx=swy=0.0;
   nwx=nwy=0.0;
   nex=ney=0.0;
   sex=sey=0.0;
   h0=dh=0.0;
   t0=dt=0.0;

   LLWGS84_swx=LLWGS84_swy=0.0;
   LLWGS84_nwx=LLWGS84_nwy=0.0;
   LLWGS84_nex=LLWGS84_ney=0.0;
   LLWGS84_sex=LLWGS84_sey=0.0;

   scaling=1.0f;
   bias=0.0f;

   minvalue=MAXFLOAT;
   maxvalue=-MAXFLOAT;

   crs=DATABUF_CRS_LINEAR;
   zone=datum=0;

   nodata=NAN;

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   data=NULL;
   bytes=0;
   }

// allocate a new memory chunk
void databuf::alloc(unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts,unsigned int ty)
   {
   unsigned int bs,cs;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   unsigned int count;

   bs=0;

   if (ty==DATABUF_TYPE_BYTE) bs=1;
   else if (ty==DATABUF_TYPE_SHORT) bs=2;
   else if (ty==DATABUF_TYPE_FLOAT) bs=4;
   else if (ty==DATABUF_TYPE_RGB) bs=3;
   else if (ty==DATABUF_TYPE_RGBA) bs=4;
   else ERRORMSG();

   cs=xs*ys*zs*ts;
   bs*=cs;

   if ((data=malloc(bs))==NULL) MEMERROR();

   if (ty==DATABUF_TYPE_SHORT)
      for (shortptr=(short int *)data,count=0; count<cs; count++) *shortptr++=0;
   else if (ty==DATABUF_TYPE_FLOAT)
      for (floatptr=(float *)data,count=0; count<cs; count++) *floatptr++=0.0f;
   else
      for (byteptr=(unsigned char *)data,count=0; count<bs; count++) *byteptr++=0;

   bytes=bs;

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   xsize=xs;
   ysize=ys;
   zsize=zs;
   tsteps=ts;
   type=ty;
   }

// set data to memory chunk
void databuf::set(void *chunk,unsigned int length,
                  unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts,unsigned int ty)
   {
   unsigned int bs;

   bs=0;

   if (ty==DATABUF_TYPE_BYTE) bs=1;
   else if (ty==DATABUF_TYPE_SHORT) bs=2;
   else if (ty==DATABUF_TYPE_FLOAT) bs=4;
   else if (ty==DATABUF_TYPE_RGB) bs=3;
   else if (ty==DATABUF_TYPE_RGBA) bs=4;
   else if (ty==DATABUF_TYPE_RGB_S3TC) bs=3;
   else if (ty==DATABUF_TYPE_RGBA_S3TC) bs=4;
   else if (ty==DATABUF_TYPE_RGB_MM) bs=3;
   else if (ty==DATABUF_TYPE_RGBA_MM) bs=4;
   else if (ty==DATABUF_TYPE_RGB_MM_S3TC) bs=3;
   else if (ty==DATABUF_TYPE_RGBA_MM_S3TC) bs=4;
   else ERRORMSG();

   bs*=xs*ys*zs*ts;

   if (ty==DATABUF_TYPE_RGB_S3TC || ty==DATABUF_TYPE_RGBA_S3TC) bs=length;
   if (ty==DATABUF_TYPE_RGB_MM || ty==DATABUF_TYPE_RGBA_MM) bs=length;
   if (ty==DATABUF_TYPE_RGB_MM_S3TC || ty==DATABUF_TYPE_RGBA_MM_S3TC) bs=length;

   if (length!=bs) ERRORMSG();

   data=chunk;
   bytes=bs;

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   xsize=xs;
   ysize=ys;
   zsize=zs;
   tsteps=ts;
   type=ty;
   }

// copy data from memory chunk
void databuf::copy(const void *chunk,unsigned int length,
                   unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts,unsigned int ty)
   {
   void *newdata;

   if (data!=NULL)
      {
      if (xs!=xsize || ys!=ysize || zs!=zsize || ts!=tsteps || ty!=type) ERRORMSG();
      newdata=data;
      }
   else
      if ((newdata=malloc(length))==NULL) MEMERROR();

   set(newdata,length,xs,ys,zs,ts,ty);
   memcpy(data,chunk,length);
   }

// copy data from buffer
void databuf::copy(const databuf *buf)
   {copy(buf->data,buf->bytes,buf->xsize,buf->ysize,buf->zsize,buf->tsteps,buf->type);}

// duplicate data from buffer
void databuf::duplicate(const databuf *buf)
   {
   *this=*buf;
   comment=NULL;

   alloc(buf->xsize,buf->ysize,buf->zsize,buf->tsteps,buf->type);
   copy(buf);

   set_comment(buf->comment);
   }

// clear buffer
void databuf::clear(float value)
   {
   unsigned int i;

   unsigned int c;
   int v;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   c=xsize*ysize*zsize*tsteps;
   v=ftrc(value+0.5f);

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=(unsigned char *)data;
         memset(byteptr,c,v);
         break;
      case DATABUF_TYPE_SHORT:
         shortptr=(short int *)data;
         for (i=0; i<c; i++) *shortptr++=v;
         break;
      case DATABUF_TYPE_FLOAT:
         floatptr=(float *)data;
         for (i=0; i<c; i++) *floatptr++=value;
         break;
      case DATABUF_TYPE_RGB:
         byteptr=(unsigned char *)data;
         memset(byteptr,c*3,v);
         break;
      case DATABUF_TYPE_RGBA:
         byteptr=(unsigned char *)data;
         memset(byteptr,c*4,v);
         break;
      default:
         ERRORMSG();
      }
   }

// reset buffer
void databuf::reset()
   {
   data=NULL;
   bytes=0;

   comment=NULL;
   }

// release buffer
void databuf::release(int keep_comment)
   {
   if (data!=NULL) free(data);

   data=NULL;
   bytes=0;

   if (keep_comment==0)
      {
      if (comment!=NULL) free(comment);
      comment=NULL;
      }
   }

// set comment string
void databuf::set_comment(const char *str)
   {
   if (comment!=NULL) free(comment);
   comment=NULL;

   if (str!=NULL) comment=strdup(str);
   }

// set native extents
void databuf::set_extents(double left,double right,double bottom,double top)
   {
   swx=left;
   swy=bottom;

   sex=right;
   sey=bottom;

   nwx=left;
   nwy=top;

   nex=right;
   ney=top;
   }

// set LLWGS84 extents
void databuf::set_LLWGS84extents(double left,double right,double bottom,double top)
   {
   LLWGS84_swx=left;
   LLWGS84_swy=bottom;

   LLWGS84_sex=right;
   LLWGS84_sey=bottom;

   LLWGS84_nwx=left;
   LLWGS84_nwy=top;

   LLWGS84_nex=right;
   LLWGS84_ney=top;
   }

// set native corners
void databuf::set_corners(double sw_corner_x,double sw_corner_y,
                          double se_corner_x,double se_corner_y,
                          double nw_corner_x,double nw_corner_y,
                          double ne_corner_x,double ne_corner_y)
   {
   swx=sw_corner_x;
   swy=sw_corner_y;

   sex=se_corner_x;
   sey=se_corner_y;

   nwx=nw_corner_x;
   nwy=nw_corner_y;

   nex=ne_corner_x;
   ney=ne_corner_y;
   }

// set LLWGS84 corners
void databuf::set_LLWGS84corners(double sw_corner_x,double sw_corner_y,
                                 double se_corner_x,double se_corner_y,
                                 double nw_corner_x,double nw_corner_y,
                                 double ne_corner_x,double ne_corner_y)
   {
   LLWGS84_swx=sw_corner_x;
   LLWGS84_swy=sw_corner_y;

   LLWGS84_sex=se_corner_x;
   LLWGS84_sey=se_corner_y;

   LLWGS84_nwx=nw_corner_x;
   LLWGS84_nwy=nw_corner_y;

   LLWGS84_nex=ne_corner_x;
   LLWGS84_ney=ne_corner_y;
   }

// set linear mapping
void databuf::set_mapping(float lin_scaling,float lin_bias)
   {
   scaling=lin_scaling;
   bias=lin_bias;
   }

// set height extent
void databuf::set_height(double bottom,double height)
   {
   h0=bottom;
   dh=height;
   }

// set time range
void databuf::set_time(double time,double range)
   {
   t0=time;
   dt=range;
   }

// set coordinate system
void databuf::set_crs(int crs_type,int crs_zone,int crs_datum)
   {
   crs=crs_type;
   zone=crs_zone;
   datum=crs_datum;
   }

// set the no-data indicator
void databuf::set_nodata(float value)
   {nodata=value;}

// write one double parameter
void databuf::writeparam(const char *tag,double v,FILE *file,int digits)
   {
   const char *ptr;

   double value,point;
   int digit;

   for (ptr=tag; *ptr!='\0'; ptr++) putc(*ptr,file);

   putc('=',file);

   if (v<0.0)
      {
      putc('-',file);
      value=-v;
      }
   else value=v;

   if (value==MAXFLOAT)
      {
      putc('m',file);
      putc('a',file);
      putc('x',file);
      }
   else if (isNAN(value))
      {
      putc('n',file);
      putc('a',file);
      putc('n',file);
      }
   else
      {
      point=1.0;

      while (10.0*point<=value) point*=10.0;

      if (value<1.0) putc('0',file);
      else
         while (point>=1.0)
            {
            digit=(int)floor(value/point);

            if (digit<0) digit=0;
            else if (digit>9) digit=9;

            value-=digit*point;
            point/=10.0;

            putc(digit+'0',file);

            digits--;
            }

      if (value>0.0)
         {
         putc('.',file);

         point=0.1;

         while (value>0.0 && digits>0)
            {
            if (digits>1) digit=(int)floor(value/point);
            else digit=(int)floor(value/point+0.5);

            if (digit<0) digit=0;
            else if (digit>9) digit=9;

            value-=digit*point;
            point/=10.0;

            putc(digit+'0',file);

            digits--;
            }
         }
      }

   putc('\n',file);
   }

// read one double parameter
int databuf::readparam(const char *tag,double *v,FILE *file)
   {
   char ch;
   const char *ptr;

   double value,point;
   double expnt,exsgn;

   ch=getc(file);

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   for (ptr=tag; ch==*ptr && *ptr!='\0'; ptr++) ch=getc(file);

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   if (ch!='=')
      {
      ungetc(ch,file);
      return(0);
      }

   ch=getc(file);

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   if (ch=='-')
      {
      point=-1.0;
      ch=getc(file);
      }
   else point=1.0;

   if (ch=='m')
      {
      ch=getc(file);

      if (ch=='a')
         {
         ch=getc(file);

         if (ch=='x') ch=getc(file);
         }

      value=MAXFLOAT;

      expnt=exsgn=0.0;
      }
   else if (ch=='n')
      {
      ch=getc(file);

      if (ch=='a')
         {
         ch=getc(file);

         if (ch=='n') ch=getc(file);
         }

      value=NAN;

      expnt=exsgn=0.0;
      }
   else
      {
      value=0.0;

      while (ch>='0' && ch<='9')
         {
         value=10.0*value+ch-'0';
         ch=getc(file);
         }

      if (ch=='.')
         {
         ch=getc(file);

         while (ch>='0' && ch<='9')
            {
            value=10.0*value+ch-'0';
            point*=10.0;

            ch=getc(file);
            }
         }

      if (ch=='e' || ch=='E')
         {
         ch=getc(file);

         if (ch=='-')
            {
            exsgn=-1.0;
            ch=getc(file);
            }
         else exsgn=1.0;

         expnt=0.0;

         while (ch>='0' && ch<='9')
            {
            expnt=10.0*expnt+ch-'0';
            ch=getc(file);
            }
         }
      else expnt=exsgn=0.0;
      }

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   ungetc(ch,file);

   *v=value/point*pow(10.0,exsgn*expnt);

   return(1);
   }

// read one float parameter
int databuf::readparamf(const char *tag,float *v,FILE *file)
   {
   double value;

   if (readparam(tag,&value,file)==0) return(0);

   *v=(float)value;

   return(1);
   }

// read one integer parameter
int databuf::readparami(const char *tag,int *v,FILE *file)
   {
   double value;

   if (readparam(tag,&value,file)==0) return(0);

   *v=dtrc(value+0.5);

   return(1);
   }

// read one unsigned parameter
int databuf::readparamu(const char *tag,unsigned int *v,FILE *file)
   {
   double value;

   if (readparam(tag,&value,file)==0) return(0);

   *v=dtrc(dabs(value)+0.5);

   return(1);
   }

// write one string parameter
void databuf::writestring(const char *tag,const char *str,FILE *file)
   {
   const char *ptr;

   for (ptr=tag; *ptr!='\0'; ptr++) putc(*ptr,file);

   putc('=',file);

   if (str!=NULL)
      for (ptr=str; *ptr!='\0' && *ptr!='\n'; ptr++) putc(*ptr,file);

   putc('\n',file);
   }

// read one string parameter
int databuf::readstring(const char *tag,char **str,FILE *file)
   {
   int ch;
   const char *ptr;

   ch=getc(file);

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   for (ptr=tag; ch==*ptr && *ptr!='\0'; ptr++) ch=getc(file);

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   if (ch!='=')
      {
      ungetc(ch,file);
      return(0);
      }

   ch=getc(file);

   *str=NULL;

   while (ch!='\n' && ch!=EOF)
      {
      if (*str==NULL)
         {
         if ((*str=(char *)malloc(2))==NULL) MEMERROR();

         (*str)[0]=ch;
         (*str)[1]='\0';
         }
      else
         {
         if ((*str=(char *)realloc(*str,strlen(*str)+2))==NULL) MEMERROR();

         (*str)[strlen(*str)+1]='\0';
         (*str)[strlen(*str)]=ch;
         }

      ch=getc(file);
      }

   return(1);
   }

// load DB data block
void databuf::loadblock(FILE *file)
   {
   const unsigned int block=1<<17;

   unsigned int cnt;

   if ((data=(unsigned char *)malloc(block))==NULL) MEMERROR();

   do
      {
      cnt=fread(&((unsigned char *)data)[bytes],1,block,file);
      bytes+=cnt;

      if (cnt==block)
         if ((data=(unsigned char *)realloc(data,bytes+block))==NULL) MEMERROR();
      }
   while (cnt==block);

   if (bytes==0) IOERROR();

   if ((data=(unsigned char *)realloc(data,bytes))==NULL) MEMERROR();
   }

// data is saved in DB format
// data is converted from native to MSB byte order
int databuf::savedata(const char *filename,
                      unsigned int extfmt)
   {
   FILE *file;

   if (data==NULL) return(0);

   if (bytes==0) ERRORMSG();

   // convert into external format
   if (extfmt!=DATABUF_EXTFMT_PLAIN) convertchunk(1,extfmt);

   // open file for writing
   if ((file=fopen(filename,"wb"))==NULL)
      {
#ifdef LIBMINI_DEBUG
      fprintf(stderr,"unable to save %s!\n",filename);
#endif
      return(0);
      }

   // save magic identifier
   writeparam("MAGIC",MAGIC6,file);

   // save mandatory metadata
   writeparam("xsize",xsize,file);
   writeparam("ysize",ysize,file);
   writeparam("zsize",zsize,file);
   writeparam("tsteps",tsteps,file);
   writeparam("type",type,file);

   // save optional comment
   writestring("comment",comment,file);

   // save optional metadata
   writeparam("swx",swx,file);
   writeparam("swy",swy,file);
   writeparam("nwx",nwx,file);
   writeparam("nwy",nwy,file);
   writeparam("nex",nex,file);
   writeparam("ney",ney,file);
   writeparam("sex",sex,file);
   writeparam("sey",sey,file);
   writeparam("h0",h0,file);
   writeparam("dh",dh,file);
   writeparam("t0",t0,file);
   writeparam("dt",dt,file);

   // save optional scaling
   writeparam("scaling",scaling,file);
   writeparam("bias",bias,file);

   // save coordinate system indicator
   writeparam("crs",crs,file);
   writeparam("zone",zone,file);
   writeparam("datum",datum,file);

   // save no-data indicator
   writeparam("nodata",nodata,file);

   // save external format indicator
   writeparam("extformat",extformat,file);

   // save implicit format indicator
   writeparam("implformat",implformat,file);

   // save optional corner points in Lat/Lon
   writeparam("LLWGS84_swx",LLWGS84_swx,file);
   writeparam("LLWGS84_swy",LLWGS84_swy,file);
   writeparam("LLWGS84_nwx",LLWGS84_nwx,file);
   writeparam("LLWGS84_nwy",LLWGS84_nwy,file);
   writeparam("LLWGS84_nex",LLWGS84_nex,file);
   writeparam("LLWGS84_ney",LLWGS84_ney,file);
   writeparam("LLWGS84_sex",LLWGS84_sex,file);
   writeparam("LLWGS84_sey",LLWGS84_sey,file);

   // save length of data chunk
   writeparam("bytes",bytes,file);

   // save terminating character
   putc('\0',file);

   // save data chunk
   if (!intel_check() || extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0)
      {
      if (fwrite(data,bytes,1,file)!=1) IOERROR();
      fclose(file);
      }
   else
      {
      swapbytes();
      if (fwrite(data,bytes,1,file)!=1) IOERROR();
      swapbytes();

      fclose(file);
      }

   return(1);
   }

// data is loaded from DB file
// data is converted from MSB to native byte order
// if stub!=0 the loading of the data chunk is omitted
// if tstart<tstop the time steps in the range [tstart,tstop[ are loaded
int databuf::loaddata(const char *filename,int stub,unsigned int tstart,unsigned int tstop)
   {
   FILE *file;

   unsigned int m;

   unsigned int tstep;

   // open file for reading
   if ((file=fopen(filename,"rb"))==NULL)
      {
#ifdef LIBMINI_DEBUG
      fprintf(stderr,"unable to load %s!\n",filename);
#endif
      return(0);
      }

   // read magic identifier
   if (readparamu("MAGIC",&m,file)==0)
      {
      fclose(file);
      return(0);
      }
   else if (m!=MAGIC1 && m!=MAGIC2 && m!=MAGIC3 && m!=MAGIC4 && m!=MAGIC5 && m!=MAGIC6)
      {
      fclose(file);
      return(0);
      }

   // read mandatory metadata
   if (readparamu("xsize",&xsize,file)==0) ERRORMSG();
   if (readparamu("ysize",&ysize,file)==0) ERRORMSG();
   if (readparamu("zsize",&zsize,file)==0) ERRORMSG();
   if (readparamu("tsteps",&tsteps,file)==0) ERRORMSG();
   if (readparamu("type",&type,file)==0) ERRORMSG();

   // read comment
   if (m==MAGIC1 || m==MAGIC2 || m==MAGIC3 || m==MAGIC4 || m==MAGIC5) comment=NULL;
   else if (readstring("comment",&comment,file)==0) ERRORMSG();

   // read optional metadata
   if (readparam("swx",&swx,file)==0) ERRORMSG();
   if (readparam("swy",&swy,file)==0) ERRORMSG();
   if (readparam("nwx",&nwx,file)==0) ERRORMSG();
   if (readparam("nwy",&nwy,file)==0) ERRORMSG();
   if (readparam("nex",&nex,file)==0) ERRORMSG();
   if (readparam("ney",&ney,file)==0) ERRORMSG();
   if (readparam("sex",&sex,file)==0) ERRORMSG();
   if (readparam("sey",&sey,file)==0) ERRORMSG();
   if (readparam("h0",&h0,file)==0) ERRORMSG();
   if (readparam("dh",&dh,file)==0) ERRORMSG();
   if (readparam("t0",&t0,file)==0) ERRORMSG();
   if (readparam("dt",&dt,file)==0) ERRORMSG();

   // read optional scaling
   if (readparamf("scaling",&scaling,file)==0) ERRORMSG();
   if (readparamf("bias",&bias,file)==0) ERRORMSG();

   // read coordinate system indicator
   if (m==MAGIC1 || m==MAGIC2 || m==MAGIC3 || m==MAGIC4)
      {
      crs=DATABUF_CRS_LINEAR;
      zone=datum=0;
      }
   else
      {
      if (readparami("crs",&crs,file)==0) ERRORMSG();
      if (readparami("zone",&zone,file)==0) ERRORMSG();
      if (readparami("datum",&datum,file)==0) ERRORMSG();
      }

   // read no-data indicator
   if (m==MAGIC1 || m==MAGIC2 || m==MAGIC3 || m==MAGIC4) nodata=-MAXFLOAT;
   else if (readparamf("nodata",&nodata,file)==0) ERRORMSG();

   // read external format indicator
   if (m==MAGIC1) extformat=DATABUF_EXTFMT_PLAIN;
   else if (readparamu("extformat",&extformat,file)==0) ERRORMSG();

   // read implicit format indicator
   if (m==MAGIC1 || m==MAGIC2) implformat=0;
   else if (readparamu("implformat",&implformat,file)==0) ERRORMSG();

   // read optional corner points in Lat/Lon
   if (m==MAGIC1 || m==MAGIC2 || m==MAGIC3)
      {
      LLWGS84_swx=LLWGS84_swy=0.0;
      LLWGS84_nwx=LLWGS84_nwy=0.0;
      LLWGS84_nex=LLWGS84_ney=0.0;
      LLWGS84_sex=LLWGS84_sey=0.0;
      }
   else
      {
      if (readparam("LLWGS84_swx",&LLWGS84_swx,file)==0) ERRORMSG();
      if (readparam("LLWGS84_swy",&LLWGS84_swy,file)==0) ERRORMSG();
      if (readparam("LLWGS84_nwx",&LLWGS84_nwx,file)==0) ERRORMSG();
      if (readparam("LLWGS84_nwy",&LLWGS84_nwy,file)==0) ERRORMSG();
      if (readparam("LLWGS84_nex",&LLWGS84_nex,file)==0) ERRORMSG();
      if (readparam("LLWGS84_ney",&LLWGS84_ney,file)==0) ERRORMSG();
      if (readparam("LLWGS84_sex",&LLWGS84_sex,file)==0) ERRORMSG();
      if (readparam("LLWGS84_sey",&LLWGS84_sey,file)==0) ERRORMSG();
      }

   // read length of data chunk
   if (readparamu("bytes",&bytes,file)==0) ERRORMSG();

   // read terminating character
   if (bytes!=0)
      if (getc(file)!='\0') ERRORMSG();

   // check for stub
   if (stub==0)
      {
      // read data chunk
      if (bytes==0)
         {
         loadblock(file);
         fclose(file);
         }
      else
         {
         if (tstart<tstop && extformat==DATABUF_EXTFMT_PLAIN && implformat==0)
            {
            tstep=xsize*ysize*zsize;

            if (type==DATABUF_TYPE_BYTE) tstep=1*tstep;
            else if (type==DATABUF_TYPE_SHORT) tstep=2*tstep;
            else if (type==DATABUF_TYPE_FLOAT) tstep=4*tstep;
            else if (type==DATABUF_TYPE_RGB) tstep=3*tstep;
            else if (type==DATABUF_TYPE_RGBA) tstep=4*tstep;
            else ERRORMSG();

            if (fseek(file,tstart*tstep,SEEK_CUR)!=0) IOERROR();

            tsteps=tstop-tstart;
            bytes=tsteps*tstep;
            t0+=tstart*dt;
            }

         if ((data=(unsigned char *)malloc(bytes))==NULL) MEMERROR();

         if (fread(data,bytes,1,file)!=1) IOERROR();
         fclose(file);
         }

      // check for LSB->MSB conversion
      if (intel_check() && extformat==DATABUF_EXTFMT_PLAIN && implformat==0) swapbytes();

      // convert from external format
      if (extformat!=DATABUF_EXTFMT_PLAIN) convertchunk(0,extformat);

      // convert from implicit format
      if (implformat!=0) interpretechunk(implformat,filename);
      }

   return(1);
   }

// convert byte order
void databuf::swap2(int is_msb)
   {
   if (extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0) return;

   if (!intel_check())
      if (is_msb!=0) return;
      else swapbytes();
   else
      if (is_msb!=0) swapbytes();
      else return;
   }

// convert to signed short (the default)
void databuf::convert2(int is_ushort,int to_ushort)
   {
   unsigned int i;

   unsigned short int maxval,*ptr;

   if (extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0) return;

   if (is_ushort!=0 && to_ushort==0)
      {
      maxval=0;
      ptr=(unsigned short int *)data;
      for (i=0; i<bytes; i+=2,ptr++)
         if (*ptr>maxval) maxval=*ptr;

      if (maxval<32768) return;

      ptr=(unsigned short int *)data;
      for (i=0; i<bytes; i+=2)
         *ptr++>>=1;
      }
   else if (is_ushort==0 && to_ushort!=0)
      {
      ptr=(unsigned short int *)data;
      for (i=0; i<bytes; i+=2,ptr++)
         if (*ptr>32767) *ptr++=0;
         else *ptr++<<=1;
      }
   }

// set conversion hook for external formats
void databuf::setconversion(int (*conversion)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data),void *data)
   {
   CONVERSION_HOOK=conversion;
   CONVERSION_DATA=data;
   }

// convert from/into external format (e.g. JPEG/PNG/Z)
void databuf::convertchunk(int israw,unsigned int extfmt)
   {
   int success;

   unsigned char *newdata;
   unsigned int newbytes;

   if (CONVERSION_HOOK==NULL)
      if (israw!=0) return;
      else ERRORMSG();

   if (israw!=0)
      if (intel_check()) swapbytes();

   success=CONVERSION_HOOK(israw,(unsigned char *)data,bytes,extfmt,&newdata,&newbytes,this,CONVERSION_DATA);

   if (!success)
      if (israw!=0) return;
      else ERRORMSG();

   release(1);

   data=newdata;
   bytes=newbytes;

   if (israw==0)
      if (intel_check()) swapbytes();

   if (israw==0) extformat=DATABUF_EXTFMT_PLAIN;
   else extformat=extfmt;
   }

// set automatic mip-mapping
void databuf::setautomipmap(int autos3tcmipmap)
   {AUTOS3TCMIPMAP=autos3tcmipmap;}

// automatic mip-mapping
void databuf::automipmap()
   {
   int i,j,k;

   int components;
   unsigned char *ptr,*ptr1,*ptr2,*ptr3,*ptr4;

   int xsize1,ysize1;
   int xsize2,ysize2;
   int bytes4;

   int value;

   if (type!=DATABUF_TYPE_RGB && type!=DATABUF_TYPE_RGBA) return;
   if (zsize>1 || tsteps>1) return;

   // check for power of 2
   if (((xsize-1)&xsize)!=0) return;
   if (((ysize-1)&ysize)!=0) return;

   if (type==DATABUF_TYPE_RGB) components=3;
   else components=4;

   xsize1=xsize;
   ysize1=ysize;

   while (xsize1>1 || ysize1>1)
      {
      xsize2=xsize1/2;
      ysize2=ysize1/2;

      if (xsize2==0) xsize2=1;
      if (ysize2==0) ysize2=1;

      bytes4=xsize2*ysize2*components;

      if ((data=realloc(data,bytes+bytes4))==NULL) MEMERROR();

      ptr=(unsigned char *)data+bytes;

      ptr1=ptr2=ptr3=ptr4=(unsigned char *)data+bytes-xsize1*ysize1*components;

      if (xsize1>1)
         {
         ptr2+=components;
         ptr4+=components;
         }

      if (ysize1>1)
         {
         ptr3+=xsize1*components;
         ptr4+=xsize1*components;
         }

      for (j=0; j<ysize2; j++)
         {
         for (i=0; i<xsize2; i++)
            {
            for (k=0; k<components; k++)
               {
               value=(*ptr1++)+(*ptr2++)+(*ptr3++)+(*ptr4++);
               (*ptr++)=(value+2)/4;
               }

            if (xsize1>1)
               {
               ptr1+=components;
               ptr2+=components;
               ptr3+=components;
               ptr4+=components;
               }
            }

         if (ysize1>1)
            {
            ptr1+=xsize1*components;
            ptr2+=xsize1*components;
            ptr3+=xsize1*components;
            ptr4+=xsize1*components;
            }
         }

      bytes+=bytes4;

      xsize1=xsize2;
      ysize1=ysize2;
      }

   if (type==DATABUF_TYPE_RGB) type=DATABUF_TYPE_RGB_MM;
   else type=DATABUF_TYPE_RGBA_MM;
   }

// check mip-mapping
int databuf::check_mipmap()
   {
   if (type==DATABUF_TYPE_RGB_MM ||
       type==DATABUF_TYPE_RGBA_MM ||
       type==DATABUF_TYPE_RGB_MM_S3TC ||
       type==DATABUF_TYPE_RGBA_MM_S3TC) return(1);

   return(0);
   }

// drop mip-mapping
void databuf::drop_mipmap()
   {
   if (type==DATABUF_TYPE_RGB_MM)
      {
      type=DATABUF_TYPE_RGB;
      bytes=3*xsize*ysize; // 3 bytes per pixel
      }
   else if (type==DATABUF_TYPE_RGBA_MM)
      {
      type=DATABUF_TYPE_RGBA;
      bytes=4*xsize*ysize; // 4 bytes per pixel
      }
   else if (type==DATABUF_TYPE_RGB_MM_S3TC)
      {
      type=DATABUF_TYPE_RGB_S3TC;
      bytes=8*((xsize+3)/4)*((ysize+3)/4); // 8 bytes per 4x4 s3tc/dxt1 rgba block
      }
   else if (type==DATABUF_TYPE_RGBA_MM_S3TC)
      {
      type=DATABUF_TYPE_RGBA_S3TC;
      bytes=8*((xsize+3)/4)*((ysize+3)/4); // 8 bytes per 4x4 s3tc/dxt1 rgba block
      }
   }

// set hook for automatic s3tc compression
void databuf::setautocompress(void (*autocompress)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,void *data),void *data)
   {
   AUTOCOMPRESS_HOOK=autocompress;
   AUTOCOMPRESS_DATA=data;
   }

// automatic s3tc compression
void databuf::autocompress()
   {
   unsigned char *s3tcdata;
   unsigned int s3tcbytes;

   if (AUTOS3TCMIPMAP!=0) automipmap();

   if (AUTOCOMPRESS_HOOK==NULL) return;

   if (type!=DATABUF_TYPE_RGB && type!=DATABUF_TYPE_RGBA && type!=DATABUF_TYPE_RGB_MM && type!=DATABUF_TYPE_RGBA_MM) return;
   if (zsize>1 || tsteps>1) return;

   if (type==DATABUF_TYPE_RGB) AUTOCOMPRESS_HOOK(0,(unsigned char *)data,bytes,&s3tcdata,&s3tcbytes,xsize,ysize,AUTOCOMPRESS_DATA);
   else if (type==DATABUF_TYPE_RGBA) AUTOCOMPRESS_HOOK(1,(unsigned char *)data,bytes,&s3tcdata,&s3tcbytes,xsize,ysize,AUTOCOMPRESS_DATA);
   else if (type==DATABUF_TYPE_RGB_MM) autocompress_mipmaps(0,&s3tcdata,&s3tcbytes);
   else autocompress_mipmaps(1,&s3tcdata,&s3tcbytes);

   if (s3tcdata!=NULL)
      {
      release(1);

      data=s3tcdata;
      bytes=s3tcbytes;

      if (type==DATABUF_TYPE_RGB) type=DATABUF_TYPE_RGB_S3TC;
      else if (type==DATABUF_TYPE_RGBA) type=DATABUF_TYPE_RGBA_S3TC;
      else if (type==DATABUF_TYPE_RGB_MM) type=DATABUF_TYPE_RGB_MM_S3TC;
      else type=DATABUF_TYPE_RGBA_MM_S3TC;
      }
   }

// automatic mip-map compression
void databuf::autocompress_mipmaps(int isrgbadata,unsigned char **s3tcdata,unsigned int *s3tcbytes)
   {
   unsigned char *mipmap,*s3tcmipmap;
   unsigned int bytesmm,s3tcbytesmm;

   int width,height;
   int components;

   if (isrgbadata==0) components=3;
   else components=4;

   width=xsize;
   height=ysize;

   mipmap=(unsigned char *)data;

   *s3tcdata=NULL;
   *s3tcbytes=0;

   while (width>0 && height>0)
      {
      bytesmm=width*height*components;

      AUTOCOMPRESS_HOOK(isrgbadata,mipmap,bytesmm,&s3tcmipmap,&s3tcbytesmm,width,height,AUTOCOMPRESS_DATA);

      if (*s3tcdata==NULL)
         {
         *s3tcdata=s3tcmipmap;
         *s3tcbytes=s3tcbytesmm;
         }
      else
         {
         if ((*s3tcdata=(unsigned char *)realloc(*s3tcdata,*s3tcbytes+s3tcbytesmm))==NULL) MEMERROR();

         memcpy(*s3tcdata+*s3tcbytes,s3tcmipmap,s3tcbytesmm);
         free(s3tcmipmap);

         *s3tcbytes+=s3tcbytesmm;
         }

      width/=2;
      height/=2;

      if (width>0 || height>0)
         {
         if (width==0) width=1;
         if (height==0) height=1;

         mipmap+=bytesmm;
         }
      }
   }

// check s3tc compression
int databuf::check_s3tc()
   {
   if (type==DATABUF_TYPE_RGB_S3TC ||
       type==DATABUF_TYPE_RGBA_S3TC ||
       type==DATABUF_TYPE_RGB_MM_S3TC ||
       type==DATABUF_TYPE_RGBA_MM_S3TC) return(1);

   return(0);
   }

// set hook for automatic s3tc decompression
void databuf::setautodecompress(void (*autodecompress)(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,unsigned char **rawdata,unsigned int *rawbytes,int width,int height,void *data),void *data)
   {
   AUTODECOMPRESS_HOOK=autodecompress;
   AUTODECOMPRESS_DATA=data;
   }

// automatic s3tc decompression
void databuf::autodecompress()
   {
   unsigned char *rawdata;
   unsigned int rawbytes;

   if (AUTODECOMPRESS_HOOK==NULL) return;

   if (type!=DATABUF_TYPE_RGB_S3TC && type!=DATABUF_TYPE_RGBA_S3TC) return;
   if (zsize>1 || tsteps>1) return;

   if (type==DATABUF_TYPE_RGB_S3TC) AUTODECOMPRESS_HOOK(0,(unsigned char *)data,bytes,&rawdata,&rawbytes,xsize,ysize,AUTODECOMPRESS_DATA);
   else AUTODECOMPRESS_HOOK(1,(unsigned char *)data,bytes,&rawdata,&rawbytes,xsize,ysize,AUTODECOMPRESS_DATA);

   if (rawdata!=NULL)
      {
      release(1);

      data=rawdata;
      bytes=rawbytes;

      if (type==DATABUF_TYPE_RGB_S3TC) type=DATABUF_TYPE_RGB;
      else type=DATABUF_TYPE_RGBA;
      }
   }

// set interpreter hook for implicit format
void databuf::setinterpreter(void (*parser)(unsigned int implformat,const char *code,int bytes,const char *path,databuf *obj,void *data),void *data,
                             void (*interpreter)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data))
   {
   INTERPRETER_INIT=parser;
   INTERPRETER_HOOK=interpreter;
   INTERPRETER_DATA=data;
   }

// convert from implicit format
void databuf::interpretechunk(unsigned int implfmt,const char *path)
   {
   int i;
   unsigned int x,y,z,t;
   float xx,yy,zz,tt;
   float value[4];
   int comps;

   if (INTERPRETER_INIT==NULL || INTERPRETER_HOOK==NULL) ERRORMSG();

   if (type!=DATABUF_TYPE_BYTE && type!=DATABUF_TYPE_SHORT && type!=DATABUF_TYPE_FLOAT &&
       type!=DATABUF_TYPE_RGB && type !=DATABUF_TYPE_RGBA) ERRORMSG();

   if (type==DATABUF_TYPE_RGB) comps=3;
   else if (type==DATABUF_TYPE_RGBA) comps=4;
   else comps=1;

   INTERPRETER_INIT(implfmt,(char *)data,bytes,path,this,INTERPRETER_DATA);

   release(1);
   alloc(xsize,ysize,zsize,tsteps,type);

   scaling=1.0f/255.0f;
   bias=0.0f;

   for (x=0; x<xsize; x++)
      for (y=0; y<ysize; y++)
         for (z=0; z<zsize; z++)
            for (t=0; t<tsteps; t++)
               {
               if (xsize>1) xx=(float)x/(xsize-1);
               else xx=0.0f;

               if (ysize>1) yy=(float)y/(ysize-1);
               else yy=0.0f;

               if (zsize>1) zz=(float)z/(zsize-1);
               else zz=0.0f;

               if (tsteps>1) tt=(float)t/(tsteps-1);
               else tt=0.0f;

               INTERPRETER_HOOK(value,comps,xx-0.5f,yy-0.5f,zz-0.5f,tt,this,INTERPRETER_DATA);

               for (i=0; i<comps; i++)
                  if (value[i]<0.0f) value[i]=0.0f;
                  else if (value[i]>1.0f) value[i]=1.0f;

               if (comps==1) setval(x,y,z,t,value[0]);
               else if (comps==3) setrgb(x,y,z,t,value);
               else setrgba(x,y,z,t,value);
               }
   }

// read one line in either Unix or Windows style
char *databuf::readoneline(FILE *file)
   {
   static const int maxstr=1000;

   static char str[maxstr],ch;

   int n;

   n=0;

   ch=fgetc(file);

   while (ch!='\n')
      {
      if (ch!='\r' && n<maxstr-1) str[n++]=ch;
      ch=fgetc(file);
      }

   str[n]='\0';

   // remove comments
   if (strchr(str,'#')!=NULL) *strchr(str,'#')='\0';
   if (strchr(str,';')!=NULL) *strchr(str,';')='\0';

   // remove trailing space
   while (strlen(str)>0)
      if (str[strlen(str)-1]==' ') str[strlen(str)-1]='\0';
      else break;

   return(str);
   }

// load data from PNM file
// data is converted from MSB into native byte order
int databuf::loadPNMdata(const char *filename)
   {
   int width,height,components;
   PNMcomment comment;

   double coord[8];
   float cellsize[2],vscale;
   int utm_zone,utm_datum,missing;

   if ((data=readPNMfile(filename,&width,&height,&components,&comment))==NULL)
      {
#ifdef LIBMINI_DEBUG
      fprintf(stderr,"unable to load %s!\n",filename);
#endif
      return(0);
      }

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   xsize=width;
   ysize=height;
   zsize=1;
   tsteps=1;

   if (components==1) type=DATABUF_TYPE_BYTE;
   else if (components==2) type=DATABUF_TYPE_SHORT;
   else if (components==3) type=DATABUF_TYPE_RGB;
   else if (components==4) type=DATABUF_TYPE_RGBA;
   else ERRORMSG();

   bytes=xsize*ysize;
   if (type==DATABUF_TYPE_SHORT) bytes*=2;
   else if (type==DATABUF_TYPE_RGB) bytes*=3;
   else if (type==DATABUF_TYPE_RGBA) bytes*=4;

   if (type==DATABUF_TYPE_SHORT)
      if (intel_check()) swapbytes();

   if (getPNMparams(&comment,
                    coord,cellsize,
                    &vscale,&missing,
                    &utm_zone,&utm_datum)!=0)
      {
      swx=coord[0];
      swy=coord[1];
      nwx=coord[2];
      nwy=coord[3];
      nex=coord[4];
      ney=coord[5];
      sex=coord[6];
      sey=coord[7];

      if (utm_zone==0) crs=DATABUF_CRS_LLH;
      else crs=DATABUF_CRS_UTM;

      zone=utm_zone;
      datum=utm_datum;

      nodata=missing;

      if (utm_zone!=0)
         {
         minicrs::UTM2LL(coord[0],coord[1],utm_zone,utm_datum,&coord[1],&coord[0]);
         minicrs::UTM2LL(coord[2],coord[3],utm_zone,utm_datum,&coord[3],&coord[2]);
         minicrs::UTM2LL(coord[4],coord[5],utm_zone,utm_datum,&coord[5],&coord[4]);
         minicrs::UTM2LL(coord[6],coord[7],utm_zone,utm_datum,&coord[7],&coord[6]);
         }

      LLWGS84_swx=coord[0]/(60*60);
      LLWGS84_swy=coord[1]/(60*60);
      LLWGS84_nwx=coord[2]/(60*60);
      LLWGS84_nwy=coord[3]/(60*60);
      LLWGS84_nex=coord[4]/(60*60);
      LLWGS84_ney=coord[5]/(60*60);
      LLWGS84_sex=coord[6]/(60*60);
      LLWGS84_sey=coord[7]/(60*60);
      }
   else vscale=1.0f;

   h0=dh=0.0;
   t0=dt=0.0;

   scaling=vscale;
   bias=0.0f;

   return(1);
   }

// data is loaded from PPM file
// data is converted into compressed native format
// CAUTION: this method is using OpenGL -> do not call it from a background thread
int databuf::loadPPMcompressed(const char *filename)
   {
   static const int strmax=1000;

   char str[strmax];

   unsigned char *cdata;
   int cbytes;

   snprintf(str,strmax,"%s.db",filename);

   if (checkfile(str)) return(loaddata(str));
   else
      {
      loadPNMdata(filename);
      if (missing() || type!=DATABUF_TYPE_RGB) ERRORMSG();

      if (compressRGBtexmap((unsigned char *)data,xsize,ysize,&cdata,&cbytes)!=0)
         {
         free(data);

         type=DATABUF_TYPE_RGB_S3TC;

         data=cdata;
         bytes=cbytes;

         savedata(str);
         }
      }

   return(1);
   }

// data is loaded from PPM file
// data is converted into normalized and compressed native format
// CAUTION: this method is using OpenGL -> do not call it from a background thread
int databuf::loadPPMnormalized(const char *filename,const char *normalizedpath)
   {
   static const int strmax=1000;

   char str[strmax],nstr[strmax];

   unsigned char *nmap;
   int width,height,components;

   unsigned char *cdata;
   int cbytes;

   snprintf(str,strmax,"%s.dbn",filename);

   if (checkfile(str)) return(loaddata(str));
   else
      {
      loadPNMdata(filename);
      if (missing() || type!=DATABUF_TYPE_RGB) ERRORMSG();

      if (strrchr(filename,'/')==NULL) snprintf(nstr,strmax,"%s/%s",normalizedpath,filename);
      else snprintf(nstr,strmax,"%s/%s",normalizedpath,strrchr(filename,'/'));
      while (isdigit(nstr[strlen(nstr)-1])) nstr[strlen(nstr)-1]='\0';

      if ((nmap=readPNMimage((unsigned char *)nstr,&width,&height,&components))==NULL) ERRORMSG();
      if (components!=3) ERRORMSG();

      mergeRGBimages((unsigned char *)data,xsize,ysize,
                     nmap,width,height);

      free(nmap);

      if (compressRGBtexmap((unsigned char *)data,xsize,ysize,&cdata,&cbytes)!=0)
         {
         free(data);

         type=DATABUF_TYPE_RGB_S3TC;

         data=cdata;
         bytes=cbytes;
         }

      savedata(str);
      }

   return(1);
   }

// data is loaded from PVM file
int databuf::loadPVMdata(const char *filename,
                         double midx,double midy,double basez,
                         double dx,double dy,double dz,
                         BOOLINT quantize,BOOLINT tobyte)
   {
   unsigned int width,height,depth,components;
   float scalex,scaley,scalez;

   if ((data=readDDSvolume(filename,
                           &width,&height,&depth,&components,
                           &scalex,&scaley,&scalez))==NULL)
      {
#ifdef LIBMINI_DEBUG
      fprintf(stderr,"unable to load %s!\n",filename);
#endif
      return(0);
      }

   if (width<2 || height<2 || depth<2) ERRORMSG();
   if (components<1 || components>4) ERRORMSG();

   if (components!=2 || !quantize)
      {
      extformat=DATABUF_EXTFMT_PLAIN;
      implformat=0;

      xsize=width;
      ysize=height;
      zsize=depth;
      tsteps=1;

      if (components==1) type=DATABUF_TYPE_BYTE;
      else if (components==2) type=DATABUF_TYPE_SHORT;
      else if (components==3) type=DATABUF_TYPE_RGB;
      else type=DATABUF_TYPE_RGBA;

      bytes=xsize*ysize*zsize*components;

      if (type==DATABUF_TYPE_SHORT)
         {
         if (intel_check()) swapbytes();
         convert2(1,0);
         }
      }
   else
      if (tobyte) quantize16to8((unsigned char *)data,width,height,depth);
      else quantize16to16((unsigned char *)data,width,height,depth);

   swx=midx-dx*scalex/2.0;
   swy=midy-dy*scaley/2.0;
   nwx=midx-dx*scalex/2.0;
   nwy=midy+dy*scaley/2.0;
   nex=midx+dx*scalex/2.0;
   ney=midy+dy*scaley/2.0;
   sex=midx+dx*scalex/2.0;
   sey=midy-dy*scaley/2.0;

   h0=basez*scalez;
   dh=dz*scalez;

   t0=dt=0.0;

   scaling=1.0f/255.0f;
   bias=0.0f;

   return(1);
   }

// data is loaded from PVM time series
int databuf::loadPVMdata(const char *filename,
                         unsigned int t,unsigned int n,
                         double timestart,double timestep,
                         double midx,double midy,double basez,
                         double dx,double dy,double dz)
   {
   static const int maxstr=1000;

   unsigned int i;

   char str[maxstr];

   unsigned char *moredata;
   unsigned int width,height,depth,components;
   float scalex,scaley,scalez;

   if (n==0) ERRORMSG();

   snprintf(str,maxstr,"%s-t%d",filename,t);

   if ((data=readDDSvolume(str,
                           &width,&height,&depth,&components,
                           &scalex,&scaley,&scalez))==NULL)
      {
      snprintf(str,maxstr,"%s-t0%d",filename,t);

      if ((data=readDDSvolume(str,
                              &width,&height,&depth,&components,
                              &scalex,&scaley,&scalez))==NULL)
         {
         snprintf(str,maxstr,"%s-t00%d",filename,t);

         if ((data=readDDSvolume(str,
                                 &width,&height,&depth,&components,
                                 &scalex,&scaley,&scalez))==NULL)
            {
#ifdef LIBMINI_DEBUG
            fprintf(stderr,"unable to load %s!\n",filename);
#endif
            return(0);
            }
         }
      }

   if (width<2 || height<2 || depth<2) ERRORMSG();
   if (components<1 || components>4) ERRORMSG();

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   xsize=width;
   ysize=height;
   zsize=depth;
   tsteps=n;

   if (components==1) type=DATABUF_TYPE_BYTE;
   else if (components==2) type=DATABUF_TYPE_SHORT;
   else if (components==3) type=DATABUF_TYPE_RGB;
   else type=DATABUF_TYPE_RGBA;

   bytes=xsize*ysize*zsize*components*tsteps;

   if ((data=realloc(data,bytes))==NULL) MEMERROR();

   for (i=t+1; i<t+n; i++)
      {
      snprintf(str,maxstr,"%s-t%d",filename,i);

      if ((moredata=readDDSvolume(str,&width,&height,&depth,&components))==NULL)
         {
         snprintf(str,maxstr,"%s-t0%d",filename,i);

         if ((moredata=readDDSvolume(str,&width,&height,&depth,&components))==NULL)
            {
            snprintf(str,maxstr,"%s-t00%d",filename,i);

            if ((moredata=readDDSvolume(str,&width,&height,&depth,&components))==NULL) ERRORMSG();
            }
         }

      if ((unsigned int)width!=xsize || (unsigned int)height!=ysize || (unsigned int)depth!=zsize) ERRORMSG();

      if (components==1 && type!=DATABUF_TYPE_BYTE) ERRORMSG();
      else if (components==2 && type!=DATABUF_TYPE_SHORT) ERRORMSG();
      else if (components==3 && type!=DATABUF_TYPE_RGB) ERRORMSG();
      else if (components==4 && type!=DATABUF_TYPE_RGBA) ERRORMSG();

      memcpy(&((unsigned char *)data)[(i-1)*xsize*ysize*zsize*components],moredata,xsize*ysize*zsize*components);
      free(moredata);
      }

   if (type==DATABUF_TYPE_SHORT)
      {
      if (intel_check()) swapbytes();
      convert2(1,0);
      }

   swx=midx-dx*scalex/2.0;
   swy=midy-dy*scaley/2.0;
   nwx=midx-dx*scalex/2.0;
   nwy=midy+dy*scaley/2.0;
   nex=midx+dx*scalex/2.0;
   ney=midy+dy*scaley/2.0;
   sex=midx+dx*scalex/2.0;
   sey=midy-dy*scaley/2.0;

   h0=basez*scalez;
   dh=dz*scalez;

   t0=timestart;
   dt=timestep;

   scaling=1.0f/255.0f;
   bias=0.0f;

   return(1);
   }

// data is loaded from MOE file
int databuf::loadMOEdata(const char *filename,float *useful_smallest,float *useful_greatest)
   {
   FILE *file;

   char *str;

   int msb=1;

   float lon,lat,zorig;
   float xspace,yspace,zspace;
   float days,seconds,tstep;
   float usefs,usefg;

   float midx,midy,dx,dy;

   if ((file=fopen(filename,"rb"))==NULL)
      {
#ifdef LIBMINI_DEBUG
      fprintf(stderr,"unable to load %s!\n",filename);
#endif
      return(0);
      }

   str=readoneline(file);
   if (strcmp(str,"MOEVar")!=0)
      {
      fclose(file);
      return(0);
      }

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // MSB or LSB
   if (strcmp(str,"MSB")==0) msb=1;
   else if (strcmp(str,"LSB")==0) msb=0;
   else ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // grid size
   if (sscanf(str,"%d, %d, %d",&xsize,&ysize,&zsize)!=3) ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // longitude in degrees, latitude in degrees, zorigin in meters
   if (sscanf(str,"%g, %g, %g",&lon,&lat,&zorig)!=3) ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // x-spacing in degrees, y-spacing in degrees, z-spacing in meters
   if (sscanf(str,"%g, %g, %g",&xspace,&yspace,&zspace)!=3) ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // time offset in days, time offset in seconds, time step in seconds
   if (sscanf(str,"%g, %g, %g",&days,&seconds,&tstep)!=3) ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // useful range of values
   if (sscanf(str,"%g, %g",&usefs,&usefg)!=2) ERRORMSG();

   str=readoneline(file);
   while (strlen(str)==0) str=readoneline(file);

   // number of time steps
   if (sscanf(str,"%d",&tsteps)!=1) ERRORMSG();

   if (xsize<2 || ysize<2 || zsize<2 || tsteps<1) ERRORMSG();

   type=DATABUF_TYPE_FLOAT;

   bytes=xsize*ysize*zsize*tsteps*4;

   if ((data=malloc(bytes))==NULL) MEMERROR();

   // read float data
   if (fread(data,bytes,1,file)!=1) IOERROR();
   fclose(file);

   // convert from MSB to native format
   if (msb!=0)
      if (intel_check()) swapbytes();

   // convert from LSB to native format
   if (msb==0)
      if (!intel_check()) swapbytes();

   extformat=DATABUF_EXTFMT_PLAIN;
   implformat=0;

   midx=lon+xspace*(xsize-1)/2.0f;
   dx=fabs(xspace)*(xsize-1);

   midy=lat+yspace*(ysize-1)/2.0f;
   dy=fabs(yspace)*(ysize-1);

   dx/=2.0f;
   dy/=2.0f;

   swx=midx-dx;
   swy=midy-dy;
   nwx=midx-dx;
   nwy=midy+dy;
   nex=midx+dx;
   ney=midy+dy;
   sex=midx+dx;
   sey=midy-dy;

   h0=zorig;
   dh=zspace*(zsize-1);

   t0=24*60*60*days+seconds;
   dt=tstep;

   scaling=1.0f;
   bias=0.0f;

   if (useful_smallest!=NULL) *useful_smallest=usefs;
   if (useful_greatest!=NULL) *useful_greatest=usefg;

   return(1);
   }

// data is saved as plain PNM image
void databuf::savePNMdata(const char *filename)
   {
   PNMcomment comment,*cptr;

   if (extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0) return;
   if (zsize>1 || tsteps>1) return;

   if (crs==DATABUF_CRS_LLH || crs==DATABUF_CRS_UTM)
      {
      putPNMparams(&comment,NULL,
                   (crs==DATABUF_CRS_LLH)?0:1,zone,datum,(crs==DATABUF_CRS_LLH)?4:2,
                   swx,swy,nwx,nwy,nex,ney,sex,sey,
                   fsqrt(fsqr(sex-swx)+fsqr(sey-swy))/xsize,fsqrt(fsqr(nwy-swy)+fsqr(nwx-swx))/ysize,
                   2,scaling,(nodata==-MAXFLOAT || isNAN(nodata))?-32678:ftrc(nodata+0.5f));

      cptr=&comment;
      }
   else cptr=NULL;

   if (type==DATABUF_TYPE_BYTE) writePNMimage(filename,(unsigned char *)data,xsize,ysize,1,cptr);
   else if (type==DATABUF_TYPE_SHORT)
      if (!intel_check()) writePNMimage(filename,(unsigned char *)data,xsize,ysize,2,cptr);
      else
         {
         swapbytes();
         writePNMimage(filename,(unsigned char *)data,xsize,ysize,2,cptr);
         swapbytes();
         }
   else if (type==DATABUF_TYPE_RGB) writePNMimage(filename,(unsigned char *)data,xsize,ysize,3,cptr);
   else if (type==DATABUF_TYPE_RGBA) writePNMimage(filename,(unsigned char *)data,xsize,ysize,4,cptr);
   }

// data is saved as PVM volume
void databuf::savePVMdata(const char *filename,BOOLINT dds)
   {
   if (extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0) return;
   if (tsteps>1) return;

   if (dds)
      {
      if (type==DATABUF_TYPE_BYTE) writeDDSvolume(filename,(unsigned char *)data,xsize,ysize,zsize,1);
      else if (type==DATABUF_TYPE_SHORT)
         {
         convert2(0,1);
         if (intel_check()) swapbytes();
         writeDDSvolume(filename,(unsigned char *)data,xsize,ysize,zsize,2);
         if (intel_check()) swapbytes();
         convert2(1,0);
         }
      else if (type==DATABUF_TYPE_RGB) writeDDSvolume(filename,(unsigned char *)data,xsize,ysize,zsize,3);
      else if (type==DATABUF_TYPE_RGBA) writeDDSvolume(filename,(unsigned char *)data,xsize,ysize,zsize,4);
      }
   else
      {
      if (type==DATABUF_TYPE_BYTE) writePVMvolume(filename,(unsigned char *)data,xsize,ysize,zsize,1);
      else if (type==DATABUF_TYPE_SHORT)
         {
         convert2(0,1);
         if (intel_check()) swapbytes();
         writePVMvolume(filename,(unsigned char *)data,xsize,ysize,zsize,2);
         if (intel_check()) swapbytes();
         convert2(1,0);
         }
      else if (type==DATABUF_TYPE_RGB) writePVMvolume(filename,(unsigned char *)data,xsize,ysize,zsize,3);
      else if (type==DATABUF_TYPE_RGBA) writePVMvolume(filename,(unsigned char *)data,xsize,ysize,zsize,4);
      }
   }

// data is generated from plane equation
void databuf::generateplane(int size,
                            double px,double py,double pz,
                            double nx,double ny,double nz,
                            double dx,double dy,double dz)
   {
   alloc(2,2,2,1,2);

   dx/=2.0;
   dy/=2.0;

   swx=px-dx/2.0;
   swy=py-dy/2.0;
   nwx=px-dx/2.0;
   nwy=py+dy/2.0;
   nex=px+dx/2.0;
   ney=py+dy/2.0;
   sex=px+dx/2.0;
   sey=py-dy/2.0;

   h0=pz-dz/2.0;
   dh=dz;

   t0=dt=0.0;

   scaling=1.0f;
   bias=0.0f;

   setval(0,0,0,(swx-px)*nx+(swy-py)*ny-dh/2.0*nz);
   setval(1,0,0,(sex-px)*nx+(sey-py)*ny-dh/2.0*nz);
   setval(0,1,0,(nwx-px)*nx+(nwy-py)*ny-dh/2.0*nz);
   setval(1,1,0,(nex-px)*nx+(ney-py)*ny-dh/2.0*nz);
   setval(0,0,1,(swx-px)*nx+(swy-py)*ny+dh/2.0*nz);
   setval(1,0,1,(sex-px)*nx+(sey-py)*ny+dh/2.0*nz);
   setval(0,1,1,(nwx-px)*nx+(nwy-py)*ny+dh/2.0*nz);
   setval(1,1,1,(nex-px)*nx+(ney-py)*ny+dh/2.0*nz);

   resampledata(size,size,size);
   }

// convert data from one type to another
void databuf::convertdata(unsigned int newtype)
   {
   unsigned int cells;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   unsigned char *rgbptr;

   unsigned int count;

   float minvalue,maxvalue,value;

   void *newdata;

   cells=xsize*ysize*zsize*tsteps;

   if (type==DATABUF_TYPE_BYTE && newtype==DATABUF_TYPE_SHORT)
      {
      if ((newdata=malloc(cells*2))==NULL) MEMERROR();

      byteptr=(unsigned char *)data;
      shortptr=(short int *)newdata;

      for (count=0; count<cells; count++) *shortptr++=*byteptr++;

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*2;
      }
   else if (type==DATABUF_TYPE_BYTE && newtype==DATABUF_TYPE_FLOAT)
      {
      if ((newdata=malloc(cells*4))==NULL) MEMERROR();

      byteptr=(unsigned char *)data;
      floatptr=(float *)newdata;

      for (count=0; count<cells; count++) *floatptr++=*byteptr++*scaling+bias;

      free(data);
      data=newdata;

      type=newtype;

      scaling=1.0f;
      bias=0.0f;

      bytes=xsize*ysize*zsize*tsteps*4;
      }
   else if (type==DATABUF_TYPE_SHORT && newtype==DATABUF_TYPE_BYTE)
      {
      shortptr=(short int *)data;

      if (*shortptr==nodata) minvalue=maxvalue=*shortptr++;
      else minvalue=maxvalue=scaling*(*shortptr++)+bias;

      for (count=1; count<cells; count++)
         {
         value=*shortptr++;
         if (value!=nodata) value=scaling*value+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells))==NULL) MEMERROR();

      shortptr=(short int *)data;
      byteptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=*shortptr++;
         if (value!=nodata) value=scaling*value+bias;

         *byteptr++=ftrc(255.0f*(value-minvalue)/(maxvalue-minvalue)+0.5f);
         }

      scaling=(maxvalue-minvalue)/255.0f;
      bias=minvalue;

      nodata=ftrc(255.0f*(nodata-minvalue)/(maxvalue-minvalue)+0.5f);

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps;
      }
   else if (type==DATABUF_TYPE_SHORT && newtype==DATABUF_TYPE_FLOAT)
      {
      if ((newdata=malloc(cells*4))==NULL) MEMERROR();

      shortptr=(short int *)data;
      floatptr=(float *)newdata;

      for (count=0; count<cells; count++) *floatptr++=*shortptr++*scaling+bias;

      free(data);
      data=newdata;

      type=newtype;

      scaling=1.0f;
      bias=0.0f;

      bytes=xsize*ysize*zsize*tsteps*4;
      }
   else if (type==DATABUF_TYPE_FLOAT && newtype==DATABUF_TYPE_BYTE)
      {
      floatptr=(float *)data;

      if (isNAN(*floatptr)) ERRORMSG();
      if (*floatptr==nodata) minvalue=maxvalue=*floatptr++;
      else minvalue=maxvalue=scaling*(*floatptr++)+bias;

      for (count=1; count<cells; count++)
         {
         value=*floatptr++;
         if (isNAN(value)) ERRORMSG();
         if (value!=nodata) value=scaling*value+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells))==NULL) MEMERROR();

      floatptr=(float *)data;
      byteptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=*floatptr++;
         if (value!=nodata) value=scaling*value+bias;

         *byteptr++=ftrc(255.0f*(value-minvalue)/(maxvalue-minvalue)+0.5f);
         }

      scaling=(maxvalue-minvalue)/255.0f;
      bias=minvalue;

      nodata=ftrc(255.0f*(nodata-minvalue)/(maxvalue-minvalue)+0.5f);

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps;
      }
   else if (type==DATABUF_TYPE_FLOAT && newtype==DATABUF_TYPE_SHORT)
      {
      floatptr=(float *)data;

      if (isNAN(*floatptr)) ERRORMSG();
      if (*floatptr==nodata) minvalue=maxvalue=*floatptr++;
      else minvalue=maxvalue=scaling*(*floatptr++)+bias;

      for (count=1; count<cells; count++)
         {
         value=*floatptr++;
         if (isNAN(value)) ERRORMSG();
         if (value!=nodata) value=scaling*value+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells*2))==NULL) MEMERROR();

      floatptr=(float *)data;
      shortptr=(short int *)newdata;

      for (count=0; count<cells; count++)
         {
         value=*floatptr++;
         if (value!=nodata) value=scaling*value+bias;

         *shortptr++=ftrc(65535.0f*(value-minvalue)/(maxvalue-minvalue)-32768.0f+0.5f);
         }

      scaling=(maxvalue-minvalue)/65535.0f;
      bias=minvalue+32768.0f*scaling;

      nodata=ftrc(65535.0f*(nodata-minvalue)/(maxvalue-minvalue)-32768.0f+0.5f);

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*2;
      }
   else if (type==DATABUF_TYPE_FLOAT && newtype==DATABUF_TYPE_FLOAT)
      {
      floatptr=(float *)data;

      if (scaling!=1.0f || bias!=0.0f)
         {
         for (count=0; count<cells; count++)
            {
            value=*floatptr;
            if (checkval(value)) value=scaling*value+bias;

            *floatptr++=value;
            }

         scaling=1.0f;
         bias=0.0f;
         }
      }
   else if (type==DATABUF_TYPE_RGB && newtype==DATABUF_TYPE_BYTE)
      {
      if ((newdata=malloc(cells))==NULL) MEMERROR();

      rgbptr=(unsigned char *)data;
      byteptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=*rgbptr++;
         value+=*rgbptr++;
         value+=*rgbptr++;

         *byteptr++=ftrc(value/3.0f+0.5f);
         }

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps;
      }
   else if (type==DATABUF_TYPE_BYTE && newtype==DATABUF_TYPE_RGB)
      {
      if ((newdata=malloc(cells*3))==NULL) MEMERROR();

      byteptr=(unsigned char *)data;
      rgbptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=*byteptr++;

         *rgbptr++=value;
         *rgbptr++=value;
         *rgbptr++=value;
         }

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*3;
      }
   else if (type==DATABUF_TYPE_RGB && newtype==DATABUF_TYPE_RGBA)
      {
      if ((newdata=malloc(cells*4))==NULL) MEMERROR();

      byteptr=(unsigned char *)data;
      rgbptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         *rgbptr++=*byteptr++;
         *rgbptr++=*byteptr++;
         *rgbptr++=*byteptr++;
         *rgbptr++=255;
         }

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*4;
      }
   else if (type==DATABUF_TYPE_RGBA && newtype==DATABUF_TYPE_RGB)
      {
      if ((newdata=malloc(cells*3))==NULL) MEMERROR();

      byteptr=(unsigned char *)data;
      rgbptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         *rgbptr++=*byteptr++;
         *rgbptr++=*byteptr++;
         *rgbptr++=*byteptr++;
         byteptr++;
         }

      free(data);
      data=newdata;

      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*3;
      }
   }

// resample data at a different resolution
void databuf::resampledata(unsigned int xs,unsigned int ys,unsigned int zs)
   {
   unsigned int i,j,k,t;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   float value;
   float color[4];

   unsigned int offset;

   if (xs<2 && ys<2 && zs<2) ERRORMSG();
   if (xsize<2 && ysize<2 && zsize<2) ERRORMSG();

   if (xs==xsize && ys==ysize && zs==zsize) return;

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         if ((byteptr=(unsigned char *)malloc(xs*ys*zs*tsteps))==NULL) MEMERROR();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     {
                     if (xs<2) value=getvalue(0.0f,0.0f,0.0f,t);
                     else if (ys<2) value=getvalue((float)i/(xs-1),0.0f,0.0f,t);
                     else if (zs<2) value=getvalue((float)i/(xs-1),(float)j/(ys-1),0.0f,t);
                     else value=getvalue((float)i/(xs-1),(float)j/(ys-1),(float)k/(zs-1),t);

                     if (value!=nodata) value=(value-bias)/scaling;

                     byteptr[i+(j+(k+t*zs)*ys)*xs]=ftrc(value+0.5f);
                     }

         free(data);
         data=byteptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps;

         break;
      case DATABUF_TYPE_SHORT:
         if ((shortptr=(short int *)malloc(xs*ys*zs*tsteps*2))==NULL) MEMERROR();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     {
                     if (xs<2) value=getvalue(0.0f,0.0f,0.0f,t);
                     else if (ys<2) value=getvalue((float)i/(xs-1),0.0f,0.0f,t);
                     else if (zs<2) value=getvalue((float)i/(xs-1),(float)j/(ys-1),0.0f,t);
                     else value=getvalue((float)i/(xs-1),(float)j/(ys-1),(float)k/(zs-1),t);

                     if (value!=nodata) value=(value-bias)/scaling;

                     shortptr[i+(j+(k+t*zs)*ys)*xs]=ftrc(value+0.5f);
                     }

         free(data);
         data=shortptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*2;

         break;
      case DATABUF_TYPE_FLOAT:
         if ((floatptr=(float *)malloc(xs*ys*zs*tsteps*4))==NULL) MEMERROR();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     {
                     if (xs<2) value=getvalue(0.0f,0.0f,0.0f,t);
                     else if (ys<2) value=getvalue((float)i/(xs-1),0.0f,0.0f,t);
                     else if (zs<2) value=getvalue((float)i/(xs-1),(float)j/(ys-1),0.0f,t);
                     else value=getvalue((float)i/(xs-1),(float)j/(ys-1),(float)k/(zs-1),t);

                     if (checkval(value)) value=(value-bias)/scaling;

                     floatptr[i+(j+(k+t*zs)*ys)*xs]=value;
                     }

         free(data);
         data=floatptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*4;

         break;
      case DATABUF_TYPE_RGB:
         if ((byteptr=(unsigned char *)malloc(xs*ys*zs*tsteps*3))==NULL) MEMERROR();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     {
                     if (xs<2) getrgbacolor(0.0f,0.0f,0.0f,t,color);
                     else if (ys<2) getrgbacolor((float)i/(xs-1),0.0f,0.0f,t,color);
                     else if (zs<2) getrgbacolor((float)i/(xs-1),(float)j/(ys-1),0.0f,t,color);
                     else getrgbacolor((float)i/(xs-1),(float)j/(ys-1),(float)k/(zs-1),t,color);

                     offset=3*(i+(j+(k+t*zs)*ys)*xs);

                     byteptr[offset]=ftrc((color[0]-bias)/scaling+0.5f);
                     byteptr[offset+1]=ftrc((color[1]-bias)/scaling+0.5f);
                     byteptr[offset+2]=ftrc((color[2]-bias)/scaling+0.5f);
                     }

         free(data);
         data=byteptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*3;

         break;
      case DATABUF_TYPE_RGBA:
         if ((byteptr=(unsigned char *)malloc(xs*ys*zs*tsteps*4))==NULL) MEMERROR();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     {
                     if (xs<2) getrgbacolor(0.0f,0.0f,0.0f,t,color);
                     else if (ys<2) getrgbacolor((float)i/(xs-1),0.0f,0.0f,t,color);
                     else if (zs<2) getrgbacolor((float)i/(xs-1),(float)j/(ys-1),0.0f,t,color);
                     else getrgbacolor((float)i/(xs-1),(float)j/(ys-1),(float)k/(zs-1),t,color);

                     offset=4*(i+(j+(k+t*zs)*ys)*xs);

                     byteptr[offset]=ftrc((color[0]-bias)/scaling+0.5f);
                     byteptr[offset+1]=ftrc((color[1]-bias)/scaling+0.5f);
                     byteptr[offset+2]=ftrc((color[2]-bias)/scaling+0.5f);
                     byteptr[offset+3]=ftrc((color[3]-bias)/scaling+0.5f);
                     }

         free(data);
         data=byteptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*4;

         break;
      }
   }

// resample data to next power of 2
void databuf::resample2(unsigned int maxsize)
   {
   unsigned int xs,ys,zs;
   unsigned int xs2,ys2,zs2;

   xs=xsize;
   ys=ysize;
   zs=zsize;

   if (maxsize>0)
      {
      if (xs>maxsize) xs=maxsize;
      if (ys>maxsize) ys=maxsize;
      if (zs>maxsize) zs=maxsize;
      }

   for (xs2=1; xs2<xs; xs2*=2);
   for (ys2=1; ys2<ys; ys2*=2);
   for (zs2=1; zs2<zs; zs2*=2);

   resampledata(xs2,ys2,zs2);
   }

// set a single scalar value
void databuf::setval(const unsigned int i,const unsigned int j,const unsigned int k,const float value)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=(unsigned char *)data;
         if (isNAN(value)) ERRORMSG();
         if (value==nodata) byteptr[i+(j+k*ysize)*xsize]=ftrc(value+0.5f);
         else byteptr[i+(j+k*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case DATABUF_TYPE_SHORT:
         shortptr=(short int *)data;
         if (isNAN(value)) ERRORMSG();
         if (value==nodata) shortptr[i+(j+k*ysize)*xsize]=ftrc(value+0.5f);
         else shortptr[i+(j+k*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case DATABUF_TYPE_FLOAT:
         floatptr=(float *)data;
         if (checknodata(value)) floatptr[i+(j+k*ysize)*xsize]=value;
         else floatptr[i+(j+k*ysize)*xsize]=(value-bias)/scaling;
         break;
      }
   }

// set a single scalar value
void databuf::setval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float value)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=(unsigned char *)data;
         if (isNAN(value)) ERRORMSG();
         if (value==nodata) byteptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc(value+0.5f);
         else byteptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case DATABUF_TYPE_SHORT:
         shortptr=(short int *)data;
         if (isNAN(value)) ERRORMSG();
         if (value==nodata) shortptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc(value+0.5f);
         else shortptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case DATABUF_TYPE_FLOAT:
         floatptr=(float *)data;
         if (checknodata(value)) floatptr[i+(j+(k+t*zsize)*ysize)*xsize]=value;
         else floatptr[i+(j+(k+t*zsize)*ysize)*xsize]=(value-bias)/scaling;
         break;
      }
   }

// get a single scalar value
float databuf::getval(const unsigned int i,const unsigned int j,const unsigned int k)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   float val;

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=(unsigned char *)data;
         val=byteptr[i+(j+k*ysize)*xsize];
         if (val==nodata) return(val);
         else return(scaling*(val+bias));
      case DATABUF_TYPE_SHORT:
         shortptr=(short int *)data;
         val=shortptr[i+(j+k*ysize)*xsize];
         if (val==nodata) return(val);
         else return(scaling*(val+bias));
      case DATABUF_TYPE_FLOAT:
         floatptr=(float *)data;
         val=floatptr[i+(j+k*ysize)*xsize];
         if (checknodata(val)) return(val);
         else return(scaling*(val+bias));
      }

   return(nodata);
   }

// get a single scalar value
float databuf::getval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   float val;

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=(unsigned char *)data;
         val=byteptr[i+(j+(k+t*zsize)*ysize)*xsize];
         if (val==nodata) return(val);
         else return(scaling*(val+bias));
      case DATABUF_TYPE_SHORT:
         shortptr=(short int *)data;
         val=shortptr[i+(j+(k+t*zsize)*ysize)*xsize];
         if (val==nodata) return(val);
         else return(scaling*(val+bias));
      case DATABUF_TYPE_FLOAT:
         floatptr=(float *)data;
         val=floatptr[i+(j+(k+t*zsize)*ysize)*xsize];
         if (checknodata(val)) return(val);
         else return(scaling*(val+bias));
      }

   return(nodata);
   }

// sample brick at normalized position (x,y,z) and time frame t
float databuf::getvalue(float x,float y,float z,unsigned int t)
   {
   unsigned int i,j,k;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   unsigned int slice;

   float val1,val2,val3,val4;
   float val5,val6,val7,val8;

   if (x<0.0f) x=0.0f;
   else if (x>1.0f) x=1.0f;

   if (y<0.0f) y=0.0f;
   else if (y>1.0f) y=1.0f;

   x*=xsize-1;
   y*=ysize-1;
   z*=zsize-1;

   i=ftrc(x);
   j=ftrc(y);
   k=ftrc(z);

   x-=i;
   y-=j;
   z-=k;

   if (i==xsize-1 && xsize>1)
      {
      i=xsize-2;
      x=1.0f;
      }

   if (j==ysize-1 && ysize>1)
      {
      j=ysize-2;
      y=1.0f;
      }

   if (k==zsize-1 && zsize>1)
      {
      k=zsize-2;
      z=1.0f;
      }

   switch (type)
      {
      case DATABUF_TYPE_BYTE:
         byteptr=&((unsigned char *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         val1=byteptr[0];

         if (val1==nodata) return(nodata);

         if (xsize<2)
            return(scaling*val1+bias);
         else
            {
            val2=byteptr[1];

            if (val2==nodata) return(nodata);

            if (ysize<2)
               return(scaling*((1.0f-x)*val1+x*val2)+bias);
            else
               {
               val3=byteptr[xsize];
               val4=byteptr[xsize+1];

               if (val3==nodata || val4==nodata) return(nodata);

               if (zsize<2)
                  return(((1.0f-y)*((1.0f-x)*val1+x*val2)+
                          y*((1.0f-x)*val3+x*val4))*scaling+bias);
               else
                  {
                  slice=xsize*ysize;

                  val5=byteptr[slice];
                  val6=byteptr[slice+1];
                  val7=byteptr[slice+xsize];
                  val8=byteptr[slice+xsize+1];

                  if (val5==nodata || val6==nodata || val7==nodata || val8==nodata) return(nodata);

                  return(scaling*((1.0f-z)*((1.0f-y)*((1.0f-x)*val1+x*val2)+
                                            y*((1.0f-x)*val3+x*val4))+
                                  z*((1.0f-y)*((1.0f-x)*val5+x*val6)+
                                     y*((1.0f-x)*val7+x*val8)))+bias);
                  }
               }
            }
      case DATABUF_TYPE_SHORT:
         shortptr=&((short int *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         val1=shortptr[0];

         if (val1==nodata) return(nodata);

         if (xsize<2)
            return(scaling*val1+bias);
         else
            {
            val2=shortptr[1];

            if (val2==nodata) return(nodata);

            if (ysize<2)
               return(scaling*((1.0f-x)*val1+x*val2)+bias);
            else
               {
               val3=shortptr[xsize];
               val4=shortptr[xsize+1];

               if (val3==nodata || val4==nodata) return(nodata);

               if (zsize<2)
                  return(((1.0f-y)*((1.0f-x)*val1+x*val2)+
                          y*((1.0f-x)*val3+x*val4))*scaling+bias);
               else
                  {
                  slice=xsize*ysize;

                  val5=shortptr[slice];
                  val6=shortptr[slice+1];
                  val7=shortptr[slice+xsize];
                  val8=shortptr[slice+xsize+1];

                  if (val5==nodata || val6==nodata || val7==nodata || val8==nodata) return(nodata);

                  return(scaling*((1.0f-z)*((1.0f-y)*((1.0f-x)*val1+x*val2)+
                                            y*((1.0f-x)*val3+x*val4))+
                                  z*((1.0f-y)*((1.0f-x)*val5+x*val6)+
                                     y*((1.0f-x)*val7+x*val8)))+bias);
                  }
               }
            }
      case DATABUF_TYPE_FLOAT:
         floatptr=&((float *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         val1=floatptr[0];

         if (checknodata(val1)) return(nodata);

         if (xsize<2)
            return(scaling*val1+bias);
         else
            {
            val2=floatptr[1];

            if (checknodata(val2)) return(nodata);

            if (ysize<2)
               return(scaling*((1.0f-x)*val1+x*val2)+bias);
            else
               {
               val3=floatptr[xsize];
               val4=floatptr[xsize+1];

               if (checknodata(val3) || checknodata(val4)) return(nodata);

               if (zsize<2)
                  return(((1.0f-y)*((1.0f-x)*val1+x*val2)+
                          y*((1.0f-x)*val3+x*val4))*scaling+bias);
               else
                  {
                  slice=xsize*ysize;

                  val5=floatptr[slice];
                  val6=floatptr[slice+1];
                  val7=floatptr[slice+xsize];
                  val8=floatptr[slice+xsize+1];

                  if (checknodata(val5) || checknodata(val6) || checknodata(val7) || checknodata(val8)) return(nodata);

                  return(scaling*((1.0f-z)*((1.0f-y)*((1.0f-x)*val1+x*val2)+
                                            y*((1.0f-x)*val3+x*val4))+
                                  z*((1.0f-y)*((1.0f-x)*val5+x*val6)+
                                     y*((1.0f-x)*val7+x*val8)))+bias);
                  }
               }
            }
      }

   return(nodata);
   }

// get a gradient magnitude value
float databuf::getgrad(const unsigned int i,const unsigned int j,const unsigned int k)
   {
   float v1,v2;
   float gx,gy,gz;

   if (i>0)
      if (i<xsize-1)
         {
         v1=getval(i+1,j,k);
         v2=getval(i-1,j,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gx=0.5f*(v1-v2);
         }
      else
         {
         v1=getval(i,j,k);
         v2=getval(i-1,j,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gx=0.5f*(v1-v2);
         }
   else
      if (i<xsize-1)
         {
         v1=getval(i+1,j,k);
         v2=getval(i,j,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gx=0.5f*(v1-v2);
         }
      else gx=0.0f;

   if (j>0)
      if (j<ysize-1)
         {
         v1=getval(i,j+1,k);
         v2=getval(i,j-1,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gy=0.5f*(v1-v2);
         }
      else
         {
         v1=getval(i,j,k);
         v2=getval(i,j-1,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gy=0.5f*(v1-v2);
         }
   else
      if (j<ysize-1)
         {
         v1=getval(i,j+1,k);
         v2=getval(i,j,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gy=0.5f*(v1-v2);
         }
      else gy=0.0f;

   if (k>0)
      if (k<zsize-1)
         {
         v1=getval(i,j,k+1);
         v2=getval(i,j,k-1);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gz=0.5f*(v1-v2);
         }
      else
         {
         v1=getval(i,j,k);
         v2=getval(i,j,k-1);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gz=0.5f*(v1-v2);
         }
   else
      if (k<zsize-1)
         {
         v1=getval(i,j,k+1);
         v2=getval(i,j,k);
         if (checknodata(v1) || checknodata(v2)) return(nodata);
         gz=0.5f*(v1-v2);
         }
      else gz=0.0f;

   return(fsqrt(gx*gx+gy*gy+gz*gz));
   }

// set rgb color
void databuf::setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const float value[3])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGB)
      {
      ptr=&((unsigned char *)data)[3*(i+(j+k*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      }
   }

// set rgb color
void databuf::setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float value[3])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGB)
      {
      ptr=&((unsigned char *)data)[3*(i+(j+(k+t*zsize)*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      }
   }

// set rgba color
void databuf::setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const float value[4])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGBA)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+k*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      ptr[3]=ftrc((value[3]-bias)/scaling+0.5f);
      }
   }

// set rgba color
void databuf::setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float value[4])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGBA)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+(k+t*zsize)*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      ptr[3]=ftrc((value[3]-bias)/scaling+0.5f);
      }
   }

// get rgb color
void databuf::getrgb(const unsigned int i,const unsigned int j,const unsigned int k,float value[3])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGB)
      {
      ptr=&((unsigned char *)data)[3*(i+(j+k*ysize)*xsize)];

      value[0]=ptr[0]*scaling+bias;
      value[1]=ptr[1]*scaling+bias;
      value[2]=ptr[2]*scaling+bias;
      }
   }

// get rgb color
void databuf::getrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,float value[3])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGB)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+(k+t*zsize)*ysize)*xsize)];

      value[0]=ptr[0]*scaling+bias;
      value[1]=ptr[1]*scaling+bias;
      value[2]=ptr[2]*scaling+bias;
      }
   }

// get rgba color
void databuf::getrgba(const unsigned int i,const unsigned int j,const unsigned int k,float value[4])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGBA)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+k*ysize)*xsize)];

      value[0]=ptr[0]*scaling+bias;
      value[1]=ptr[1]*scaling+bias;
      value[2]=ptr[2]*scaling+bias;
      value[3]=ptr[3]*scaling+bias;
      }
   }

// get rgba color
void databuf::getrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,float value[4])
   {
   unsigned char *ptr;

   if (type==DATABUF_TYPE_RGBA)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+(k+t*zsize)*ysize)*xsize)];

      value[0]=ptr[0]*scaling+bias;
      value[1]=ptr[1]*scaling+bias;
      value[2]=ptr[2]*scaling+bias;
      value[3]=ptr[3]*scaling+bias;
      }
   }

// get an interpolated rgba color
void databuf::getrgbacolor(float x,float y,float z,unsigned int t,float color[4])
   {
   int channel;
   int channels;

   unsigned int i,j,k;

   unsigned char *ptr;

   unsigned int slice;

   float val1,val2,val3,val4;
   float val5,val6,val7,val8;

   if (x<0.0f) x=0.0f;
   else if (x>1.0f) x=1.0f;

   if (y<0.0f) y=0.0f;
   else if (y>1.0f) y=1.0f;

   color[0]=color[1]=color[2]=color[3]=0.0f;

   if (type==DATABUF_TYPE_RGB) channels=3;
   else if (type==DATABUF_TYPE_RGBA) channels=4;
   else return;

   x*=xsize-1;
   y*=ysize-1;
   z*=zsize-1;

   i=ftrc(x);
   j=ftrc(y);
   k=ftrc(z);

   x-=i;
   y-=j;
   z-=k;

   if (i==xsize-1 && xsize>1)
      {
      i=xsize-2;
      x=1.0f;
      }

   if (j==ysize-1 && ysize>1)
      {
      j=ysize-2;
      y=1.0f;
      }

   if (k==zsize-1 && zsize>1)
      {
      k=zsize-2;
      z=1.0f;
      }

   for (channel=channels-1; channel>=0; channel--)
      {
      ptr=&((unsigned char *)data)[channels*(i+(j+(k+t*zsize)*ysize)*xsize)+channel];

      val1=ptr[0];

      if (channel==3)
         if (val1==0.0f) return;

      if (xsize<2)
         color[channel]=scaling*val1+bias;
      else
         {
         val2=ptr[channels];

         if (channel==3)
            if (val2==0.0f) return;

         if (ysize<2)
            color[channel]=scaling*((1.0f-x)*val1+x*val2)+bias;
         else
            {
            val3=ptr[channels*xsize];
            val4=ptr[channels*(xsize+1)];

            if (channel==3)
               if (val3==0.0f || val4==0.0f) return;

            if (zsize<2)
               color[channel]=scaling*((1.0f-y)*((1.0f-x)*val1+x*val2)+
                                       y*((1.0f-x)*val3+x*val4))+bias;
            else
               {
               slice=channels*xsize*ysize;

               val5=ptr[slice];
               val6=ptr[slice+channels];
               val7=ptr[slice+channels*xsize];
               val8=ptr[slice+channels*(xsize+1)];

               if (channel==3)
                  if (val5==0.0f || val6==0.0f || val7==0.0f || val8==0.0f) return;

               color[channel]=scaling*((1.0f-z)*((1.0f-y)*((1.0f-x)*val1+x*val2)+
                                                 y*((1.0f-x)*val3+x*val4))+
                                       z*((1.0f-y)*((1.0f-x)*val5+x*val6)+
                                          y*((1.0f-x)*val7+x*val8)))+bias;
               }
            }
         }
      }

   if (channels==3) color[3]=scaling*255.0f+bias;
   }

// get the minimum and maximum scalar value
void databuf::getminmax(float *minval,float *maxval)
   {
   unsigned int i,j,k,t;

   float val;

   if (minvalue>maxvalue)
      {
      minvalue=MAXFLOAT;
      maxvalue=-MAXFLOAT;

      for (t=0; t<tsteps; t++)
         for (i=0; i<xsize; i++)
            for (j=0; j<ysize; j++)
               for (k=0; k<zsize; k++)
                  {
                  val=getval(i,j,k,t);

                  if (checkval(val))
                     {
                     if (val<minvalue) minvalue=val;
                     if (val>maxvalue) maxvalue=val;
                     }
                  }

      if (minvalue>maxvalue) minvalue=maxvalue=nodata;
      }

   if (minval!=NULL && maxval!=NULL)
      {
      *minval=minvalue;
      *maxval=maxvalue;
      }
   }

// get the minimum and maximum scalar value within a useful range
void databuf::getminmax(float usefs,float usefg,
                        float *minval,float *maxval)
   {
   unsigned int i,j,k,t;

   float val;

   if (minvalue>maxvalue)
      {
      minvalue=MAXFLOAT;
      maxvalue=-MAXFLOAT;

      // check all scalar values except the no-data values
      for (t=0; t<tsteps; t++)
         for (i=0; i<xsize; i++)
            for (j=0; j<ysize; j++)
               for (k=0; k<zsize; k++)
                  {
                  val=getval(i,j,k,t);

                  if (checkval(val))
                      if ((val<usefs || val>usefg) &&
                          (val>usefs || val<usefg))
                         {
                         if (val<minvalue) minvalue=val;
                         if (val>maxvalue) maxvalue=val;
                         }
                  }

      if (minvalue>maxvalue) minvalue=maxvalue=nodata;
      }

   if (minval!=NULL && maxval!=NULL)
      {
      *minval=minvalue;
      *maxval=maxvalue;
      }
   }

// replace data values
unsigned int databuf::replacedata(float value1,float value2)
   {
   unsigned int count;

   unsigned int i,j,k,t;

   count=0;

   // search for data values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               if (getval(i,j,k,t)==value1)
                  {
                  setval(i,j,k,t,value2);
                  count++;
                  }

   return(count);
   }

// replace rgb values
unsigned int databuf::replacergb(const float value1[3],const float value2[3])
   {
   unsigned int count;

   unsigned int i,j,k,t;

   float rgb[3];

   count=0;

   if (type!=DATABUF_TYPE_RGB) return(0);

   rgb[0]=rgb[1]=rgb[2]=0.0f;

   // search for rgb values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               getrgb(i,j,k,t,rgb);

               if (rgb[0]==value1[0] && rgb[1]==value1[1] && rgb[2]==value1[2])
                  {
                  setrgb(i,j,k,t,value2);
                  count++;
                  }
               }

   return(count);
   }

// replace rgba values
unsigned int databuf::replacergba(const float value1[4],const float value2[4])
   {
   unsigned int count;

   unsigned int i,j,k,t;

   float rgba[4];

   count=0;

   if (type!=DATABUF_TYPE_RGBA) return(0);

   rgba[0]=rgba[1]=rgba[2]=rgba[3]=0.0f;

   // search for rgb values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               getrgba(i,j,k,t,rgba);

               if (rgba[0]==value1[0] && rgba[1]==value1[1] && rgba[2]==value1[2] && rgba[3]==value1[3])
                  {
                  setrgba(i,j,k,t,value2);
                  count++;
                  }
               }

   return(count);
   }

// check for no-data values
int databuf::checknodata()
   {
   unsigned int i,j,k,t;

   // check for no-data values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               if (checknodata(getval(i,j,k,t))) return(1);

   return(0);
   }

// replace no-data values
unsigned int databuf::replacenodata(float value)
   {
   unsigned int count;

   unsigned int i,j,k,t;

   count=0;

   // search for no-data values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               if (checknodata(getval(i,j,k,t)))
                  {
                  setval(i,j,k,t,value);
                  count++;
                  }

   return(count);
   }

// fill-in no-data values
unsigned int databuf::fillnodata(int radius)
   {
   int oldtype;

   unsigned int count;

   datafill buf;

   oldtype=type;

   if (type==DATABUF_TYPE_RGB)
      if (nodata==-MAXFLOAT) return(0);
      else convertdata(DATABUF_TYPE_BYTE);

   if (type!=DATABUF_TYPE_BYTE && type!=DATABUF_TYPE_SHORT && type!=DATABUF_TYPE_FLOAT) return(0);

   if (checknodata()==0) return(0);

   if (type==DATABUF_TYPE_BYTE || type==DATABUF_TYPE_SHORT) convertdata(DATABUF_TYPE_FLOAT);

   buf=*this;
   count=buf.fillin_by_regiongrowing(radius);

   if (oldtype==DATABUF_TYPE_BYTE || oldtype==DATABUF_TYPE_RGB) clamp(bias,255*scaling+bias);
   if (oldtype==DATABUF_TYPE_SHORT) clamp(-32768*scaling+bias,32767*scaling+bias);

   if (oldtype==DATABUF_TYPE_RGB) convertdata(DATABUF_TYPE_BYTE);
   convertdata(oldtype);

   return(count);
   }

// replace invalid values
unsigned int databuf::replaceinvalid(float usefs,float usefg,float useful)
   {
   unsigned int count;

   unsigned int i,j,k,t;

   float val;

   count=0;

   // search for no-data values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if (checkval(val))
                  if ((val<usefs || val>usefg) &&
                      (val>usefs || val<usefg))
                     {
                     setval(i,j,k,t,useful);
                     count++;
                     }
               }

   return(count);
   }

// clamp to range
unsigned int databuf::clamp(float usefs,float usefg)
   {
   unsigned int count;

   unsigned int i,j,k,t;

   float val;

   count=0;

   // search for out-of-bounds values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if (checkval(val))
                  if ((val<usefs || val>usefg) &&
                      (val>usefs || val<usefg))
                     {
                     if (fabs(val-usefs)<fabs(val-usefg)) setval(i,j,k,t,usefs);
                     else setval(i,j,k,t,usefg);
                     count++;
                     }
               }

   return(count);
   }

// compute absolute values
void databuf::computeabsolute()
   {
   unsigned int i,j,k,t;

   float val;

   // search for negative values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if (checkval(val))
                  if (val<0.0f) setval(i,j,k,t,-val);
               }
   }

// quantize 16 bit unsigned data to 16 bit signed data
void databuf::quantize16to16(unsigned char *data,
                             unsigned int width,unsigned int height,unsigned int depth,
                             BOOLINT nofree)
   {
   unsigned int i,j,k;

   unsigned short int *data2;

   int v,vmin,vmax;

   if ((data2=(unsigned short int*)malloc(width*height*depth*sizeof(unsigned short int)))==NULL) MEMERROR();

   vmin=65535;
   vmax=0;

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            {
            v=256*data[2*(i+(j+k*height)*width)]+data[2*(i+(j+k*height)*width)+1];
            data2[i+(j+k*height)*width]=v;

            if (v<vmin) vmin=v;
            if (v>vmax) vmax=v;
            }

   if (!nofree) free(data);

   if (vmin==vmax) vmax=vmin+1;

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            data2[i+(j+k*height)*width]=(int)(32767.0*get_ushort(data2,width,height,depth,i,j,k)/(vmax-vmin)+0.5);

   set(data2,width*height*depth,width,height,depth,1,DATABUF_TYPE_SHORT);
   set_mapping(1.0f/32767,0.0f);
   }

// quantize 16 bit unsigned data to 8 bit using a non-linear mapping
void databuf::quantize16to8(unsigned char *data,
                            unsigned int width,unsigned int height,unsigned int depth,
                            BOOLINT linear,BOOLINT nofree)
   {
   unsigned int i,j,k;

   unsigned char *data2;
   unsigned short int *data3;

   int v,vmin,vmax;

   double *err,eint;

   BOOLINT done;

   if ((data3=(unsigned short int*)malloc(width*height*depth*sizeof(unsigned short int)))==NULL) MEMERROR();

   vmin=65535;
   vmax=0;

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            {
            v=256*data[2*(i+(j+k*height)*width)]+data[2*(i+(j+k*height)*width)+1];
            data3[i+(j+k*height)*width]=v;

            if (v<vmin) vmin=v;
            if (v>vmax) vmax=v;
            }

   if (!nofree) free(data);

   if (vmin==vmax) vmax=vmin+1;

   if (vmax-vmin<256) linear=TRUE;

   err=new double[65536];

   if (linear)
      for (i=0; i<65536; i++) err[i]=255*(double)(i-vmin)/(vmax-vmin);
   else
      {
      for (i=0; i<65536; i++) err[i]=0.0;

      for (k=0; k<depth; k++)
         for (j=0; j<height; j++)
            for (i=0; i<width; i++)
               err[get_ushort(data3,width,height,depth,i,j,k)]+=sqrt(get_ushort_grad(data3,width,height,depth,i,j,k));

      for (i=0; i<65536; i++) err[i]=pow(err[i],1.0/3);

      err[vmin]=err[vmax]=0.0;

      for (k=0; k<256; k++)
         {
         for (eint=0.0,i=0; i<65536; i++) eint+=err[i];

         done=TRUE;

         for (i=0; i<65536; i++)
            if (err[i]>eint/256)
               {
               err[i]=eint/256;
               done=FALSE;
               }

         if (done) break;
         }

      for (i=1; i<65536; i++) err[i]+=err[i-1];

      if (err[65535]>0.0f)
         for (i=0; i<65536; i++) err[i]*=255.0/err[65535];
      }

   if ((data2=(unsigned char *)malloc(width*height*depth))==NULL) MEMERROR();

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            data2[i+(j+k*height)*width]=(int)(err[get_ushort(data3,width,height,depth,i,j,k)]+0.5);

   delete err;
   free(data3);

   set(data2,width*height*depth,width,height,depth);
   set_mapping(1.0f/255,0.0f);
   }

// print information
void databuf::print_info()
   {
   float minval,maxval;

   printf("type=");
   if (type==DATABUF_TYPE_BYTE) printf("byte");
   else if (type==DATABUF_TYPE_SHORT) printf("short");
   else if (type==DATABUF_TYPE_FLOAT) printf("float");
   else if (type==DATABUF_TYPE_RGB ||
            type==DATABUF_TYPE_RGB_MM ||
            type==DATABUF_TYPE_RGB_S3TC ||
            type==DATABUF_TYPE_RGB_MM_S3TC) printf("rgb");
   else if (type==DATABUF_TYPE_RGBA ||
            type==DATABUF_TYPE_RGBA_MM ||
            type==DATABUF_TYPE_RGBA_S3TC ||
            type==DATABUF_TYPE_RGBA_MM_S3TC) printf("rgba");
   if (type==DATABUF_TYPE_RGB_S3TC ||
       type==DATABUF_TYPE_RGBA_S3TC ||
       type==DATABUF_TYPE_RGB_MM_S3TC ||
       type==DATABUF_TYPE_RGBA_MM_S3TC) printf("/s3tc");
   if (type==DATABUF_TYPE_RGB_MM ||
       type==DATABUF_TYPE_RGBA_MM ||
       type==DATABUF_TYPE_RGB_MM_S3TC ||
       type==DATABUF_TYPE_RGBA_MM_S3TC) printf("/mipmap");
   printf(" bytes=%u\n",bytes);
   if (zsize==1 && tsteps==1) printf("xsize=%d ysize=%d\n",xsize,ysize);
   else if (zsize!=1 && tsteps==1) printf("xsize=%d ysize=%d zsize=%d\n",xsize,ysize,zsize);
   else printf("xsize=%d ysize=%d zsize=%d tsteps=%d\n",xsize,ysize,zsize,tsteps);
   if (type==DATABUF_TYPE_BYTE ||
       type==DATABUF_TYPE_SHORT ||
       type==DATABUF_TYPE_FLOAT)
      {
      if (isNAN(nodata)) printf("nodata=nan");
      else if (nodata==-MAXFLOAT) printf("nodata=-MAXFLOAT");
      else printf("nodata=%g",nodata);
      printf(" hasnodata=%s\n",checknodata()?"yes":"no");
      getminmax(&minval,&maxval);
      printf("minval=%g maxval=%g\n",minval,maxval);
      }
   }

// print values
void databuf::print_values()
   {
   unsigned int i,j,k,t;

   float val;
   float value[4]={0.0};

   if (missing()) printf("empty\n");
   else
      if (type==DATABUF_TYPE_BYTE || type==DATABUF_TYPE_SHORT || type==DATABUF_TYPE_FLOAT ||
          type==DATABUF_TYPE_RGB || type==DATABUF_TYPE_RGBA)
         for (t=0; t<tsteps; t++)
            {
            for (k=0; k<zsize; k++)
               {
               for (j=0; j<ysize; j++)
                  {
                  for (i=0; i<xsize; i++)
                     if (type==DATABUF_TYPE_BYTE || type==DATABUF_TYPE_SHORT || type==DATABUF_TYPE_FLOAT)
                        {
                        val=getval(i,j,k,t);

                        if (checkval(val)) printf("%g ",val);
                        else printf("none ");
                        }
                     else if (type==DATABUF_TYPE_RGB)
                        {
                        getrgb(i,j,k,t,value);
                        printf("(%g,%g,%g) ",value[0],value[1],value[2]);
                        }
                     else
                        {
                        getrgba(i,j,k,t,value);
                        printf("(%g,%g,%g,%g) ",value[0],value[1],value[2],value[3]);
                        }

                  printf("\n");
                  }

               if (k<zsize-1) printf("\n");
               }

            if (t<tsteps-1) printf("\n");
            }
      else printf("unprintable\n");
   }

// unsigned short int evaluator
int databuf::get_ushort(unsigned short int *data,
                        unsigned int width,unsigned int height,unsigned int depth,
                        unsigned int i,unsigned int j,unsigned int k)
   {
   if (i>=width || j>=height || k>=depth) ERRORMSG();
   return(data[i+(j+k*height)*width]);
   }

// unsigned short int gradient magnitude
double databuf::get_ushort_grad(unsigned short int *data,
                                unsigned int width,unsigned int height,unsigned int depth,
                                unsigned int i,unsigned int j,unsigned int k)
   {
   double gx,gy,gz;

   if (i>0)
      if (i<width-1) gx=(get_ushort(data,width,height,depth,i+1,j,k)-get_ushort(data,width,height,depth,i-1,j,k))/2.0;
      else gx=get_ushort(data,width,height,depth,i,j,k)-get_ushort(data,width,height,depth,i-1,j,k);
   else
      if (i<width-1) gx=get_ushort(data,width,height,depth,i+1,j,k)-get_ushort(data,width,height,depth,i,j,k);
      else gx=0.0;

   if (j>0)
      if (j<height-1) gy=(get_ushort(data,width,height,depth,i,j+1,k)-get_ushort(data,width,height,depth,i,j-1,k))/2.0;
      else gy=get_ushort(data,width,height,depth,i,j,k)-get_ushort(data,width,height,depth,i,j-1,k);
   else
      if (j<height-1) gy=get_ushort(data,width,height,depth,i,j+1,k)-get_ushort(data,width,height,depth,i,j,k);
      else gy=0.0;

   if (k>0)
      if (k<depth-1) gz=(get_ushort(data,width,height,depth,i,j,k+1)-get_ushort(data,width,height,depth,i,j,k-1))/2.0;
      else gz=get_ushort(data,width,height,depth,i,j,k)-get_ushort(data,width,height,depth,i,j,k-1);
   else
      if (k<depth-1) gz=get_ushort(data,width,height,depth,i,j,k+1)-get_ushort(data,width,height,depth,i,j,k);
      else gz=0.0;

   return(sqrt(gx*gx+gy*gy+gz*gz));
   }

// swap byte ordering between MSB and LSB
void databuf::swapbytes()
   {
   unsigned short int *ptr1,tmp1;
   unsigned int *ptr2,tmp2;

   if (type!=DATABUF_TYPE_SHORT && type!=DATABUF_TYPE_FLOAT) return;

   if (type==DATABUF_TYPE_SHORT)
      {
      if (bytes==0 || bytes%2!=0) ERRORMSG();

      ptr1=(unsigned short int *)((unsigned char *)data+bytes);

      while (ptr1--!=(unsigned short int *)data)
         {
         tmp1=*ptr1;
         *ptr1=(tmp1>>8)|((tmp1&255)<<8);
         }
      }
   else
      {
      if (bytes==0 || bytes%4!=0) ERRORMSG();

      ptr2=(unsigned int *)((unsigned char *)data+bytes);

      while (ptr2--!=(unsigned int *)data)
         {
         tmp2=*ptr2;
         *ptr2=(tmp2>>24)|((tmp2&(255<<16))>>8)|((tmp2&(255<<8))<<8)|((tmp2&255)<<24);
         }
      }
   }
