// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minitime.h>

#include <mini/database.h>

#include <mini/squishbase.h>

#include <mini/miniOGL.h>

#include "miniscene.h"

// get approximate earth radius
double miniscene::getorbradius()
   {return(EARTH->getorbradius());}

// default constructor
miniscene::miniscene()
   {
   // configurable parameters:

   PARAMS.winwidth=1024; // window width
   PARAMS.winheight=512; // window height

   PARAMS.fps=25.0f;     // frames per second (target frame rate)

   PARAMS.fovy=60.0f;    // field of view (degrees)
   PARAMS.nearp=10.0f;   // near plane (meters)
   PARAMS.farp=10000.0f; // far plane (meters)

   // auto-adaption:

   PARAMS.autoadapt=TRUE;   // auto-adaption switch
   PARAMS.autores=TRUE;     // adapt global resolution
   PARAMS.autorange=TRUE;   // adapt texture paging range
   PARAMS.autotarget=60.0f; // targeted cpu load (percent)
   PARAMS.automemory=32.0f; // targeted texture memory (mb)
   PARAMS.autoseconds=1.0f; // adaption period (seconds)

   // feature switches:

   PARAMS.usewireframe=FALSE;

   // image conversion parameters:

   dataconv::setparameters(&PARAMS.conversion_params);

   // initialize state:

   EARTH=new miniearth();
   TERRAIN=EARTH->getterrain();

   START=gettime();
   TIMER=0.0;

   THREADBASE=new threadbase();
   CURLBASE=new curlbase();

   inithooks();
   }

// destructor
miniscene::~miniscene()
   {
   delete EARTH;

   delete THREADBASE;
   delete CURLBASE;
   }

// get parameters
void miniscene::get(MINISCENE_PARAMS &params)
   {params=PARAMS;}

// set parameters
void miniscene::set(MINISCENE_PARAMS &params)
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
void miniscene::propagate()
   {set(PARAMS);}

// initialize the terrain hooks
void miniscene::inithooks()
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

   // register libMini conversion hook (JPEG/PNG/Z)
   dataconv::setconversion(&PARAMS.conversion_params);

   // register auto-compression hook
   databuf::setautocompress(squishbase::autocompress,NULL);

   // turn on auto-mipmapping and auto-compression
   TERRAIN->get()->automipmap=TRUE;
   TERRAIN->get()->autocompress=TRUE;
   propagate();
   }

// get initial view point
minicoord miniscene::getinitial()
   {return(TERRAIN->getinitial());}

// set initial eye point
void miniscene::initeyepoint(const minicoord &e)
   {TERRAIN->initeyepoint(e);}

// clear scene
void miniscene::clear()
   {EARTH->clear();}

// enable a specific focus point
void miniscene::enablefocus(const minicoord &f)
   {TERRAIN->enablefocus(f);}

// disable the focus point
void miniscene::disablefocus()
   {TERRAIN->disablefocus();}

// generate and cache scene for a particular eye point
void miniscene::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect)
   {EARTH->cache(e,d,u,aspect,time());}

// render cached scene
void miniscene::render()
   {
   miniv4d mtx[3];
   double oglmtx[16];

   // enable wireframe mode
   if (PARAMS.usewireframe) polygonmode(1);

   // render earth
   EARTH->render();

   // render ecef geometry
   if (PARAMS.farp>=getorbradius())
      if ((EARTH->get()->warpmode==WARPMODE_AFFINE ||
           EARTH->get()->warpmode==WARPMODE_AFFINE_REF) &&
          EARTH->get()->nonlin)
         {
         EARTH->get_ecef_matrix(mtx);
         mtxget(mtx,oglmtx);
         mtxpushscoped();
         mtxmult(oglmtx);
         render_ecef_geometry(time());
         }

   // disable wireframe mode
   if (PARAMS.usewireframe) polygonmode(0);
   }

// get time
double miniscene::time()
   {return(gettime()-START);}

// start timer
void miniscene::starttimer()
   {TIMER=gettime()-START;}

// read out timer
double miniscene::gettimer()
   {return(gettime()-START-TIMER);}

// idle for the remainder of the frame
void miniscene::idle(double dt)
   {waitfor(1.0/PARAMS.fps-dt);}

// adapt quality parameters
void miniscene::adapt(double dt)
   {
   float load;

   miniterrain::MINITERRAIN_PARAMS tparams;

   TERRAIN->get(tparams);

   tparams.res=miniload::calcres(PARAMS.winheight,PARAMS.fovy);
   tparams.range=miniload::calcrange(tparams.refres,PARAMS.winheight,PARAMS.fovy)/PARAMS.farp;

   if (PARAMS.autoadapt)
      if (!EARTH->isstatic() && !EARTH->isfrozen())
         {
         if (PARAMS.autores)
            {
            load=dt*PARAMS.fps/PARAMS.autotarget*100.0f;

            tparams.relres2*=fpow(load,-0.5f/(PARAMS.fps*PARAMS.autoseconds));

            if (tparams.relres2<1.0f) tparams.relres2=1.0f;
            if (tparams.relres2>1E2f) tparams.relres2=1E2f;
            }

         if (PARAMS.autorange)
            {
            load=TERRAIN->gettexmem()/PARAMS.automemory;

            tparams.relrange2*=fpow(load,-0.5f/(PARAMS.fps*PARAMS.autoseconds));

            if (tparams.relrange2<1.0f) tparams.relrange2=1.0f;
            if (tparams.relrange2>1E2f) tparams.relrange2=1E2f;
            }
         }

   TERRAIN->set(tparams);
   }

// shoot a ray at the scene
double miniscene::shoot(const minicoord &o,const miniv3d &d,double mindist)
   {return(EARTH->shoot(o,d,mindist));}

// shoot a ray at the surface
double miniscene::shoot_surface(const minicoord &o,const miniv3d &d,double mindist)
   {return(EARTH->shoot(o,d,mindist));}

// shoot a ray at the orb
double miniscene::shoot_orb(const minicoord &o,const miniv3d &d,double mindist)
   {return(EARTH->shoot_orb(o,d,mindist));}

// extract triangles that [possibly] intersect a plane
minidyna<miniv3d> miniscene::extract(const minicoord &p,const miniv3d &v,double radius)
   {return(EARTH->extract(p,v,radius));}

// set locking callbacks
void miniscene::setraycallbacks(void (*lock)(void *data),void *data,
                                void (*unlock)(void *data))
   {miniray::setcallbacks(lock,data,unlock);}

// check ecef geometry
void miniscene::check_ecef_geometry(miniv3d &center,double &radius)
   {
   // specify empty bounding sphere with radius zero
   center=miniv3d(0,0,0);
   radius=0.0;

   // overwrite in derived class and return bounding sphere of
   // additional geometry defined in ecef coordinates here:
   // ...

   // for example a bounding sphere
   // with a size of 110% of the diameter of the earth:
   /*
   center=miniv3d(0,0,0);
   radius=1.1*getorbradius();
   */
   }

// render ecef geometry
void miniscene::render_ecef_geometry(double)
   {
   // overwrite in derived class and add opengl calls
   // to render additional geometry defined in ecef coordinates here:
   // ...

   // for example a line along the ecef z-axis
   // with a size of 110% of the diameter of the earth:
   /*
   initstate();
   color(miniv3d(0.5,0.5,0.5));
   renderline(miniv3d(0.0,0.0,-1.1*getorbradius()),
              miniv3d(0.0,0.0,1.1*getorbradius()));
   exitstate();
   */
   }
