// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"
#include "miniio.h"
#include "minidir.h"
#include "ministrip.h"

#include "rawbase.h"

#define SOBEL

float RAW_TARGET_RATIO=0.5f;
long long RAW_TARGET_CELLS=250000000;

unsigned short int RAW_INTEL=1;

inline void RAW_swap4(char *x)
   {
   char a=x[0];
   char b=x[1];
   char c=x[2];
   char d=x[3];

   x[0]=d;
   x[1]=c;
   x[2]=b;
   x[3]=a;
   }

// analyze RAW file format
BOOLINT readRAWinfo(char *filename,
                    long long *width,long long *height,long long *depth,long long *steps,
                    unsigned int *components,unsigned int *bits,BOOLINT *sign,BOOLINT *msb,
                    float *scalex,float *scaley,float *scalez)
   {
   unsigned int rawcomps=1; // scalar
   BOOLINT rawsign=FALSE; // unsigned
   BOOLINT rawbits=8; // byte
   BOOLINT rawmsb=TRUE; // most significant byte first
   int rawscalex=1000000,rawscaley=1000000,rawscalez=1000000; // 1E6um = 1m
   int rawmaxscale=0; // 0um

   char *dot,*dotdot;
   int count;

   dot=strrchr(filename,'.');

   if (dot==NULL) return(FALSE);

   if (strcasecmp(dot,".raw")!=0) return(FALSE);

   *dot='\0';
   dotdot=strrchr(filename,'.');
   *dot='.';

   if (dotdot==NULL) return(FALSE);

   *width=*height=*depth=*steps=1;

   dotdot++;

   if (sscanf(dotdot,"%lldx%lld%n",width,height,&count)!=2) return(FALSE);

   dotdot+=count;

   if (*dotdot=='x')
      {
      dotdot++;

      if (sscanf(dotdot,"%lld%n",depth,&count)!=1) return(FALSE);

      dotdot+=count;
      }

   if (*dotdot=='x')
      {
      dotdot++;

      if (sscanf(dotdot,"%lld%n",steps,&count)!=1) return(FALSE);

      dotdot+=count;
      }

   if (*dotdot=='_')
      {
      dotdot++;

      while (*dotdot!='.' && *dotdot!='_')
         {
         switch (*dotdot)
            {
            case '1': rawcomps=1; rawbits=8; break; // char
            case '2': rawcomps=1; rawbits=16; break; // short
            case '3': rawcomps=3; rawbits=8; break; // rgb
            case '4': rawcomps=4; rawbits=8; break; // rgba
            case '6': rawcomps=3; rawbits=16; break; // rgb 16-bit
            case '8': rawcomps=4; rawbits=16; break; // rgba 16-bit
            case 'f': rawcomps=1; rawbits=32; break; // float 32-bit
            case 'u': rawsign=FALSE; break; // unsigned
            case 's': rawsign=TRUE; break; // signed
            case 'm': rawmsb=TRUE; break; // MSB
            case 'l': rawmsb=FALSE; break; // LSB
            default: return(FALSE);
            }

         dotdot++;
         }
      }

   if (*dotdot=='_')
      {
      dotdot++;

      if (sscanf(dotdot,"%dx%d%n",&rawscalex,&rawscaley,&count)!=2) return(FALSE);

      dotdot+=count;

      if (*dotdot=='x')
         {
         dotdot++;

         if (sscanf(dotdot,"%d%n",&rawscalez,&count)!=1) return(FALSE);

         dotdot+=count;
         }
      }

   if (*dotdot!='.') return(FALSE);

   if (bits==NULL)
      if (rawcomps==1 && rawbits==16)
         {
         rawcomps=2;
         rawbits=8;
         }

   if (rawcomps!=1 && components==NULL) return(FALSE);
   if (rawbits!=8 && bits==NULL) return(FALSE);
   if (rawsign!=FALSE && sign==NULL) return(FALSE);
   if (rawmsb!=TRUE && msb==NULL) return(FALSE);

   if (components!=NULL) *components=rawcomps;
   if (bits!=NULL) *bits=rawbits;
   if (sign!=NULL) *sign=rawsign;
   if (msb!=NULL) *msb=rawmsb;

   if (rawscalex>rawmaxscale) rawmaxscale=rawscalex;
   if (rawscaley>rawmaxscale) rawmaxscale=rawscaley;
   if (rawscalez>rawmaxscale) rawmaxscale=rawscalez;

   if (rawmaxscale==0) return(FALSE);

   if (scalex!=NULL) *scalex=rawscalex/1E6f;
   if (scaley!=NULL) *scaley=rawscaley/1E6f;
   if (scalez!=NULL) *scalez=rawscalez/1E6f;

   return(TRUE);
   }

// define RAW file format
char *makeRAWinfo(long long width,long long height,long long depth,long long steps,
                  unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                  float scalex,float scaley,float scalez)
   {
   static const int maxlen=100;

   char info[maxlen];
   float maxscale=0.0f;

   snprintf(info,maxlen,".%lldx%lld",width,height);
   if (depth>1) snprintf(&info[strlen(info)],maxlen-strlen(info),"x%lld",depth);
   if (steps>1) snprintf(&info[strlen(info)],maxlen-strlen(info),"x%lld",steps);

   if (components!=1 || bits!=8 || sign!=FALSE || msb!=TRUE ||
       scalex!=1.0f || scaley!=1.0f || scalez!=1.0f)
      {
      snprintf(&info[strlen(info)],maxlen-strlen(info),"_");

      if (sign==FALSE) snprintf(&info[strlen(info)],maxlen-strlen(info),"u");
      else snprintf(&info[strlen(info)],maxlen-strlen(info),"s");

      if (components==1 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"1");
      else if (components==1 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"2");
      else if (components==2 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"2");
      else if (components==3 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"3");
      else if (components==4 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"4");
      else if (components==3 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"6");
      else if (components==4 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"8");
      else return(NULL);

      if (components==2 || bits==16)
         if (msb==TRUE) snprintf(&info[strlen(info)],maxlen-strlen(info),"m");
         else snprintf(&info[strlen(info)],maxlen-strlen(info),"l");

      if (scalex>maxscale) maxscale=scalex;
      if (scaley>maxscale) maxscale=scaley;
      if (scalez>maxscale) maxscale=scalez;

      if (maxscale==0.0f) return(NULL);

      if (scalex!=1.0f || scaley!=1.0f || scalez!=1.0f)
         {
         snprintf(&info[strlen(info)],maxlen-strlen(info),"_%dx%d",int(1E6f*scalex+0.5f),int(1E6f*scaley+0.5f));
         if (depth>1) snprintf(&info[strlen(info)],maxlen-strlen(info),"x%d",int(1E6f*scalez+0.5f));
         }
      }

   snprintf(&info[strlen(info)],maxlen-strlen(info),".raw");

   return(strdup(info));
   }

// remove .raw suffix
char *removeRAWsuffix(const char *filename)
   {
   char *filename2,*dot;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   filename2=strdup(filename);

   if (readRAWinfo(filename2,
                   &rawwidth,&rawheight,&rawdepth,&rawsteps,
                   &rawcomps,&rawbits,&rawsign,&rawmsb,
                   &rawscalex,&rawscaley,&rawscalez))
      {
      dot=strrchr(filename2,'.');
      if (dot!=NULL)
         {
         *dot='\0';
         dot=strrchr(filename2,'.');
         if (dot!=NULL) *dot='\0';
         }
      }
   else
      {
      dot=strrchr(filename2,'.');
      if (dot!=NULL)
         if (strcasecmp(dot,".raw")==0) *dot='\0';
      }

   return(filename2);
   }

// append RAW file format suffix
char *appendRAWinfo(const char *filename,
                    long long width,long long height,long long depth,long long steps,
                    unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                    float scalex,float scaley,float scalez)
   {
   char *filename2;
   char *info;
   char *filename3;

   // define RAW info
   info=makeRAWinfo(width,height,depth,steps,
                    components,bits,sign,msb,
                    scalex,scaley,scalez);

   if (info==NULL) return(NULL);

   // remove suffix
   filename2=removeRAWsuffix(filename);

   // append RAW info to filename
   filename3=strdup2(filename2,info);
   free(filename2);
   free(info);

   return(filename3);
   }

// read a RAW volume
unsigned char *readRAWvolume(const char *filename,
                             long long *width,long long *height,long long *depth,long long *steps,
                             unsigned int *components,unsigned int *bits,BOOLINT *sign,BOOLINT *msb,
                             float *scalex,float *scaley,float *scalez)
   {
   FILE *file;

   char *name;

   unsigned char *volume;
   long long bytes;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    width,height,depth,steps,
                    components,bits,sign,msb,
                    scalex,scaley,scalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   bytes=(*width)*(*height)*(*depth)*(*components)*(*steps);

   if (bits!=NULL)
      if (*bits==16) bytes*=2;
      else if (*bits==32) bytes*=4;

   if ((volume=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

   // read RAW volume
   if (fread(volume,bytes,1,file)!=1)
      {
      free(volume);
      fclose(file);
      return(NULL);
      }

   fclose(file);

   return(volume);
   }

// write a RAW volume
char *writeRAWvolume(const char *filename, // /wo suffix .raw
                     unsigned char *volume,
                     long long width,long long height,long long depth,long long steps,
                     unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                     float scalex,float scaley,float scalez)
   {
   FILE *file;

   char *output;
   long long bytes;

   // make RAW info
   output=appendRAWinfo(filename,
                        width,height,depth,steps,
                        components,bits,sign,msb,
                        scalex,scaley,scalez);

   if (output==NULL) return(NULL);

   // open RAW output file
   if ((file=fopen(output,"wb"))==NULL)
      {
      free(output);
      return(NULL);
      }

   bytes=width*height*depth*components*steps;

   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // write RAW volume
   if (fwrite(volume,bytes,1,file)!=1)
      {
      fclose(file);
      return(NULL);
      }

   fclose(file);

   return(output);
   }

// copy a RAW volume
char *copyRAWvolume(FILE *file, // source file desc
                    const char *output, // destination file name /wo suffix .raw
                    long long width,long long height,long long depth,long long steps,
                    unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                    float scalex,float scaley,float scalez)
   {
   long long i,j;

   unsigned char *slice;
   long long bytes;

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   bytes=width*height*components;

   // compute total number of bytes per slice
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // make RAW info
   outname=appendRAWinfo(output,
                         width,height,depth,steps,
                         components,bits,sign,msb,
                         scalex,scaley,scalez);

   if (outname==NULL) return(NULL);

   // open RAW output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      free(outname);
      return(NULL);
      }

   if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

   // process out-of-core slice by slice
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         if (fwrite(slice,bytes,1,outfile)!=1)
            {
            free(slice);
            free(outname);
            fclose(outfile);
            return(NULL);
            }
         }

   free(slice);
   fclose(outfile);

   return(outname);
   }

// copy a RAW volume
char *copyRAWvolume(const char *filename, // source file
                    const char *output) // destination file name /wo suffix .raw
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   outname=copyRAWvolume(file,output,
                         rawwidth,rawheight,rawdepth,rawsteps,
                         rawcomps,rawbits,rawsign,rawmsb,
                         rawscalex,rawscaley,rawscalez);

   fclose(file);

   return(outname);
   }

// convert a RAW array to a 16-bit unsigned array
unsigned short int *convert2short(unsigned char *source,long long cells,unsigned int &components,
                                  unsigned int &bits,BOOLINT sign,BOOLINT msb)
   {
   long long i;

   unsigned short int *shorts;

   float v;

   if (components==2 && bits==8)
      {
      components=1;
      bits=16;
      }

   cells*=components;

   if ((shorts=(unsigned short int *)malloc(cells*sizeof(unsigned short int)))==NULL) ERRORMSG();

   if (bits==8)
      if (sign)
         for (i=0; i<cells; i++) shorts[i]=source[i]+128;
      else
         for (i=0; i<cells; i++) shorts[i]=source[i];
   else if (bits==16)
      if (msb)
         if (sign)
            for (i=0; i<cells; i++) shorts[i]=(signed short)(256*source[i<<1]+source[(i<<1)+1])+32768;
         else
            for (i=0; i<cells; i++) shorts[i]=(unsigned short)(256*source[i<<1]+source[(i<<1)+1]);
      else
         if (sign)
            for (i=0; i<cells; i++) shorts[i]=(signed short)(source[i<<1]+256*source[(i<<1)+1])+32768;
         else
            for (i=0; i<cells; i++) shorts[i]=(unsigned short)(source[i<<1]+256*source[(i<<1)+1]);
   else if (bits==32)
      if (msb)
         if (RAW_ISINTEL)
            for (i=0; i<cells; i++)
               {
               v=fabs(*(float *)(&source[i<<2]));
               RAW_swap4((char *)&v);
               shorts[i]=v>1.0f?65535:(unsigned int)ffloor(65535.0f*v+0.5f);
               }
         else
            for (i=0; i<cells; i++)
               {
               v=fabs(*(float *)(&source[i<<2]));
               shorts[i]=v>1.0f?65535:(unsigned int)ffloor(65535.0f*v+0.5f);
               }
      else
         if (RAW_ISINTEL)
            for (i=0; i<cells; i++)
               {
               v=fabs(*(float *)(&source[i<<2]));
               shorts[i]=v>1.0f?65535:(unsigned int)ffloor(65535.0f*v+0.5f);
               }
         else
            for (i=0; i<cells; i++)
               {
               v=fabs(*(float *)(&source[i<<2]));
               RAW_swap4((char *)&v);
               shorts[i]=v>1.0f?65535:(unsigned int)ffloor(65535.0f*v+0.5f);
               }
   else ERRORMSG();

   return(shorts);
   }

// strip a 16-bit to a 8-bit array
unsigned char *strip2byte(unsigned short int *source,long long cells,unsigned int components)
   {
   long long i;

   unsigned char *chars;

   cells*=components;

   if ((chars=(unsigned char *)malloc(cells*sizeof(unsigned char)))==NULL) ERRORMSG();

   for (i=0; i<cells; i++) chars[i]=source[i];

   return(chars);
   }

// compute minimum and maximum 16-bit unsigned value
void convert2minmax(unsigned short int *shorts,long long cells,unsigned int components,
                    unsigned int &minval,unsigned int &maxval)
   {
   long long i;

   unsigned int v;

   cells*=components;

   minval=65535;
   maxval=0;

   for (i=0; i<cells; i++)
      {
      v=shorts[i];
      if (v<minval) minval=v;
      if (v>maxval) maxval=v;
      }
   }

// quantize a 16-bit unsigned array to a char array
unsigned char *convert2char(unsigned short int *shorts,long long cells,unsigned int components,
                            unsigned int minval,unsigned int maxval)
   {
   long long i;

   unsigned char *chars;

   if (minval==maxval) maxval++;

   cells*=components;

   if ((chars=(unsigned char *)malloc(cells))==NULL) ERRORMSG();

   for (i=0; i<cells; i++)
      chars[i]=(int)ffloor(255.0f*(shorts[i]-minval)/(maxval-minval)+0.5f);

   return(chars);
   }

// copy a RAW volume with out-of-core linear quantization
char *copyRAWvolume_linear(FILE *file, // source file desc
                           const char *output, // destination file name /wo suffix .raw
                           long long width,long long height,long long depth,long long steps,
                           unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                           float scalex,float scaley,float scalez,
                           void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   long long i,j;

   unsigned char *slice;
   long long cells;
   long long bytes;

   unsigned short int *shorts;
   unsigned char *chars;

   long long tellpos;

   unsigned int minval0,minval;
   unsigned int maxval0,maxval;

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   cells=bytes=width*height;

   // compute total number of bytes per slice
   bytes*=components;
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // remember seek position
   tellpos=ftell(file);

   minval0=65535;
   maxval0=0;

   // scan for minimum and maximum value
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("scanning data",0.5f*(i*depth+j+1)/(depth*steps),obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            return(NULL);
            }

         shorts=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         convert2minmax(shorts,cells,components,minval,maxval);
         free(shorts);

         if (minval<minval0) minval0=minval;
         if (maxval>maxval0) maxval0=maxval;
         }

   // seek back to start
   if (fseek(file,tellpos,SEEK_SET)==-1) return(NULL);

   // make RAW info
   outname=appendRAWinfo(output,
                         width,height,depth,steps,
                         components,8,FALSE,TRUE,
                         scalex,scaley,scalez);

   if (outname==NULL) return(NULL);

   // open RAW output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      free(outname);
      return(NULL);
      }

   // process out-of-core slice by slice
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("writing data",0.5f*(i*depth+j+1)/(depth*steps)+0.5f,obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         shorts=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         chars=convert2char(shorts,cells,components,minval0,maxval0);
         free(shorts);

         if (fwrite(chars,cells*components,1,outfile)!=1)
            {
            free(chars);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         free(chars);
         }

   fclose(outfile);

   return(outname);
   }

// copy a RAW volume with out-of-core linear quantization
char *copyRAWvolume_linear(const char *filename, // source file
                           const char *output, // destination file name /wo suffix .raw
                           void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   // check for quantization
   if (rawcomps==1 && rawbits==8 && !rawsign)
      outname=strdup(filename);
   else
      outname=copyRAWvolume_linear(file,output,
                                   rawwidth,rawheight,rawdepth,rawsteps,
                                   rawcomps,rawbits,rawsign,rawmsb,
                                   rawscalex,rawscaley,rawscalez,
                                   feedback,obj);

   fclose(file);

   return(outname);
   }

// helper to get a short value from 3 consecutive slices
inline int getshort(unsigned short int *shorts[],
                    unsigned int width,unsigned int /*height*/,unsigned int components,
                    unsigned int i,unsigned int j,int k=0)
   {
   unsigned int c;

   unsigned int idx;
   unsigned int value;

   idx=(i+j*width)*components;
   for (value=0,c=0; c<components; c++) value+=shorts[k+1][idx+c];

   return(value/components);
   }

// helper to get a gradient value from 3 consecutive slices
inline double getgrad(unsigned short int *shorts[],
                      unsigned int width,unsigned int height,unsigned int components,
                      unsigned int i,unsigned int j,int k=0)
   {
   double gx,gy,gz;

   if (i>0)
      if (i<width-1) gx=(getshort(shorts,width,height,components,i+1,j,k)-getshort(shorts,width,height,components,i-1,j,k))/2.0;
      else gx=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i-1,j,k);
   else
      if (i<width-1) gx=getshort(shorts,width,height,components,i+1,j,k)-getshort(shorts,width,height,components,i,j,k);
      else gx=0.0;

   if (j>0)
      if (j<height-1) gy=(getshort(shorts,width,height,components,i,j+1,k)-getshort(shorts,width,height,components,i,j-1,k))/2.0;
      else gy=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i,j-1,k);
   else
      if (j<height-1) gy=getshort(shorts,width,height,components,i,j+1,k)-getshort(shorts,width,height,components,i,j,k);
      else gy=0.0;

   if (shorts[k]!=NULL)
      if (shorts[k+2]!=NULL) gz=(getshort(shorts,width,height,components,i,j,k+1)-getshort(shorts,width,height,components,i,j,k-1))/2.0;
      else gz=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i,j,k-1);
   else
      if (shorts[k+2]!=NULL) gz=getshort(shorts,width,height,components,i,j,k+1)-getshort(shorts,width,height,components,i,j,k);
      else gz=0.0;

   return(sqrt(gx*gx+gy*gy+gz*gz));
   }

// helper to get a gradient vector from 3 consecutive slices
inline miniv3f getgradvec(unsigned short int *shorts[],
                          unsigned int width,unsigned int height,unsigned int components,
                          unsigned int i,unsigned int j,int k=0)
   {
   miniv3f gv;

   if (i>0)
      if (i<width-1) gv.x=(getshort(shorts,width,height,components,i+1,j,k)-getshort(shorts,width,height,components,i-1,j,k))/2.0;
      else gv.x=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i-1,j,k);
   else
      if (i<width-1) gv.x=getshort(shorts,width,height,components,i+1,j,k)-getshort(shorts,width,height,components,i,j,k);
      else gv.x=0.0;

   if (j>0)
      if (j<height-1) gv.y=(getshort(shorts,width,height,components,i,j+1,k)-getshort(shorts,width,height,components,i,j-1,k))/2.0;
      else gv.y=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i,j-1,k);
   else
      if (j<height-1) gv.y=getshort(shorts,width,height,components,i,j+1,k)-getshort(shorts,width,height,components,i,j,k);
      else gv.y=0.0;

   if (shorts[k]!=NULL)
      if (shorts[k+2]!=NULL) gv.z=(getshort(shorts,width,height,components,i,j,k+1)-getshort(shorts,width,height,components,i,j,k-1))/2.0;
      else gv.z=getshort(shorts,width,height,components,i,j,k)-getshort(shorts,width,height,components,i,j,k-1);
   else
      if (shorts[k+2]!=NULL) gv.z=getshort(shorts,width,height,components,i,j,k+1)-getshort(shorts,width,height,components,i,j,k);
      else gv.z=0.0;

   return(gv);
   }

// helper to get a sobelized gradient vector from 3 consecutive slices
inline miniv3f getsobelvec(unsigned short int *shorts[],
                           unsigned int width,unsigned int height,unsigned int components,
                           unsigned int i,unsigned int j,int k=0)
   {
   miniv3f sgv;

   int v[27];

   if (i>0 && i<width-1 &&
       j>0 && j<height-1 &&
       shorts[k]!=NULL && shorts[k+2]!=NULL)
      {
      v[0]=getshort(shorts,width,height,components,i-1,j-1,k-1);
      v[1]=3*getshort(shorts,width,height,components,i,j-1,k-1);
      v[2]=getshort(shorts,width,height,components,i+1,j-1,k-1);
      v[3]=3*getshort(shorts,width,height,components,i-1,j,k-1);
      v[4]=6*getshort(shorts,width,height,components,i,j,k-1);
      v[5]=3*getshort(shorts,width,height,components,i+1,j,k-1);
      v[6]=getshort(shorts,width,height,components,i-1,j+1,k-1);
      v[7]=3*getshort(shorts,width,height,components,i,j+1,k-1);
      v[8]=getshort(shorts,width,height,components,i+1,j+1,k-1);

      v[9]=3*getshort(shorts,width,height,components,i-1,j-1,k);
      v[10]=6*getshort(shorts,width,height,components,i,j-1,k);
      v[11]=3*getshort(shorts,width,height,components,i+1,j-1,k);
      v[12]=6*getshort(shorts,width,height,components,i-1,j,k);
      v[13]=getshort(shorts,width,height,components,i,j,k);
      v[14]=6*getshort(shorts,width,height,components,i+1,j,k);
      v[15]=3*getshort(shorts,width,height,components,i-1,j+1,k);
      v[16]=6*getshort(shorts,width,height,components,i,j+1,k);
      v[17]=3*getshort(shorts,width,height,components,i+1,j+1,k);

      v[18]=getshort(shorts,width,height,components,i-1,j-1,k+1);
      v[19]=3*getshort(shorts,width,height,components,i,j-1,k+1);
      v[20]=getshort(shorts,width,height,components,i+1,j-1,k+1);
      v[21]=3*getshort(shorts,width,height,components,i-1,j,k+1);
      v[22]=6*getshort(shorts,width,height,components,i,j,k+1);
      v[23]=3*getshort(shorts,width,height,components,i+1,j,k+1);
      v[24]=getshort(shorts,width,height,components,i-1,j+1,k+1);
      v[25]=3*getshort(shorts,width,height,components,i,j+1,k+1);
      v[26]=getshort(shorts,width,height,components,i+1,j+1,k+1);

      sgv.x=(-v[0]-v[3]-v[6]-v[9]-v[12]-v[15]-v[18]-v[21]-v[24]+v[2]+v[5]+v[8]+v[11]+v[14]+v[17]+v[20]+v[23]+v[26])/44.0f;
      sgv.y=(-v[0]-v[1]-v[2]-v[9]-v[10]-v[11]-v[18]-v[19]-v[20]+v[6]+v[7]+v[8]+v[15]+v[16]+v[17]+v[24]+v[25]+v[26])/44.0f;
      sgv.z=(-v[0]-v[1]-v[2]-v[3]-v[4]-v[5]-v[6]-v[7]-v[8]+v[18]+v[19]+v[20]+v[21]+v[22]+v[23]+v[24]+v[25]+v[26])/44.0f;
      }
   else sgv=getgradvec(shorts,width,height,components,i,j,k);

   return(sgv);
   }

// update error table
void convert2error(unsigned short int *shorts[],unsigned int width,unsigned int height,unsigned int components,
                   double *err)
   {
   unsigned int i,j;

   for (i=0; i<width; i++)
      for (j=0; j<height; j++)
         err[getshort(shorts,width,height,components,i,j)]+=sqrt(getgrad(shorts,width,height,components,i,j));
   }

// integrate error table
void integrate(double *err,unsigned int maxval,unsigned int minval)
   {
   unsigned int i,k;

   double eint;

   BOOLINT done;

   for (i=0; i<65536; i++) err[i]=pow(err[i],1.0/3);

   err[minval]=err[maxval]=0.0;

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

// quantize a 16-bit unsigned array to a char array using an integrated error table
unsigned char *convert2char(unsigned short int *shorts,long long cells,unsigned int components,
                            double *err)
   {
   long long i;

   unsigned char *chars;

   cells*=components;

   if ((chars=(unsigned char *)malloc(cells))==NULL) ERRORMSG();

   for (i=0; i<cells; i++)
      chars[i]=(int)(err[shorts[i]]+0.5);

   return(chars);
   }

// copy a RAW volume with out-of-core non-linear quantization
char *copyRAWvolume_nonlinear(FILE *file, // source file desc
                              const char *output, // destination file name /wo suffix .raw
                              long long width,long long height,long long depth,long long steps,
                              unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                              float scalex,float scaley,float scalez,
                              void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   long long i,j;

   unsigned char *slice;
   long long cells;
   long long bytes;

   unsigned short int *shorts[3];
   unsigned char *chars;

   long long tellpos;

   unsigned int minval0,minval;
   unsigned int maxval0,maxval;

   BOOLINT linear;

   double *err;

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   cells=bytes=width*height;

   // compute total number of bytes per slice
   bytes*=components;
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // remember seek position
   tellpos=ftell(file);

   minval0=65535;
   maxval0=0;

   // scan for minimum and maximum value
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("scanning data",1.0f/3*(i*depth+j+1)/(depth*steps),obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            return(NULL);
            }

         shorts[1]=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         convert2minmax(shorts[1],cells,components,minval,maxval);
         free(shorts[1]);

         if (minval<minval0) minval0=minval;
         if (maxval>maxval0) maxval0=maxval;
         }

   // seek back to start
   if (fseek(file,tellpos,SEEK_SET)==-1) return(NULL);

   if (minval0==maxval0) maxval0=minval0+1;

   linear=(maxval0-minval0<256);

   err=new double[65536];

   // populate error table
   if (linear)
      for (i=0; i<65536; i++) err[i]=255*(double)(i-minval0)/(maxval0-minval0);
   else
      {
      for (i=0; i<65536; i++) err[i]=0.0;

      shorts[0]=shorts[1]=shorts[2]=NULL;

      for (i=0; i<steps; i++)
         for (j=0; j<depth; j++)
            {
            if (feedback!=NULL) feedback("analyzing data",1.0f/3*(i*depth+j+1)/(depth*steps)+1.0f/3,obj);

            if (j==0)
               {
               if (shorts[0]!=NULL) free(shorts[0]);
               if (shorts[1]!=NULL) free(shorts[1]);
               if (shorts[2]!=NULL) free(shorts[2]);

               shorts[0]=shorts[1]=shorts[2]=NULL;

               if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

               if (fread(slice,bytes,1,file)!=1)
                  {
                  delete err;
                  free(slice);
                  return(NULL);
                  }

               shorts[2]=convert2short(slice,cells,components,bits,sign,msb);
               free(slice);
               }

            if (shorts[0]!=NULL) free(shorts[0]);

            shorts[0]=shorts[1];
            shorts[1]=shorts[2];
            shorts[2]=NULL;

            if (j<depth-1)
               {
               if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

               if (fread(slice,bytes,1,file)!=1)
                  {
                  delete err;
                  free(slice);
                  if (shorts[0]!=NULL) free(shorts[0]);
                  if (shorts[1]!=NULL) free(shorts[1]);
                  if (shorts[2]!=NULL) free(shorts[2]);
                  return(NULL);
                  }

               shorts[2]=convert2short(slice,cells,components,bits,sign,msb);
               free(slice);
               }

            convert2error(shorts,width,height,components,err);
            }

      if (shorts[0]!=NULL) free(shorts[0]);
      if (shorts[1]!=NULL) free(shorts[1]);
      if (shorts[2]!=NULL) free(shorts[2]);

      integrate(err,minval0,maxval0);
      }

   // seek back to start
   if (fseek(file,tellpos,SEEK_SET)==-1)
      {
      delete err;
      return(NULL);
      }

   // make RAW info
   outname=appendRAWinfo(output,
                         width,height,depth,steps,
                         components,8,FALSE,TRUE,
                         scalex,scaley,scalez);

   if (outname==NULL)
      {
      delete err;
      return(NULL);
      }

   // open RAW output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      delete err;
      free(outname);
      return(NULL);
      }

   // process out-of-core slice by slice
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("writing data",1.0f/3*(i*depth+j+1)/(depth*steps)+2.0f/3,obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            delete err;
            free(slice);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         shorts[1]=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         chars=convert2char(shorts[1],cells,components,err);
         free(shorts[1]);

         if (fwrite(chars,cells*components,1,outfile)!=1)
            {
            delete err;
            free(chars);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         free(chars);
         }

   delete err;
   fclose(outfile);

   return(outname);
   }

// copy a RAW volume with out-of-core non-linear quantization
char *copyRAWvolume_nonlinear(const char *filename, // source file
                              const char *output, // destination file name /wo suffix .raw
                              void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   // check for quantization
   if (rawcomps==1 && rawbits==8 && !rawsign)
      outname=strdup(filename);
   else
      outname=copyRAWvolume_nonlinear(file,output,
                                      rawwidth,rawheight,rawdepth,rawsteps,
                                      rawcomps,rawbits,rawsign,rawmsb,
                                      rawscalex,rawscaley,rawscalez,
                                      feedback,obj);

   fclose(file);

   return(outname);
   }

// populate histogram from short array
void convert2histogram(unsigned short int *shorts,long long cells,unsigned int components,
                       double *histo)
   {
   long long i;

   cells*=components;

   for (i=0; i<cells; i++)
      histo[shorts[i]]++;
   }

// check whether or not a cell exceeds the threshold to be a boundary cell
inline BOOLINT isboundary(unsigned short int *shorts,unsigned int columns,unsigned int components,
                          unsigned int x,unsigned int y,
                          unsigned int thres)
   {
   unsigned int i;

   unsigned int idx;

   idx=(x+y*columns)*components;

   for (i=0; i<components; i++)
      if (shorts[idx+i]>=thres) return(TRUE);

   return(FALSE);
   }

// compute boundary of volume crop box
void convert2boundary(unsigned short int *shorts,unsigned int width,unsigned int height,unsigned int slice,unsigned int components,
                      unsigned int thres,
                      unsigned int &crop_x1,unsigned int &crop_x2,
                      unsigned int &crop_y1,unsigned int &crop_y2,
                      unsigned int &crop_z1,unsigned int &crop_z2)
   {
   unsigned int i,j;

   unsigned int count;

   // left side
   for (i=0; i<width; i++)
      {
      for (count=0,j=0; j<height; j++)
         if (isboundary(shorts,width,components,i,j,thres)) count++;

      if (count>0)
         {
         if (i<crop_x1) crop_x1=i;
         break;
         }
      }

   // right side
   for (i=0; i<width; i++)
      {
      for (count=0,j=0; j<height; j++)
         if (isboundary(shorts,width,components,width-1-i,j,thres)) count++;

      if (count>0)
         {
         if (width-1-i>crop_x2) crop_x2=width-1-i;
         break;
         }
      }

   // bottom side
   for (j=0; j<height; j++)
      {
      for (count=0,i=0; i<width; i++)
         if (isboundary(shorts,width,components,i,j,thres)) count++;

      if (count>0)
         {
         if (j<crop_y1) crop_y1=j;
         break;
         }
      }

   // top side
   for (j=0; j<height; j++)
      {
      for (count=0,i=0; i<width; i++)
         if (isboundary(shorts,width,components,i,height-1-j,thres)) count++;

      if (count>0)
         {
         if (height-1-j>crop_y2) crop_y2=height-1-j;
         break;
         }
      }

   // entire slice
   for (count=0,i=0; i<width; i++)
      for (j=0; j<height; j++)
         if (isboundary(shorts,width,components,i,j,thres)) count++;

   // front slice
   if (count==0)
      if (slice==crop_z1) crop_z1=slice+1;

   // back slice
   if (count!=0) crop_z2=slice;
   }

// copy a RAW volume with out-of-core cropping
char *cropRAWvolume(FILE *file, // source file desc
                    const char *output, // destination file name /wo .raw
                    long long width,long long height,long long depth,long long steps,
                    unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                    float scalex,float scaley,float scalez,
                    float ratio, // crop volume ratio
                    void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   long long i,j,k;

   unsigned char *slice;
   long long cells;
   long long bytes;

   unsigned short int *shorts;

   long long tellpos;

   double *histo;

   double tsum,wsum;

   unsigned int thres;

   unsigned int crop_x1,crop_y1;
   unsigned int crop_x2,crop_y2;
   unsigned int crop_z1,crop_z2;

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   cells=bytes=width*height;

   // compute total number of bytes per slice
   bytes*=components;
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // remember seek position
   tellpos=ftell(file);

   histo=new double[65536];

   // initialize histogram
   for (i=0; i<65536; i++) histo[i]=0.0;

   // populate histogram
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("scanning data",1.0f/3*(i*depth+j+1)/(depth*steps),obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            delete histo;
            free(slice);
            return(NULL);
            }

         shorts=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         convert2histogram(shorts,cells,components,histo);
         free(shorts);
         }

   // integrate histogram
   tsum=0.0;
   for (i=0; i<65536; i++)
      tsum+=histo[i]*i/65535.0;

   // compute threshold from integrated histogram
   wsum=0.0;
   for (thres=0; thres<65536; thres++)
      {
      wsum+=histo[thres]*thres/65535.0;
      if (wsum>=ratio*tsum) break;
      }

   delete histo;

   // seek back to start
   if (fseek(file,tellpos,SEEK_SET)==-1) return(NULL);

   crop_x1=width-1;
   crop_x2=0;

   crop_y1=height-1;
   crop_y2=0;

   crop_z1=0;
   crop_z2=depth-1;

   // compute crop boundary
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("analyzing data",1.0f/3*(i*depth+j+1)/(depth*steps)+1.0f/3,obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            return(NULL);
            }

         shorts=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         convert2boundary(shorts,width,height,j,components,thres,crop_x1,crop_x2,crop_y1,crop_y2,crop_z1,crop_z2);
         free(shorts);
         }

   // check bounding box
   if (crop_x1==crop_x2 || crop_y1==crop_y2)
      {
      crop_x1=0;
      crop_x2=width-1;

      crop_y1=0;
      crop_y2=height-1;
      }

   // seek back to start
   if (fseek(file,tellpos,SEEK_SET)==-1) return(NULL);

   // make RAW info
   outname=appendRAWinfo(output,
                         crop_x2-crop_x1+1,crop_y2-crop_y1+1,crop_z2-crop_z1+1,steps,
                         components,16,FALSE,!RAW_ISINTEL,
                         scalex,scaley,scalez);

   if (outname==NULL) return(NULL);

   // open RAW output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      free(outname);
      return(NULL);
      }

   // process out-of-core slice by slice
   for (i=0; i<steps; i++)
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("writing data",1.0f/3*(i*depth+j+1)/(depth*steps)+2.0f/3,obj);

         if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

         if (fread(slice,bytes,1,file)!=1)
            {
            free(slice);
            free(outname);
            fclose(outfile);
            return(NULL);
            }

         shorts=convert2short(slice,cells,components,bits,sign,msb);
         free(slice);

         if (j>=crop_z1 && j<=crop_z2)
            for (k=crop_y1; k<=crop_y2; k++)
               if (fwrite(&shorts[(crop_x1+k*width)*components],(crop_x2-crop_x1+1)*components*sizeof(unsigned short int),1,outfile)!=1)
                  {
                  free(shorts);
                  free(outname);
                  fclose(outfile);
                  return(NULL);
                  }

         free(shorts);
         }

   fclose(outfile);

   return(outname);
   }

// copy a RAW volume with out-of-core cropping
char *cropRAWvolume(const char *filename, // source file
                    const char *output, // destination file name /wo suffix .raw
                    float ratio, // crop volume ratio
                    void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // check for cropping
   if (ratio<=0.0f) return(strdup(filename));

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   outname=cropRAWvolume(file,output,
                         rawwidth,rawheight,rawdepth,rawsteps,
                         rawcomps,rawbits,rawsign,rawmsb,
                         rawscalex,rawscaley,rawscalez,
                         ratio,
                         feedback,obj);

   fclose(file);

   return(outname);
   }

// average and down-size two slices
unsigned short int *convert2down(unsigned short int *shorts[],unsigned int width,unsigned int height,unsigned int components)
   {
   unsigned int i,j,k;

   unsigned int i2,j2;
   unsigned int width2,height2;

   unsigned short int *down;

   width2=(width>>1);
   height2=(height>>1);

   if ((down=(unsigned short int *)malloc(width2*height2*components*sizeof(unsigned short int)))==NULL) ERRORMSG();

   for (i=0; i<width2; i++)
      for (j=0; j<height2; j++)
         for (k=0; k<components; k++)
            {
            i2=(i<<1);
            j2=(j<<1);

            down[(i+j*width2)*components+k]=((int)shorts[0][(i2+j2*width)*components+k]+
                                             (int)shorts[0][((i2+1)+j2*width)*components+k]+
                                             (int)shorts[0][(i2+(j2+1)*width)*components+k]+
                                             (int)shorts[0][((i2+1)+(j2+1)*width)*components+k]+
                                             (int)shorts[1][(i2+j2*width)*components+k]+
                                             (int)shorts[1][((i2+1)+j2*width)*components+k]+
                                             (int)shorts[1][(i2+(j2+1)*width)*components+k]+
                                             (int)shorts[1][((i2+1)+(j2+1)*width)*components+k]+4)/8;
            }

   return(down);
   }

// copy a RAW volume with out-of-core down-sizing
char *downsizeRAWvolume(FILE *file, // source file desc
                        const char *output, // destination file name /wo suffix .raw
                        long long width,long long height,long long depth,long long steps,
                        unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                        float scalex,float scaley,float scalez,
                        void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   long long i,j;

   unsigned char *slice;
   long long cells;
   long long bytes;

   unsigned short int *shorts[2];
   unsigned short int *down;
   unsigned char *down2;

   long long sizex,sizey,sizez;

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   cells=bytes=width*height;

   // compute total number of bytes per slice
   bytes*=components;
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   sizex=width>>1;
   sizey=height>>1;
   sizez=depth>>1;

   if (sizex<1 || sizey<1 || sizez<1) return(NULL);

   // make RAW info
   if (bits==8)
      outname=appendRAWinfo(output,
                            sizex,sizey,sizez,steps,
                            components,8,FALSE,!RAW_ISINTEL,
                            2*scalex,2*scaley,2*scalez);
   else
      outname=appendRAWinfo(output,
                            sizex,sizey,sizez,steps,
                            components,16,FALSE,!RAW_ISINTEL,
                            2*scalex,2*scaley,2*scalez);

   if (outname==NULL) return(NULL);

   // open RAW output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      free(outname);
      return(NULL);
      }

   // process out-of-core slice by slice
   for (i=0; i<steps; i++)
      {
      for (j=0; j<depth; j++)
         {
         if (feedback!=NULL) feedback("writing data",(float)(i*depth+j+1)/(depth*steps),obj);

         if (j%2==0)
            {
            if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

            if (fread(slice,bytes,1,file)!=1)
               {
               free(slice);
               free(outname);
               fclose(outfile);
               return(NULL);
               }

            shorts[0]=convert2short(slice,cells,components,bits,sign,msb);
            free(slice);
            }
         else
            {
            if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

            if (fread(slice,bytes,1,file)!=1)
               {
               free(slice);
               free(outname);
               fclose(outfile);
               return(NULL);
               }

            shorts[1]=convert2short(slice,cells,components,bits,sign,msb);
            free(slice);

            down=convert2down(shorts,width,height,components);
            free(shorts[0]);
            free(shorts[1]);

            if (bits==8)
               {
               down2=strip2byte(down,sizex*sizey,components);

               if (fwrite(down2,sizex*sizey*components*sizeof(unsigned char),1,outfile)!=1)
                  {
                  free(down);
                  free(down2);
                  free(outname);
                  fclose(outfile);
                  return(NULL);
                  }

               free(down2);
               }
            else
               if (fwrite(down,sizex*sizey*components*sizeof(unsigned short int),1,outfile)!=1)
                  {
                  free(down);
                  free(outname);
                  fclose(outfile);
                  return(NULL);
                  }

            free(down);
            }
         }

      if (j%2==1) free(shorts[0]);
      }

   fclose(outfile);

   return(outname);
   }

// copy a RAW volume with out-of-core down-sizing
char *downsizeRAWvolume(const char *filename, // source file
                        const char *output, // destination file name /wo suffix .raw
                        void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   outname=downsizeRAWvolume(file,output,
                             rawwidth,rawheight,rawdepth,rawsteps,
                             rawcomps,rawbits,rawsign,rawmsb,
                             rawscalex,rawscaley,rawscalez,
                             feedback,obj);

   fclose(file);

   return(outname);
   }

// process a RAW volume with out-of-core cropping and non-linear quantization
char *processRAWvolume(FILE *file, // source file desc
                       const char *output, // destination file name
                       long long width,long long height,long long depth,long long steps,
                       unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                       float scalex,float scaley,float scalez,
                       const char *infix, // optional infix for destination
                       float ratio, // crop volume ratio
                       long long maxcells, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj),void *obj) // feedback callback
   {
   char *outname;
   const char *preoutname;

   char *filename2,*filename3,*filename4,*filename5,*filename6,*filename7,*filename8,*filename9;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;

   long long cells;

   outname=NULL;

   // remove suffix
   filename2=removeRAWsuffix(output);

   // append optional infix
   if (infix!=NULL)
      {
      filename3=strdup2(filename2,infix);
      free(filename2);
      filename2=filename3;
      }

   // search for existing crop volume
   filename3=strdup2(filename2,"_crop*.raw");
   filesearch(filename3);
   free(filename3);
   preoutname=findfile();

   // check for pre-processed file
   if (preoutname!=NULL)
      {
      free(filename2);
      return(strdup(preoutname));
      }

   // append crop suffix to filename
   filename3=strdup2(filename2,"_crop");
   free(filename2);

   if (feedback!=NULL) feedback("cropping out-of-core",0,obj);

   // crop
   if (filename4=cropRAWvolume(file,filename3,
                               width,height,depth,steps,
                               components,bits,sign,msb,
                               scalex,scaley,scalez,
                               ratio,
                               feedback,obj))
      {
      // remove suffix
      filename5=removeRAWsuffix(filename4);

      // append quantize suffix to filename
      filename6=strdup2(filename5,"_quant");
      free(filename5);

      if (feedback!=NULL) feedback("quantizing out-of-core",0,obj);

      // quantize
      filename7=copyRAWvolume_nonlinear(filename4,filename6,feedback,obj);
      free(filename6);

      // remove temporary volume
      if (strcmp(filename4,filename7)!=0) removefile(filename4);
      free(filename4);

      if (!readRAWinfo(filename7,
                       &rawwidth,&rawheight,&rawdepth,&rawsteps,
                       &rawcomps,&rawbits,&rawsign,&rawmsb)) ERRORMSG();

      cells=rawwidth*rawheight*rawdepth*rawsteps*rawcomps;

      if (cells>maxcells && maxcells>0)
         while (cells>maxcells)
            {
            // remove suffix
            filename8=removeRAWsuffix(filename7);

            // append down-size suffix to filename
            filename9=strdup2(filename8,"_down");
            free(filename8);

            if (feedback!=NULL) feedback("down-sizing out-of-core",0,obj);

            // down-size
            outname=downsizeRAWvolume(filename7,filename9,feedback,obj);
            free(filename9);

            // remove temporary volume
            removefile(filename7);
            free(filename7);

            if (!readRAWinfo(outname,
                             &rawwidth,&rawheight,&rawdepth,&rawsteps,
                             &rawcomps,&rawbits,&rawsign,&rawmsb)) ERRORMSG();

            cells=rawwidth*rawheight*rawdepth*rawsteps*rawcomps;

            filename7=outname;
            }
      else outname=filename7;
      }

   free(filename3);

   return(outname);
   }

// process a RAW volume with out-of-core cropping and non-linear quantization
char *processRAWvolume(const char *filename, // source file
                       const char *infix, // optional infix for destination
                       float ratio, // crop volume ratio
                       long long maxcells, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj),void *obj) // feedback callback
   {
   char *outname;
   const char *preoutname;

   char *filename2,*filename3,*filename4,*filename5,*filename6,*filename7,*filename8,*filename9;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;

   long long cells;

   outname=NULL;

   // remove suffix
   filename2=removeRAWsuffix(filename);

   // append optional infix
   if (infix!=NULL)
      {
      filename3=strdup2(filename2,infix);
      free(filename2);
      filename2=filename3;
      }

   // search for existing crop volume
   filename3=strdup2(filename2,"_crop*.raw");
   filesearch(filename3);
   free(filename3);
   preoutname=findfile();

   // check for pre-processed file
   if (preoutname!=NULL)
      {
      free(filename2);
      return(strdup(preoutname));
      }

   // append crop suffix to filename
   filename3=strdup2(filename2,"_crop");
   free(filename2);

   if (feedback!=NULL) feedback("cropping out-of-core",0,obj);

   // crop
   if (filename4=cropRAWvolume(filename,filename3,ratio,feedback,obj))
      {
      // remove suffix
      filename5=removeRAWsuffix(filename4);

      // append quantize suffix to filename
      filename6=strdup2(filename5,"_quant");
      free(filename5);

      if (feedback!=NULL) feedback("quantizing out-of-core",0,obj);

      // quantize
      filename7=copyRAWvolume_nonlinear(filename4,filename6,feedback,obj);
      free(filename6);

      // remove temporary volume
      if (strcmp(filename4,filename)!=0 && strcmp(filename4,filename7)!=0) removefile(filename4);
      free(filename4);

      if (!readRAWinfo(filename7,
                       &rawwidth,&rawheight,&rawdepth,&rawsteps,
                       &rawcomps,&rawbits,&rawsign,&rawmsb)) ERRORMSG();

      cells=rawwidth*rawheight*rawdepth*rawsteps*rawcomps;

      if (cells>maxcells && maxcells>0)
         while (cells>maxcells)
            {
            // remove suffix
            filename8=removeRAWsuffix(filename7);

            // append down-size suffix to filename
            filename9=strdup2(filename8,"_down");
            free(filename8);

            if (feedback!=NULL) feedback("down-sizing out-of-core",0,obj);

            // down-size
            outname=downsizeRAWvolume(filename7,filename9,feedback,obj);
            free(filename9);

            // remove temporary volume
            if (strcmp(filename7,filename)!=0) removefile(filename7);
            free(filename7);

            if (!readRAWinfo(outname,
                             &rawwidth,&rawheight,&rawdepth,&rawsteps,
                             &rawcomps,&rawbits,&rawsign,&rawmsb)) ERRORMSG();

            cells=rawwidth*rawheight*rawdepth*rawsteps*rawcomps;

            filename7=outname;
            }
      else outname=filename7;
      }

   free(filename3);

   return(outname);
   }

// read a RAW volume out-of-core
unsigned char *readRAWvolume_ooc(const char *filename,
                                 long long *width,long long *height,long long *depth,unsigned int *components,
                                 float *scalex,float *scaley,float *scalez,
                                 float ratio, // crop volume ratio
                                 long long maxcells, // down-size threshold
                                 void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   char *outname;

   unsigned char *volume;
   long long steps;

   volume=NULL;

   outname=processRAWvolume(filename,NULL,ratio,maxcells,feedback,obj);

   if (outname!=NULL)
      {
      volume=readRAWvolume(outname,
                           width,height,depth,&steps,
                           components,NULL,NULL,NULL,
                           scalex,scaley,scalez);

      free(outname);
      }

   return(volume);
   }

// swap the hi and lo byte of 16 bit data
void swapRAWbytes(unsigned char *data,long long bytes)
   {
   long long i;
   unsigned char *ptr,tmp;

   for (ptr=data,i=0; i<bytes/2; i++,ptr+=2)
      {
      tmp=*ptr;
      *ptr=*(ptr+1);
      *(ptr+1)=tmp;
      }
   }

// convert from signed short to unsigned short
void convRAWbytes(unsigned char *data,long long bytes)
   {
   long long i;
   unsigned char *ptr;
   int v,vmin;

   for (vmin=32767,ptr=data,i=0; i<bytes/2; i++,ptr+=2)
      {
      v=256*(*ptr)+*(ptr+1);
      if (v>32767) v=v-65536;
      if (v<vmin) vmin=v;
      }

   for (ptr=data,i=0; i<bytes/2; i++,ptr+=2)
      {
      v=256*(*ptr)+*(ptr+1);
      if (v>32767) v=v-65536;

      *ptr=(v-vmin)/256;
      *(ptr+1)=(v-vmin)%256;
      }
   }

// convert from float to unsigned short
void convRAWfloat(unsigned char *data,long long bytes)
   {
   long long i;
   unsigned char *ptr;
   float v,vmax;

   for (vmax=1.0f,ptr=data,i=0; i<bytes/4; i++,ptr+=4)
      {
      if (RAW_ISINTEL) RAW_swap4((char *)ptr);

      v=fabs(*((float *)ptr));
      if (v>vmax) vmax=v;
      }

   for (ptr=data,i=0; i<bytes/4; i++,ptr+=4)
      {
      v=fabs(*((float *)ptr))/vmax;

      data[2*i]=ftrc(65535.0f*v+0.5f)/256;
      data[2*i+1]=ftrc(65535.0f*v+0.5f)%256;
      }
   }

// helper to get a short value from a volume
inline int getshort(unsigned short int *data,
                    long long width,long long height,long long /*depth*/,
                    long long i,long long j,long long k)
   {return(data[i+(j+k*height)*width]);}

// helper to get a short gradient value from a volume
inline double getgrad(unsigned short int *data,
                      long long width,long long height,long long depth,
                      long long i,long long j,long long k)
   {
   double gx,gy,gz;

   if (i>0)
      if (i<width-1) gx=(getshort(data,width,height,depth,i+1,j,k)-getshort(data,width,height,depth,i-1,j,k))/2.0;
      else gx=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i-1,j,k);
   else
      if (i<width-1) gx=getshort(data,width,height,depth,i+1,j,k)-getshort(data,width,height,depth,i,j,k);
      else gx=0.0;

   if (j>0)
      if (j<height-1) gy=(getshort(data,width,height,depth,i,j+1,k)-getshort(data,width,height,depth,i,j-1,k))/2.0;
      else gy=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i,j-1,k);
   else
      if (j<height-1) gy=getshort(data,width,height,depth,i,j+1,k)-getshort(data,width,height,depth,i,j,k);
      else gy=0.0;

   if (k>0)
      if (k<depth-1) gz=(getshort(data,width,height,depth,i,j,k+1)-getshort(data,width,height,depth,i,j,k-1))/2.0;
      else gz=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i,j,k-1);
   else
      if (k<depth-1) gz=getshort(data,width,height,depth,i,j,k+1)-getshort(data,width,height,depth,i,j,k);
      else gz=0.0;

   return(sqrt(gx*gx+gy*gy+gz*gz));
   }

// quantize 16 bit data to 8 bit using a non-linear mapping
unsigned char *quantizeRAW(unsigned char *data,
                           long long width,long long height,long long depth,
                           BOOLINT msb,
                           BOOLINT linear,BOOLINT nofree)
   {
   long long i,j,k;

   unsigned char *data2;
   unsigned short int *data3;
   long long idx;

   int v,vmin,vmax;

   double *err,eint;

   BOOLINT done;

   if ((data3=(unsigned short int*)malloc(width*height*depth*sizeof(unsigned short int)))==NULL) ERRORMSG();

   vmin=65535;
   vmax=0;

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            {
            idx=i+(j+k*height)*width;

            if (msb)
               v=256*data[2*idx]+data[2*idx+1];
            else
               v=data[2*idx]+256*data[2*idx+1];
            data3[idx]=v;

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
               err[getshort(data3,width,height,depth,i,j,k)]+=sqrt(getgrad(data3,width,height,depth,i,j,k));

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

   if ((data2=(unsigned char *)malloc(width*height*depth))==NULL) ERRORMSG();

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
            {
            idx=i+(j+k*height)*width;
            data2[idx]=(int)(err[data3[idx]]+0.5);
            }

   delete err;
   free(data3);

   return(data2);
   }

// define ISO file information
char *makeISOinfo(double isovalue)
   {
   static const int maxlen=100;

   char info[maxlen];

   if (isovalue>=0.0 && isovalue<=1.0)
      snprintf(info,maxlen,"_iso%03d",int(1000.0*isovalue+0.5));
   else return(NULL);

   snprintf(&info[strlen(info)],maxlen-strlen(info),".geo");

   return(strdup(info));
   }

// append ISO file format suffix
char *appendISOinfo(const char *filename,double isovalue)
   {
   char *filename2;
   char *info;
   char *filename3;
   char *dot;

   // define ISO info
   info=makeISOinfo(isovalue);
   if (info==NULL) return(NULL);

   // remove RAW suffix
   filename2=removeRAWsuffix(filename);

   // remove ISO suffix
   dot=strrchr(filename2,'.');
   if (dot!=NULL)
      if (strcasecmp(dot,".geo")==0) *dot='\0';

   // append RAW info to filename
   filename3=strdup2(filename2,info);
   free(filename2);
   free(info);

   return(filename3);
   }

// forward declaration
void extractiso(const double isoval,
                const miniv3d &p1,const double c1,const miniv3d &n1,
                const miniv3d &p2,const double c2,const miniv3d &n2,
                const miniv3d &p3,const double c3,const miniv3d &n3,
                const miniv3d &p4,const double c4,const miniv3d &n4,
                FILE *file);

// forward declaration
void extractiso1(const miniv3d &v1,const miniv3d &n1,const double d1,
                 const miniv3d &v2,const miniv3d &n2,const double d2,
                 const miniv3d &v3,const miniv3d &n3,const double d3,
                 const miniv3d &v4,const miniv3d &n4,const double d4,
                 FILE *file);

// forward declaration
void extractiso2(const miniv3d &v1,const miniv3d &n1,const double d1,
                 const miniv3d &v2,const miniv3d &n2,const double d2,
                 const miniv3d &v3,const miniv3d &n3,const double d3,
                 const miniv3d &v4,const miniv3d &n4,const double d4,
                 FILE *file);

// calculate position from index
inline miniv3d getpos(unsigned int i,unsigned int j,unsigned int k,
               unsigned int width,unsigned int height,unsigned int depth,
               double scalex,double scaley,double scalez)
   {
   return(miniv3d((i-0.5*(width-1))*scalex,
                  (j-0.5*(height-1))*scaley,
                  (k-0.5*(depth-1))*scalez));
   }

// extract iso surface from one voxel
// p1 = p(i,j,k)
// p2 = p(i+s,j,k)
// p3 = p(i,j+s,k)
// p4 = p(i+s,j+s,k)
// p5 = p(i,j,k+s)
// p6 = p(i+s,j,k+s)
// p7 = p(i,j+s,k+s)
// p8 = p(i+s,j+s,k+s)
void extractvoxel(const double isoval,
                  const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int s,
                  const miniv3d &p1,const miniv3d &p2,const miniv3d &p3,const miniv3d &p4,const miniv3d &p5,const miniv3d &p6,const miniv3d &p7,const miniv3d &p8,
                  const double vc1,const double vc2,const double vc3,const double vc4,const double vc5,const double vc6,const double vc7,const double vc8,
                  const miniv3d &nc1,const miniv3d &nc2,const miniv3d &nc3,const miniv3d &nc4,const miniv3d &nc5,const miniv3d &nc6,const miniv3d &nc7,const miniv3d &nc8,
                  FILE *file)
   {
   if ((((i+j+k)/s)%2)==0)
      {
      extractiso(isoval,
                 p1,vc1,nc1,
                 p4,vc4,nc4,
                 p6,vc6,nc6,
                 p7,vc7,nc7,
                 file);

      extractiso(isoval,
                 p2,vc2,nc2,
                 p1,vc1,nc1,
                 p4,vc4,nc4,
                 p6,vc6,nc6,
                 file);

      extractiso(isoval,
                 p3,vc3,nc3,
                 p1,vc1,nc1,
                 p7,vc7,nc7,
                 p4,vc4,nc4,
                 file);

      extractiso(isoval,
                 p5,vc5,nc5,
                 p1,vc1,nc1,
                 p6,vc6,nc6,
                 p7,vc7,nc7,
                 file);

      extractiso(isoval,
                 p8,vc8,nc8,
                 p4,vc4,nc4,
                 p7,vc7,nc7,
                 p6,vc6,nc6,
                 file);
      }
   else
      {
      extractiso(isoval,
                 p2,vc2,nc2,
                 p3,vc3,nc3,
                 p8,vc8,nc8,
                 p5,vc5,nc5,
                 file);

      extractiso(isoval,
                 p1,vc1,nc1,
                 p2,vc2,nc2,
                 p5,vc5,nc5,
                 p3,vc3,nc3,
                 file);

      extractiso(isoval,
                 p4,vc4,nc4,
                 p2,vc2,nc2,
                 p3,vc3,nc3,
                 p8,vc8,nc8,
                 file);

      extractiso(isoval,
                 p6,vc6,nc6,
                 p2,vc2,nc2,
                 p8,vc8,nc8,
                 p5,vc5,nc5,
                 file);

      extractiso(isoval,
                 p7,vc7,nc7,
                 p3,vc3,nc3,
                 p5,vc5,nc5,
                 p8,vc8,nc8,
                 file);
      }
   }

// marching tetrahedra
void extractiso(const double isoval,
                const miniv3d &p1,const double c1,const miniv3d &n1,
                const miniv3d &p2,const double c2,const miniv3d &n2,
                const miniv3d &p3,const double c3,const miniv3d &n3,
                const miniv3d &p4,const double c4,const miniv3d &n4,
                FILE *file)
   {
   int flag=0;

   double d1,d2,d3,d4;

   d1=c1-isoval;
   d2=c2-isoval;
   d3=c3-isoval;
   d4=c4-isoval;

   if (d1<0.0) flag|=1;
   if (d2<0.0) flag|=2;
   if (d3<0.0) flag|=4;
   if (d4<0.0) flag|=8;

   switch (flag)
      {
      // 1st case: isoval<c for one and isoval>=c for other three vertices
      // 2nd case: isoval>=c for one and isoval<c for other three vertices
      case 1:
         extractiso1(p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p3,n3,dabs(d3),
                     p4,n4,dabs(d4),file); break;
      case 2+4+8:
         extractiso1(p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p4,n4,dabs(d4),
                     p3,n3,dabs(d3),file); break;
      case 2:
         extractiso1(p2,n2,dabs(d2),
                     p1,n1,dabs(d1),
                     p4,n4,dabs(d4),
                     p3,n3,dabs(d3),file); break;
      case 1+4+8:
         extractiso1(p2,n2,dabs(d2),
                     p1,n1,dabs(d1),
                     p3,n3,dabs(d3),
                     p4,n4,dabs(d4),file); break;
      case 4:
         extractiso1(p3,n3,dabs(d3),
                     p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p4,n4,dabs(d4),file); break;
      case 1+2+8:
         extractiso1(p3,n3,dabs(d3),
                     p1,n1,dabs(d1),
                     p4,n4,dabs(d4),
                     p2,n2,dabs(d2),file); break;
      case 8:
         extractiso1(p4,n4,dabs(d4),
                     p1,n1,dabs(d1),
                     p3,n3,dabs(d3),
                     p2,n2,dabs(d2),file); break;
      case 1+2+4:
         extractiso1(p4,n4,dabs(d4),
                     p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p3,n3,dabs(d3),file); break;

      // 1st case: isoval<c for two and isoval>=c for other two vertices
      // 2nd case: isoval>=c for two and isoval<c for other two vertices
      case 1+2:
         extractiso2(p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p3,n3,dabs(d3),
                     p4,n4,dabs(d4),file); break;
      case 4+8:
         extractiso2(p1,n1,dabs(d1),
                     p2,n2,dabs(d2),
                     p4,n4,dabs(d4),
                     p3,n3,dabs(d3),file); break;
      case 1+4:
         extractiso2(p1,n1,dabs(d1),
                     p3,n3,dabs(d3),
                     p4,n4,dabs(d4),
                     p2,n2,dabs(d2),file); break;
      case 2+8:
         extractiso2(p1,n1,dabs(d1),
                     p3,n3,dabs(d3),
                     p2,n2,dabs(d2),
                     p4,n4,dabs(d4),file); break;
      case 2+4:
         extractiso2(p2,n2,dabs(d2),
                     p3,n3,dabs(d3),
                     p1,n1,dabs(d1),
                     p4,n4,dabs(d4),file); break;
      case 1+8:
         extractiso2(p2,n2,dabs(d2),
                     p3,n3,dabs(d3),
                     p4,n4,dabs(d4),
                     p1,n1,dabs(d1),file); break;
      }
   }

// marching tetrahedra subcase #1
void extractiso1(const miniv3d &v1,const miniv3d &n1,const double d1,
                 const miniv3d &v2,const miniv3d &n2,const double d2,
                 const miniv3d &v3,const miniv3d &n3,const double d3,
                 const miniv3d &v4,const miniv3d &n4,const double d4,
                 FILE *file)
   {
   double f1,f2,f3;

   miniv3d p1,p2,p3;
   miniv3d pn1,pn2,pn3;

   f1=1.0/(d1+d2);
   f2=1.0/(d1+d3);
   f3=1.0/(d1+d4);

   p1=f1*(d2*v1+d1*v2);
   pn1=f1*(d2*n1+d1*n2);

   p2=f2*(d3*v1+d1*v3);
   pn2=f2*(d3*n1+d1*n3);

   p3=f3*(d4*v1+d1*v4);
   pn3=f3*(d4*n1+d1*n4);

   fprintf(file,"%d\n",3);
   fprintf(file,"%g %g %g %g %g %g\n",p1.x,p1.y,p1.z,pn1.x,pn1.y,pn1.z);
   fprintf(file,"%g %g %g %g %g %g\n",p2.x,p2.y,p2.z,pn2.x,pn2.y,pn2.z);
   fprintf(file,"%g %g %g %g %g %g\n",p3.x,p3.y,p3.z,pn3.x,pn3.y,pn3.z);
   }

// marching tetrahedra subcase #2
void extractiso2(const miniv3d &v1,const miniv3d &n1,const double d1,
                 const miniv3d &v2,const miniv3d &n2,const double d2,
                 const miniv3d &v3,const miniv3d &n3,const double d3,
                 const miniv3d &v4,const miniv3d &n4,const double d4,
                 FILE *file)
   {
   double f1,f2,f3,f4;

   miniv3d p1,p2,p3,p4;
   miniv3d pn1,pn2,pn3,pn4;

   f1=1.0/(d1+d3);
   f2=1.0/(d1+d4);
   f3=1.0/(d2+d3);
   f4=1.0/(d2+d4);

   p1=f1*(d3*v1+d1*v3);
   pn1=f1*(d3*n1+d1*n3);

   p2=f2*(d4*v1+d1*v4);
   pn2=f2*(d4*n1+d1*n4);

   p3=f3*(d3*v2+d2*v3);
   pn3=f3*(d3*n2+d2*n3);

   p4=f4*(d4*v2+d2*v4);
   pn4=f4*(d4*n2+d2*n4);

   fprintf(file,"%d\n",4);
   fprintf(file,"%g %g %g %g %g %g\n",p3.x,p3.y,p3.z,pn3.x,pn3.y,pn3.z);
   fprintf(file,"%g %g %g %g %g %g\n",p1.x,p1.y,p1.z,pn1.x,pn1.y,pn1.z);
   fprintf(file,"%g %g %g %g %g %g\n",p4.x,p4.y,p4.z,pn4.x,pn4.y,pn4.z);
   fprintf(file,"%g %g %g %g %g %g\n",p2.x,p2.y,p2.z,pn2.x,pn2.y,pn2.z);
   }

// extract iso surface
void convert2iso(unsigned short int *shorts[],unsigned int width,unsigned int height,unsigned int depth,unsigned int components,
                 unsigned int bits,unsigned int slab,
                 double scalex,double scaley,double scalez,
                 double isovalue,FILE *file)
   {
   int k;

   unsigned int i,j;

   double scalex1,scaley1,scalez1;

   scalex1=1.0/scalex;
   scaley1=1.0/scaley;
   scalez1=1.0/scalez;

   // loop over voxels of the actual slab
   for (i=0; i<width-1; i++)
      for (j=0; j<height-1; j++)
         {
         double sv[8];
         miniv3d gv[8];
         miniv3d p[8];

         double svmin,svmax;
         double gm;

         // get scalar values of actual voxel
         sv[0]=getshort(shorts,width,height,components,i,j,0);
         sv[1]=getshort(shorts,width,height,components,i+1,j,0);
         sv[2]=getshort(shorts,width,height,components,i,j+1,0);
         sv[3]=getshort(shorts,width,height,components,i+1,j+1,0);
         sv[4]=getshort(shorts,width,height,components,i,j,1);
         sv[5]=getshort(shorts,width,height,components,i+1,j,1);
         sv[6]=getshort(shorts,width,height,components,i,j+1,1);
         sv[7]=getshort(shorts,width,height,components,i+1,j+1,1);

         // normalize scalar values
         for (k=0; k<8; k++)
            if (bits==8) sv[k]/=255.0;
            else sv[k]/=65535.0;

         // calculate scalar value range
         svmin=svmax=sv[0];
         for (k=1; k<8; k++)
            {
            if (sv[k]<svmin) svmin=sv[k];
            if (sv[k]>svmax) svmax=sv[k];
            }

         // check for inclusion of iso surface
         if (isovalue>=svmin && isovalue<=svmax)
            {
            // found a voxel that contains an iso surface patch
            // now extract the corresponding voxel

            // get gradient vectors of actual voxel
#ifndef SOBEL
            gv[0]=getgradvec(shorts,width,height,components,i,j,0);
            gv[1]=getgradvec(shorts,width,height,components,i+1,j,0);
            gv[2]=getgradvec(shorts,width,height,components,i,j+1,0);
            gv[3]=getgradvec(shorts,width,height,components,i+1,j+1,0);
            gv[4]=getgradvec(shorts,width,height,components,i,j,1);
            gv[5]=getgradvec(shorts,width,height,components,i+1,j,1);
            gv[6]=getgradvec(shorts,width,height,components,i,j+1,1);
            gv[7]=getgradvec(shorts,width,height,components,i+1,j+1,1);
#else
            gv[0]=getsobelvec(shorts,width,height,components,i,j,0);
            gv[1]=getsobelvec(shorts,width,height,components,i+1,j,0);
            gv[2]=getsobelvec(shorts,width,height,components,i,j+1,0);
            gv[3]=getsobelvec(shorts,width,height,components,i+1,j+1,0);
            gv[4]=getsobelvec(shorts,width,height,components,i,j,1);
            gv[5]=getsobelvec(shorts,width,height,components,i+1,j,1);
            gv[6]=getsobelvec(shorts,width,height,components,i,j+1,1);
            gv[7]=getsobelvec(shorts,width,height,components,i+1,j+1,1);
#endif

            // normalize gradient vectors
            for (k=0; k<8; k++)
               {
               gv[k].x*=scalex1;
               gv[k].y*=scaley1;
               gv[k].z*=scalez1;

               gm=gv[k].getlength();
               if (gm>0.0) gv[k]/=gm;
               }

            // get corner positions of actual voxel
            p[0]=getpos(i,j,slab,width,height,depth,scalex,scaley,scalez);
            p[1]=getpos(i+1,j,slab,width,height,depth,scalex,scaley,scalez);
            p[2]=getpos(i,j+1,slab,width,height,depth,scalex,scaley,scalez);
            p[3]=getpos(i+1,j+1,slab,width,height,depth,scalex,scaley,scalez);
            p[4]=getpos(i,j,slab+1,width,height,depth,scalex,scaley,scalez);
            p[5]=getpos(i+1,j,slab+1,width,height,depth,scalex,scaley,scalez);
            p[6]=getpos(i,j+1,slab+1,width,height,depth,scalex,scaley,scalez);
            p[7]=getpos(i+1,j+1,slab+1,width,height,depth,scalex,scaley,scalez);

            // iso surface extraction
            extractvoxel(isovalue,
                         i,j,slab,1,
                         p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
                         sv[0],sv[1],sv[2],sv[3],sv[4],sv[5],sv[6],sv[7],
                         gv[0],gv[1],gv[2],gv[3],gv[4],gv[5],gv[6],gv[7],
                         file);
            }
         }
   }

// extract an iso-surface from a RAW volume out-of-core
char *extractRAWvolume(FILE *file, // source file desc
                       const char *output, // destination file name
                       long long width,long long height,long long depth,long long steps,
                       unsigned int components,unsigned int bits,BOOLINT sign,BOOLINT msb,
                       float scalex,float scaley,float scalez,
                       double isovalue, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   long long i,j;

   unsigned char *slice;
   long long cells;
   long long bytes;

   unsigned short int *shorts[4];

   char *outname;
   FILE *outfile;

   // compute total number of cells per slice
   cells=bytes=width*height;

   // compute total number of bytes per slice
   bytes*=components;
   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // make ISO info
   outname=appendISOinfo(output,isovalue);
   if (outname==NULL) return(NULL);

   // open output file
   if ((outfile=fopen(outname,"wb"))==NULL)
      {
      free(outname);
      return(NULL);
      }

   // write GEO header
   fprintf(outfile,"geo 1.0\n");
   fprintf(outfile,"0 3 0\n"); // /wo color /w normal /wo texcoord

   shorts[0]=shorts[1]=shorts[2]=shorts[3]=NULL;

   // calculate gradients and extract iso-surface
   for (i=0; i<steps; i++)
      for (j=0; j<depth-1; j++)
         {
         if (feedback!=NULL) feedback("extracting iso-surface",(float)(i*(depth-1)+j+1)/((depth-1)*steps),obj);

         if (j==0)
            {
            if (shorts[0]!=NULL) free(shorts[0]);
            if (shorts[1]!=NULL) free(shorts[1]);
            if (shorts[2]!=NULL) free(shorts[2]);
            if (shorts[3]!=NULL) free(shorts[3]);

            shorts[0]=shorts[1]=shorts[2]=shorts[3]=NULL;

            if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

            if (fread(slice,bytes,1,file)!=1)
               {
               free(slice);
               fclose(outfile);
               return(NULL);
               }

            shorts[2]=convert2short(slice,cells,components,bits,sign,msb);
            free(slice);

            if (j<depth-1)
               {
               if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

               if (fread(slice,bytes,1,file)!=1)
                  {
                  free(slice);
                  if (shorts[0]!=NULL) free(shorts[0]);
                  if (shorts[1]!=NULL) free(shorts[1]);
                  if (shorts[2]!=NULL) free(shorts[2]);
                  if (shorts[3]!=NULL) free(shorts[3]);
                  fclose(outfile);
                  return(NULL);
                  }

               shorts[3]=convert2short(slice,cells,components,bits,sign,msb);
               free(slice);
               }
            }

         if (shorts[0]!=NULL) free(shorts[0]);

         shorts[0]=shorts[1];
         shorts[1]=shorts[2];
         shorts[2]=shorts[3];
         shorts[3]=NULL;

         if (j<depth-2)
            {
            if ((slice=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

            if (fread(slice,bytes,1,file)!=1)
               {
               free(slice);
               if (shorts[0]!=NULL) free(shorts[0]);
               if (shorts[1]!=NULL) free(shorts[1]);
               if (shorts[2]!=NULL) free(shorts[2]);
               if (shorts[3]!=NULL) free(shorts[3]);
               fclose(outfile);
               return(NULL);
               }

            shorts[3]=convert2short(slice,cells,components,bits,sign,msb);
            free(slice);
            }

         convert2iso(shorts,width,height,depth,components,
                     bits,j,scalex,scaley,scalez,
                     isovalue,outfile);
         }

   if (shorts[0]!=NULL) free(shorts[0]);
   if (shorts[1]!=NULL) free(shorts[1]);
   if (shorts[2]!=NULL) free(shorts[2]);
   if (shorts[3]!=NULL) free(shorts[3]);

   // close output file
   fclose(outfile);

   return(outname);
   }

// extract an iso-surface from a RAW volume out-of-core
char *extractRAWvolume(const char *filename, // source file
                       const char *output, // destination file name /wo suffix .raw
                       double isovalue, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj),void *obj) // feedback callback
   {
   FILE *file;

   char *name;

   long long rawwidth,rawheight,rawdepth,rawsteps;
   unsigned int rawcomps,rawbits;
   BOOLINT rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   char *outname;

   // open RAW file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze RAW info
   name=strdup(filename);
   if (!readRAWinfo(name,
                    &rawwidth,&rawheight,&rawdepth,&rawsteps,
                    &rawcomps,&rawbits,&rawsign,&rawmsb,
                    &rawscalex,&rawscaley,&rawscalez))
      {
      free(name);
      fclose(file);
      return(NULL);
      }
   free(name);

   outname=extractRAWvolume(file,output,
                            rawwidth,rawheight,rawdepth,rawsteps,
                            rawcomps,rawbits,rawsign,rawmsb,
                            rawscalex,rawscaley,rawscalez,
                            isovalue,
                            feedback,obj);

   fclose(file);

   return(outname);
   }
