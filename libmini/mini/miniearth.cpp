// (c) by Stefan Roettger

#include "miniio.h"

#include "miniOGL.h"
#include "minishader.h"

#include "miniutm.h"
#include "miniwarp.h"

#include "miniearth.h"

// default constructor
miniearth::miniearth()
   {
   // configurable parameters:

   PARAMS.warpmode=4;    // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4

   PARAMS.fps=25.0f;     // frames per second (target frame rate)

   PARAMS.fovy=60.0f;    // field of view (degrees)
   PARAMS.nearp=10.0f;   // near plane (meters)
   PARAMS.farp=10000.0f; // far plane (meters)

   // feature switches:

   PARAMS.usefog=FALSE;
   PARAMS.useshaders=FALSE;
   PARAMS.usediffuse=FALSE;
   PARAMS.usevisshader=FALSE;
   PARAMS.usebathymap=FALSE;
   PARAMS.usecontours=FALSE;
   PARAMS.usenprshader=FALSE;
   PARAMS.usewireframe=FALSE;
   PARAMS.useskydome=FALSE;
   PARAMS.usewaypoints=FALSE;
   PARAMS.usebricks=FALSE;
   PARAMS.useearth=TRUE;
   PARAMS.useflat=FALSE;

   // optional spherical fog:

   PARAMS.fogcolor[0]=0.65f;
   PARAMS.fogcolor[1]=0.7f;
   PARAMS.fogcolor[2]=0.7f;

   PARAMS.fogstart=0.5f;   // start of fog relative to far plane
   PARAMS.fogdensity=0.5f; // relative fog density

   // optional sky-dome:

   PARAMS.skydome="SkyDome.ppm"; // skydome file

   // optional earth globe:

   PARAMS.lightdir=miniv3d(0.0,0.0,1.0); // directional light

   PARAMS.lightbias=0.75f;   // lighting bias
   PARAMS.lightoffset=0.25f; // lighting offset

   PARAMS.transbias=4.0f;    // transition bias between night and day
   PARAMS.transoffset=0.01f; // transition offset between night and day

   PARAMS.frontname="EarthDay.ppm";  // file name of front earth texture
   PARAMS.backname="EarthNight.ppm"; // file name of back earth texture

   // initialize state:

   TERRAIN=new miniterrain();

   SKYDOME=new minisky();
   EARTH=new miniglobe();

   LOADED=FALSE;
   }

// destructor
miniearth::~miniearth()
   {
   delete TERRAIN;

   delete SKYDOME;
   delete EARTH;
   }

// get parameters
void miniearth::get(EARTH_PARAMS &params)
   {params=PARAMS;}

// set parameters
void miniearth::set(EARTH_PARAMS &params)
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
   tparams.usediffuse=PARAMS.usediffuse;
   tparams.usevisshader=PARAMS.usevisshader;
   tparams.usebathymap=PARAMS.usebathymap;
   tparams.usecontours=PARAMS.usecontours;
   tparams.usenprshader=PARAMS.usenprshader;
   tparams.usewaypoints=PARAMS.usewaypoints;
   tparams.usebricks=PARAMS.usebricks;

   tparams.lightdir=PARAMS.lightdir;

   tparams.lightbias=PARAMS.lightbias;
   tparams.lightoffset=PARAMS.lightoffset;

   tparams.fogcolor[0]=PARAMS.fogcolor[0];
   tparams.fogcolor[1]=PARAMS.fogcolor[1];
   tparams.fogcolor[2]=PARAMS.fogcolor[2];

   tparams.fogstart=PARAMS.fogstart;
   tparams.fogdensity=PARAMS.fogdensity;

   if (PARAMS.useflat)
      if (PARAMS.warpmode==4) tparams.warpmode=2;
      else if (PARAMS.warpmode==3) tparams.warpmode=1;
      else tparams.warpmode=0;
   else tparams.warpmode=PARAMS.warpmode;

   // finally pass the updated terrain state
   TERRAIN->set(tparams);
   }

// propagate parameters
void miniearth::propagate()
   {set(PARAMS);}

// initialize the OpenGL wrapper
void miniearth::initOGL()
   {
   // turn on hw-accelerated OpenGL mipmap generation
   miniOGL::configure_generatemm(1);

   // turn off on-the-fly OpenGL texture compression
   miniOGL::configure_compression(0);
   }

// load tileset (short version)
BOOLINT miniearth::load(const char *url,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // initialize the OpenGL wrapper
   initOGL();

   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(url,loadopts,reset));
   }

// load tileset (long version)
BOOLINT miniearth::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // initialize the OpenGL wrapper
   initOGL();

   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(baseurl,baseid,basepath1,basepath2,loadopts,reset));
   }

// load optional features
void miniearth::loadopts()
   {
   minilayer *ref;
   minilayer::MINILAYER_PARAMS lparams;

   if (LOADED) return;

   ref=TERRAIN->getlayer(TERRAIN->getreference());

   if (ref==NULL) return;

   ref->get(lparams);

   // load skydome:

   char *skyname=NULL;

   if (ref->getcache()!=NULL) skyname=ref->getcache()->getfile(PARAMS.skydome,lparams.altpath);
   else skyname=getfile(PARAMS.skydome,lparams.altpath);

   if (skyname!=NULL)
      {
      SKYDOME->loadskydome(skyname);
      free(skyname);
      }

   // load earth textures:

   char *ename1=NULL;

   if (ref->getcache()!=NULL) ename1=ref->getcache()->getfile(PARAMS.frontname,lparams.altpath);
   else ename1=getfile(PARAMS.frontname,lparams.altpath);

   if (ename1!=NULL)
      {
      EARTH->configure_frontname(ename1);
      free(ename1);
      }

   char *ename2=NULL;

   if (ref->getcache()!=NULL) ename2=ref->getcache()->getfile(PARAMS.backname,lparams.altpath);
   else ename2=getfile(PARAMS.backname,lparams.altpath);

   if (ename2!=NULL)
      {
      EARTH->configure_backname(ename2);
      free(ename2);
      }

   LOADED=TRUE;
   }

// get file
char *miniearth::getfile(const char *src_file,const char *altpath)
   {
   char *file;

   if (checkfile(src_file)) return(strdup(src_file));

   file=strcct(altpath,src_file);

   if (file!=NULL)
      if (checkfile(file)) return(file);
      else free(file);

   return(NULL);
   }

// get initial view point
minicoord miniearth::getinitial()
   {return(TERRAIN->getinitial());}

// set initial eye point
void miniearth::initeyepoint(const minicoord &e)
   {TERRAIN->initeyepoint(e);}

// generate and cache scene for a particular eye point and time step
void miniearth::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time)
   {TERRAIN->cache(e,d,u,aspect,time);}

// render cached scene
void miniearth::render()
   {
   minilayer *ref;
   minilayer::MINILAYER_PARAMS lparams;

   minicoord egl;

   GLfloat color[4];

   miniwarp warp;

   miniv4d mtx[3];
   double oglmtx[16];

   miniv3d lgl;
   float light[3];

   ref=getreference();

   if (ref!=NULL)
      {
      ref->get(lparams);

      egl=ref->map_g2o(lparams.eye);

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
         glFogf(GL_FOG_START,PARAMS.fogstart*ref->len_g2o(PARAMS.farp));
         glFogf(GL_FOG_END,ref->len_g2o(PARAMS.farp));

         glEnable(GL_FOG);
         }

      // draw skydome
      if (PARAMS.useskydome)
         if (ref->get()->warpmode==0 || ref->get()->warpmode==2)
            {
            SKYDOME->setpos(egl.vec.x,egl.vec.y,egl.vec.z,
                            1.9*ref->len_g2o(PARAMS.farp));

            SKYDOME->drawskydome();
            }

      // render earth globe (without Z writing)
      if (PARAMS.useearth)
         if (ref->get()->warpmode!=0)
            {
            EARTH->setscale(ref->len_o2g(1.0));

            warp=*getearth()->getwarp();
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

            lgl=getnull()->rot_g2i(PARAMS.lightdir,getnull()->getcenter());

            light[0]=lgl.x;
            light[1]=lgl.y;
            light[2]=lgl.z;

            if (PARAMS.usediffuse)
               {
               EARTH->setshadedirectparams(light,PARAMS.lightbias,PARAMS.lightoffset);
               EARTH->settexturedirectparams(light,PARAMS.transbias,PARAMS.transbias*PARAMS.transoffset);
               }
            else
               {
               EARTH->setshadedirectparams(light,0.0f,1.0f);
               EARTH->settexturedirectparams(light,0.0f,1.0f);
               }

            EARTH->setfogparams((PARAMS.usefog)?PARAMS.fogstart/2.0f*ref->len_g2o(PARAMS.farp):0.0f,(PARAMS.usefog)?ref->len_g2o(PARAMS.farp):0.0f,
                                PARAMS.fogdensity,
                                PARAMS.fogcolor);

            EARTH->render(MINIGLOBE_FIRST_RENDER_PHASE);
            }

      // render terrain
      TERRAIN->render();

      // render earth globe (without RGB writing)
      if (PARAMS.useearth)
         if (ref->get()->warpmode!=0)
            EARTH->render(MINIGLOBE_LAST_RENDER_PHASE);

      // disable fog
      if (PARAMS.usefog) glDisable(GL_FOG);

      // disable wireframe mode
      if (PARAMS.usewireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      }
   }

// shoot a ray at the scene
double miniearth::shoot(const minicoord &o,const miniv3d &d)
   {return(TERRAIN->shoot(o,d));}
