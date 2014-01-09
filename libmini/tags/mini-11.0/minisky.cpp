// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "pnmbase.h"
#include "miniOGL.h"

#include "minisky.h"

// default constructor
minisky::minisky()
   {
   LOADED=FALSE;

   ALPHA_STEPS=30;
   BETA_STEPS=15;
   }

// destructor
minisky::~minisky()
   {
   if (LOADED)
      {
      delete STRIP;
      deletetexmap(TEXID);
      }
   }

void minisky::loadskydome(const char *skyfile,
                          float mx,float my,float mz,
                          float diameter,
                          float scaley)
   {
   unsigned char *image;
   int width,height,components;

   if ((image=readPNMfile(skyfile,&width,&height,&components))==NULL) return;
   if (width<2 || height<2 || components!=3) ERRORMSG();

   if (LOADED)
      {
      delete STRIP;
      deletetexmap(TEXID);
      }

   STRIP=new ministrip(0,0,2);

   TEXID=buildRGBtexmap(image,&width,&height);
   free(image);

   createskydome(width,height);
   setpos(mx,my,mz,diameter,scaley);

   LOADED=TRUE;
   }

void minisky::setpos(float mx,float my,float mz,
                     float diameter,
                     float scaley)
   {
   CENTERX=mx;
   CENTERY=my;
   CENTERZ=mz;

   RADIUS=diameter/2.0f;
   SCALEY=scaley;
   }

inline float minisky::mapalpha(float alpha,int texsize)
   {
   float a=alpha/PI/2.0f;
   return((1.0f-a)*0.5f/texsize+a*(1.0f+0.5f/texsize));
   }

inline float minisky::mapbeta(float beta,int texsize)
   {
   float b=2.0f*beta/PI;
   if (b>1.0f) return(1.0f-0.5f/texsize);
   return((1.0f-b)*0.5f/texsize+b*(1.0f-0.5f/texsize));
   }

inline void minisky::addvertex(float alpha,float beta,int width,int height)
   {
   STRIP->settex(mapalpha(alpha,width),mapbeta(beta,height));
   STRIP->addvtx(fsin(alpha)*fsin(beta),fcos(beta),fcos(alpha)*fsin(beta));
   }

void minisky::createskydome(int width,int height)
   {
   int i,j;

   float alpha,beta;

   STRIP->setcol(1.0f,1.0f,1.0f);

   beta=0.0f;

   for (i=0; i<BETA_STEPS; i++)
      {
      STRIP->beginstrip();

      alpha=0.0f;

      for (j=0; j<=ALPHA_STEPS; j++)
         {
         addvertex(alpha,beta,width,height);
         addvertex(alpha,beta+PI/BETA_STEPS,width,height);

         alpha+=2.0f*PI/ALPHA_STEPS;
         }

      beta+=PI/BETA_STEPS;
      }
   }

void minisky::drawskydome()
   {
   if (LOADED)
      {
      initstate();

      enableFFculling();

      mtxpush();
      mtxtranslate(CENTERX,CENTERY,CENTERZ);
      mtxscale(RADIUS,RADIUS*SCALEY,RADIUS);

      bindtexmap(TEXID);
      STRIP->render();
      bindtexmap(0);

      mtxpop();

      enableBFculling();

      exitstate();
      }
   }
