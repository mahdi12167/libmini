// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "miniio.h"
#include "minidds.h"
#include "minicrs.h"

#include "pnmbase.h"

namespace pnmbase {

// PNMcomment implementation:

PNMcomment::PNMcomment()
   {
   maxc=MAX_COM;
   if ((com=(char *)malloc(maxc))==NULL) MEMERROR();
   reset();
   }

PNMcomment::~PNMcomment()
   {free(com);}

void PNMcomment::reset()
   {
   c=0;
   com[c]='\0';
   }

char PNMcomment::addchar(char ch)
   {
   if (ch==' ' && c>0)
      if (com[c-1]==' ') return(ch);

   if (c>=maxc-1)
      {
      maxc*=2;
      if ((com=(char *)realloc(com,maxc))==NULL) MEMERROR();
      }

   com[c++]=ch;
   com[c]='\0';

   return(ch);
   }

void PNMcomment::addstring(const char *str)
   {while (*str!='\0') addchar(*str++);}

void PNMcomment::addfloat(double v,int leading)
   {
   char str[MAX_STR];
   if (leading==0) snprintf(str,MAX_STR,"%g",v);
   else snprintf(str,MAX_STR,"%12f",v);
   addstring(str);
   }

void PNMcomment::addunits(int units)
   {
   if (units==0) addstring("radians");
   else if (units==1) addstring("feet");
   else if (units==2) addstring("meters");
   else if (units==3) addstring("decimeters");
   else if (units==4) addstring("arc-seconds");
   }

// convert unsigned short data to signed short data (the default)
void ushort2short(unsigned char *data,unsigned int bytes)
   {
   unsigned int i;

   unsigned short int maxval,*ptr;

   maxval=0;
   ptr=(unsigned short int *)data;
   for (i=0; i<bytes; i+=2,ptr++)
      if (*ptr>maxval) maxval=*ptr;

   if (maxval<32768) return;

   ptr=(unsigned short int *)data;
   for (i=0; i<bytes; i+=2)
      *ptr++>>=1;
   }

// write a PNM image
int writePNMimage(const char *pnmfilename,
                  unsigned char *image,
                  int width,int height,int components,
                  PNMcomment *comment)
   {
   FILE *file;

   char *com;

   if (width<1 || height<1) ERRORMSG();

   if ((file=fopen(pnmfilename,"wb"))==NULL) return(0);

   if (components==1 || components==2) fprintf(file,"P5");
   else if (components==3) fprintf(file,"P6");
   else if (components==4) fprintf(file,"P8");
   else ERRORMSG();

   if (comment!=NULL)
      {
      com=comment->str();
      fprintf(file,"\n#");
      while (*com!='\0')
         if (*com!='\n') fputc(*com++,file);
         else if (*++com!='\0') fprintf(file,"\n#");
      }

   fprintf(file,"\n%d %d\n",width,height);

   if (components==1 || components==3 || components==4) fprintf(file,"255\n");
   else fprintf(file,"32767\n");

   if (fwrite(image,width*height*components,1,file)!=1) IOERROR();
   fclose(file);

   return(1);
   }

// read a PNM image
unsigned char *readPNMimage(const unsigned char *pnmimage,
                            int *width,int *height,int *components,
                            int loaded,unsigned char **ptr,
                            PNMcomment *comment)
   {
   FILE *file;

   int pnmtype,maxval;
   unsigned char *image;

   int ch;
   PNMcomment com;

   int val;

   if (loaded==0)
      {
      if ((file=fopen((char *)pnmimage,"rb"))==NULL) return(NULL);

      if (fscanf(file,"P%1d\n",&pnmtype)!=1)
         {
         fclose(file);
         return(NULL);
         }

      while ((ch=fgetc(file))=='#')
         while (com.addchar(ch=fgetc(file))!='\n')
            if (ch==EOF) ERRORMSG();
      ungetc(ch,file);

      if (fscanf(file,"%d %d\n",width,height)!=2) ERRORMSG();
      if (fscanf(file,"%d",&maxval)!=1) ERRORMSG();
      if (fgetc(file)!='\n') ERRORMSG();

      if (*width<1 || *height<1) ERRORMSG();

      if (pnmtype==5 && maxval==255) *components=1;
      else if (pnmtype==5 && (maxval==32767 || maxval==65535)) *components=2;
      else if (pnmtype==6 && maxval==255) *components=3;
      else if (pnmtype==8 && maxval==255) *components=4;
      else ERRORMSG();

      if ((image=(unsigned char *)malloc((*width)*(*height)*(*components)))==NULL) MEMERROR();

      if (fread(image,(*width)*(*height)*(*components),1,file)!=1) IOERROR();
      fclose(file);
      }
   else
      {
      image=(unsigned char *)pnmimage;

      ch=*image++;

      if (ch!='P') return(NULL);

      ch=*image++;

      pnmtype=ch-'0';

      ch=*image++;

      if (ch!='\n') return(NULL);

      ch=*image++;

      while (ch=='#')
         {
         ch=*image++;
         while (com.addchar(ch)!='\n') ch=*image++;
         ch=*image++;
         }

      val=0;

      while (ch>='0' && ch<='9')
         {
         val=10*val+ch-'0';
         ch=*image++;
         }

      *width=val;

      if (ch!=' ') ERRORMSG();

      ch=*image++;

      val=0;

      while (ch>='0' && ch<='9')
         {
         val=10*val+ch-'0';
         ch=*image++;
         }

      *height=val;

      if (ch!='\n') ERRORMSG();

      ch=*image++;

      maxval=0;

      while (ch>='0' && ch<='9')
         {
         maxval=10*maxval+ch-'0';
         ch=*image++;
         }

      if (ch!='\n') ERRORMSG();

      if (pnmtype==5 && maxval==255) *components=1;
      else if (pnmtype==5 && (maxval==32767 || maxval==65535)) *components=2;
      else if (pnmtype==6 && maxval==255) *components=3;
      else if (pnmtype==8 && maxval==255) *components=4;
      else ERRORMSG();

      if (ptr!=NULL) *ptr=image+(*width)*(*height)*(*components);
      }

   if (maxval==65535)
      ushort2short(image,(*width)*(*height)*2);

   if (comment!=NULL)
      {
      comment->reset();
      comment->addstring(com.str());
      }

   return(image);
   }

// read a PNM file
unsigned char *readPNMfile(const char *pnmfilename,
                           int *width,int *height,int *components,
                           PNMcomment *comment)
   {
   return(readPNMimage((unsigned char *)pnmfilename,
                       width,height,components,
                       0,NULL,comment));
   }

// write a PVM volume
int writePVMvolume(const char *filename,unsigned char *volume,
                   unsigned int width,unsigned int height,unsigned int depth,unsigned int components,
                   float scalex,float scaley,float scalez,
                   PNMcomment *comment)
   {
   FILE *file;

   int version=1;

   char *com;

   if (width<1 || height<1 || depth<1 || components<1) ERRORMSG();

   if ((file=fopen(filename,"wb"))==NULL) return(0);

   if (scalex==1.0f && scaley==1.0f && scalez==1.0f) version=2;

   fprintf(file,"PVM%s",(version==1)?"":"2");

   if (comment!=NULL)
      {
      com=comment->str();
      fprintf(file,"\n#");
      while (*com!='\0')
         if (*com!='\n') fputc(*com++,file);
         else if (*++com!='\0') fprintf(file,"\n#");
      }

   fprintf(file,"\n%u %u %u\n",width,height,depth);
   if (version==2) fprintf(file,"%g %g %g\n",scalex,scaley,scalez);
   fprintf(file,"%u\n",components);

   if (fwrite(volume,width*height*depth*components,1,file)!=1) IOERROR();
   fclose(file);

   return(1);
   }

// read a PVM volume
unsigned char *readPVMvolume(const char *filename,
                             unsigned int *width,unsigned int *height,unsigned int *depth,unsigned int *components,
                             float *scalex,float *scaley,float *scalez,
                             PNMcomment *comment)
   {
   FILE *file;

   int version=1;

   int ch;
   PNMcomment com;

   int maxval;

   unsigned char *volume;

   float sx=1.0f,sy=1.0f,sz=1.0f;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   if (fgetc(file)!='P') version=0;
   if (fgetc(file)!='V') version=0;
   if (fgetc(file)!='M') version=0;
   if ((ch=fgetc(file))!='\n')
      if (ch>='1' && ch<='9')
         {
         version=ch-'0';
         if (fgetc(file)!='\n') version=0;
         }
      else version=0;

   if (version==0)
      {
      fclose(file);
      return(NULL);
      }

   while ((ch=fgetc(file))=='#')
      while (com.addchar(ch=fgetc(file))!='\n')
         if (ch==EOF) ERRORMSG();
   ungetc(ch,file);

   if (fscanf(file,"%u %u %u\n",width,height,depth)!=3) ERRORMSG();
   if (version>1)
      if (fscanf(file,"%g %g %g\n",&sx,&sy,&sy)!=3) ERRORMSG();
   if (fscanf(file,"%d",&maxval)!=1) ERRORMSG();
   if (fgetc(file)!='\n') ERRORMSG();

   if (*width<1 || *height<1 || *depth<1) ERRORMSG();

   if (maxval==1 || maxval==255) *components=1;
   else if (maxval==2 || maxval==32767 || maxval==65535) *components=2;
   else if (maxval==3) *components=3;
   else if (maxval==4) *components=4;
   else ERRORMSG();

   if ((volume=(unsigned char *)malloc((*width)*(*height)*(*depth)*(*components)))==NULL) MEMERROR();

   if (fread(volume,(*width)*(*height)*(*depth)*(*components),1,file)!=1) IOERROR();
   fclose(file);

   if (maxval==65535)
      ushort2short(volume,(*width)*(*height)*(*depth)*2);

   if (scalex!=NULL && scaley!=NULL && scalez!=NULL)
      {
      *scalex=sx;
      *scaley=sy;
      *scalez=sz;
      }

   if (comment!=NULL)
      {
      comment->reset();
      comment->addstring(com.str());
      }

   return(volume);
   }

// write a compressed DDS volume
void writeDDSvolume(const char *filename,unsigned char *volume,
                    unsigned int width,unsigned int height,unsigned int depth,unsigned int components,
                    float scalex,float scaley,float scalez,
                    unsigned char *description,
                    unsigned char *courtesy,
                    unsigned char *parameter,
                    unsigned char *comment)
   {
   static const int MAXSTR=256;

   char str[MAXSTR];

   unsigned char *data;

   unsigned int len1=1,len2=1,len3=1,len4=1;

   if (width<1 || height<1 || depth<1 || components<1) ERRORMSG();

   if (description==NULL && courtesy==NULL && parameter==NULL && comment==NULL)
      if (scalex==1.0f && scaley==1.0f && scalez==1.0f)
         snprintf(str,MAXSTR,"PVM\n%d %d %d\n%d\n",width,height,depth,components);
      else
         snprintf(str,MAXSTR,"PVM2\n%d %d %d\n%g %g %g\n%d\n",width,height,depth,scalex,scaley,scalez,components);
   else
      snprintf(str,MAXSTR,"PVM3\n%d %d %d\n%g %g %g\n%d\n",width,height,depth,scalex,scaley,scalez,components);

   if (description==NULL && courtesy==NULL && parameter==NULL && comment==NULL)
      {
      if ((data=(unsigned char *)malloc(strlen(str)+width*height*depth*components))==NULL) MEMERROR();

      memcpy(data,str,strlen(str));
      memcpy(data+strlen(str),volume,width*height*depth*components);

      writeDDSfile(filename,data,strlen(str)+width*height*depth*components,components,width);
      }
   else
      {
      if (description!=NULL) len1=strlen((char *)description)+1;
      if (courtesy!=NULL) len2=strlen((char *)courtesy)+1;
      if (parameter!=NULL) len3=strlen((char *)parameter)+1;
      if (comment!=NULL) len4=strlen((char *)comment)+1;

      if ((data=(unsigned char *)malloc(strlen(str)+width*height*depth*components+len1+len2+len3+len4))==NULL) MEMERROR();

      memcpy(data,str,strlen(str));
      memcpy(data+strlen(str),volume,width*height*depth*components);

      if (description==NULL) *(data+strlen(str)+width*height*depth*components)='\0';
      else memcpy(data+strlen(str)+width*height*depth*components,description,len1);

      if (courtesy==NULL) *(data+strlen(str)+width*height*depth*components+len1)='\0';
      else memcpy(data+strlen(str)+width*height*depth*components+len1,courtesy,len2);

      if (parameter==NULL) *(data+strlen(str)+width*height*depth*components+len1+len2)='\0';
      else memcpy(data+strlen(str)+width*height*depth*components+len1+len2,parameter,len3);

      if (comment==NULL) *(data+strlen(str)+width*height*depth*components+len1+len2+len3)='\0';
      else memcpy(data+strlen(str)+width*height*depth*components+len1+len2+len3,comment,len4);

      writeDDSfile(filename,data,strlen(str)+width*height*depth*components+len1+len2+len3+len4,components,width);
      }
   }

// read a compressed DDS volume
unsigned char *readDDSvolume(const char *filename,
                             unsigned int *width,unsigned int *height,unsigned int *depth,unsigned int *components,
                             float *scalex,float *scaley,float *scalez,
                             unsigned char **description,
                             unsigned char **courtesy,
                             unsigned char **parameter,
                             unsigned char **comment)
   {
   unsigned char *data,*ptr;
   unsigned int bytes,numc;

   int version=1;

   unsigned char *volume;

   float sx=1.0f,sy=1.0f,sz=1.0f;

   unsigned int len1=0,len2=0,len3=0,len4=0;

   if ((data=readDDSfile(filename,&bytes))==NULL)
      if ((data=readfile(filename,&bytes))==NULL) return(NULL);

   if (bytes<5) return(NULL);

   if ((data=(unsigned char *)realloc(data,bytes+1))==NULL) MEMERROR();
   data[bytes]='\0';

   if (strncmp((char *)data,"PVM\n",4)!=0)
      {
      if (strncmp((char *)data,"PVM2\n",5)==0) version=2;
      else if (strncmp((char *)data,"PVM3\n",5)==0) version=3;
      else return(NULL);

      ptr=&data[5];
      if (sscanf((char *)ptr,"%d %d %d\n%g %g %g\n",width,height,depth,&sx,&sy,&sz)!=6) ERRORMSG();
      if (*width<1 || *height<1 || *depth<1 || sx<=0.0f || sy<=0.0f || sz<=0.0f) ERRORMSG();
      ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
      }
   else
      {
      ptr=&data[4];
      while (*ptr=='#')
         while (*ptr++!='\n') ;

      if (sscanf((char *)ptr,"%d %d %d\n",width,height,depth)!=3) ERRORMSG();
      if (*width<1 || *height<1 || *depth<1) ERRORMSG();
      }

   if (scalex!=NULL && scaley!=NULL && scalez!=NULL)
      {
      *scalex=sx;
      *scaley=sy;
      *scalez=sz;
      }

   ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
   if (sscanf((char *)ptr,"%d\n",&numc)!=1) ERRORMSG();
   if (numc<1) ERRORMSG();

   if (components!=NULL) *components=numc;
   else if (numc!=1) ERRORMSG();

   ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
   if (version==3) len1=strlen((char *)(ptr+(*width)*(*height)*(*depth)*numc))+1;
   if (version==3) len2=strlen((char *)(ptr+(*width)*(*height)*(*depth)*numc+len1))+1;
   if (version==3) len3=strlen((char *)(ptr+(*width)*(*height)*(*depth)*numc+len1+len2))+1;
   if (version==3) len4=strlen((char *)(ptr+(*width)*(*height)*(*depth)*numc+len1+len2+len3))+1;
   if ((volume=(unsigned char *)malloc((*width)*(*height)*(*depth)*numc+len1+len2+len3+len4))==NULL) MEMERROR();
   if (data+bytes!=ptr+(*width)*(*height)*(*depth)*numc+len1+len2+len3+len4) ERRORMSG();

   memcpy(volume,ptr,(*width)*(*height)*(*depth)*numc+len1+len2+len3+len4);
   free(data);

   if (description!=NULL)
      if (len1>1) *description=volume+(*width)*(*height)*(*depth)*numc;
      else *description=NULL;

   if (courtesy!=NULL)
      if (len2>1) *courtesy=volume+(*width)*(*height)*(*depth)*numc+len1;
      else *courtesy=NULL;

   if (parameter!=NULL)
      if (len3>1) *parameter=volume+(*width)*(*height)*(*depth)*numc+len1+len2;
      else *parameter=NULL;

   if (comment!=NULL)
      if (len4>1) *comment=volume+(*width)*(*height)*(*depth)*numc+len1+len2+len3;
      else *comment=NULL;

   return(volume);
   }

// put a geographic grid descriptor into a PNM comment
// default values correspond to USGS 1:250,000 DEM quads
void putPNMparams(PNMcomment *comment,
                  const char *pnm_description,
                  int coord_sys,
                  int coord_zone,
                  int coord_datum,
                  int coord_units,
                  double coord_SW_x,double coord_SW_y,
                  double coord_NW_x,double coord_NW_y,
                  double coord_NE_x,double coord_NE_y,
                  double coord_SE_x,double coord_SE_y,
                  float cell_size_x,float cell_size_y,
                  int scaling_units,
                  float vertical_scaling,
                  int missing_value)
   {
   if (coord_sys<0 || coord_sys>1) ERRORMSG();
   if (coord_zone<-60 || coord_zone>60) ERRORMSG();
   if (coord_units<0 || coord_units>4) ERRORMSG();
   if (cell_size_x<=0.0f || cell_size_y<=0.0f) ERRORMSG();
   if (scaling_units<1 || scaling_units>3) ERRORMSG();

   if (coord_sys==0 && coord_units==3)
      {
      coord_SW_x=LONSUB(coord_SW_x);
      coord_NW_x=LONSUB(coord_NW_x);
      coord_NE_x=LONSUB(coord_NE_x);
      coord_SE_x=LONSUB(coord_SE_x);

      if (coord_SW_y>=coord_NW_y || coord_SE_y>=coord_NE_y) ERRORMSG();
      }

   comment->reset();

   // magic descriptor
   comment->addstring(" BOX\n");

   // data description
   comment->addstring(" description=");
   if (pnm_description!=NULL) comment->addstring(pnm_description);
   comment->addchar('\n');

   // reference coordinate system
   comment->addstring(" coordinate system=");
   switch (coord_sys)
      {
      case 0: comment->addstring("LL\n"); break; // LL=Lat/Lon=Latitude/Longitude=geographic
      case 1: comment->addstring("UTM\n"); break; // UTM=Universal Transverse Mercator
      }
   comment->addstring(" coordinate zone=");
   comment->addfloat(coord_zone);
   comment->addchar('\n');
   comment->addstring(" coordinate datum=");
   comment->addfloat(coord_datum);
   comment->addchar('\n');

   // coordinates of corners
   comment->addstring(" SW corner=");
   comment->addfloat(coord_SW_x,1);
   comment->addchar('/');
   comment->addfloat(coord_SW_y,1);
   comment->addchar(' ');
   comment->addunits(coord_units);
   comment->addchar('\n');
   comment->addstring(" NW corner=");
   comment->addfloat(coord_NW_x,1);
   comment->addchar('/');
   comment->addfloat(coord_NW_y,1);
   comment->addchar(' ');
   comment->addunits(coord_units);
   comment->addchar('\n');
   comment->addstring(" NE corner=");
   comment->addfloat(coord_NE_x,1);
   comment->addchar('/');
   comment->addfloat(coord_NE_y,1);
   comment->addchar(' ');
   comment->addunits(coord_units);
   comment->addchar('\n');
   comment->addstring(" SE corner=");
   comment->addfloat(coord_SE_x,1);
   comment->addchar('/');
   comment->addfloat(coord_SE_y,1);
   comment->addchar(' ');
   comment->addunits(coord_units);
   comment->addchar('\n');

   // spatial resolution
   comment->addstring(" cell size=");
   comment->addfloat(cell_size_x);
   comment->addchar('/');
   comment->addfloat(cell_size_y);
   comment->addchar(' ');
   comment->addunits(coord_units);
   comment->addchar('\n');
   comment->addstring(" vertical scaling=");
   comment->addfloat(vertical_scaling);
   comment->addchar(' ');
   comment->addunits(scaling_units);
   comment->addchar('\n');

   // missing value
   comment->addstring(" missing value=");
   comment->addfloat(missing_value);
   comment->addchar('\n');
   }

// calculate the grid parameters in geographic coordinates
// returns 1 if PNM comment contains a valid descriptor, 0 otherwise
int getPNMparams(PNMcomment *comment,
                 double *coord,
                 float *cell_size,
                 float *vertical_scaling,
                 int *missing_value,
                 int *utm_zone,
                 int *utm_datum)
   {
   int line;
   char *ptr;

   PNMcomment com;
   char str[PNMcomment::MAX_STR];

   BOOLINT texflag=FALSE;
   int coord_sys=0,coord_zone=0,coord_datum=0;
   double coord_SW_x=0.0,coord_SW_y=0.0;
   double coord_NW_x=0.0,coord_NW_y=0.0;
   double coord_NE_x=0.0,coord_NE_y=0.0;
   double coord_SE_x=0.0,coord_SE_y=0.0;
   int coord_units=0;
   float cell_size_x=0.0f,cell_size_y=0.0f;
   float extent_x=0.0f,extent_y=0.0f;
   int scaling_units=0;

   double dxxSW,dxySW,dyxSW,dyySW;
   double dxxNW,dxyNW,dyxNW,dyyNW;
   double dxxNE,dxyNE,dyxNE,dyyNE;
   double dxxSE,dxySE,dyxSE,dyySE;
   double length;

   BOOLINT check;

   ptr=comment->str();

   check=TRUE;

   for (line=1; *ptr!='\0' && check; line++)
      {
      com.reset();

      while (*ptr==' ') ptr++;
      while (com.addchar(*ptr++)!='\n') ;

      switch (line)
         {
         case 1: // magic descriptor
            if (sscanf(com.str(),"%3s",
                       str)!=1) {check=FALSE; break;}

            check=FALSE;

            if (strcmp(str,"BOX")==0 || strcmp(str,"DEM")==0)
               {
               texflag=FALSE;
               check=TRUE;
               }

            if (strcmp(str,"TEX")==0)
               {
               texflag=TRUE;
               check=TRUE;
               }
            break;
         case 2: // data description
            if (sscanf(com.str(),"%11s=",
                       str)!=1) {check=FALSE; break;}

            if (strcmp(str,"description")!=0) check=FALSE;
            break;
         case 3: // coordinate system
            if (sscanf(com.str(),"coordinate system=%s",
                       str)!=1) {check=FALSE; break;}

            if (strcmp(str,"LL")==0) coord_sys=0;
            else if (strcmp(str,"UTM")==0) coord_sys=1;
            else check=FALSE;
            break;
         case 4: // coordinate zone
            if (sscanf(com.str(),"coordinate zone=%d",
                       &coord_zone)!=1) {check=FALSE; break;}

            if (coord_sys==0 && coord_zone!=0) check=FALSE;
            if (coord_sys==1 && coord_zone==0) check=FALSE;
            if (coord_zone<-60 || coord_zone>60) check=FALSE;
            break;
         case 5: // coordinate datum
            if (sscanf(com.str(),"coordinate datum=%d",
                       &coord_datum)!=1) {check=FALSE; break;}

            if (coord_sys==0 && coord_datum!=0) check=FALSE;
            break;
         case 6: // SW corner
            if (sscanf(com.str(),"SW corner=%lg/%lg %s",
                       &coord_SW_x,&coord_SW_y,str)!=3) {check=FALSE; break;}

            if (strcmp(str,"radians")==0) coord_units=0;
            else if (strcmp(str,"feet")==0) coord_units=1;
            else if (strcmp(str,"meters")==0) coord_units=2;
            else if (strcmp(str,"decimeters")==0) coord_units=3;
            else if (strcmp(str,"arc-seconds")==0) coord_units=4;
            else {check=FALSE; break;}

            if (coord_units==0)
               {
               coord_SW_x*=360*60*60/(2*PI);
               coord_SW_y*=360*60*60/(2*PI);
               coord_units=4;
               }

            if (coord_units==1)
               {
               coord_SW_x*=0.3048;
               coord_SW_y*=0.3048;
               coord_units=2;
               }

            if (coord_units==3)
               {
               coord_SW_x*=0.1;
               coord_SW_y*=0.1;
               coord_units=2;
               }

            if (coord_sys==0 && coord_units!=4) check=FALSE;
            if (coord_sys==1 && coord_units!=2) check=FALSE;
            break;
         case 7: // NW corner
            if (sscanf(com.str(),"NW corner=%lg/%lg %s",
                       &coord_NW_x,&coord_NW_y,str)!=3) {check=FALSE; break;}

            if (strcmp(str,"radians")==0) coord_units=0;
            else if (strcmp(str,"feet")==0) coord_units=1;
            else if (strcmp(str,"meters")==0) coord_units=2;
            else if (strcmp(str,"decimeters")==0) coord_units=3;
            else if (strcmp(str,"arc-seconds")==0) coord_units=4;
            else {check=FALSE; break;}

            if (coord_units==0)
               {
               coord_NW_x*=360*60*60/(2*PI);
               coord_NW_y*=360*60*60/(2*PI);
               coord_units=4;
               }

            if (coord_units==1)
               {
               coord_NW_x*=0.3048;
               coord_NW_y*=0.3048;
               coord_units=2;
               }

            if (coord_units==3)
               {
               coord_NW_x*=0.1;
               coord_NW_y*=0.1;
               coord_units=2;
               }

            if (coord_sys==0 && coord_units!=4) check=FALSE;
            if (coord_sys==1 && coord_units!=2) check=FALSE;
            break;
         case 8: // NE corner
            if (sscanf(com.str(),"NE corner=%lg/%lg %s",
                       &coord_NE_x,&coord_NE_y,str)!=3) {check=FALSE; break;}

            if (strcmp(str,"radians")==0) coord_units=0;
            else if (strcmp(str,"feet")==0) coord_units=1;
            else if (strcmp(str,"meters")==0) coord_units=2;
            else if (strcmp(str,"decimeters")==0) coord_units=3;
            else if (strcmp(str,"arc-seconds")==0) coord_units=4;
            else {check=FALSE; break;}

            if (coord_units==0)
               {
               coord_NE_x*=360*60*60/(2*PI);
               coord_NE_y*=360*60*60/(2*PI);
               coord_units=4;
               }

            if (coord_units==1)
               {
               coord_NE_x*=0.3048;
               coord_NE_y*=0.3048;
               coord_units=2;
               }

            if (coord_units==3)
               {
               coord_NE_x*=0.1;
               coord_NE_y*=0.1;
               coord_units=2;
               }

            if (coord_sys==0 && coord_units!=4) check=FALSE;
            if (coord_sys==1 && coord_units!=2) check=FALSE;
            break;
         case 9: // SE corner
            if (sscanf(com.str(),"SE corner=%lg/%lg %s",
                       &coord_SE_x,&coord_SE_y,str)!=3) {check=FALSE; break;}

            if (strcmp(str,"radians")==0) coord_units=0;
            else if (strcmp(str,"feet")==0) coord_units=1;
            else if (strcmp(str,"meters")==0) coord_units=2;
            else if (strcmp(str,"decimeters")==0) coord_units=3;
            else if (strcmp(str,"arc-seconds")==0) coord_units=4;
            else {check=FALSE; break;}

            if (coord_units==0)
               {
               coord_SE_x*=360*60*60/(2*PI);
               coord_SE_y*=360*60*60/(2*PI);
               coord_units=4;
               }

            if (coord_units==1)
               {
               coord_SE_x*=0.3048;
               coord_SE_y*=0.3048;
               coord_units=2;
               }

            if (coord_units==3)
               {
               coord_SE_x*=0.1;
               coord_SE_y*=0.1;
               coord_units=2;
               }

            if (coord_sys==0 && coord_units!=4) check=FALSE;
            if (coord_sys==1 && coord_units!=2) check=FALSE;
            break;
         case 10: // cell size
            if (sscanf(com.str(),"cell size=%g/%g %s",
                       &cell_size_x,&cell_size_y,str)!=3) {check=FALSE; break;}

            if (strcmp(str,"radians")==0) coord_units=0;
            else if (strcmp(str,"feet")==0) coord_units=1;
            else if (strcmp(str,"meters")==0) coord_units=2;
            else if (strcmp(str,"decimeters")==0) coord_units=3;
            else if (strcmp(str,"arc-seconds")==0) coord_units=4;
            else {check=FALSE; break;}

            if (coord_units==0)
               {
               cell_size_x*=360*60*60/(2*PI);
               cell_size_y*=360*60*60/(2*PI);
               coord_units=4;
               }

            if (coord_units==1)
               {
               cell_size_x*=0.3048f;
               cell_size_y*=0.3048f;
               coord_units=2;
               }

            if (coord_units==3)
               {
               cell_size_x*=0.1f;
               cell_size_y*=0.1f;
               coord_units=2;
               }

            if (coord_sys==0 && coord_units!=4) check=FALSE;
            if (coord_sys==1 && coord_units!=2) check=FALSE;
            break;
         case 11: // vertical scaling
            if (sscanf(com.str(),"vertical scaling=%g %s",
                       vertical_scaling,str)!=2) {check=FALSE; break;}

            if (strcmp(str,"feet")==0) scaling_units=1;
            else if (strcmp(str,"meters")==0) scaling_units=2;
            else if (strcmp(str,"decimeters")==0) scaling_units=3;
            else {check=FALSE; break;}

            if (scaling_units==1)
               {
               *vertical_scaling*=0.3048f;
               scaling_units=2;
               }

            if (scaling_units==3)
               {
               *vertical_scaling*=0.1f;
               scaling_units=2;
               }
            break;
         case 12: // missing value
            if (sscanf(com.str(),"missing value=%d",
                       missing_value)!=1) check=FALSE;
            break;
         }
      }

   if (line<13 || !check) return(0);

   // x coordinates extend from -180*60*60 arc-seconds (-180 degrees) to 180*60*60 arc-seconds (180 degrees)
   // y coordinates extend from -90*60*60 arc-seconds (-90 degrees) to 90*60*60 arc-seconds (90 degrees)
   if (coord_sys==0)
      {
      coord_SW_x=LONSUB(coord_SW_x);
      coord_NW_x=LONSUB(coord_NW_x);
      coord_NE_x=LONSUB(coord_NE_x);
      coord_SE_x=LONSUB(coord_SE_x);

      if (coord_SW_y>=coord_NW_y || coord_SE_y>=coord_NE_y) ERRORMSG();

      if (utm_zone!=NULL && utm_datum!=NULL)
         {
         *utm_zone=0;
         *utm_datum=0;
         }
      }
   else if (coord_sys==1)
      {
      if (coord_SW_x>=coord_SE_x || coord_NW_x>=coord_NE_x) ERRORMSG();
      if (coord_SW_y>=coord_NW_y || coord_SE_y>=coord_NE_y) ERRORMSG();

      if (utm_zone!=NULL && utm_datum!=NULL)
         {
         *utm_zone=coord_zone;
         *utm_datum=coord_datum;
         }
      else
         {
         // calculate extent:

         extent_x=sqrt((coord_SE_x-coord_SW_x)*(coord_SE_x-coord_SW_x)+
                       (coord_SE_y-coord_SW_y)*(coord_SE_y-coord_SW_y))+
                  sqrt((coord_NE_x-coord_NW_x)*(coord_NE_x-coord_NW_x)+
                       (coord_NE_y-coord_NW_y)*(coord_NE_y-coord_NW_y));

         extent_y=sqrt((coord_NW_x-coord_SW_x)*(coord_NW_x-coord_SW_x)+
                       (coord_NW_y-coord_SW_y)*(coord_NW_y-coord_SW_y))+
                  sqrt((coord_NE_x-coord_SE_x)*(coord_NE_x-coord_SE_x)+
                       (coord_NE_y-coord_SE_y)*(coord_NE_y-coord_SE_y));

         // transform corners
         minicrs::UTM2LL(coord_SW_x,coord_SW_y,coord_zone,coord_datum,&coord_SW_y,&coord_SW_x);
         minicrs::UTM2LL(coord_NW_x,coord_NW_y,coord_zone,coord_datum,&coord_NW_y,&coord_NW_x);
         minicrs::UTM2LL(coord_NE_x,coord_NE_y,coord_zone,coord_datum,&coord_NE_y,&coord_NE_x);
         minicrs::UTM2LL(coord_SE_x,coord_SE_y,coord_zone,coord_datum,&coord_SE_y,&coord_SE_x);

         // cell size changes approximately by the same factor as the extent changes:

         cell_size_x*=(sqrt(dsqr(LONADD(coord_SE_x,-coord_SW_x))+
                            (coord_SE_y-coord_SW_y)*(coord_SE_y-coord_SW_y))+
                       sqrt(dsqr(LONADD(coord_NE_x,-coord_NW_x))+
                            (coord_NE_y-coord_NW_y)*(coord_NE_y-coord_NW_y)))/extent_x;

         cell_size_y*=(sqrt(dsqr(LONSUB(coord_NW_x,coord_SW_x))+
                            (coord_NW_y-coord_SW_y)*(coord_NW_y-coord_SW_y))+
                       sqrt(dsqr(LONSUB(coord_NE_x,coord_SE_x))+
                            (coord_NE_y-coord_SE_y)*(coord_NE_y-coord_SE_y)))/extent_y;

         coord_zone=coord_datum=0;
         coord_units=4;
         coord_sys=0;
         }
      }

   // textures are usually provided using a cell centric data representation
   // so we need to shrink them by 0.5 texels on each side
   // to convert from a cell centric to a corner centric data representation
   if (texflag)
      if (coord_sys==1)
         {
         // SW corner:

         dxxSW=coord_SE_x-coord_SW_x;
         dxySW=coord_SE_y-coord_SW_y;

         if ((length=sqrt(dxxSW*dxxSW+dxySW*dxySW))==0.0) ERRORMSG();
         dxxSW/=length;
         dxySW/=length;

         dyxSW=coord_NW_x-coord_SW_x;
         dyySW=coord_NW_y-coord_SW_y;

         if ((length=sqrt(dyxSW*dyxSW+dyySW*dyySW))==0.0) ERRORMSG();
         dyxSW/=length;
         dyySW/=length;

         // NW corner:

         dxxNW=coord_NE_x-coord_NW_x;
         dxyNW=coord_NE_y-coord_NW_y;

         if ((length=sqrt(dxxNW*dxxNW+dxyNW*dxyNW))==0.0) ERRORMSG();
         dxxNW/=length;
         dxyNW/=length;

         dyxNW=coord_SW_x-coord_NW_x;
         dyyNW=coord_SW_y-coord_NW_y;

         if ((length=sqrt(dyxNW*dyxNW+dyyNW*dyyNW))==0.0) ERRORMSG();
         dyxNW/=length;
         dyyNW/=length;

         // NE corner:

         dxxNE=coord_NW_x-coord_NE_x;
         dxyNE=coord_NW_y-coord_NE_y;

         if ((length=sqrt(dxxNE*dxxNE+dxyNE*dxyNE))==0.0) ERRORMSG();
         dxxNE/=length;
         dxyNE/=length;

         dyxNE=coord_SE_x-coord_NE_x;
         dyyNE=coord_SE_y-coord_NE_y;

         if ((length=sqrt(dyxNE*dyxNE+dyyNE*dyyNE))==0.0) ERRORMSG();
         dyxNE/=length;
         dyyNE/=length;

         // SE corner:

         dxxSE=coord_SW_x-coord_SE_x;
         dxySE=coord_SW_y-coord_SE_y;

         if ((length=sqrt(dxxSE*dxxSE+dxySE*dxySE))==0.0) ERRORMSG();
         dxxSE/=length;
         dxySE/=length;

         dyxSE=coord_NE_x-coord_SE_x;
         dyySE=coord_NE_y-coord_SE_y;

         if ((length=sqrt(dyxSE*dyxSE+dyySE*dyySE))==0.0) ERRORMSG();
         dyxSE/=length;
         dyySE/=length;

         // shrink by 0.5 texel:

         coord_SW_x+=0.5*(cell_size_x*dxxSW+cell_size_y*dyxSW);
         coord_SW_y+=0.5*(cell_size_x*dxySW+cell_size_y*dyySW);

         coord_NW_x+=0.5*(cell_size_x*dxxNW+cell_size_y*dyxNW);
         coord_NW_y+=0.5*(cell_size_x*dxyNW+cell_size_y*dyyNW);

         coord_NE_x+=0.5*(cell_size_x*dxxNE+cell_size_y*dyxNE);
         coord_NE_y+=0.5*(cell_size_x*dxyNE+cell_size_y*dyyNE);

         coord_SE_x+=0.5*(cell_size_x*dxxSE+cell_size_y*dyxSE);
         coord_SE_y+=0.5*(cell_size_x*dxySE+cell_size_y*dyySE);
         }
      else
         {
         // SW corner:

         dxxSW=LONADD(coord_SE_x,-coord_SW_x);
         dxySW=coord_SE_y-coord_SW_y;

         if ((length=sqrt(dxxSW*dxxSW+dxySW*dxySW))==0.0) ERRORMSG();
         dxxSW/=length;
         dxySW/=length;

         dyxSW=LONSUB(coord_NW_x,coord_SW_x);
         dyySW=coord_NW_y-coord_SW_y;

         if ((length=sqrt(dyxSW*dyxSW+dyySW*dyySW))==0.0) ERRORMSG();
         dyxSW/=length;
         dyySW/=length;

         // NW corner:

         dxxNW=LONADD(coord_NE_x,-coord_NW_x);
         dxyNW=coord_NE_y-coord_NW_y;

         if ((length=sqrt(dxxNW*dxxNW+dxyNW*dxyNW))==0.0) ERRORMSG();
         dxxNW/=length;
         dxyNW/=length;

         dyxNW=LONSUB(coord_SW_x,coord_NW_x);
         dyyNW=coord_SW_y-coord_NW_y;

         if ((length=sqrt(dyxNW*dyxNW+dyyNW*dyyNW))==0.0) ERRORMSG();
         dyxNW/=length;
         dyyNW/=length;

         // NE corner:

         dxxNE=-LONADD(coord_NE_x,-coord_NW_x);
         dxyNE=coord_NW_y-coord_NE_y;

         if ((length=sqrt(dxxNE*dxxNE+dxyNE*dxyNE))==0.0) ERRORMSG();
         dxxNE/=length;
         dxyNE/=length;

         dyxNE=LONSUB(coord_SE_x,coord_NE_x);
         dyyNE=coord_SE_y-coord_NE_y;

         if ((length=sqrt(dyxNE*dyxNE+dyyNE*dyyNE))==0.0) ERRORMSG();
         dyxNE/=length;
         dyyNE/=length;

         // SE corner:

         dxxSE=-LONADD(coord_SE_x,-coord_SW_x);
         dxySE=coord_SW_y-coord_SE_y;

         if ((length=sqrt(dxxSE*dxxSE+dxySE*dxySE))==0.0) ERRORMSG();
         dxxSE/=length;
         dxySE/=length;

         dyxSE=LONSUB(coord_NE_x,coord_SE_x);
         dyySE=coord_NE_y-coord_SE_y;

         if ((length=sqrt(dyxSE*dyxSE+dyySE*dyySE))==0.0) ERRORMSG();
         dyxSE/=length;
         dyySE/=length;

         // shrink by 0.5 texel:

         coord_SW_x=LONSUB(coord_SW_x,-0.5*(cell_size_x*dxxSW+cell_size_y*dyxSW));
         coord_SW_y+=0.5*(cell_size_x*dxySW+cell_size_y*dyySW);

         coord_NW_x=LONSUB(coord_NW_x,-0.5*(cell_size_x*dxxNW+cell_size_y*dyxNW));
         coord_NW_y+=0.5*(cell_size_x*dxyNW+cell_size_y*dyyNW);

         coord_NE_x=LONSUB(coord_NE_x,-0.5*(cell_size_x*dxxNE+cell_size_y*dyxNE));
         coord_NE_y+=0.5*(cell_size_x*dxyNE+cell_size_y*dyyNE);

         coord_SE_x=LONSUB(coord_SE_x,-0.5*(cell_size_x*dxxSE+cell_size_y*dyxSE));
         coord_SE_y+=0.5*(cell_size_x*dxySE+cell_size_y*dyySE);
         }

   coord[0]=coord_SW_x;
   coord[1]=coord_SW_y;
   coord[2]=coord_NW_x;
   coord[3]=coord_NW_y;
   coord[4]=coord_NE_x;
   coord[5]=coord_NE_y;
   coord[6]=coord_SE_x;
   coord[7]=coord_SE_y;

   cell_size[0]=cell_size_x;
   cell_size[1]=cell_size_y;

   return(1);
   }

}
