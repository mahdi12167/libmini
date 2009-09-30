// (c) by Stefan Roettger

#ifndef NOVIEWER

#include <mini/minitime.h>

#include <mini/database.h>

#include <mini/squishbase.h>

#include <mini/miniOGL.h>

#include "viewerbase.h"

// default constructor
viewerbase::viewerbase()
   {
   // configurable parameters:

   PARAMS.winwidth=1024; // window width
   PARAMS.winheight=512; // window height

   PARAMS.fps=25.0f;     // frames per second (target frame rate)

   PARAMS.fovy=60.0f;    // field of view (degrees)
   PARAMS.nearp=10.0f;   // near plane (meters)
   PARAMS.farp=10000.0f; // far plane (meters)

   // feature switches:

   PARAMS.usewireframe=FALSE;

   // image conversion parameters:

   convbase::setparameters(&PARAMS.conversion_params);

   // initialize state:

   EARTH=new miniearth();

   START=gettime();
   TIMER=0.0;

   THREADBASE=new threadbase();
   CURLBASE=new curlbase();

   inithooks();
   }

// destructor
viewerbase::~viewerbase()
   {
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
   miniearth::MINIEARTH_PARAMS eparams;

   // set new state
   PARAMS=params;

   // get the actual earth state
   EARTH->get(eparams);

   // update the terrain state:

   eparams.fps=PARAMS.fps;

   eparams.fovy=PARAMS.fovy;
   eparams.nearp=PARAMS.nearp;
   eparams.farp=PARAMS.farp;

   // finally pass the updated earth state
   EARTH->set(eparams);
   }

// propagate parameters
void viewerbase::propagate()
   {set(PARAMS);}

// initialize the terrain hooks
void viewerbase::inithooks()
   {
   // register callbacks
   EARTH->getterrain()->setcallbacks(THREADBASE,
                                     threadbase::threadinit,threadbase::threadexit,
                                     threadbase::startthread,threadbase::jointhread,
                                     threadbase::lock_cs,threadbase::unlock_cs,
                                     threadbase::lock_io,threadbase::unlock_io,
                                     CURLBASE,
                                     curlbase::curlinit,curlbase::curlexit,
                                     curlbase::getURL,curlbase::checkURL);

   // register libMini conversion hook (JPEG/PNG/Z)
   convbase::setconversion(&PARAMS.conversion_params);

   // register auto-compression hook
   databuf::setautocompress(squishbase::autocompress,NULL);

   // turn on auto-mipmapping and auto-compression
   EARTH->getterrain()->get()->automipmap=TRUE;
   EARTH->getterrain()->get()->autocompress=TRUE;
   propagate();
   }

// get initial view point
minicoord viewerbase::getinitial()
   {return(EARTH->getterrain()->getinitial());}

// set initial eye point
void viewerbase::initeyepoint(const minicoord &e)
   {EARTH->getterrain()->initeyepoint(e);}

// clear scene
void viewerbase::clear()
   {EARTH->clear();}

// enable a specific focus point
void viewerbase::enablefocus(const minicoord &f)
   {EARTH->getterrain()->enablefocus(f);}

// disable the focus point
void viewerbase::disablefocus()
   {EARTH->getterrain()->disablefocus();}

// generate and cache scene for a particular eye point
void viewerbase::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect)
   {EARTH->cache(e,d,u,aspect,gettime());}

// render cached scene
void viewerbase::render()
   {
   // enable wireframe mode
   if (PARAMS.usewireframe) polygonmode(1);

   // render earth
   EARTH->render();

   // disable wireframe mode
   if (PARAMS.usewireframe) polygonmode(0);
   }

// get time
double viewerbase::gettime()
   {return(gettime()-START);}

// start timer
void viewerbase::starttimer()
   {TIMER=gettime()-START;}

// read out timer
double viewerbase::gettimer()
   {return(gettime()-START-TIMER);}

// idle for the remainder of the frame
void viewerbase::idle(double dt)
   {waitfor(1.0/PARAMS.fps-dt);}

// adapt quality parameters
void viewerbase::adapt(double dt)
   {
   miniterrain::MINITERRAIN_PARAMS tparams;

   getearth()->getterrain()->get(tparams);
   tparams.range=miniload::calcrange(tparams.refres/tparams.scale,PARAMS.winheight,PARAMS.fovy);
   getearth()->getterrain()->set(tparams);
   }

// shoot a ray at the scene
double viewerbase::shoot(const minicoord &o,const miniv3d &d)
   {return(EARTH->shoot(o,d));}

// extract triangles that [possibly] intersect a plane
minidyna<miniv3d> viewerbase::extract(const minicoord &p,const miniv3d &v,double radius)
   {return(EARTH->extract(p,v,radius));}

// set locking callbacks
void viewerbase::setraycallbacks(void (*lock)(void *data),void *data,
                                 void (*unlock)(void *data))
   {miniray::setcallbacks(lock,data,unlock);}

#endif
