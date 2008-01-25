// (c) by Stefan Roettger

#include "minibase.h"

#include "miniio.h"
#include "miniutm.h"
#include "pnmbase.h"
#include "pnmsample.h"

#include "miniOGL.h"

#include "database.h"

// magic identifier
unsigned int databuf::MAGIC1=12640; // original magic identifier of DB version 1
unsigned int databuf::MAGIC2=13048; // backwards compatibility for DB version 2
unsigned int databuf::MAGIC3=13091; // backwards compatibility for DB version 3
unsigned int databuf::MAGIC4=13269; // actual magic identifier of DB version 4

// helper variable for LSB vs. MSB check
unsigned short int databuf::INTEL_CHECK=1;

// static hook for conversion from and to an external format
int (*databuf::CONVERSION_HOOK)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data)=NULL;
void *databuf::CONVERSION_DATA=NULL;

// static hook for automatic s3tc compression
void (*databuf::AUTOCOMPRESS_HOOK)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data)=NULL;
void *databuf::AUTOCOMPRESS_DATA=NULL;

// static hooks for conversion from an implicit format
void (*databuf::INTERPRETER_INIT)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data)=NULL;
void (*databuf::INTERPRETER_HOOK)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data)=NULL;
void *databuf::INTERPRETER_DATA=NULL;

// default constructor
databuf::databuf()
   {
   xsize=ysize=zsize=0;
   tsteps=0;
   type=0;

   swx=swy=0.0f;
   nwx=nwy=0.0f;
   nex=ney=0.0f;
   sex=sey=0.0f;
   h0=dh=0.0f;
   t0=dt=0.0f;

   LLWGS84_swx=LLWGS84_swy=0.0f;
   LLWGS84_nwx=LLWGS84_nwy=0.0f;
   LLWGS84_nex=LLWGS84_ney=0.0f;
   LLWGS84_sex=LLWGS84_sey=0.0f;

   scaling=1.0f;
   bias=0.0f;

   minvalue=1.0f;
   maxvalue=0.0f;

   extformat=0;
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

   if (ty==0) bs=1;
   else if (ty==1) bs=2;
   else if (ty==2) bs=4;
   else if (ty==3) bs=3;
   else if (ty==4) bs=4;
   else ERRORMSG();

   cs=xs*ys*zs*ts;
   bs*=cs;

   if ((data=malloc(bs))==NULL) ERRORMSG();

   if (ty==1)
      for (shortptr=(short int *)data,count=0; count<cs; count++) *shortptr++=0;
   else if (ty==2)
      for (floatptr=(float *)data,count=0; count<cs; count++) *floatptr++=0.0f;
   else
      for (byteptr=(unsigned char *)data,count=0; count<bs; count++) *byteptr++=0;

   bytes=bs;

   extformat=0;
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

   if (ty==0) bs=1;
   else if (ty==1) bs=2;
   else if (ty==2) bs=4;
   else if (ty==3) bs=3;
   else if (ty==4) bs=4;
   else if (ty==5) bs=3;
   else if (ty==6) bs=4;
   else ERRORMSG();

   bs*=xs*ys*zs*ts;

   if (ty==5 || ty==6) bs=length;

   if (length!=bs) ERRORMSG();

   data=chunk;
   bytes=bs;

   extformat=0;
   implformat=0;

   xsize=xs;
   ysize=ys;
   zsize=zs;
   tsteps=ts;
   type=ty;
   }

// copy data from memory chunk
void databuf::copy(void *chunk,unsigned int length,
                   unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts,unsigned int ty)
   {
   void *newdata;

   if ((newdata=malloc(length))==NULL) ERRORMSG();
   memcpy(newdata,chunk,length);

   set(newdata,length,xs,ys,zs,ts,ty);
   }

// reset buffer
void databuf::reset()
   {
   data=NULL;
   bytes=0;
   }

// release buffer
void databuf::release()
   {
   if (data!=NULL) free(data);

   data=NULL;
   bytes=0;
   }

// set native extents
void databuf::set_extents(float left,float right,float bottom,float top)
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
void databuf::set_LLWGS84extents(float left,float right,float bottom,float top)
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
void databuf::set_corners(float sw_corner_x,float sw_corner_y,
                          float se_corner_x,float se_corner_y,
                          float nw_corner_x,float nw_corner_y,
                          float ne_corner_x,float ne_corner_y)
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
void databuf::set_LLWGS84corners(float sw_corner_x,float sw_corner_y,
                                 float se_corner_x,float se_corner_y,
                                 float nw_corner_x,float nw_corner_y,
                                 float ne_corner_x,float ne_corner_y)
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

// write one float parameter
void databuf::writeparam(char *tag,float v,FILE *file,int digits)
   {
   char *ptr;

   double value,point;
   int digit;

   for (ptr=tag; *ptr!='\0'; ptr++) putc(*ptr,file);

   putc('=',file);

   if (v<0.0f)
      {
      putc('-',file);
      value=-v;
      }
   else value=v;

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

   putc('\n',file);
   }

// read one float parameter
int databuf::readparam(char *tag,float *v,FILE *file)
   {
   char ch,*ptr;

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

   while (ch==' ' || ch=='\n' || ch=='\r') ch=getc(file);

   ungetc(ch,file);

   *v=value/point*pow(10.0,exsgn*expnt);

   return(1);
   }

// read one integer parameter
int databuf::readparami(char *tag,int *v,FILE *file)
   {
   float value;

   if (readparam(tag,&value,file)==0) return(0);

   *v=ftrc(value+0.5f);

   return(1);
   }

// read one unsigned parameter
int databuf::readparamu(char *tag,unsigned int *v,FILE *file)
   {
   float value;

   if (readparam(tag,&value,file)==0) return(0);

   *v=ftrc(fabs(value)+0.5f);

   return(1);
   }

// save data as DB
// data is saved in MSB format
void databuf::savedata(const char *filename,
                       unsigned int extfmt)
   {
   FILE *file;

   if (data==NULL) return;

   if (bytes==0) ERRORMSG();

   // convert into external format
   if (extfmt!=0) convertchunk(1,extfmt);

   // open file for writing
   if ((file=fopen(filename,"wb"))==NULL) ERRORMSG();

   // save magic identifier
   writeparam("MAGIC",MAGIC4,file);

   // save mandatory metadata
   writeparam("xsize",xsize,file);
   writeparam("ysize",ysize,file);
   writeparam("zsize",zsize,file);
   writeparam("tsteps",tsteps,file);
   writeparam("type",type,file);

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
   if (*((unsigned char *)(&INTEL_CHECK))==0 || extformat!=0 || implformat!=0)
      {
      if (fwrite(data,bytes,1,file)!=1) ERRORMSG();
      fclose(file);
      }
   else
      {
      swapbytes();
      if (fwrite(data,bytes,1,file)!=1) ERRORMSG();
      swapbytes();

      fclose(file);
      }
   }

// load DB block
void databuf::loadblock(FILE *file)
   {
   const unsigned int block=1<<17;

   unsigned int cnt;

   if ((data=(unsigned char *)malloc(block))==NULL) ERRORMSG();

   do
      {
      cnt=fread(&((unsigned char *)data)[bytes],1,block,file);
      bytes+=cnt;

      if (cnt==block)
         if ((data=(unsigned char *)realloc(data,bytes+block))==NULL) ERRORMSG();
      }
   while (cnt==block);

   if (bytes==0) ERRORMSG();

   if ((data=(unsigned char *)realloc(data,bytes))==NULL) ERRORMSG();
   }

// load data from DB
// data is converted from MSB into native format
int databuf::loaddata(const char *filename)
   {
   FILE *file;

   unsigned int m;

   // open file for reading
   if ((file=fopen(filename,"rb"))==NULL)
      {
      fprintf(stderr,"unable to load %s!\n",filename);
      WARNMSG();
      return(0);
      }

   // read magic identifier
   if (readparamu("MAGIC",&m,file)==0)
      {
      fclose(file);
      return(0);
      }
   else if (m!=MAGIC1 && m!=MAGIC2 && m!=MAGIC3 && m!=MAGIC4)
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
   if (readparam("scaling",&scaling,file)==0) ERRORMSG();
   if (readparam("bias",&bias,file)==0) ERRORMSG();

   // read external format indicator
   if (m==MAGIC1) extformat=0;
   else if (readparamu("extformat",&extformat,file)==0) ERRORMSG();

   // read implicit format indicator
   if (m==MAGIC1 || m==MAGIC2) implformat=0;
   else if (readparamu("implformat",&implformat,file)==0) ERRORMSG();

   // read optional corner points in Lat/Lon
   if (m==MAGIC1 || m==MAGIC2 || m==MAGIC3)
      {
      LLWGS84_swx=LLWGS84_swy=0.0f;
      LLWGS84_nwx=LLWGS84_nwy=0.0f;
      LLWGS84_nex=LLWGS84_ney=0.0f;
      LLWGS84_sex=LLWGS84_sey=0.0f;
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

   // read data chunk
   if (bytes==0)
      {
      loadblock(file);
      fclose(file);
      }
   else
      {
      if ((data=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

      if (fread(data,bytes,1,file)!=1) ERRORMSG();
      fclose(file);
      }

   // check for lsb->msb conversion
   if (*((unsigned char *)(&INTEL_CHECK))!=0 && extformat==0 && implformat==0) swapbytes();

   // convert from external format
   if (extformat!=0) convertchunk(0,extformat);

   // convert from implicit format
   if (implformat!=0) interpretechunk(implformat);

   return(1);
   }

// convert from/into external format (e.g. JPEG/PNG)
void databuf::convertchunk(int israw,unsigned int extfmt)
   {
   int success;

   unsigned char *newdata;
   unsigned int newbytes;

   if (CONVERSION_HOOK==NULL)
      if (israw!=0) return;
      else ERRORMSG();

   if (israw!=0)
      if (*((unsigned char *)(&INTEL_CHECK))!=0) swapbytes();

   success=CONVERSION_HOOK(israw,(unsigned char *)data,bytes,extfmt,&newdata,&newbytes,this,CONVERSION_DATA);

   if (!success)
      if (israw!=0) return;
      else ERRORMSG();

   release();

   data=newdata;
   bytes=newbytes;

   if (israw==0)
      if (*((unsigned char *)(&INTEL_CHECK))!=0) swapbytes();

   if (israw==0) extformat=0;
   else extformat=extfmt;
   }

// set conversion hook for external formats
void databuf::setconversion(int (*conversion)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data),void *data)
   {
   CONVERSION_HOOK=conversion;
   CONVERSION_DATA=data;
   }

// automatic s3tc compression
void databuf::autocompress()
   {
   unsigned char *s3tcdata;
   unsigned int s3tcbytes;

   if (AUTOCOMPRESS_HOOK==NULL) return;

   if (type!=3 && type!=4) return;

   if (type==3) AUTOCOMPRESS_HOOK(0,(unsigned char *)data,bytes,&s3tcdata,&s3tcbytes,this,AUTOCOMPRESS_DATA);
   else AUTOCOMPRESS_HOOK(1,(unsigned char *)data,bytes,&s3tcdata,&s3tcbytes,this,AUTOCOMPRESS_DATA);

   release();

   data=s3tcdata;
   bytes=s3tcbytes;

   if (type==3) type=5;
   else type=6;
   }

// set hook for automatic s3tc compression
void databuf::setautocompress(void (*autocompress)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data),void *data)
   {
   AUTOCOMPRESS_HOOK=autocompress;
   AUTOCOMPRESS_DATA=data;
   }

// convert from implicit format
void databuf::interpretechunk(unsigned int implfmt)
   {
   int i;
   unsigned int x,y,z,t;
   float xx,yy,zz,tt;
   float value[4];
   int comps;

   if (INTERPRETER_INIT==NULL || INTERPRETER_HOOK==NULL) ERRORMSG();

   if (type>4) ERRORMSG();

   if (type==3) comps=3;
   else if (type==4) comps=4;
   else comps=1;

   INTERPRETER_INIT(implfmt,(char *)data,bytes,this,INTERPRETER_DATA);

   release();
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

// set interpreter hook for implicit format
void databuf::setinterpreter(void (*parser)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data),void *data,
                             void (*interpreter)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data))
   {
   INTERPRETER_INIT=parser;
   INTERPRETER_HOOK=interpreter;
   INTERPRETER_DATA=data;
   }

// read one line in either UNIX or WINDOWS format
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

// data is converted from PNM into native format
int databuf::loadPNMdata(const char *filename)
   {
   int width,height,components;
   PNMcomment comment;

   float coord[8],cellsize[2],vscale;
   int utm_zone,utm_datum,missing;

   if ((data=readPNMfile(filename,&width,&height,&components,&comment))==NULL)
      {
      fprintf(stderr,"unable to load %s!\n",filename);
      WARNMSG();
      return(0);
      }

   extformat=0;
   implformat=0;

   xsize=width;
   ysize=height;
   zsize=1;
   tsteps=1;

   if (components==1) type=0;
   else if (components==2) type=1;
   else if (components==3) type=3;
   else if (components==4) type=4;
   else ERRORMSG();

   bytes=xsize*ysize;
   if (type==1) bytes*=2;
   else if (type==3) bytes*=3;
   else if (type==4) bytes*=4;

   if (type==1)
      if (*((unsigned char *)(&INTEL_CHECK))!=0) swapbytes();

   if (getPNMparamsLL(&comment,
                      coord,cellsize,
                      &vscale,&missing,
                      &utm_zone,&utm_datum)==0) ERRORMSG();

   if (utm_zone!=0)
      {
      miniutm::UTM2LL(coord[0],coord[1],utm_zone,utm_datum,&coord[1],&coord[0]);
      miniutm::UTM2LL(coord[2],coord[3],utm_zone,utm_datum,&coord[3],&coord[2]);
      miniutm::UTM2LL(coord[4],coord[5],utm_zone,utm_datum,&coord[5],&coord[4]);
      miniutm::UTM2LL(coord[6],coord[7],utm_zone,utm_datum,&coord[7],&coord[6]);
      }

   swx=coord[0];
   swy=coord[1];
   nwx=coord[2];
   nwy=coord[3];
   nex=coord[4];
   ney=coord[5];
   sex=coord[6];
   sey=coord[7];

   h0=dh=0.0f;
   t0=dt=0.0f;

   scaling=vscale;
   bias=0.0f;

   LLWGS84_swx=coord[0]/(60*60);
   LLWGS84_swy=coord[1]/(60*60);
   LLWGS84_nwx=coord[2]/(60*60);
   LLWGS84_nwy=coord[3]/(60*60);
   LLWGS84_nex=coord[4]/(60*60);
   LLWGS84_ney=coord[5]/(60*60);
   LLWGS84_sex=coord[6]/(60*60);
   LLWGS84_sey=coord[7]/(60*60);

   return(1);
   }

// data is converted from PPM into compressed native format
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
      if (missing() || type!=3) ERRORMSG();

      if (compressRGBtexmap((unsigned char *)data,xsize,ysize,&cdata,&cbytes)!=0)
         {
         free(data);

         type=5;
         data=cdata;
         bytes=cbytes;

         savedata(str);
         }
      }

   return(1);
   }

// data is converted from normalized PPM into compressed native format
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
      if (missing() || type!=3) ERRORMSG();

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

         type=5;
         data=cdata;
         bytes=cbytes;
         }

      savedata(str);
      }

   return(1);
   }

// data is converted from PVM into native format
int databuf::loadPVMdata(const char *filename,
                         float midx,float midy,float basez,
                         float dx,float dy,float dz)
   {
   int width,height,depth,components;

   if ((data=readPVMvolume(filename,&width,&height,&depth,&components))==NULL)
      {
      fprintf(stderr,"unable to load %s!\n",filename);
      WARNMSG();
      return(0);
      }

   if (width<2 || height<2 || depth<2 || components!=1) ERRORMSG();

   extformat=0;
   implformat=0;

   xsize=width;
   ysize=height;
   zsize=depth;
   tsteps=1;
   type=0;

   bytes=xsize*ysize*zsize;

   swx=midx-dx/2.0f;
   swy=midy-dy/2.0f;
   nwx=midx-dx/2.0f;
   nwy=midy+dy/2.0f;
   nex=midx+dx/2.0f;
   ney=midy+dy/2.0f;
   sex=midx+dx/2.0f;
   sey=midy-dy/2.0f;

   h0=basez;
   dh=dz;

   t0=dt=0.0f;

   scaling=1.0f/255.0f;
   bias=0.0f;

   return(1);
   }

// data is converted from multiple time-dependent PVM files into native format
int databuf::loadPVMdata(const char *filename,
                         unsigned int t,unsigned int n,
                         float timestart,float timestep,
                         float midx,float midy,float basez,
                         float dx,float dy,float dz)
   {
   static const int maxstr=1000;

   unsigned int i;

   char str[maxstr];

   unsigned char *moredata;
   int width,height,depth,components;

   if (n==0) ERRORMSG();

   snprintf(str,maxstr,"%s-t%d",filename,t);

   if ((data=readPVMvolume(str,&width,&height,&depth,&components))==NULL)
      {
      snprintf(str,maxstr,"%s-t0%d",filename,t);

      if ((data=readPVMvolume(str,&width,&height,&depth,&components))==NULL)
         {
         snprintf(str,maxstr,"%s-t00%d",filename,t);

         if ((data=readPVMvolume(str,&width,&height,&depth,&components))==NULL)
            {
            fprintf(stderr,"unable to load %s!\n",filename);
            WARNMSG();
            return(0);
            }
         }
      }

   if (width<2 || height<2 || depth<2 || components!=1) ERRORMSG();

   extformat=0;
   implformat=0;

   xsize=width;
   ysize=height;
   zsize=depth;
   tsteps=n;
   type=0;

   bytes=xsize*ysize*zsize*tsteps;

   if ((data=realloc(data,bytes))==NULL) ERRORMSG();

   for (i=t+1; i<t+n; i++)
      {
      snprintf(str,maxstr,"%s-t%d",filename,i);

      if ((moredata=readPVMvolume(str,&width,&height,&depth,&components))==NULL)
         {
         snprintf(str,maxstr,"%s-t0%d",filename,i);

         if ((moredata=readPVMvolume(str,&width,&height,&depth,&components))==NULL)
            {
            snprintf(str,maxstr,"%s-t00%d",filename,i);

            if ((moredata=readPVMvolume(str,&width,&height,&depth,&components))==NULL) ERRORMSG();
            }
         }

      if ((unsigned int)width!=xsize || (unsigned int)height!=ysize || (unsigned int)depth!=zsize || components!=1) ERRORMSG();

      memcpy(&((unsigned char *)data)[(i-1)*xsize*ysize*zsize],moredata,xsize*ysize*zsize);
      free(moredata);
      }

   swx=midx-dx/2.0f;
   swy=midy-dy/2.0f;
   nwx=midx-dx/2.0f;
   nwy=midy+dy/2.0f;
   nex=midx+dx/2.0f;
   ney=midy+dy/2.0f;
   sex=midx+dx/2.0f;
   sey=midy-dy/2.0f;

   h0=basez;
   dh=dz;

   t0=timestart;
   dt=timestep;

   scaling=1.0f/255.0f;
   bias=0.0f;

   return(1);
   }

// data is converted from MOE into native format
int databuf::loadMOEdata(const char *filename,float *useful_smallest,float *useful_greatest)
   {
   FILE *file;

   char *str;

   int msb;
   float lon,lat,zorig;
   float xspace,yspace,zspace;
   float days,seconds,tstep;
   float usefs,usefg;

   float midx,midy,dx,dy;

   if ((file=fopen(filename,"rb"))==NULL)
      {
      fprintf(stderr,"unable to load %s!\n",filename);
      WARNMSG();
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

   type=2;

   bytes=xsize*ysize*zsize*tsteps*4;

   if ((data=malloc(bytes))==NULL) ERRORMSG();

   // read float data
   if (fread(data,bytes,1,file)!=1) ERRORMSG();
   fclose(file);

   // convert from MSB to native format
   if (msb!=0)
      if (*((unsigned char *)(&INTEL_CHECK))!=0) swapbytes();

   // convert from LSB to native format
   if (msb==0)
      if (*((unsigned char *)(&INTEL_CHECK))==0) swapbytes();

   extformat=0;
   implformat=0;

   midx=lon+xspace*(xsize-1)/2.0f;
   dx=fabs(xspace)*(xsize-1);

   midy=lat+yspace*(ysize-1)/2.0f;
   dy=fabs(yspace)*(ysize-1);

   swx=midx-dx/2.0f;
   swy=midy-dy/2.0f;
   nwx=midx-dx/2.0f;
   nwy=midy+dy/2.0f;
   nex=midx+dx/2.0f;
   ney=midy+dy/2.0f;
   sex=midx+dx/2.0f;
   sey=midy-dy/2.0f;

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
void databuf::savePNMimage(const char *filename)
   {
   if (extformat!=0 || implformat!=0) ERRORMSG();

   if (type==0) writePNMimage(filename,(unsigned char *)data,xsize,ysize,1);
   else if (type==1)
      if (*((unsigned char *)(&INTEL_CHECK))==0) writePNMimage(filename,(unsigned char *)data,xsize,ysize,2);
      else
         {
         swapbytes();
         writePNMimage(filename,(unsigned char *)data,xsize,ysize,2);
         swapbytes();
         }
   else if (type==3) writePNMimage(filename,(unsigned char *)data,xsize,ysize,3);
   else if (type==4) writePNMimage(filename,(unsigned char *)data,xsize,ysize,4);
   else ERRORMSG();
   }

// data is generated from plane equation
void databuf::generateplane(int size,
                            float px,float py,float pz,
                            float nx,float ny,float nz,
                            float dx,float dy,float dz)
   {
   alloc(2,2,2,1,2);

   swx=px-dx/2.0f;
   swy=py-dy/2.0f;
   nwx=px-dx/2.0f;
   nwy=py+dy/2.0f;
   nex=px+dx/2.0f;
   ney=py+dy/2.0f;
   sex=px+dx/2.0f;
   sey=py-dy/2.0f;

   h0=pz-dz/2.0f;
   dh=dz;

   t0=dt=0.0f;

   scaling=1.0f;
   bias=0.0f;

   setval(0,0,0,(swx-px)*nx+(swy-py)*ny-dh/2.0f*nz);
   setval(1,0,0,(sex-px)*nx+(sey-py)*ny-dh/2.0f*nz);
   setval(0,1,0,(nwx-px)*nx+(nwy-py)*ny-dh/2.0f*nz);
   setval(1,1,0,(nex-px)*nx+(ney-py)*ny-dh/2.0f*nz);
   setval(0,0,1,(swx-px)*nx+(swy-py)*ny+dh/2.0f*nz);
   setval(1,0,1,(sex-px)*nx+(sey-py)*ny+dh/2.0f*nz);
   setval(0,1,1,(nwx-px)*nx+(nwy-py)*ny+dh/2.0f*nz);
   setval(1,1,1,(nex-px)*nx+(ney-py)*ny+dh/2.0f*nz);

   resampledata(size,size,size);
   }

// convert data from one type to another
void databuf::convertdata(unsigned int newtype)
   {
   unsigned int cells;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   unsigned int count;

   float minvalue,maxvalue,value;

   void *newdata;

   cells=xsize*ysize*zsize*tsteps;

   if (type==0 && newtype==1)
      {
      if ((newdata=malloc(cells*2))==NULL) ERRORMSG();

      byteptr=(unsigned char *)data;
      shortptr=(short int *)newdata;

      for (count=0; count<cells; count++) *shortptr++=*byteptr++;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*2;
      }
   else if (type==0 && newtype==2)
      {
      if ((newdata=malloc(cells*4))==NULL) ERRORMSG();

      byteptr=(unsigned char *)data;
      floatptr=(float *)newdata;

      for (count=0; count<cells; count++) *floatptr++=scaling*(*byteptr++)+bias;

      scaling=1.0f;
      bias=0.0f;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*4;
      }
   else if (type==1 && newtype==0)
      {
      shortptr=(short int *)data;
      minvalue=maxvalue=scaling*(*shortptr++)+bias;

      for (count=1; count<cells; count++,shortptr++)
         {
         value=scaling*(*shortptr)+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells))==NULL) ERRORMSG();

      shortptr=(short int *)data;
      byteptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=scaling*(*shortptr++)+bias;
         *byteptr++=ftrc(255.0f*(value-minvalue)/(maxvalue-minvalue)+0.5f);
         }

      scaling=(maxvalue-minvalue)/255.0f;
      bias=minvalue;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps;
      }
   else if (type==1 && newtype==2)
      {
      if ((newdata=malloc(cells*4))==NULL) ERRORMSG();

      shortptr=(short int *)data;
      floatptr=(float *)newdata;

      for (count=0; count<cells; count++) *floatptr++=scaling*(*shortptr++)+bias;

      scaling=1.0f;
      bias=0.0f;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*4;
      }
   else if (type==2 && newtype==0)
      {
      floatptr=(float *)data;
      minvalue=maxvalue=scaling*(*floatptr++)+bias;

      for (count=1; count<cells; count++,floatptr++)
         {
         value=scaling*(*floatptr)+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells))==NULL) ERRORMSG();

      floatptr=(float *)data;
      byteptr=(unsigned char *)newdata;

      for (count=0; count<cells; count++)
         {
         value=scaling*(*floatptr++)+bias;
         *byteptr++=ftrc(255.0f*(value-minvalue)/(maxvalue-minvalue)+0.5f);
         }

      scaling=(maxvalue-minvalue)/255.0f;
      bias=minvalue;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps;
      }
   else if (type==2 && newtype==1)
      {
      floatptr=(float *)data;
      minvalue=maxvalue=scaling*(*floatptr++)+bias;

      for (count=1; count<cells; count++,floatptr++)
         {
         value=scaling*(*floatptr)+bias;

         if (value<minvalue) minvalue=value;
         else if (value>maxvalue) maxvalue=value;
         }

      if (minvalue==maxvalue) maxvalue++;

      if ((newdata=malloc(cells*2))==NULL) ERRORMSG();

      floatptr=(float *)data;
      shortptr=(short int *)newdata;

      for (count=0; count<cells; count++)
         {
         value=scaling*(*floatptr++)+bias;
         *shortptr++=ftrc(65535.0f*(value-minvalue)/(maxvalue-minvalue)-32768.0f+0.5f);
         }

      scaling=(maxvalue-minvalue)/65535.0f;
      bias=minvalue+32768.0f*scaling;

      free(data);
      data=newdata;
      type=newtype;

      bytes=xsize*ysize*zsize*tsteps*2;
      }
   else if (type==2 && newtype==2)
      {
      floatptr=(float *)data;

      if (scaling!=1.0f || bias!=0.0f)
         for (count=0; count<cells; count++,floatptr++) *floatptr=scaling*(*floatptr)+bias;

      scaling=1.0f;
      bias=0.0f;
      }
   }

// resample data at a different resolution
void databuf::resampledata(unsigned int xs,unsigned int ys,unsigned int zs)
   {
   unsigned int i,j,k,t;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   if (xs<2 || ys<2 || zs<2) ERRORMSG();
   if (xsize<2 || ysize<2 || zsize<2) ERRORMSG();

   if (xs==xsize && ys==ysize && zs==zsize) return;

   switch (type)
      {
      case 0:
         if ((byteptr=(unsigned char *)malloc(xs*ys*zs*tsteps))==NULL) ERRORMSG();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     byteptr[i+(j+(k+t*zs)*ys)*xs]=ftrc(getvalue((float)i/(xs-1),
                                                                 (float)j/(ys-1),
                                                                 (float)k/(zs-1),t)+0.5f);

         free(data);
         data=byteptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps;

         break;
      case 1:
         if ((shortptr=(short int *)malloc(xs*ys*zs*tsteps*2))==NULL) ERRORMSG();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     shortptr[i+(j+(k+t*zs)*ys)*xs]=ftrc(getvalue((float)i/(xs-1),
                                                                  (float)j/(ys-1),
                                                                  (float)k/(zs-1),t)+0.5f);

         free(data);
         data=shortptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*2;

         break;
      case 2:
         if ((floatptr=(float *)malloc(xs*ys*zs*tsteps*4))==NULL) ERRORMSG();

         for (t=0; t<tsteps; t++)
            for (i=0; i<xs; i++)
               for (j=0; j<ys; j++)
                  for (k=0; k<zs; k++)
                     floatptr[i+(j+(k+t*zs)*ys)*xs]=getvalue((float)i/(xs-1),
                                                             (float)j/(ys-1),
                                                             (float)k/(zs-1),t);

         free(data);
         data=floatptr;

         xsize=xs;
         ysize=ys;
         zsize=zs;

         bytes=xsize*ysize*zsize*tsteps*4;

         break;
      }
   }

// set a single scalar value
void databuf::setval(const unsigned int i,const unsigned int j,const unsigned int k,const float value)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   switch (type)
      {
      case 0:
         byteptr=(unsigned char *)data;
         byteptr[i+(j+k*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case 1:
         shortptr=(short int *)data;
         shortptr[i+(j+k*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case 2:
         floatptr=(float *)data;
         floatptr[i+(j+k*ysize)*xsize]=(value-bias)/scaling;
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
      case 0:
         byteptr=(unsigned char *)data;
         byteptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case 1:
         shortptr=(short int *)data;
         shortptr[i+(j+(k+t*zsize)*ysize)*xsize]=ftrc((value-bias)/scaling+0.5f);
         break;
      case 2:
         floatptr=(float *)data;
         floatptr[i+(j+(k+t*zsize)*ysize)*xsize]=(value-bias)/scaling;
         break;
      }
   }

// get a single scalar value
float databuf::getval(const unsigned int i,const unsigned int j,const unsigned int k)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   switch (type)
      {
      case 0:
         byteptr=(unsigned char *)data;
         return(scaling*(byteptr[i+(j+k*ysize)*xsize]+bias));
      case 1:
         shortptr=(short int *)data;
         return(scaling*(shortptr[i+(j+k*ysize)*xsize]+bias));
      case 2:
         floatptr=(float *)data;
         return(scaling*(floatptr[i+(j+k*ysize)*xsize]+bias));
      }

   return(0.0f);
   }

// get a single scalar value
float databuf::getval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t)
   {
   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   switch (type)
      {
      case 0:
         byteptr=(unsigned char *)data;
         return(scaling*(byteptr[i+(j+(k+t*zsize)*ysize)*xsize]+bias));
      case 1:
         shortptr=(short int *)data;
         return(scaling*(shortptr[i+(j+(k+t*zsize)*ysize)*xsize]+bias));
      case 2:
         floatptr=(float *)data;
         return(scaling*(floatptr[i+(j+(k+t*zsize)*ysize)*xsize]+bias));
      }

   return(0.0f);
   }

// sample brick at normalized position (x,y,z) and time frame t
float databuf::getvalue(float x,float y,float z,unsigned int t)
   {
   unsigned int i,j,k;

   unsigned char *byteptr;
   short int *shortptr;
   float *floatptr;

   unsigned int slice;

   x*=xsize-1;
   y*=ysize-1;
   z*=zsize-1;

   i=ftrc(x);
   j=ftrc(y);
   k=ftrc(z);

   x-=i;
   y-=j;
   z-=k;

   if (i==xsize-1)
      {
      i=xsize-2;
      x=1.0f;
      }

   if (j==ysize-1)
      {
      j=ysize-2;
      y=1.0f;
      }

   if (k==zsize-1)
      {
      k=zsize-2;
      z=1.0f;
      }

   slice=xsize*ysize;

   switch (type)
      {
      case 0:
         byteptr=&((unsigned char *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         return((1.0f-z)*((1.0f-y)*((1.0f-x)*byteptr[0]+x*byteptr[1])+
                          y*((1.0f-x)*byteptr[xsize]+x*byteptr[xsize+1]))+
                z*((1.0f-y)*((1.0f-x)*byteptr[slice]+x*byteptr[slice+1])+
                   y*((1.0f-x)*byteptr[slice+xsize]+x*byteptr[slice+xsize+1])));
      case 1:
         shortptr=&((short int *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         return((1.0f-z)*((1.0f-y)*((1.0f-x)*shortptr[0]+x*shortptr[1])+
                          y*((1.0f-x)*shortptr[xsize]+x*shortptr[xsize+1]))+
                z*((1.0f-y)*((1.0f-x)*shortptr[slice]+x*shortptr[slice+1])+
                   y*((1.0f-x)*shortptr[slice+xsize]+x*shortptr[slice+xsize+1])));
      case 2:
         floatptr=&((float *)data)[i+(j+(k+t*zsize)*ysize)*xsize];

         return((1.0f-z)*((1.0f-y)*((1.0f-x)*floatptr[0]+x*floatptr[1])+
                          y*((1.0f-x)*floatptr[xsize]+x*floatptr[xsize+1]))+
                z*((1.0f-y)*((1.0f-x)*floatptr[slice]+x*floatptr[slice+1])+
                   y*((1.0f-x)*floatptr[slice+xsize]+x*floatptr[slice+xsize+1])));
      }

   return(0.0f);
   }

// set rgb color
void databuf::setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const float *value)
   {
   unsigned char *ptr;

   if (type==3)
      {
      ptr=&((unsigned char *)data)[3*(i+(j+k*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      }
   }

// set rgb color
void databuf::setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value)
   {
   unsigned char *ptr;

   if (type==3)
      {
      ptr=&((unsigned char *)data)[3*(i+(j+(k+t*zsize)*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      }
   }

// set rgba color
void databuf::setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const float *value)
   {
   unsigned char *ptr;

   if (type==4)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+k*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      ptr[3]=ftrc((value[3]-bias)/scaling+0.5f);
      }
   }

// set rgba color
void databuf::setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value)
   {
   unsigned char *ptr;

   if (type==4)
      {
      ptr=&((unsigned char *)data)[4*(i+(j+(k+t*zsize)*ysize)*xsize)];

      ptr[0]=ftrc((value[0]-bias)/scaling+0.5f);
      ptr[1]=ftrc((value[1]-bias)/scaling+0.5f);
      ptr[2]=ftrc((value[2]-bias)/scaling+0.5f);
      ptr[3]=ftrc((value[3]-bias)/scaling+0.5f);
      }
   }

// get the minimum and maximum scalar value
void databuf::getminmax(float *minval,float *maxval)
   {
   unsigned int i,j,k,t;

   float val;
   float minv,maxv;

   if (minval==NULL || maxval==NULL)
      if (minvalue<=maxvalue) return;
      else
         {
         minval=&minvalue;
         maxval=&maxvalue;
         }

   minv=maxv=getval(0,0,0,0);

   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if (val<minv) minv=val;
               else if (val>maxv) maxv=val;
               }

   *minval=minv;
   *maxval=maxv;
   }

// get the minimum and maximum scalar value within a useful range
void databuf::getminmax(float usefs,float usefg,
                        float *minval,float *maxval)
   {
   unsigned int i,j,k,t;

   float val;
   float minv,maxv;

   if (minval==NULL || maxval==NULL)
      if (minvalue<=maxvalue) return;
      else
         {
         minval=&minvalue;
         maxval=&maxvalue;
         }

   minv=0.0f;
   maxv=1.0f;

   // search valid scalar value
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if ((val>=usefs && val<=usefg) ||
                   (val<=usefs && val>=usefg))
                  {
                  minv=maxv=val;

                  i=xsize;
                  j=ysize;
                  k=zsize;
                  t=tsteps;
                  }
               }

   // check all scalar values except the "no data" values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if ((val<usefs || val>usefg) &&
                   (val>usefs || val<usefg))
                  if (val<minv) minv=val;
                  else if (val>maxv) maxv=val;
               }

   *minval=minv;
   *maxval=maxv;
   }

// replace invalid values
void databuf::replaceinvalid(float usefs,float usefg,float useful)
   {
   unsigned int i,j,k,t;

   float val;

   // check for "no data" values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               {
               val=getval(i,j,k,t);

               if ((val<usefs || val>usefg) &&
                   (val>usefs || val<usefg)) setval(i,j,k,t,useful);
               }
   }

// compute absolute values
void databuf::computeabsolute()
   {
   unsigned int i,j,k,t;

   float val;

   // check for negative values
   for (t=0; t<tsteps; t++)
      for (i=0; i<xsize; i++)
         for (j=0; j<ysize; j++)
            for (k=0; k<zsize; k++)
               if ((val=getval(i,j,k,t))<0.0f) setval(i,j,k,t,-val);
   }

// swap byte ordering between MSB and LSB
void databuf::swapbytes()
   {
   unsigned short int *ptr1,tmp1;
   unsigned int *ptr2,tmp2;

   if (type==0 || type>2) return;

   if (type==1)
      {
      if (bytes==0 || bytes%2!=0) ERRORMSG();

      ptr1=(unsigned short int *)((unsigned char *)data+bytes);

      while (ptr1--!=(unsigned short int *)data)
         {
         tmp1=*ptr1;
         *ptr1=(tmp1>>8)|((tmp1&255)<<8);
         }
      }
   else if (type==2)
      {
      if (bytes==0 || bytes%4!=0) ERRORMSG();

      ptr2=(unsigned int *)((unsigned char *)data+bytes);

      while (ptr2--!=(unsigned int *)data)
         {
         tmp2=*ptr2;
         *ptr2=(tmp2>>24)|((tmp2&(255<<16))>>8)|((tmp2&(255<<8))<<8)|((tmp2&255)<<24);
         }
      }
   else ERRORMSG();
   }
