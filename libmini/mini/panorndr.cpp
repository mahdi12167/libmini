// (c) by Stefan Roettger

#include <mini/miniOGL.h>
#include <mini/database.h>

#include "panorndr.h"

// default constructor
minipointrndr_panorndr::minipointrndr_panorndr():
   minipointrndr(minipointopts::OPTION_TYPE_FREE,1)
   {
   STRIP=new ministrip(0,3,2);
   SLOT=STRIP->getfreeslot();

   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_BEGIN);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_HEADER);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_BASIC);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_VIEWPOS);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_NORMAL);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_TEX);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_FOOTER);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_END);

   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_BEGIN);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_HEADER);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_BASIC);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_TEX_MASK);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_SHADE_ALPHA2);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_FOOTER);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_END);

   STRIP->useshader(SLOT);

   create_sphere(1.0f,1.0f,1.0f,1.0f,1.0f,10);
   }

// destructor
minipointrndr_panorndr::~minipointrndr_panorndr()
   {delete STRIP;}

// create a sphere with constant radius and color
void minipointrndr_panorndr::create_sphere(float radius,
                                           const float r,const float g,const float b,const float a,
                                           int stripes)
   {
   int i,j;

   const int alpha_steps=4*stripes;
   const int beta_steps=stripes;

   float u,v;
   float alpha,beta;
   float nx,ny,nz;
   float tb;

   STRIP->setcol(r,g,b,a);

   for (j=-beta_steps+1; j<beta_steps-1; j++)
      {
      STRIP->beginstrip();

      for (i=alpha_steps; i>=0; i--)
         {
         u=(float)i/alpha_steps;
         v=(float)j/beta_steps;

         alpha=u*2*PI;
         beta=v*PI/2;

         nx=fcos(alpha)*fcos(beta);
         nz=-fsin(alpha)*fcos(beta);
         ny=fsin(beta);

         tb=ftan(beta);

         STRIP->setnrm(nx,ny,nz);
         STRIP->settex(u,0.5f-tb/2);
         STRIP->addvtx(nx*radius,ny*radius,nz*radius);

         v=(float)(j+1)/beta_steps;
         beta=v*PI/2;

         tb=ftan(beta);

         nx=fcos(alpha)*fcos(beta);
         nz=-fsin(alpha)*fcos(beta);
         ny=fsin(beta);

         STRIP->setnrm(nx,ny,nz);
         STRIP->settex(u,0.5f-tb/2);
         STRIP->addvtx(nx*radius,ny*radius,nz*radius);
         }
      }
   }

// init method
BOOLINT minipointrndr_panorndr::init(minipoint *points,
                                     float ex,float ey,float ez,
                                     float dx,float dy,float dz,
                                     float nearp,float farp,float fovy,float aspect,
                                     double time,minipointopts *global,
                                     BOOLINT usewarp)
   {
   if (points==NULL ||
       dx==MAXFLOAT || dy==MAXFLOAT || dz==MAXFLOAT ||
       nearp<=0.0f || farp<=0.0f || fovy<=0.0f || aspect<=0.0f ||
       time<0.0) ERRORMSG();

   if (usewarp) return(FALSE);

   POINTS=points;

   EX=ex;
   EY=ey;
   EZ=ez;

   GLOBAL=global;

   SCALEELEV=points->getscaleelev();

   initstate();
   enableFFculling();
   enableblending();
   enableAtest(0.01f);

   return(TRUE);
   }

// pre-render method
void minipointrndr_panorndr::pre(int pass)
   {if (pass!=1) ERRORMSG();}

// rendering method
void minipointrndr_panorndr::render(minipointdata *vpoint,int pass)
   {
   float range;
   float size;

   char *filename;
   databuf buf;

   float scalex,scaley;

   if (pass==1)
      {
      // calculate maximum range
      range=(GLOBAL->datarange>0.0f)?GLOBAL->datarange:GLOBAL->signpostrange;
      if (vpoint->opts!=NULL)
         if (vpoint->opts->datarange>0.0f) range=vpoint->opts->datarange*SCALEELEV;

      // check distance
      if (POINTS->getdistance2(EX,EZ,EY,vpoint)>fsqr(range))
         {
         // release already loaded texture
         if (vpoint->opts!=NULL)
            if (vpoint->opts->dataswitch==0 && vpoint->opts->databoolvalue)
               {
               if (vpoint->opts->datatexid!=0) deletetexmap(vpoint->opts->datatexid);

               vpoint->opts->datatexid=0;
               vpoint->opts->datatexwidth=0;
               vpoint->opts->datatexheight=0;
               vpoint->opts->datatexmipmaps=0;

               vpoint->opts->databoolvalue=FALSE;
               }

         // skip rendering
         return;
         }

      // push matrices
      mtxtex();
      mtxpush();
      mtxid();
      mtxmodel();
      mtxpush();

      // translate by waypoint coordinates
      mtxtranslate(vpoint->x,vpoint->elev,-vpoint->y);

      if (vpoint->opts!=NULL)
         {
         // calculate size
         if (vpoint->opts->datasize>0.0f) size=vpoint->opts->datasize*SCALEELEV;
         else if (GLOBAL->datasize>0.0f) size=GLOBAL->datasize;
         else size=GLOBAL->signpostsize;

         // report render size and offset
         vpoint->size=size;
         vpoint->offset=vpoint->elev+0.5f*size-vpoint->height;

         // rotate and scale waypoint
         mtxrotate(-90.0f-vpoint->opts->dataturn,0.0f,1.0f,0.0f);
         mtxscale(0.5f*size,0.5f*size,0.5f*size);
         mtxtranslate(0.0f,1.0f,0.0f);

         // enable texture
         if (vpoint->opts->datafile!=NULL && vpoint->opts->dataswitch==0)
            {
            // load texture and store texture id
            if (!vpoint->opts->databoolvalue)
               {
               filename=POINTS->getfile(vpoint->opts->datafile);

               if (filename!=NULL)
                  {
                  buf.loaddata(filename);
                  free(filename);

                  vpoint->opts->datatexmipmaps=1;
                  vpoint->opts->datatexid=db2texid(&buf,&vpoint->opts->datatexwidth,&vpoint->opts->datatexheight,&vpoint->opts->datatexmipmaps);
                  buf.release();
                  }

               vpoint->opts->databoolvalue=TRUE;
               }

            // bind texture id
            if (vpoint->opts->datatexid!=0 && vpoint->opts->datatexwidth>0 && vpoint->opts->datatexheight>0)
               {
               // pass texture id
               STRIP->setpixshadertexid(SLOT,vpoint->opts->datatexid,vpoint->opts->datatexwidth,vpoint->opts->datatexheight,1,vpoint->opts->datatexmipmaps);

               // set default texture coordinate scaling
               scalex=scaley=1.0f;

               // calculate texture coordinate scaling for proper texel aspect ratio
               if (vpoint->opts->datatexwidth>=vpoint->opts->datatexheight)
                  {
                  // horizontally aligned texture
                  if (vpoint->opts->datacontrol!=0.0f) scalex=360.0f/vpoint->opts->datacontrol;
                  scaley=1.0f/(vpoint->opts->datatexheight/(scalex*vpoint->opts->datatexwidth))/PI;

                  // apply texture coordinate shift
                  if (vpoint->opts->dataincline!=0.0f)
                     {
                     mtxtex();
                     mtxtranslate(0.0f,vpoint->opts->dataincline/90.0f,0.0f);
                     mtxmodel();
                     }
                  }
               else
                  {
                  // vertically aligned texture
                  if (vpoint->opts->datacontrol!=0.0f) scalex=360.0f/vpoint->opts->datacontrol;
                  scaley=1.0f/(vpoint->opts->datatexwidth/(scalex*vpoint->opts->datatexheight))/PI;

                  // flip axis
                  mtxtex();
                  if (vpoint->opts->datacontrol!=0.0f) mtxtranslate(0.0f,vpoint->opts->dataincline/vpoint->opts->datacontrol,0.0f);
                  else mtxtranslate(0.0f,vpoint->opts->dataincline/360.0f,0.0f);
                  mtxtranslate(0.5f,0.5f,0.0f);
                  mtxrotate(90.0f,0.0f,0.0f,1.0f);
                  mtxtranslate(-0.5f,-0.5f,0.0f);
                  mtxmodel();
                  mtxrotate(-90.0f,1.0f,0.0f,0.0f);
                  }

               // apply texture coordinate scaling
               mtxtex();
               mtxtranslate(0.5f,0.5f,0.0f);
               mtxscale(-scalex,scaley,0.0f);
               mtxtranslate(-0.5f,-0.5f,0.0f);
               mtxmodel();
               }
            else vpoint->opts->dataswitch=1;
            }
         }

      // render the strip if not disabled
      if (vpoint->opts->dataswitch==0) STRIP->render();

      // pop matrices
      mtxtex();
      mtxpop();
      mtxmodel();
      mtxpop();
      }
   }

// post-render method
void minipointrndr_panorndr::post(int pass)
   {if (pass!=1) ERRORMSG();}

// exit method
void minipointrndr_panorndr::exit()
   {
   disableAtest();
   disableblending();
   enableBFculling();
   exitstate();
   }
