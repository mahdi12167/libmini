// (c) by Stefan Roettger

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
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_TEX);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_FOOTER);
   STRIP->concatvtxshader(SLOT,MINI_SNIPPET_VTX_END);

   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_BEGIN);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_HEADER);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_BASIC);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_TEX);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_FOOTER);
   STRIP->concatpixshader(SLOT,MINI_SNIPPET_FRG_END);

   STRIP->useshader(SLOT);

   create_sphere(1.0f,1.0f,1.0f,1.0f,0.9f,10);
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

   STRIP->setcol(r,g,b,a);

   for (j=-beta_steps; j<beta_steps; j++)
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

         STRIP->setnrm(nx,ny,nz);
         STRIP->settex(u,0.5f-v/2);
         STRIP->addvtx(nx*radius,ny*radius,nz*radius);

         v=(float)(j+1)/beta_steps;
         beta=v*PI/2;

         nx=fcos(alpha)*fcos(beta);
         nz=-fsin(alpha)*fcos(beta);
         ny=fsin(beta);

         STRIP->setnrm(nx,ny,nz);
         STRIP->settex(u,0.5f-v/2);
         STRIP->addvtx(nx*radius,ny*radius,nz*radius);
         }
      }
   }

// init method
void minipointrndr_panorndr::init(minipoint *points,
                                  float ex,float ey,float ez,
                                  float dx,float dy,float dz,
                                  float nearp,float farp,float fovy,float aspect,
                                  double time,minipointopts *global)
   {
   if (points==NULL ||
       dx==MAXFLOAT || dy==MAXFLOAT || dz==MAXFLOAT ||
       nearp<=0.0f || farp<=0.0f || fovy<=0.0f || aspect<=0.0f ||
       time<0.0) ERRORMSG();

   EX=ex;
   EY=ey;
   EZ=ez;

   GLOBAL=global;

   SCALEELEV=points->getscaleelev();

   initstate();
   enableblending();
   }

// pre-render method
void minipointrndr_panorndr::pre(int pass)
   {if (pass!=1) ERRORMSG();}

// rendering method
void minipointrndr_panorndr::render(minipointdata *vpoint,int pass)
   {
   databuf buf;

   if (pass==1)
      {
      mtxpush();
      mtxtranslate(vpoint->x,vpoint->elev,-vpoint->y);

      if (vpoint->opts!=NULL)
         {
         mtxrotate(vpoint->opts->dataturn,0.0f,1.0f,0.0f);
         mtxscale(vpoint->opts->datasize,vpoint->opts->datasize,vpoint->opts->datasize);

         if (vpoint->opts->datafile!=NULL)
            if (!vpoint->opts->dataloaded)
               {
               buf.loaddata(vpoint->opts->datafile); //!! get file
               vpoint->opts->dataloaded=TRUE;

               STRIP->setpixshadertexbuf(SLOT,&buf,1); //!! pass texid
               }
         }

      STRIP->setscale(SCALEELEV);
      STRIP->render();

      mtxpop();
      }
   }

// post-render method
void minipointrndr_panorndr::post(int pass)
   {if (pass!=1) ERRORMSG();}

// exit method
void minipointrndr_panorndr::exit()
   {
   disableblending();
   exitstate();
   }
