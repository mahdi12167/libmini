// (c) by Stefan Roettger

#include "minitime.h"

#ifndef NOSQUISH
#include "squishbase.h"
#endif

#include "miniOGL.h"
#include "minishader.h"

#include "viewerbase.h"

// default constructor
viewerbase::viewerbase()
   {
   // configurable parameters:

   PARAMS.fps=25.0f;     // frames per second (target frame rate)

   PARAMS.fovy=60.0f;    // field of view (degrees)
   PARAMS.nearp=10.0f;   // near plane (meters)
   PARAMS.farp=10000.0f; // far plane (meters)

   // feature switches:

   PARAMS.usefog=FALSE;
   PARAMS.useshaders=FALSE;
   PARAMS.usebathymap=FALSE;
   PARAMS.usecontours=FALSE;
   PARAMS.usenprshader=FALSE;
   PARAMS.usediffuse=TRUE;
   PARAMS.usewireframe=FALSE;
   PARAMS.useskydome=FALSE;
   PARAMS.usewaypoints=FALSE;
   PARAMS.usebricks=FALSE;
   PARAMS.useearth=FALSE;

   // optional spherical fog:

   PARAMS.fogcolor[0]=0.65f;
   PARAMS.fogcolor[1]=0.7f;
   PARAMS.fogcolor[2]=0.7f;

   PARAMS.fogstart=0.5f;   // start of fog relative to far plane
   PARAMS.fogdensity=0.5f; // relative fog density

   // optional sky-dome:

   PARAMS.skydome="SkyDome.ppm"; // skydome file

   // optional earth globe:

   PARAMS.lightdir[0]=0.0f; // directional light
   PARAMS.lightdir[1]=1.0f; // directional light
   PARAMS.lightdir[2]=0.0f; // directional light

   PARAMS.transition=4.0f;  // transition gradient between night and day

   PARAMS.frontname="EarthDay.ppm";  // file name of front earth texture
   PARAMS.backname="EarthNight.ppm"; // file name of back earth texture

   // image conversion parameters:

   PARAMS.conversion_params.jpeg_quality=75.0f; // jpeg quality in percent

   PARAMS.conversion_params.usegreycstoration=FALSE; // use greycstoration for image denoising

   PARAMS.conversion_params.greyc_p=0.8f; // greycstoration sharpness, useful range=[0.7-0.9]
   PARAMS.conversion_params.greyc_a=0.4f; // greycstoration anisotropy, useful range=[0.1-0.5]

   // initialize state:

   TERRAIN=new miniterrain();

   SKYDOME=new minisky();
   EARTH=new miniglobe();

   START=minigettime();
   TIMER=0.0;

   THREADBASE=new threadbase();
   CURLBASE=new curlbase();
   }

// destructor
viewerbase::~viewerbase()
   {
   delete TERRAIN;

   delete SKYDOME;
   delete EARTH;

   delete THREADBASE;
   delete CURLBASE;
   }

// get parameters
void viewerbase::get(VIEWER_PARAMS &params)
   {params=PARAMS;}

// set parameters
void viewerbase::set(VIEWER_PARAMS &params)
   {
   miniterrain::MINITERRAIN_PARAMS tparams;

   // set new state
   PARAMS=params;

   // get the actual terrain state
   TERRAIN->get(tparams);

   // update the terrain state:

   tparams.fps=PARAMS.fps;

   tparams.fovy=PARAMS.fovy;
   tparams.nearp=PARAMS.nearp;
   tparams.farp=PARAMS.farp;

   tparams.usefog=PARAMS.usefog;
   tparams.useshaders=PARAMS.useshaders;
   tparams.usebathymap=PARAMS.usebathymap;
   tparams.usecontours=PARAMS.usecontours;
   tparams.usenprshader=PARAMS.usenprshader;
   tparams.usewaypoints=PARAMS.usewaypoints;
   tparams.usebricks=PARAMS.usebricks;

   tparams.fogcolor[0]=PARAMS.fogcolor[0];
   tparams.fogcolor[1]=PARAMS.fogcolor[1];
   tparams.fogcolor[2]=PARAMS.fogcolor[2];

   tparams.fogstart=PARAMS.fogstart;
   tparams.fogdensity=PARAMS.fogdensity;

   if (PARAMS.useearth) tparams.warpmode=1; // switch to reference mode
   else tparams.warpmode=0; // switch to linear mode

   // finally pass the updated terrain state
   TERRAIN->set(tparams);
   }

// propagate parameters
void viewerbase::propagate()
   {set(PARAMS);}

#ifndef NOSQUISH

// S3TC auto-compression hook
void viewerbase::autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                              unsigned char **s3tcdata,unsigned int *s3tcbytes,
                              databuf *obj,void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::compressS3TC(isrgbadata,rawdata,bytes,
                            s3tcdata,s3tcbytes,obj->xsize,obj->ysize);
   }

#endif

// initialize the terrain hooks
void viewerbase::inithooks()
   {
   // register callbacks
   TERRAIN->setcallbacks(THREADBASE,
                         threadbase::threadinit,threadbase::threadexit,
                         threadbase::startthread,threadbase::jointhread,
                         threadbase::lock_cs,threadbase::unlock_cs,
                         threadbase::lock_io,threadbase::unlock_io,
                         CURLBASE,
                         curlbase::curlinit,curlbase::curlexit,
                         curlbase::getURL,curlbase::checkURL);

   // register libMini conversion hook (JPEG/PNG)
   convbase::setconversion(&PARAMS.conversion_params);

#ifndef NOSQUISH

   // register auto-compression hook
   databuf::setautocompress(autocompress,NULL);

#endif
   }

// load tileset (short version)
BOOLINT viewerbase::load(const char *url,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // initialize the terrain hooks
   inithooks();

   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(url,loadopts,reset));
   }

// load tileset (long version)
BOOLINT viewerbase::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // initialize the terrain hooks
   inithooks();

   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(baseurl,baseid,basepath1,basepath2,loadopts,reset));
   }

// load optional features
void viewerbase::loadopts()
   {
   minilayer *layer;
   minilayer::MINILAYER_PARAMS lparams;

   layer=TERRAIN->getlayer(TERRAIN->getreference());

   if (layer==NULL) return;

   layer->get(lparams);

   // load skydome:

   char *skyname=layer->getcache()->getfile(PARAMS.skydome,lparams.altpath);

   if (skyname!=NULL)
      {
      SKYDOME->loadskydome(skyname);
      free(skyname);
      }

   // load earth textures:

   char *ename1=layer->getcache()->getfile(PARAMS.frontname,lparams.altpath);

   if (ename1!=NULL)
      {
      EARTH->configure_frontname(ename1);
      free(ename1);
      }

   char *ename2=layer->getcache()->getfile(PARAMS.backname,lparams.altpath);

   if (ename2!=NULL)
      {
      EARTH->configure_backname(ename2);
      free(ename2);
      }
   }

// get initial view point
minicoord viewerbase::getinitial()
   {return(TERRAIN->getinitial());}

// set initial eye point
void viewerbase::initeyepoint(const minicoord &e)
   {TERRAIN->initeyepoint(e);}

// generate and cache scene for a particular eye point
void viewerbase::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect)
   {TERRAIN->cache(e,d,u,aspect,gettime());}

// render cached scene
void viewerbase::render()
   {
   minicoord egl;

   minilayer *layer;
   minilayer::MINILAYER_PARAMS lparams;

   GLfloat color[4];

   miniwarp warp;
   miniv4d one[3],mtx[3];
   double oglmtx[16];

   float light0[3]={0.0f,0.0f,0.0f};

   layer=TERRAIN->getlayer(TERRAIN->getreference());

   if (layer!=NULL)
      {
      layer->get(lparams);

      egl=layer->map_g2o(lparams.eye);

      // enable wireframe mode
      if (PARAMS.usewireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

      // enable fog
      if (PARAMS.usefog)
         {
         color[0]=PARAMS.fogcolor[0];
         color[1]=PARAMS.fogcolor[1];
         color[2]=PARAMS.fogcolor[2];
         color[3]=1.0f;

         glFogfv(GL_FOG_COLOR,color);

         glFogi(GL_FOG_MODE,GL_LINEAR);
         glFogf(GL_FOG_START,PARAMS.fogstart*layer->len_g2o(PARAMS.farp));
         glFogf(GL_FOG_END,layer->len_g2o(PARAMS.farp));

         glEnable(GL_FOG);
         }

      // draw skydome
      if (PARAMS.useskydome)
         {
         SKYDOME->setpos(egl.vec.x,egl.vec.y,egl.vec.z,
                         1.9*layer->len_g2o(PARAMS.farp));

         SKYDOME->drawskydome();
         }

      // render earth globe (without Z writing)
      if (PARAMS.useearth)
         {
         EARTH->setscale(layer->len_o2g(1.0));

         one[0]=miniv4d(1.0,0.0,0.0);
         one[1]=miniv4d(0.0,1.0,0.0);
         one[2]=miniv4d(0.0,0.0,1.0);

         warp=*getreference()->getwarp();
         warp.def_2affine(one);
         warp.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_FINAL);
         warp.getwarp(mtx);

         oglmtx[0]=mtx[0].x;
         oglmtx[1]=mtx[1].x;
         oglmtx[2]=mtx[2].x;
         oglmtx[3]=0.0;

         oglmtx[4]=mtx[0].y;
         oglmtx[5]=mtx[1].y;
         oglmtx[6]=mtx[2].y;
         oglmtx[7]=0.0;

         oglmtx[8]=mtx[0].z;
         oglmtx[9]=mtx[1].z;
         oglmtx[10]=mtx[2].z;
         oglmtx[11]=0.0;

         oglmtx[12]=mtx[0].w;
         oglmtx[13]=mtx[1].w;
         oglmtx[14]=mtx[2].w;
         oglmtx[15]=1.0;

         EARTH->setmatrix(oglmtx);

         if (PARAMS.usediffuse) EARTH->settexturedirectparams(PARAMS.lightdir,PARAMS.transition);
         else EARTH->settexturedirectparams(light0,PARAMS.transition);

         EARTH->setfogparams((PARAMS.usefog)?PARAMS.fogstart/2.0f*layer->len_g2o(PARAMS.farp):0.0f,(PARAMS.usefog)?layer->len_g2o(PARAMS.farp):0.0f,
                             PARAMS.fogdensity,
                             PARAMS.fogcolor);

         EARTH->render(MINIGLOBE_FIRST_RENDER_PHASE);
         }

      // render terrain
      TERRAIN->render();

      // render earth globe (without RGB writing)
      if (PARAMS.useearth) EARTH->render(MINIGLOBE_LAST_RENDER_PHASE);

      // disable fog
      if (PARAMS.usefog) glDisable(GL_FOG);

      // disable wireframe mode
      if (PARAMS.usewireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      }
   }

// get time
double viewerbase::gettime()
   {return(minigettime()-START);}

// start timer
void viewerbase::starttimer()
   {TIMER=minigettime()-START;}

// measure timer
double viewerbase::gettimer()
   {return(minigettime()-START-TIMER);}

// idle for the remainder of the frame
void viewerbase::idle(double dt)
   {miniwaitfor(1.0/PARAMS.fps-dt);}

// shoot a ray at the scene
double viewerbase::shoot(const minicoord &o,const miniv3d &d)
   {return(TERRAIN->shoot(o,d));}
