// (c) by Stefan Roettger

#include "minibase.h"

#include "miniio.h"

#include "miniutm.h"

#include "miniOGL.h"
#include "minitext.h"

#include "lunascan.h"

#include "minipoint.h"

// default constructor
minipointopts::minipointopts()
   {
   type=0;

   signpostsize=0.0f;
   signpostheight=0.0f;
   signpostalpha=0.0f;

   brickfile=NULL;
   bricksize=0.0f;
   brickturn=0.0f;
   brickincline=0.0f;
   brickpasses=0;
   brickcolor_red=0.0f;
   brickcolor_green=0.0f;
   brickcolor_blue=0.0f;

   brickloaded=0;
   brickindex=-1;
   }

// destructor
minipointopts::~minipointopts()
   {if (brickfile!=NULL) free(brickfile);}

// default constructor
minipoint::minipoint(minitile *tile)
   {
   TILE=tile;

   POINTS=NULL;
   PNUM=0;

   VPOINTS=NULL;
   MAXVNUM=0;
   VNUM=0;

   CACHE=NULL;
   ALTPATH=NULL;

   TAKEN=TRANS=0;

   OFFSETLAT=OFFSETLON=0.0f;
   SCALEX=SCALEY=SCALEELEV=1.0f;

   LODS=NULL;

   BRICKNAME=NULL;

   CONFIGURE_SRCDATUM=3; // WGS84
   CONFIGURE_DSTZONE=0; // LatLon
   CONFIGURE_DSTDATUM=3; // WGS84
   CONFIGURE_AUTOMAP=0;

   CONFIGURE_SIGNPOSTALPHA=0.5f;

   CONFIGURE_BRICKCEILING=0.0f;
   CONFIGURE_BRICKPASSES=1;
   CONFIGURE_BRICKALPHA=0.5f;
   CONFIGURE_BRICKLODS=16;
   CONFIGURE_BRICKSTAGGER=1.25f;
   CONFIGURE_BRICKSTRIPES=0.0f;
   }

// destructor
minipoint::~minipoint()
   {
   int i,j;

   if (POINTS!=NULL)
      {
      for (i=0; i<COLS*ROWS; i++)
         {
         for (j=0; j<NUM[i]; j++)
            {
            if (POINTS[i][j].desc!=NULL) free(POINTS[i][j].desc);
            if (POINTS[i][j].meta!=NULL) free(POINTS[i][j].meta);
            if (POINTS[i][j].comment!=NULL) free(POINTS[i][j].comment);
            if (POINTS[i][j].system!=NULL) free(POINTS[i][j].system);
            if (POINTS[i][j].latitude!=NULL) free(POINTS[i][j].latitude);
            if (POINTS[i][j].longitude!=NULL) free(POINTS[i][j].longitude);
            if (POINTS[i][j].elevation!=NULL) free(POINTS[i][j].elevation);

            if (POINTS[i][j].opts!=NULL) delete POINTS[i][j].opts;
            }

         free(POINTS[i]);
         }

      free(POINTS);

      free(MAXNUM);
      free(NUM);
      }

   if (VPOINTS!=NULL) free(VPOINTS);

   if (ALTPATH!=NULL) free(ALTPATH);

   if (LODS!=NULL) delete LODS;

   if (BRICKNAME!=NULL) free(BRICKNAME);
   }

// get file
char *minipoint::getfile(char *filename,char *altpath)
   {
   if (checkfile(filename)!=0) return(strdup(filename));

   if (CACHE!=NULL) return(CACHE->getfile(filename,altpath));

   return(NULL);
   }

// add waypoint
void minipoint::add(minipointdata *point)
   {
   int i;

   float posx,posy;
   int col,row;

   if (TILE==NULL) ERRORMSG();

   if (POINTS==NULL)
      {
      COLS=TILE->getcols();
      ROWS=TILE->getrows();

      if ((POINTS=(minipointdata **)malloc(COLS*ROWS*sizeof(minipointdata *)))==NULL) ERRORMSG();

      if ((NUM=(int *)malloc(COLS*ROWS*sizeof(int)))==NULL) ERRORMSG();
      if ((MAXNUM=(int *)malloc(COLS*ROWS*sizeof(int)))==NULL) ERRORMSG();

      for (i=0; i<COLS*ROWS; i++)
         {
         POINTS[i]=NULL;
         MAXNUM[i]=NUM[i]=0;
         }
      }

   posx=(point->x-TILE->getcenterx())/TILE->getcoldim()+COLS/2.0f;
   posy=(-point->y-TILE->getcenterz())/TILE->getrowdim()+ROWS/2.0f;

   if (posx<0.0f || posx>COLS || posy<0.0f || posy>ROWS) return;

   col=ftrc(posx);
   row=ftrc(posy);

   if (col==COLS) col--;
   if (row==ROWS) row--;

   if (NUM[col+row*COLS]>=MAXNUM[col+row*COLS])
      {
      MAXNUM[col+row*COLS]=2*MAXNUM[col+row*COLS]+1;

      if (POINTS[col+row*COLS]==NULL)
         {if ((POINTS[col+row*COLS]=(minipointdata *)malloc(MAXNUM[col+row*COLS]*sizeof(minipointdata)))==NULL) ERRORMSG();}
      else
         {if ((POINTS[col+row*COLS]=(minipointdata *)realloc(POINTS[col+row*COLS],MAXNUM[col+row*COLS]*sizeof(minipointdata)))==NULL) ERRORMSG();}
      }

   point->number=PNUM++;

   if (strlen(point->comment)>0) parsecomment(point);

   POINTS[col+row*COLS][NUM[col+row*COLS]++]=*point;

   point->desc=point->meta=point->comment=NULL;
   point->system=point->latitude=point->longitude=point->elevation=NULL;

   point->opts=NULL;
   }

// add character to string
char *minipoint::addch(char *str,char ch)
   {
   const int space=4;

   if (str==NULL)
      {
      if ((str=(char *)malloc(space))==NULL) ERRORMSG();
      str[0]='\0';
      }
   else if (strlen(str)%space>=space-1)
      {
      char *str2;
      if ((str2=(char *)malloc(strlen(str)+1+space))==NULL) ERRORMSG();
      strcpy(str2,str);
      free(str);
      str=str2;
      }

   str[strlen(str)+1]='\0';
   str[strlen(str)]=ch;

   return(str);
   }

// parse comment
void minipoint::parsecomment(minipointdata *point)
   {
   lunascan scanner;

   scanner.addtoken("$",minipointopts::OPTION_DELIMITER);

   scanner.addtoken("=",minipointopts::OPTION_EQ);
   scanner.addtoken(":=",minipointopts::OPTION_EQ);

   scanner.addtoken("type",minipointopts::OPTION_TYPE);

   scanner.addtoken("signpostsize",minipointopts::OPTION_SIGNPOSTSIZE);
   scanner.addtoken("signpostheight",minipointopts::OPTION_SIGNPOSTHEIGHT);

   scanner.addtoken("brickfile",minipointopts::OPTION_BRICKFILE);
   scanner.addtoken("bricksize",minipointopts::OPTION_BRICKSIZE);
   scanner.addtoken("brickturn",minipointopts::OPTION_BRICKTURN);
   scanner.addtoken("brickincline",minipointopts::OPTION_BRICKINCLINE);
   scanner.addtoken("brickcolor_red",minipointopts::OPTION_BRICKCOLOR_RED);
   scanner.addtoken("brickcolor_green",minipointopts::OPTION_BRICKCOLOR_GREEN);
   scanner.addtoken("brickcolor_blue",minipointopts::OPTION_BRICKCOLOR_BLUE);
   scanner.addtoken("brickpasses",minipointopts::OPTION_BRICKPASSES);

   scanner.setcode(point->comment);

   // options are of the form $option=value
   while (scanner.gettoken()!=lunascan::LUNA_END)
      if (scanner.gettoken()==minipointopts::OPTION_DELIMITER)
         {
         scanner.next();
         parseoption(point,&scanner);
         }
      else scanner.next();
   }

// parse option
void minipoint::parseoption(minipointdata *point,lunascan *scanner)
   {
   int option;
   float value;
   char *name;

   option=scanner->gettoken();
   scanner->next();

   if (scanner->gettoken()==minipointopts::OPTION_EQ)
      {
      scanner->next();

      if (scanner->gettoken()==lunascan::LUNA_VALUE ||
          scanner->gettoken()==lunascan::LUNA_STRING)
         {
         value=scanner->getvalue();

         name=scanner->getstring();
         if (strcmp(name,"")==0) name=NULL;

         if (point->opts==NULL) point->opts=new minipointopts;

         switch (option)
            {
            case minipointopts::OPTION_TYPE: point->opts->type=ftrc(value+0.5f); break;
            case minipointopts::OPTION_SIGNPOSTSIZE: point->opts->signpostsize=value; break;
            case minipointopts::OPTION_SIGNPOSTHEIGHT: point->opts->signpostheight=value; break;
            case minipointopts::OPTION_SIGNPOSTALPHA: point->opts->signpostalpha=value; break;
            case minipointopts::OPTION_BRICKFILE: if (name!=NULL) point->opts->brickfile=strdup(name); break;
            case minipointopts::OPTION_BRICKSIZE: point->opts->bricksize=value; break;
            case minipointopts::OPTION_BRICKTURN: point->opts->brickturn=value; break;
            case minipointopts::OPTION_BRICKINCLINE: point->opts->brickincline=value; break;
            case minipointopts::OPTION_BRICKCOLOR_RED: point->opts->brickcolor_red=value; break;
            case minipointopts::OPTION_BRICKCOLOR_GREEN: point->opts->brickcolor_green=value; break;
            case minipointopts::OPTION_BRICKCOLOR_BLUE: point->opts->brickcolor_blue=value; break;
            case minipointopts::OPTION_BRICKPASSES: point->opts->brickpasses=ftrc(value+0.5f); break;
            }
         }

      scanner->next();
      }
   }

// set file cache
void minipoint::setcache(datacache *cache,char *altpath)
   {
   CACHE=cache;

   if (ALTPATH!=NULL) free(ALTPATH);
   ALTPATH=NULL;

   if (altpath!=NULL) ALTPATH=strdup(altpath);
   }

// load waypoints
void minipoint::load(char *filename,
                     float offsetlat,float offsetlon,
                     float scalex,float scaley,float scaleelev,
                     minitile *tile,
                     char delimiter)
   {
   unsigned int i;

   char *wpname;

   FILE *file;

   int ch;

   minipointdata point;

   int zone;

   if (tile!=NULL)
      if (TILE==NULL) TILE=tile;
      else if (tile!=TILE) ERRORMSG();

   if (TILE==NULL) ERRORMSG();

   if (TILE->getwarp()!=NULL)
      {
      if (TILE->getwarp()->getdat()==minicoord::MINICOORD_LLH)
         {
         CONFIGURE_DSTZONE=0;
         TAKEN=1;
         }
      else if (TILE->getwarp()->getdat()==minicoord::MINICOORD_UTM)
         {
         CONFIGURE_DSTZONE=TILE->getwarp()->getutmzone();
         CONFIGURE_DSTDATUM=TILE->getwarp()->getutmdatum();

         TAKEN=1;
         }
      }

   wpname=getfile(filename,ALTPATH);

   if (wpname==NULL) return;

   if ((file=fopen(wpname,"rb"))==NULL)
      {
      free(wpname);
      return;
      }

   free(wpname);

   ch=fgetc(file);

   while (ch=='\n' || ch=='\r') ch=fgetc(file);

   while (ch!=EOF)
      {
      point.x=point.y=0.0f;
      point.elev=point.height=0.0f;

      point.desc=point.meta=point.comment=NULL;
      point.system=point.latitude=point.longitude=point.elevation=NULL;

      point.zone=point.datum=0;

      point.opts=NULL;

      // read full description
      while (ch!='\n' && ch!='\r' && ch!=EOF)
         {
         point.meta=addch(point.meta,ch);
         ch=fgetc(file);
         }

      if (point.meta==NULL) ERRORMSG();

      // skip end of line
      while (ch=='\n' || ch=='\r') ch=fgetc(file);

      // copy description without comment
      for (i=0; i<strlen(point.meta); i++)
         {
         if (point.meta[i]==delimiter) break;
         point.desc=addch(point.desc,point.meta[i]);
         }

      if (point.desc==NULL) point.desc=addch(point.desc,'\0');

      // read lines starting with comment delimiter
      while (ch==delimiter)
         {
         ch=fgetc(file);

         while (ch!='\n' && ch!='\r' && ch!=EOF)
            {
            point.comment=addch(point.comment,ch);
            ch=fgetc(file);
            }

         while (ch=='\n' || ch=='\r')
            {
            if (ch=='\n') point.comment=addch(point.comment,ch);
            ch=fgetc(file);
            }
         }

      if (point.comment==NULL) point.comment=addch(point.comment,'\0');

      // read coordinate system
      while (ch!='\n' && ch!='\r' && ch!=EOF)
         {
         point.system=addch(point.system,ch);
         ch=fgetc(file);
         }

      // skip end of line
      while (ch=='\n' || ch=='\r') ch=fgetc(file);

      if (point.system==NULL) ERRORMSG();

      // read Lat/Lon coordinates
      if (strcmp(point.system,"LL")==0)
         {
         while (ch!='\n' && ch!='\r' && ch!=EOF)
            {
            point.latitude=addch(point.latitude,ch);
            ch=fgetc(file);
            }

         while (ch=='\n' || ch=='\r') ch=fgetc(file);

         if (point.latitude==NULL) ERRORMSG();

         while (ch!='\n' && ch!='\r' && ch!=EOF)
            {
            point.longitude=addch(point.longitude,ch);
            ch=fgetc(file);
            }

         while (ch=='\n' || ch=='\r') ch=fgetc(file);

         if (point.longitude==NULL) ERRORMSG();

         if (sscanf(point.latitude,"%g",&point.y)!=1) ERRORMSG();
         if (sscanf(point.longitude,"%g",&point.x)!=1) ERRORMSG();

         point.x*=60*60;
         point.y*=60*60;

         point.x=LONSUB(point.x);
         if (point.y<-90*60*60 || point.y>90*60*60) ERRORMSG();

         if (CONFIGURE_AUTOMAP!=0 && TAKEN==0)
            {
            CONFIGURE_DSTZONE=0;
            TAKEN=1;
            }
         }
      // read UTM coordinates
      else if (sscanf(point.system,"UTM %d",&zone)==1)
         {
         if (zone==0 || zone<-60 || zone>60) ERRORMSG();

         point.zone=zone;
         point.datum=CONFIGURE_SRCDATUM;

         while (ch!='\n' && ch!='\r' && ch!=EOF)
            {
            point.longitude=addch(point.longitude,ch);
            ch=fgetc(file);
            }

         while (ch=='\n' || ch=='\r') ch=fgetc(file);

         if (point.longitude==NULL) ERRORMSG();

         while (ch!='\n' && ch!='\r' && ch!=EOF)
            {
            point.latitude=addch(point.latitude,ch);
            ch=fgetc(file);
            }

         while (ch=='\n' || ch=='\r') ch=fgetc(file);

         if (point.latitude==NULL) ERRORMSG();

         if (sscanf(point.longitude,"%g",&point.x)!=1) ERRORMSG();
         if (sscanf(point.latitude,"%g",&point.y)!=1) ERRORMSG();

         miniutm::UTM2LL(point.x,point.y,zone,CONFIGURE_SRCDATUM,&point.y,&point.x);

         if (CONFIGURE_AUTOMAP!=0 && TAKEN==0)
            {
            CONFIGURE_DSTZONE=zone;
            CONFIGURE_DSTDATUM=CONFIGURE_SRCDATUM;

            TAKEN=1;
            }
         }
      else ERRORMSG();

      // read elevation
      while (ch!='\n' && ch!='\r' && ch!=EOF)
         {
         point.elevation=addch(point.elevation,ch);
         ch=fgetc(file);
         }

      // skip end of line
      while (ch=='\n' || ch=='\r') ch=fgetc(file);

      if (point.elevation==NULL) ERRORMSG();

      if (sscanf(point.elevation,"%g",&point.elev)!=1) ERRORMSG();

      if (CONFIGURE_DSTZONE==0)
         {
         point.x=LONSUB(point.x,-offsetlon);
         point.y+=offsetlat;

         if (point.y<-90*60*60 || point.y>90*60*60) ERRORMSG();
         }
      else
         {
         miniutm::LL2UTM(point.y,point.x,CONFIGURE_DSTZONE,CONFIGURE_DSTDATUM,&point.x,&point.y);

         point.x+=offsetlon;
         point.y+=offsetlat;
         }

      point.x*=scalex;
      point.y*=scaley;

      point.elev*=scaleelev;
      point.height=-MAXFLOAT;

      add(&point);

      if (point.desc!=NULL) free(point.desc);
      if (point.meta!=NULL) free(point.meta);
      if (point.comment!=NULL) free(point.comment);
      if (point.system!=NULL) free(point.system);
      if (point.latitude!=NULL) free(point.latitude);
      if (point.longitude!=NULL) free(point.longitude);
      if (point.elevation!=NULL) free(point.elevation);

      if (point.opts!=NULL) delete point.opts;
      }

   fclose(file);

   if (TRANS==0)
      {
      OFFSETLAT=offsetlat;
      OFFSETLON=offsetlon;

      SCALEX=scalex;
      SCALEY=scaley;
      SCALEELEV=scaleelev;

      TRANS=1;
      }
   }

// get waypoint
minipointdata *minipoint::getpoint(int p)
   {
   int i,j,k;

   if (TILE==NULL) return(NULL);

   for (i=0; i<COLS; i++)
      for (j=0; j<ROWS; j++)
         for (k=0; k<NUM[i+j*COLS]; k++)
            if (p==POINTS[i+j*COLS][k].number) return(&POINTS[i+j*COLS][k]);

   return(NULL);
   }

// calculate visible waypoints
void minipoint::calcvdata(int type1,int type2)
   {
   int i,j,k;

   minipointdata *point;

   int type;

   if (TILE==NULL) return;

   VNUM=0;

   for (i=TILE->getvisibleleft(); i<=TILE->getvisibleright(); i++)
      for (j=TILE->getvisiblebottom(); j<=TILE->getvisibletop(); j++)
         for (k=0; k<NUM[i+j*COLS]; k++)
            {
            point=&POINTS[i+j*COLS][k];

            if (point->opts==NULL) type=0;
            else type=point->opts->type;

            if (type!=type1 && type!=type2) continue;

            point->height=TILE->getheight(point->x,-point->y);

            if (VNUM>=MAXVNUM)
               {
               MAXVNUM=2*MAXVNUM+1;

               if (VPOINTS==NULL)
                  {if ((VPOINTS=(minipointdata **)malloc(MAXVNUM*sizeof(minipointdata *)))==NULL) ERRORMSG();}
               else
                  {if ((VPOINTS=(minipointdata **)realloc(VPOINTS,MAXVNUM*sizeof(minipointdata *)))==NULL) ERRORMSG();}
               }

            VPOINTS[VNUM]=point;

            VNUM++;
            }
   }

// sort visible waypoints along viewing axis
void minipoint::sortvdata(float x,float y,float elev,
                          float dx,float dy,float de)
   {
   if (VPOINTS!=NULL)
      shellsort(VPOINTS,VNUM,x,y,elev,dx,dy,de);
   }

// shellsort as proposed by Robert Sedgewick in "Algorithms"
void minipoint::shellsort(minipointdata **a,const int n,
                          float x,float y,float elev,
                          float dx,float dy,float de)
   {
   int i,j,h;

   minipointdata *v;

   for (h=1; h<=(n-1)/9; h=3*h+1);

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && compare(v,a[j-h],x,y,elev,dx,dy,de))
            {
            a[j]=a[j-h];
            j-=h;
            }
         a[j]=v;
         }
      h/=3;
      }
   }

// comparison operator for shellsort
inline int minipoint::compare(minipointdata *a,minipointdata *b,
                              float x,float y,float elev,
                              float dx,float dy,float de)
   {
   return(dx*(a->x-x)+dy*(a->y-y)+de*(a->height-elev)>
          dx*(b->x-x)+dy*(b->y-y)+de*(b->height-elev));
   }

// get nearest waypoint
minipointdata *minipoint::getnearest(float x,float y,float elev,int type1,int type2)
   {
   int i;

   minipointdata **vpoint=VPOINTS,*nearest=NULL;

   int type;

   if (vpoint==NULL) return(NULL);

   for (i=0; i<VNUM; i++,vpoint++)
      {
      if ((*vpoint)->opts==NULL) type=0;
      else type=(*vpoint)->opts->type;

      if (type!=type1 && type!=type2) continue;

      if (nearest==NULL) nearest=*vpoint;
      else if (getdistance2(x,y,elev,*vpoint)<getdistance2(x,y,elev,nearest)) nearest=*vpoint;
      }

   return(nearest);
   }

// get squared distance to waypoint
float minipoint::getdistance2(float x,float y,float elev,minipointdata *point)
   {return(fsqr(point->x-x)+fsqr(point->y-y)+fsqr(point->height-elev));}

// render waypoints with signposts
void minipoint::drawsignposts(float ex,float ey,float ez,
                              float height,float range,
                              float turn,float yon,
                              int type1,int type2)
   {
   int i;

   float sint,cost;
   float siny,cosy;

   float dx,dy,dz;

   const int maxinfo=1000;
   static char info[maxinfo];

   minipointdata **vpoint;
   minipointdata *nearest;

   float sheight,ssize;
   float salpha;

   // calculate visible points
   calcvdata(type1,type2);
   vpoint=getvdata();

   // check if any points were found
   if (vpoint==NULL) return;

   nearest=getnearest(ex,ez,ey,type1,type2);

   sint=sin(2.0f*PI/360.0f*turn);
   cost=cos(2.0f*PI/360.0f*turn);

   siny=sin(2.0f*PI/360.0f*yon);
   cosy=cos(2.0f*PI/360.0f*yon);

   dx=cosy*sint;
   dz=cosy*cost;
   dy=siny;

   // sort visible points
   sortvdata(ex,ez,ey,dx,dz,dy);

   initstate();
   disableculling();
   enableblending();

   linewidth(2);
   enablelinesmooth();

   // mark all waypoints with a post
   for (i=0; i<getvnum(); i++,vpoint++)
      {
      sheight=height;

      if ((*vpoint)->opts!=NULL)
         if ((*vpoint)->opts->signpostheight>0.0f) sheight=(*vpoint)->opts->signpostheight*SCALEELEV;

      if (*vpoint==nearest) color(1.0f,0.0f,0.0f);
      else color(0.0f,0.0f,1.0f);

      drawline((*vpoint)->x,(*vpoint)->height,-(*vpoint)->y,(*vpoint)->x,(*vpoint)->height+sheight,-(*vpoint)->y);
      }

   linewidth(1);

   minitext::configure_zfight(0.975f);

   // display nearest waypoint
   if (nearest!=NULL)
      {
      ssize=sheight=height;

      if (nearest->opts!=NULL)
         {
         if (nearest->opts->signpostsize>0.0f) ssize=nearest->opts->signpostsize*SCALEELEV;
         if (nearest->opts->signpostheight>0.0f) sheight=nearest->opts->signpostheight*SCALEELEV;
         }

      // compile label information of nearest waypoint
      if (nearest->zone==0)
         snprintf(info,maxinfo,"\n %s \n\n Lat=%s Lon=%s \n Elev=%s \n",
                  nearest->desc,
                  nearest->latitude,nearest->longitude,
                  nearest->elevation);
      else
         snprintf(info,maxinfo,"\n %s \n\n %s East=%s North=%s \n Elev=%s \n",
                  nearest->desc,
                  nearest->system,nearest->longitude,nearest->latitude,
                  nearest->elevation);

      // label nearest waypoint
      mtxpush();
      mtxtranslate(0.0f,sheight,0.0f);
      mtxtranslate(nearest->x,nearest->height,-nearest->y);
      mtxrotate(-turn,0.0f,1.0f,0.0f);
      mtxscale(2.0f*ssize,2.0f*ssize,2.0f*ssize);
      mtxrotate(yon,1.0f,0.0f,0.0f);
      mtxtranslate(-0.5f,0.0f,0.0f);
      minitext::drawstring(1.0f,240.0f,0.5f,0.5f,1.0f,info,1.0f,1.0f);
      mtxpop();
      }

   // process waypoints from back to front
   vpoint=getvdata();
   for (i=0; i<getvnum(); i++,vpoint++)
      if (*vpoint!=nearest)
         if (getdistance2(ex,ez,ey,*vpoint)<fsqr(range))
            {
            ssize=sheight=height;
            salpha=CONFIGURE_SIGNPOSTALPHA;

            if ((*vpoint)->opts!=NULL)
               {
               if ((*vpoint)->opts->signpostsize>0.0f) ssize=(*vpoint)->opts->signpostsize*SCALEELEV;
               if ((*vpoint)->opts->signpostheight>0.0f) sheight=(*vpoint)->opts->signpostheight*SCALEELEV;
               if ((*vpoint)->opts->signpostalpha>0.0f) salpha=(*vpoint)->opts->signpostalpha;
               }

            // compile label information of waypoint within range
            snprintf(info,maxinfo,"\n %s \n",(*vpoint)->desc);

            // label waypoint within range
            mtxpush();
            mtxtranslate(0.0f,sheight,0.0f);
            mtxtranslate((*vpoint)->x,(*vpoint)->height,-(*vpoint)->y);
            mtxrotate(-turn,0.0f,1.0f,0.0f);
            mtxrotate(yon,1.0f,0.0f,0.0f);
            mtxscale(2.0f*ssize,2.0f*ssize,2.0f*ssize);
            mtxtranslate(-0.5f,0.0f,0.0f);
            minitext::drawstring(1.0f,240.0f,0.5f,0.5f,1.0f,info,1.0f,salpha);
            mtxpop();
            }

   disablelinesmooth();

   disableblending();
   enableBFculling();
   exitstate();
   }

// render waypoints with bricks
void minipoint::drawbricks(float ex,float ey,float ez,
                           float brad,float farp,
                           float fovy,float aspect,
                           float size,
                           int type1,int type2)
   {
   int passes;

   minipointdata **vpoint;

   // calculate visible points
   calcvdata(type1,type2);
   vpoint=getvdata();

   // check if any points were found
   if (vpoint==NULL) return;

   // initialize renderer
   if (LODS==NULL)
      {
      LODS=new minilod(OFFSETLAT,OFFSETLON,SCALEX,SCALEY,SCALEELEV);
      LODS->configure_brickpasses(CONFIGURE_BRICKPASSES);

      // check if a brick file name was set
      if (BRICKNAME!=NULL)
         LODS->addbrick(BRICKNAME,brad,CONFIGURE_BRICKLODS,CONFIGURE_BRICKSTAGGER);
      }

   // set stripe pattern
   LODS->configure_brickoffset(CONFIGURE_BRICKSTRIPES);

   // draw multiple pass sequences
   for (passes=1; passes<=4; passes++)
      {
      if (CONFIGURE_BRICKPASSES==passes) drawsequence(ex,ey,ez,brad,farp,fovy,aspect,size,0,passes);
      drawsequence(ex,ey,ez,brad,farp,fovy,aspect,size,passes,passes);
      }
   }

// render waypoints with multiple pass sequence
void minipoint::drawsequence(float ex,float ey,float ez,
                             float brad,float farp,
                             float fovy,float aspect,
                             float size,int mpasses,int passes)
   {
   int i;

   minipointdata **vpoint;

   float bsize;

   int bindex,vindex;
   int bpasses;

   char *bname;

   float bred,bgreen,bblue;

   float midx,midy,basez;
   float color,r,g,b;

   float alpha,beta;
   float sa,ca,sb,cb;

   // get visible points
   vpoint=getvdata();

   // clear all volumes
   LODS->clearvolumes();

   // update visible points
   for (i=0; i<getvnum(); i++,vpoint++)
      {
      // get brick size
      if ((*vpoint)->opts==NULL) bsize=size;
      else if ((*vpoint)->opts->bricksize>0.0f) bsize=(*vpoint)->opts->bricksize*SCALEELEV;
      else bsize=size;

      // get brick index
      if ((*vpoint)->opts!=NULL) bindex=(*vpoint)->opts->brickindex;
      else if (BRICKNAME!=NULL) bindex=0;
      else bindex=-1;

      // get brick passes
      if ((*vpoint)->opts==NULL) bpasses=0;
      else bpasses=(*vpoint)->opts->brickpasses;

      // check brick passes
      if (bpasses!=mpasses) continue;

      // check for individual brick file
      if ((*vpoint)->opts!=NULL)
         {
         if ((*vpoint)->opts->brickfile!=NULL)
            if ((*vpoint)->opts->brickloaded==0)
               {
               bname=getfile((*vpoint)->opts->brickfile,ALTPATH);

               if (bname!=NULL)
                  {
                  bindex=(*vpoint)->opts->brickindex=LODS->addbrick(bname,brad,CONFIGURE_BRICKLODS,CONFIGURE_BRICKSTAGGER);
                  free(bname);
                  }

               (*vpoint)->opts->brickloaded=1;
               }

         if ((*vpoint)->opts->brickloaded==0)
            if (BRICKNAME!=NULL) bindex=(*vpoint)->opts->brickindex=0;
         }

      // check for valid brick index
      if (bindex<0) continue;

      // calculate position
      midx=(*vpoint)->x/SCALEX-OFFSETLON;
      midy=(*vpoint)->y/SCALEY-OFFSETLAT;
      basez=((*vpoint)->elev-0.25f*bsize)/SCALEELEV;

      // get individual brick color
      if ((*vpoint)->opts==NULL) bred=bgreen=bblue=0.0f;
      else
         {
         bred=(*vpoint)->opts->brickcolor_red;
         bgreen=(*vpoint)->opts->brickcolor_green;
         bblue=(*vpoint)->opts->brickcolor_blue;
         }

      // check for individual brick color
      if (bred>0.0f || bgreen>0.0f || bblue>0.0f)
         {
         r=bred;
         g=bgreen;
         b=bblue;
         }
      else
         {
         // check elevation
         if (CONFIGURE_BRICKCEILING==0.0f) color=0.0f;
         else color=basez/CONFIGURE_BRICKCEILING;

         // calculate color
         if (color<0.5f)
            {
            r=0.0f;
            g=2.0f*color;
            b=1.0f-2.0f*color;
            }
         else
            {
            r=2.0f*color-1.0f;
            g=2.0f-2.0f*color;
            b=0.0f;
            }
         }

      // set position and color
      vindex=LODS->addvolume(bindex,
                             midx,midy,basez,
                             bsize/SCALEX,bsize/SCALEY,bsize/SCALEELEV,
                             r,g,b,CONFIGURE_BRICKALPHA);

      // set orientation
      if ((*vpoint)->opts!=NULL)
         {
         alpha=-(*vpoint)->opts->brickturn*RAD;
         beta=(*vpoint)->opts->brickincline*RAD;

         if (alpha!=0.0f || beta!=0.0f)
            {
            sa=fsin(alpha);
            ca=fcos(alpha);

            sb=fsin(beta);
            cb=fcos(beta);

            LODS->addorientation(vindex,
                                 ca,0.0f,sa,
                                 -sa*sb,cb,ca*sb,
                                 -sa*cb,-sb,ca*cb);
            }
         }

      // set rendering passes
      LODS->addpasses(bindex,passes);
      }

   // render visible points
   LODS->render(ex,ey,ez,farp,fovy,aspect);
   }

// set brick file name
void minipoint::setbrick(char *filename)
   {
   if (filename==NULL) ERRORMSG();

   if (BRICKNAME!=NULL) free(BRICKNAME);
   BRICKNAME=strdup(filename);
   }

// configuring:

void minipoint::configure_srcdatum(int datum)
   {CONFIGURE_SRCDATUM=datum;}

void minipoint::configure_dstzone(int zone)
   {CONFIGURE_DSTZONE=zone;}

void minipoint::configure_dstdatum(int datum)
   {CONFIGURE_DSTDATUM=datum;}

void minipoint::configure_automap(int automap)
   {CONFIGURE_AUTOMAP=automap;}

void minipoint::configure_brickceiling(float brickceiling)
   {CONFIGURE_BRICKCEILING=brickceiling;}

void minipoint::configure_signpostalpha(float signpostalpha)
   {CONFIGURE_SIGNPOSTALPHA=signpostalpha;}

void minipoint::configure_brickpasses(int brickpasses)
   {CONFIGURE_BRICKPASSES=brickpasses;}

void minipoint::configure_brickalpha(float brickalpha)
   {CONFIGURE_BRICKALPHA=brickalpha;}

void minipoint::configure_bricklods(int bricklods)
   {CONFIGURE_BRICKLODS=bricklods;}

void minipoint::configure_brickstagger(float brickstagger)
   {CONFIGURE_BRICKSTAGGER=brickstagger;}

void minipoint::configure_brickstripes(float brickstripes)
   {CONFIGURE_BRICKSTRIPES=brickstripes;}
