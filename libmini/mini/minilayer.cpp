// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minimath.h"
#include "minitime.h"

#include "database.h"
#include "datacloud.h"
#include "datacache.h"

#include "miniOGL.h"

#include "miniearth.h"

#include "geoid.h"

#include "minilayer.h"

// default constructor
minilayer::minilayer(miniearth *earth,
                     minicache *cache)
   {
   // auto-determined parameters upon load:

   LPARAMS.cols=0;                // number of columns per tileset
   LPARAMS.rows=0;                // number of rows per tileset

   LPARAMS.stretch=0.0f;          // horizontal coordinate stretching
   LPARAMS.stretchx=0.0f;         // horizontal stretching in x-direction
   LPARAMS.stretchy=0.0f;         // horizontal stretching in y-direction

   LPARAMS.basesize=0;            // base size of texture maps

   LPARAMS.usepnm=FALSE;          // use either PNM or DB loader

   LPARAMS.extent[0]=0.0f;        // x-extent of tileset
   LPARAMS.extent[1]=0.0f;        // y-extent of tileset
   LPARAMS.extent[2]=0.0f;        // z-extent of tileset

   LPARAMS.offset[0]=0.0f;        // x-offset of tileset center
   LPARAMS.offset[1]=0.0f;        // y-offset of tileset center
   LPARAMS.offset[2]=0.0f;        // z-offset of tileset center

   LPARAMS.scaling[0]=0.0f;       // x-scaling factor of tileset
   LPARAMS.scaling[1]=0.0f;       // y-scaling factor of tileset
   LPARAMS.scaling[2]=0.0f;       // z-scaling factor of tileset

   LPARAMS.offsetDAT=minicoord(); // original tileset offset
   LPARAMS.extentDAT=minicoord(); // original tileset extent

   LPARAMS.centerGEO=minicoord(); // geo-referenced center point
   LPARAMS.northGEO=minicoord();  // geo-referenced north point

   // auto-set parameters during rendering:

   LPARAMS.eye=minicoord(miniv3d(0.0),minicoord::MINICOORD_LINEAR); // eye point
   LPARAMS.dir=miniv3d(0.0,0.0,-1.0); // viewing direction
   LPARAMS.up=miniv3d(0.0,1.0,0.0f); // up vector

   LPARAMS.aspect=1.0f; // aspect ratio

   LPARAMS.time=0.0; // local time

   // configurable parameters:

   LPARAMS.warpmode=WARPMODE_LINEAR;  // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4
   LPARAMS.nonlin=FALSE;              // use non-linear warp

   LPARAMS.vicinity=0.5f;             // projected vicinity of flat warp mode relative to earth radius

   LPARAMS.shift[0]=0.0f;             // manual scene x-shift (lon)
   LPARAMS.shift[1]=0.0f;             // manual scene y-shift (lat)
   LPARAMS.shift[2]=0.0f;             // manual scene z-shift (alt)

   LPARAMS.scale=100.0f;              // scaling of scene
   LPARAMS.exaggeration=1.0f;         // exaggeration of elevations
   LPARAMS.maxelev=15000.0f;          // absolute maximum of expected elevations

   LPARAMS.load=1E-9f;                // initially loaded area relative to far plane
   LPARAMS.preload=1.5f;              // continuously preloaded area relative to far plane

   LPARAMS.minres=9.0f;               // minimum resolution of triangulation
   LPARAMS.fastinit=2;                // fast initialization level
   LPARAMS.avgd2value=0.5f;           // average d2value for fast initialization

   LPARAMS.sead2=0.5f;                // influence of sea level on d2-values
   LPARAMS.seamin=-1.0f;              // lower boundary of sea level influence range
   LPARAMS.seamax=1.0f;               // upper boundary of sea level influence range

   LPARAMS.lazyness=1;                // lazyness of tileset paging
   LPARAMS.update=1.0f;               // update period for tileset paging in seconds
   LPARAMS.expire=60.0f;              // tile expiration time in seconds

   LPARAMS.upload=0.25f;              // tile upload time per frame relative to 1/fps
   LPARAMS.keep=0.25f;                // time to keep tiles in the cache in minutes
   LPARAMS.maxdelay=1.0f;             // time after which tiles are regarded as delayed relative to update time
   LPARAMS.cache=128.0f;              // memory footprint of the cache in mega bytes

   LPARAMS.keepalive=10.0f;           // time for which idling threads are kept alive in seconds
   LPARAMS.timeslice=0.001f;          // time for which idling threads sleep in seconds

   LPARAMS.fps=25.0f;                 // frames per second (target frame rate)
   LPARAMS.spu=0.5f;                  // update period for render buffer in seconds

   LPARAMS.res=1.0E3f;                // global resolution of triangulation
   LPARAMS.relres1=1.0f;              // relative adjustment factor #1 for global resolution
   LPARAMS.relres2=1.0f;              // relative adjustment factor #2 for global resolution

   LPARAMS.fovy=60.0f;                // field of view (degrees)
   LPARAMS.nearp=10.0f;               // near plane (meters)
   LPARAMS.farp=10000.0f;             // far plane (meters)

   LPARAMS.reduction1=2.0f;           // reduction parameter #1 for invisible tiles
   LPARAMS.reduction2=3.0f;           // reduction parameter #2 for invisible tiles

   LPARAMS.cullslope=0.05f;           // slope under which the terrain is culled

   LPARAMS.range=0.001f;              // texture paging range relative to far plane
   LPARAMS.relrange1=1.0f;            // relative adjustment factor #1 for texture paging range
   LPARAMS.relrange2=1.0f;            // relative adjustment factor #2 for texture paging range
   LPARAMS.refres=1.0f;               // reference resolution for texture paging in meters
   LPARAMS.radius=3.0f;               // non-linear kick-in distance relative to texture range
   LPARAMS.dropoff=1.0f;              // non-linear lod dropoff at kick-in distance

   LPARAMS.sealevel=-MAXFLOAT;        // sea-level height in meters (off=-MAXFLOAT)

   LPARAMS.level=0;                   // layer level affects farp (2^level*farp)
   LPARAMS.baselevel=0;               // base layer level

   LPARAMS.genmipmaps=FALSE;          // enable on-the-fly generation of mipmaps
   LPARAMS.automipmap=FALSE;          // auto mip-map raw textures

   LPARAMS.autocompress=FALSE;        // auto-compress raw textures with S3TC
   LPARAMS.lod0uncompressed=FALSE;    // keep LOD0 textures uncompressed

   LPARAMS.useskirts=FALSE;           // enable skirts

   LPARAMS.locthreads=2;              // number of local threads
   LPARAMS.numthreads=10;             // number of net threads

   LPARAMS.proxyname=NULL;            // proxy server name
   LPARAMS.proxyport=NULL;            // proxy server port

   LPARAMS.elevprefix="elev.";        // elev tileset prefix
   LPARAMS.imagprefix="imag.";        // imag tileset prefix
   LPARAMS.tilesetfile="tileset.sav"; // tileset sav file
   LPARAMS.vtbinisuffix=".ini";       // suffix of vtb ini file
   LPARAMS.startupfile="startup.sav"; // startup sav file

#ifndef _WIN32
   LPARAMS.localpath="/var/tmp/";           // local directory
#else
   LPARAMS.localpath="C:\\Windows\\Temp\\"; // local directory for Windows
#endif

   LPARAMS.altpath="data/";             // alternative data path
   LPARAMS.instpath="/usr/share/mini/"; // installation data path

   // optional feature switches:

   LPARAMS.usewaypoints=FALSE;
   LPARAMS.usebricks=FALSE;

   // optional detail textures:

   LPARAMS.detailcenter=minicoord();
   LPARAMS.detailwest=minicoord();
   LPARAMS.detailnorth=minicoord();

   LPARAMS.detailalpha=0.0f;

   LPARAMS.maxdetailsize=2048;

   // optional way-points:

   LPARAMS.waypoints="Waypoints.txt"; // waypoint file
   LPARAMS.extrawps="Waypoints2.txt"; // extra waypoint file

   LPARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   LPARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   LPARAMS.signpostheight=100.0f; // height of signposts in meters
   LPARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   LPARAMS.brick="Cone.db";     // brick file

   LPARAMS.bricksize=100.0f;    // brick size in meters
   LPARAMS.brickradius=1000.0f; // brick display radius in meters

   LPARAMS.brickpasses=4;       // brick render passes
   LPARAMS.brickceiling=3.0f;   // upper boundary for brick color mapping relative to elevation of first waypoint
   LPARAMS.brickscroll=0.5f;    // scroll period of striped bricks in seconds

   // initialize state:

   EARTH=earth;
   CACHE=cache;

   TERRAIN=NULL;
   TILECACHE=NULL;

   WARP=NULL;

   WARPMODE=LPARAMS.warpmode;
   NONLIN=LPARAMS.nonlin;
   SCALE=LPARAMS.scale;

   REFERENCE=NULL;
   MTXREF[0]=MTXREF[1]=MTXREF[2]=miniv4d(0.0);

   DETAILTEXID=0;
   DETAILWIDTH=0;
   DETAILHEIGHT=0;
   DETAILMIPMAPS=0;

   DETAILOWNER=FALSE;

   POINTS=NULL;

   LOADED=FALSE;

   VISIBLE=TRUE;

   THREADDATA=NULL;
   THREADINIT=NULL;
   THREADEXIT=NULL;
   STARTTHREAD=NULL;
   JOINTHREAD=NULL;
   LOCK_CS=NULL;
   UNLOCK_CS=NULL;
   LOCK_IO=NULL;
   UNLOCK_IO=NULL;

   CURLDATA=NULL;
   CURLINIT=NULL;
   CURLEXIT=NULL;
   GETURL=NULL;
   CHECKURL=NULL;

   UPD=1;
   }

// destructor
minilayer::~minilayer()
   {
   if (LOADED)
      {
      if (TERRAIN!=NULL)
         {
         // detach tileset from render cache
         CACHE->detach(TERRAIN->getminitile());

         // save the state of the tile cache
         TILECACHE->save();

         // delete the tile cache
         delete TILECACHE;

         // clean-up pthreads and libcurl
         threadexit(getthreadid());
         curlexit(getthreadid());

         // delete the terrain
         delete TERRAIN;

         // delete detail texture
         if (DETAILOWNER)
            if (DETAILTEXID!=0) deletetexmap(DETAILTEXID);

         // delete the waypoints
         if (POINTS!=NULL) delete POINTS;
         }

      // delete the warp
      delete WARP;
      }
   }

// get parameters
void minilayer::get(MINILAYER_PARAMS &lparams)
   {lparams=LPARAMS;}

// set parameters
void minilayer::set(MINILAYER_PARAMS &lparams)
   {
   float range,texres;

   // set new state
   LPARAMS=lparams;

   // pass parameters that need to be treated explicitly
   if (LOADED)
      {
      if (TILECACHE!=NULL)
         {
         TILECACHE->getcloud()->getterrain()->setpreload(LPARAMS.preload*LPARAMS.farp/LPARAMS.scale,ftrc(fceil(LPARAMS.update*LPARAMS.fps)));
         TILECACHE->getcloud()->getterrain()->setexpire(ftrc(fceil(LPARAMS.expire*LPARAMS.fps)));

         range=LPARAMS.range;

         // adjust texture paging range to reference texture resolution
         if (LPARAMS.refres>0.0f)
            if (LPARAMS.cols>0 && LPARAMS.rows>0 && LPARAMS.basesize>0)
               {
               texres=(LPARAMS.extent[0]/LPARAMS.cols+LPARAMS.extent[1]/LPARAMS.rows)/2.0f/LPARAMS.basesize*LPARAMS.scale;
               range*=texres/LPARAMS.refres;
               }

         TILECACHE->getcloud()->getterrain()->setrange(range*LPARAMS.relrange1*LPARAMS.relrange2*LPARAMS.farp/LPARAMS.scale);
         TILECACHE->getcloud()->getterrain()->setradius(LPARAMS.radius*range*LPARAMS.relrange1*LPARAMS.relrange2*LPARAMS.farp/LPARAMS.scale,LPARAMS.dropoff);

         TILECACHE->getcloud()->getterrain()->setsealevel((LPARAMS.sealevel==-MAXFLOAT)?LPARAMS.sealevel:LPARAMS.sealevel*LPARAMS.exaggeration/LPARAMS.scale);

         TILECACHE->getcloud()->setschedule(LPARAMS.upload/LPARAMS.fps,LPARAMS.keep,LPARAMS.maxdelay*LPARAMS.update);
         TILECACHE->getcloud()->setmaxsize(LPARAMS.cache);

         TILECACHE->getcloud()->configure_keepalive(LPARAMS.keepalive);
         TILECACHE->getcloud()->configure_timeslice(LPARAMS.timeslice);
         }

      if (LPARAMS.warpmode!=WARPMODE ||
          LPARAMS.nonlin!=NONLIN ||
          LPARAMS.scale!=SCALE)
         {
         createwarp(LPARAMS.offsetDAT,LPARAMS.extentDAT,
                    LPARAMS.centerGEO,LPARAMS.northGEO,
                    miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
                    LPARAMS.scale);

         updatecoords();

         WARPMODE=LPARAMS.warpmode;
         NONLIN=LPARAMS.nonlin;
         SCALE=LPARAMS.scale;

         update();
         }
      }
   }

// load requested data
void minilayer::request_callback(const char *file,int istexture,databuf *buf,void *data)
   {
   minilayer *obj=(minilayer *)data;

   if (!obj->LPARAMS.usepnm) buf->loaddata(file);
   else buf->loadPNMdata(file);

   if (!buf->missing())
      {
      if (istexture==0 && (buf->type==databuf::DATABUF_TYPE_RGB || buf->type==databuf::DATABUF_TYPE_RGBA)) ERRORMSG();
      if (istexture==0 && (buf->type==databuf::DATABUF_TYPE_RGB_S3TC || buf->type==databuf::DATABUF_TYPE_RGBA_S3TC)) ERRORMSG();
      if (istexture==0 && (buf->type==databuf::DATABUF_TYPE_RGB_MM || buf->type==databuf::DATABUF_TYPE_RGBA_MM)) ERRORMSG();
      if (istexture==0 && (buf->type==databuf::DATABUF_TYPE_RGB_MM_S3TC || buf->type==databuf::DATABUF_TYPE_RGBA_MM_S3TC)) ERRORMSG();

      if (istexture!=0 && (buf->type==databuf::DATABUF_TYPE_BYTE || buf->type==databuf::DATABUF_TYPE_SHORT || buf->type==databuf::DATABUF_TYPE_FLOAT)) ERRORMSG();
      }
   }

// wrappers for internal callbacks:

int minilayer::getthreadid()
   {
   if (TERRAIN->getminitile()==NULL) return(0);
   else return(TERRAIN->getminitile()->getid()+1);
   }

void minilayer::threadinit(int threads,int id)
   {THREADINIT(threads,id,THREADDATA);}

void minilayer::threadexit(int id)
   {THREADEXIT(id,THREADDATA);}

void minilayer::startthread(void *(*thread)(void *background),backarrayelem *background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->STARTTHREAD(thread,background,obj->getthreadid(),obj->THREADDATA);
   }

void minilayer::jointhread(backarrayelem *background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->JOINTHREAD(background,obj->getthreadid(),obj->THREADDATA);
   }

void minilayer::lock_cs(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->LOCK_CS(obj->getthreadid(),obj->THREADDATA);
   }

void minilayer::unlock_cs(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->UNLOCK_CS(obj->getthreadid(),obj->THREADDATA);
   }

void minilayer::lock_io(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->LOCK_IO(obj->THREADDATA);
   }

void minilayer::unlock_io(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->UNLOCK_IO(obj->THREADDATA);
   }

void minilayer::curlinit(int threads,int id,const char *proxyname,const char *proxyport)
   {CURLINIT(threads,id,proxyname,proxyport,CURLDATA);}

void minilayer::curlexit(int id)
   {CURLEXIT(id,CURLDATA);}

void minilayer::getURL(const char *src_url,const char *src_id,const char *src_file,const char *dst_file,int background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->GETURL(src_url,src_id,src_file,dst_file,background,obj->getthreadid(),obj->CURLDATA);
   }

int minilayer::checkURL(const char *src_url,const char *src_id,const char *src_file,void *data)
   {
   minilayer *obj=(minilayer *)data;
   return(obj->CHECKURL(src_url,src_id,src_file,obj->getthreadid(),obj->CURLDATA));
   }

// set internal callbacks
void minilayer::setcallbacks(void *threaddata,
                             void (*threadinit)(int threads,int id,void *data),
                             void (*threadexit)(int id,void *data),
                             void (*startthread)(void *(*thread)(void *background),backarrayelem *background,int id,void *data),
                             void (*jointhread)(backarrayelem *background,int id,void *data),
                             void (*lock_cs)(int id,void *data),void (*unlock_cs)(int id,void *data),
                             void (*lock_io)(void *data),void (*unlock_io)(void *data),
                             void *curldata,
                             void (*curlinit)(int threads,int id,const char *proxyname,const char *proxyport,void *data),
                             void (*curlexit)(int id,void *data),
                             void (*geturl)(const char *src_url,const char *src_id,const char *src_file,const char *dst_file,int background,int id,void *data),
                             int (*checkurl)(const char *src_url,const char *src_id,const char *src_file,int id,void *data))
   {
   THREADDATA=threaddata;

   THREADINIT=threadinit;
   THREADEXIT=threadexit;

   STARTTHREAD=startthread;
   JOINTHREAD=jointhread;
   LOCK_CS=lock_cs;
   UNLOCK_CS=unlock_cs;
   LOCK_IO=lock_io;
   UNLOCK_IO=unlock_io;

   CURLDATA=curldata;

   CURLINIT=curlinit;
   CURLEXIT=curlexit;

   GETURL=geturl;
   CHECKURL=checkurl;
   }

// load tileset
BOOLINT minilayer::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,BOOLINT reset,
                        int level,int baselevel)
   {
   int success;

   float outparams[5];
   float outscale[3];

   float as2m[2];

   char *elevtilesetfile,*imagtilesetfile;
   char *vtbelevinifile,*vtbimaginifile;

   if (LOADED) return(FALSE);

   // create the terrain
   TERRAIN=new miniload;

   // concatenate tileset info file names
   elevtilesetfile=strdup2(LPARAMS.elevprefix,LPARAMS.tilesetfile);
   imagtilesetfile=strdup2(LPARAMS.imagprefix,LPARAMS.tilesetfile);

   // concatenate vtb ini file names
   vtbelevinifile=strdup2(basepath1,LPARAMS.vtbinisuffix);
   vtbimaginifile=strdup2(basepath2,LPARAMS.vtbinisuffix);

   // attach the tile cache
   TILECACHE=new datacache(TERRAIN);
   TILECACHE->setremoteid(baseid);
   TILECACHE->setremoteurl(baseurl);
   TILECACHE->setlocalpath(LPARAMS.localpath);
   TILECACHE->setelevtilesetfile(elevtilesetfile);
   TILECACHE->setimagtilesetfile(imagtilesetfile);
   TILECACHE->setvtbelevinifile(vtbelevinifile);
   TILECACHE->setvtbimaginifile(vtbimaginifile);
   TILECACHE->setvtbelevpath(basepath1);
   TILECACHE->setvtbimagpath(basepath2);
   TILECACHE->setstartupfile(LPARAMS.startupfile);
   TILECACHE->setloader(minilayer::request_callback,this,1,LPARAMS.preload*LPARAMS.farp/LPARAMS.scale,LPARAMS.range*LPARAMS.relrange1*LPARAMS.relrange2*LPARAMS.farp/LPARAMS.scale,LPARAMS.basesize,LPARAMS.lazyness,ftrc(fceil(LPARAMS.update*LPARAMS.fps)),ftrc(fceil(LPARAMS.expire*LPARAMS.fps)));
   TILECACHE->getcloud()->getterrain()->setradius(LPARAMS.radius*LPARAMS.range*LPARAMS.relrange1*LPARAMS.relrange2*LPARAMS.farp/LPARAMS.scale,LPARAMS.dropoff);
   TILECACHE->getcloud()->getterrain()->setsealevel((LPARAMS.sealevel==-MAXFLOAT)?LPARAMS.sealevel:LPARAMS.sealevel*LPARAMS.exaggeration/LPARAMS.scale);
   TILECACHE->getcloud()->setschedule(LPARAMS.upload/LPARAMS.fps,LPARAMS.keep,LPARAMS.maxdelay*LPARAMS.update);
   TILECACHE->getcloud()->setmaxsize(LPARAMS.cache);
   TILECACHE->getcloud()->setthread(minilayer::startthread,this,minilayer::jointhread,minilayer::lock_cs,minilayer::unlock_cs,minilayer::lock_io,minilayer::unlock_io);
   TILECACHE->getcloud()->configure_automipmap(LPARAMS.automipmap);
   TILECACHE->getcloud()->configure_autocompress(LPARAMS.autocompress);
   TILECACHE->getcloud()->configure_lod0uncompressed(LPARAMS.lod0uncompressed);
   TILECACHE->getcloud()->configure_keepalive(LPARAMS.keepalive);
   TILECACHE->getcloud()->configure_timeslice(LPARAMS.timeslice);
   TILECACHE->configure_locthreads(LPARAMS.locthreads);
   TILECACHE->configure_netthreads(LPARAMS.numthreads);
   TILECACHE->setreceiver(minilayer::getURL,this,minilayer::checkURL);

   // free tileset info file names
   free(elevtilesetfile);
   free(imagtilesetfile);

   // free vtb ini file names
   free(vtbelevinifile);
   free(vtbimaginifile);

   // initialize libcurl
   curlinit(1,0,LPARAMS.proxyname,LPARAMS.proxyport);

   // load persistent startup file
   TILECACHE->load(reset);

   // clean-up libcurl
   curlexit(0);

   // check tileset info
   if (TILECACHE->haselevinfo())
      {
      // set size of tileset
      LPARAMS.cols=TILECACHE->getelevinfo_tilesx();
      LPARAMS.rows=TILECACHE->getelevinfo_tilesy();

      // set local offset of tileset center
      LPARAMS.offset[0]=TILECACHE->getelevinfo_centerx();
      LPARAMS.offset[1]=TILECACHE->getelevinfo_centery();
      LPARAMS.offset[2]=0.0f;

      // set horizontal stretching
      LPARAMS.stretch=1.0f;
      LPARAMS.stretchx=1.0f;
      LPARAMS.stretchy=1.0f;

      // set base size of textures
      if (TILECACHE->hasimaginfo()) LPARAMS.basesize=TILECACHE->getimaginfo_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(LPARAMS.basesize);

      // use PNM loader
      LPARAMS.usepnm=TRUE;

      // update maximum elevation
      LPARAMS.maxelev=fmax(fmin(TILECACHE->getelevinfo_maxelev(),LPARAMS.maxelev)*LPARAMS.exaggeration,1.0f);

      // get original data coordinates
      LPARAMS.offsetDAT=minicoord(miniv3d(TILECACHE->getelevinfo_centerx(),TILECACHE->getelevinfo_centery(),0.0),minicoord::MINICOORD_LLH);
      LPARAMS.extentDAT=minicoord(miniv3d(TILECACHE->getelevinfo_sizex(),TILECACHE->getelevinfo_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_LLH);

      // get geo-referenced coordinates
      LPARAMS.centerGEO=LPARAMS.offsetDAT;
      LPARAMS.northGEO=LPARAMS.offsetDAT+minicoord(miniv3d(0.0,1.0,0.0),minicoord::MINICOORD_LLH);
      }
   // check tileset ini
   else if (TILECACHE->haselevini())
      {
      // set size of tileset
      LPARAMS.cols=TILECACHE->getelevini_tilesx();
      LPARAMS.rows=TILECACHE->getelevini_tilesy();

      // set local offset of tileset center
      LPARAMS.offset[0]=TILECACHE->getelevini_centerx();
      LPARAMS.offset[1]=TILECACHE->getelevini_centery();
      LPARAMS.offset[2]=0.0f;

      // set horizontal stretching
      LPARAMS.stretch=1.0f;
      LPARAMS.stretchx=1.0f;
      LPARAMS.stretchy=1.0f;

      // set base size of textures
      if (TILECACHE->hasimagini()) LPARAMS.basesize=TILECACHE->getimagini_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(LPARAMS.basesize);

      // use DB loader
      LPARAMS.usepnm=FALSE;

      // update maximum elevation
      LPARAMS.maxelev=fmax(fmin(fmax(TILECACHE->getelevini_maxelev(),-TILECACHE->getelevini_minelev()),LPARAMS.maxelev)*LPARAMS.exaggeration,1.0f);

      // get original data coordinates
      LPARAMS.offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_LINEAR);
      LPARAMS.extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_LINEAR);

      // get geo-referenced coordinates
      if (TILECACHE->haselevini_geo())
         {
         LPARAMS.centerGEO=minicoord(miniv3d(3600.0*TILECACHE->getelevini_centerx_llwgs84(),3600.0*TILECACHE->getelevini_centery_llwgs84(),0.0),minicoord::MINICOORD_LLH);
         LPARAMS.northGEO=minicoord(miniv3d(3600.0*TILECACHE->getelevini_northx_llwgs84(),3600.0*TILECACHE->getelevini_northy_llwgs84(),0.0),minicoord::MINICOORD_LLH);

         if (TILECACHE->haselevini_coordsys())
            if (TILECACHE->getelevini_coordsys()==databuf::DATABUF_CRS_LLH)
               {
               if (TILECACHE->getelevini_coordsys_datum()!=databuf::DATABUF_DATUM_WGS84 && // WGS84 support only
                   TILECACHE->getelevini_coordsys_datum()!=databuf::DATABUF_DATUM_NAD83 && // treat NAD83 as WGS84
                   TILECACHE->getelevini_coordsys_datum()!=databuf::DATABUF_DATUM_NONE) ERRORMSG(); // treat NONE as WGS84

               // get original data coordinates as LL
               LPARAMS.offsetDAT=minicoord(miniv3d(3600.0*TILECACHE->getelevini_centerx(),3600.0*TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_LLH);
               LPARAMS.extentDAT=minicoord(miniv3d(3600.0*TILECACHE->getelevini_sizex(),3600.0*TILECACHE->getelevini_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_LLH);

               // adjust horizontal stretching of vtb exported LL data
               minicrs::arcsec2meter(LPARAMS.offsetDAT.vec.y,as2m);
               LPARAMS.stretch=3600.0f;
               LPARAMS.offset[0]*=3600.0f;
               LPARAMS.offset[1]*=3600.0f;
               LPARAMS.stretchx=as2m[0];
               LPARAMS.stretchy=as2m[1];
               }
            else if (TILECACHE->getelevini_coordsys()==databuf::DATABUF_CRS_UTM)
               {
               // get original data coordinates as UTM
               LPARAMS.offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_UTM,TILECACHE->getelevini_coordsys_zone(),(minicoord::MINICOORD_DATUM)TILECACHE->getelevini_coordsys_datum());
               LPARAMS.extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_UTM,TILECACHE->getelevini_coordsys_zone(),(minicoord::MINICOORD_DATUM)TILECACHE->getelevini_coordsys_datum());
               }
            else if (TILECACHE->getelevini_coordsys()==databuf::DATABUF_CRS_MERC)
               {
               // get original data coordinates as Mercator
               LPARAMS.offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_MERC);
               LPARAMS.extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_MERC);
               }
            else if (TILECACHE->getelevini_coordsys()==databuf::DATABUF_CRS_OGH)
               {
               // get original data coordinates as Gnomonic
               LPARAMS.offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_OGH,TILECACHE->getelevini_coordsys_zone(),(minicoord::MINICOORD_DATUM)TILECACHE->getelevini_coordsys_datum());
               LPARAMS.extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*LPARAMS.maxelev),minicoord::MINICOORD_OGH,TILECACHE->getelevini_coordsys_zone(),(minicoord::MINICOORD_DATUM)TILECACHE->getelevini_coordsys_datum());
               }
         }
      else
         {
         LPARAMS.centerGEO=minicoord(miniv3d(0.0,0.0,0.0),minicoord::MINICOORD_LINEAR);
         LPARAMS.northGEO=minicoord(miniv3d(0.0,1.0,0.0),minicoord::MINICOORD_LINEAR);
         }
      }

   // check the size of the tileset to detect load failures
   if (LPARAMS.cols==0 || LPARAMS.rows==0)
      {
      delete TERRAIN;
      delete TILECACHE;
      return(FALSE);
      }

   // use .db file numbering starting with zero for compatibility with vtp
   if (!LPARAMS.usepnm) TERRAIN->configure_usezeronumbering(1);

   // select either PNM or DB loader
   TERRAIN->configure_usepnm(LPARAMS.usepnm);

   // configure on-the-fly generation of mipmaps
   TERRAIN->configure_mipmaps(LPARAMS.genmipmaps);

   // initialize libcurl
   curlinit(1,0,LPARAMS.proxyname,LPARAMS.proxyport);

   // load tiles
   success=TERRAIN->load(LPARAMS.cols,LPARAMS.rows, // number of columns and rows
                         basepath1,basepath2,NULL, // directories for tiles and textures (and no fogmaps)
                         LPARAMS.shift[0]-LPARAMS.offset[0],LPARAMS.shift[1]-LPARAMS.offset[1], // horizontal offset
                         LPARAMS.shift[2]+LPARAMS.offset[2], // vertical offset
                         LPARAMS.stretch,LPARAMS.stretchx,LPARAMS.stretchy, // horizontal stretching
                         LPARAMS.exaggeration,LPARAMS.scale, // vertical exaggeration and global scale
                         0.0f,0.0f, // no fog parameters required
                         0.0f, // choose default minimum resolution
                         0.0f, // disable base offset safety
                         outparams, // geometric parameters
                         outscale); // scaling parameters

   // clean-up libcurl
   curlexit(0);

   // check for load errors
   if (success==0)
      {
      delete TERRAIN;
      delete TILECACHE;
      return(FALSE);
      }

   // set layer level
   LPARAMS.level=level;
   LPARAMS.baselevel=baselevel;

   // set extent of tileset
   LPARAMS.extent[0]=LPARAMS.cols*outparams[0];
   LPARAMS.extent[1]=LPARAMS.rows*outparams[1];
   LPARAMS.extent[2]=2.0f*fmin(outparams[4],LPARAMS.maxelev/LPARAMS.scale);

   // set offset of tileset center
   LPARAMS.offset[0]+=outparams[2];
   LPARAMS.offset[1]+=-outparams[3];
   LPARAMS.offset[2]=0.0f;

   // set scaling factor of tileset
   LPARAMS.scaling[0]=outscale[0];
   LPARAMS.scaling[1]=outscale[1];
   LPARAMS.scaling[2]=outscale[2];

   // create the warp
   createwarp(LPARAMS.offsetDAT,LPARAMS.extentDAT,
              LPARAMS.centerGEO,LPARAMS.northGEO,
              miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
              LPARAMS.scale);

   // update warp objects for each exposed coordinate transformation
   updatecoords();

   // set relative scale to neutralize exaggeration
   TERRAIN->setrelscale(1.0f/LPARAMS.exaggeration);

   // set minimum resolution
   TERRAIN->configure_minres(LPARAMS.minres);

   // enable fast initialization
   TERRAIN->setfastinit(LPARAMS.fastinit,LPARAMS.avgd2value);

   // set sea level influence and range
   TERRAIN->configure_sead2(LPARAMS.sead2);
   TERRAIN->configure_seamin((LPARAMS.seamin==-MAXFLOAT)?LPARAMS.seamin:LPARAMS.seamin/LPARAMS.scale);
   TERRAIN->configure_seamax((LPARAMS.seamax==-MAXFLOAT)?LPARAMS.seamax:LPARAMS.seamax/LPARAMS.scale);

   // enable skirts
   TERRAIN->configure_skirts(LPARAMS.useskirts);

   // attach tileset to render cache
   CACHE->attach(TERRAIN->getminitile());

   // lock io to make pnm loader threadsafe
   TILECACHE->configure_autolockio(LPARAMS.usepnm);

   // initialize pthreads and libcurl
   threadinit(LPARAMS.numthreads,getthreadid());
   curlinit(LPARAMS.numthreads,getthreadid(),LPARAMS.proxyname,LPARAMS.proxyport);

   // success
   return(LOADED=TRUE);
   }

// load optional features
void minilayer::loadopts()
   {
   if (!LOADED || TILECACHE==NULL) return;

   if (LPARAMS.level!=LPARAMS.baselevel) return;

   // load waypoints:

   char *wpname;

   wpname=TILECACHE->getfile(LPARAMS.waypoints,LPARAMS.altpath);

   if (wpname!=NULL)
      {
      POINTS=new minipoint;

      POINTS->setcache(TILECACHE,LPARAMS.altpath);

      if (!LPARAMS.usepnm) POINTS->configure_automap(1);

      POINTS->load(wpname,-LPARAMS.offset[0],-LPARAMS.offset[1],-LPARAMS.offset[2],LPARAMS.scaling[0],LPARAMS.scaling[1],LPARAMS.exaggeration/LPARAMS.scale,TERRAIN->getminitile());
      free(wpname);

      if (POINTS->getfirst()!=NULL)
         POINTS->configure_brickceiling(LPARAMS.brickceiling*POINTS->getfirst()->elev*LPARAMS.scale/LPARAMS.exaggeration);
      }

   // load extra waypoints:

   char *ewname;

   ewname=TILECACHE->getfile(LPARAMS.extrawps,LPARAMS.altpath);

   if (ewname!=NULL)
      {
      if (POINTS!=NULL) POINTS->load(ewname,-LPARAMS.offset[0],-LPARAMS.offset[1],-LPARAMS.offset[2],LPARAMS.scaling[0],LPARAMS.scaling[1],LPARAMS.exaggeration/LPARAMS.scale,TERRAIN->getminitile());
      free(ewname);
      }

   // load brick data:

   char *bname;

   bname=TILECACHE->getfile(LPARAMS.brick,LPARAMS.altpath);
   if (bname==NULL) bname=TILECACHE->getfile(LPARAMS.brick,LPARAMS.instpath);

   if (bname!=NULL)
      {
      if (POINTS!=NULL) POINTS->setbrick(bname);
      free(bname);
      }
   }

// create null reference layer
void minilayer::setnull()
   {
   if (LOADED) return;

   // set original data coordinates
   LPARAMS.offsetDAT=minicoord(miniv3d(0.0,0.0,0.0),minicoord::MINICOORD_ECEF);
   LPARAMS.extentDAT=minicoord(miniv3d(1.0,1.0,1.0),minicoord::MINICOORD_ECEF);

   // set geo-referenced coordinates
   LPARAMS.centerGEO=LPARAMS.offsetDAT;
   LPARAMS.northGEO=minicoord(miniv3d(0.0,0.0,1.0),minicoord::MINICOORD_ECEF);

   // set extent
   LPARAMS.extent[0]=1.0f;
   LPARAMS.extent[1]=1.0f;
   LPARAMS.extent[2]=1.0f;

   // set offset
   LPARAMS.offset[0]=0.0f;
   LPARAMS.offset[1]=0.0f;
   LPARAMS.offset[2]=0.0f;

   // set scaling factor
   LPARAMS.scaling[0]=1.0f;
   LPARAMS.scaling[1]=1.0f;
   LPARAMS.scaling[2]=1.0f;

   // create the warp
   createwarp(LPARAMS.offsetDAT,LPARAMS.extentDAT,
              LPARAMS.centerGEO,LPARAMS.northGEO,
              miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
              1.0);

   // update warp objects for each exposed coordinate transformation
   updatecoords();

   // make invisible
   VISIBLE=FALSE;

   LOADED=TRUE;
   }

// create earth reference layer
void minilayer::setearth()
   {
   if (LOADED) return;

   // set original data coordinates
   LPARAMS.offsetDAT=minicoord(miniv3d(0.0,0.0,0.0),minicoord::MINICOORD_ECEF);
   LPARAMS.extentDAT=minicoord(miniv3d(2*EARTH->getorbradius(),2*EARTH->getorbradius(),2*EARTH->getorbradius()),minicoord::MINICOORD_ECEF);

   // set geo-referenced coordinates
   LPARAMS.centerGEO=LPARAMS.offsetDAT;
   LPARAMS.northGEO=minicoord(miniv3d(0.0,0.0,EARTH->getorbradius()),minicoord::MINICOORD_ECEF);

   // set extent
   LPARAMS.extent[0]=2*EARTH->getorbradius();
   LPARAMS.extent[1]=2*EARTH->getorbradius();
   LPARAMS.extent[2]=2*EARTH->getorbradius();

   // set offset
   LPARAMS.offset[0]=0.0f;
   LPARAMS.offset[1]=0.0f;
   LPARAMS.offset[2]=0.0f;

   // set scaling factor
   LPARAMS.scaling[0]=1.0f;
   LPARAMS.scaling[1]=1.0f;
   LPARAMS.scaling[2]=1.0f;

   // create the warp
   createwarp(LPARAMS.offsetDAT,LPARAMS.extentDAT,
              LPARAMS.centerGEO,LPARAMS.northGEO,
              miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
              1.0);

   // update warp objects for each exposed coordinate transformation
   updatecoords();

   // make invisible
   VISIBLE=FALSE;

   LOADED=TRUE;
   }

// create empty reference layer
void minilayer::setempty(minicoord &center,minicoord &north)
   {
   if (center.type!=north.type) ERRORMSG();

   if (LOADED) return;

   // set original data coordinates
   LPARAMS.offsetDAT=center;
   LPARAMS.extentDAT=minicoord(miniv3d(1.0,1.0,1.0),center.type);

   // set geo-referenced coordinates
   LPARAMS.centerGEO=center;
   LPARAMS.northGEO=north;

   // set extent
   LPARAMS.extent[0]=1.0f;
   LPARAMS.extent[1]=1.0f;
   LPARAMS.extent[2]=1.0f;

   // set offset
   LPARAMS.offset[0]=0.0f;
   LPARAMS.offset[1]=0.0f;
   LPARAMS.offset[2]=0.0f;

   // set scaling factor
   LPARAMS.scaling[0]=1.0f/LPARAMS.scale;
   LPARAMS.scaling[1]=1.0f/LPARAMS.scale;
   LPARAMS.scaling[2]=1.0f/LPARAMS.scale;

   // create the warp
   createwarp(LPARAMS.offsetDAT,LPARAMS.extentDAT,
              LPARAMS.centerGEO,LPARAMS.northGEO,
              miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
              LPARAMS.scale);

   // update warp objects for each exposed coordinate transformation
   updatecoords();

   // make invisible
   VISIBLE=FALSE;

   LOADED=TRUE;
   }

// set reference layer
void minilayer::setreference(minilayer *ref)
   {
   miniv4d mtxREF[3];

   REFERENCE=ref;

   if (WARP!=NULL)
      {
      mtxREF[0]=miniv4d(1.0,0.0,0.0);
      mtxREF[1]=miniv4d(0.0,1.0,0.0);
      mtxREF[2]=miniv4d(0.0,0.0,1.0);

      if (LPARAMS.warpmode==WARPMODE_LINEAR ||
          WARP->getgeo()==minicoord::MINICOORD_LINEAR ||
          LPARAMS.warpmode==WARPMODE_FLAT_REF || LPARAMS.warpmode==WARPMODE_AFFINE_REF)
         if (REFERENCE!=NULL)
            if (REFERENCE->getwarp()!=NULL)
               REFERENCE->getwarp()->get_invaff(mtxREF);

      if (mtxREF[0]!=MTXREF[0] ||
          mtxREF[1]!=MTXREF[1] ||
          mtxREF[2]!=MTXREF[2])
         {
         WARP->def_2reference(mtxREF);

         updatecoords();

         MTXREF[0]=mtxREF[0];
         MTXREF[1]=mtxREF[1];
         MTXREF[2]=mtxREF[2];
         }
      }
   }

// create the warp
void minilayer::createwarp(minicoord offsetDAT,minicoord extentDAT,
                           minicoord centerGEO,minicoord northGEO,
                           miniv3d offsetLOC,miniv3d scalingLOC,
                           double scaleLOC)
   {
   minicoord bboxDAT[2];

   miniv4d mtxAFF[3];

   miniv4d mtxFLT[3];
   miniv4d invFLT[3];

   minicoord center,north;
   miniv3d normal;

   minicoord center0,north0;
   miniv3d normal0;

   double radius,scale;

   // create warp object:

   if (WARP==NULL) WARP=new miniwarp();

   // define tileset coordinates:

   WARP->def_tileset(minicoord::MINICOORD_ECEF);

   // define data coordinates:

   bboxDAT[0]=offsetDAT-extentDAT/2.0;
   bboxDAT[1]=offsetDAT+extentDAT/2.0;

   WARP->def_data(bboxDAT);

   // define geo-graphic coordinates:

   WARP->def_geo(centerGEO,northGEO);

   // define local coordinates:

   WARP->def_2local(-offsetLOC,scalingLOC,scaleLOC);

   // define affine coordinates:

   if (LPARAMS.warpmode==WARPMODE_LINEAR || // linear warp mode
       WARP->getgeo()==minicoord::MINICOORD_LINEAR)
      {
      if (REFERENCE==NULL) scale=1.0;
      else scale=scaleLOC/REFERENCE->getwarp()->getscaleloc();

      mtxAFF[0]=miniv4d(1.0,0.0,0.0,offsetLOC.x*scalingLOC.x*scale);
      mtxAFF[1]=miniv4d(0.0,1.0,0.0,offsetLOC.y*scalingLOC.y*scale);
      mtxAFF[2]=miniv4d(0.0,0.0,1.0,offsetLOC.z*scalingLOC.z*scale);
      }
   else if (LPARAMS.warpmode==WARPMODE_FLAT || LPARAMS.warpmode==WARPMODE_FLAT_REF) // flat warp modes
      {
      mtxAFF[0]=miniv4d(1.0,0.0,0.0);
      mtxAFF[1]=miniv4d(0.0,1.0,0.0);
      mtxAFF[2]=miniv4d(0.0,0.0,1.0);

      if (REFERENCE!=NULL)
         if (REFERENCE->getwarp()->getgeo()!=minicoord::MINICOORD_LINEAR)
            {
            center0=REFERENCE->getwarp()->getcenter();
            center0.convert2(minicoord::MINICOORD_ECEF);

            north0=REFERENCE->getwarp()->getnorth();
            north0.convert2(minicoord::MINICOORD_ECEF);

            normal0=center0.vec;

            radius=center0.vec.getlength();

            if (radius>0.0)
               {
               pointwarp(center0,north0,normal0,1.0,mtxFLT);
               inv_mtx(invFLT,mtxFLT);

               center=WARP->getcenter();
               center.convert2(minicoord::MINICOORD_ECEF);

               north=WARP->getnorth();
               north.convert2(minicoord::MINICOORD_ECEF);

               if (center.vec.getlength()>0.0)
                  {
                  center=minicoord(miniv3d(invFLT[0]*center.vec,invFLT[1]*center.vec,invFLT[2]*center.vec),minicoord::MINICOORD_ECEF);

                  if (center.vec.z>radius*(1.0f-LPARAMS.vicinity)) center.vec.z=radius;
                  else center0.vec*=-1.0;

                  center=minicoord(miniv3d(mtxFLT[0]*center.vec,mtxFLT[1]*center.vec,mtxFLT[2]*center.vec),minicoord::MINICOORD_ECEF);

                  north=minicoord(miniv3d(invFLT[0]*north.vec,invFLT[1]*north.vec,invFLT[2]*north.vec),minicoord::MINICOORD_ECEF);
                  if (north.vec.z>radius*(1.0f-LPARAMS.vicinity)) north.vec.z=radius;
                  north=minicoord(miniv3d(mtxFLT[0]*north.vec,mtxFLT[1]*north.vec,mtxFLT[2]*north.vec),minicoord::MINICOORD_ECEF);

                  if (REFERENCE==NULL) scale=1.0/scaleLOC;
                  else scale=1.0/REFERENCE->getwarp()->getscaleloc();

                  pointwarp(center,north,normal0,scale,mtxAFF);
                  }
               else
                  {
                  center=minicoord(miniv3d(invFLT[0]*center.vec,invFLT[1]*center.vec,invFLT[2]*center.vec),minicoord::MINICOORD_ECEF);
                  center.vec.z=radius;
                  center=minicoord(miniv3d(mtxFLT[0]*center.vec,mtxFLT[1]*center.vec,mtxFLT[2]*center.vec),minicoord::MINICOORD_ECEF);

                  if (REFERENCE==NULL) scale=1.0/scaleLOC;
                  else scale=1.0/REFERENCE->getwarp()->getscaleloc();

                  mtxAFF[0]=miniv4d(1.0,0.0,0.0,center.vec.x*scale);
                  mtxAFF[1]=miniv4d(0.0,1.0,0.0,center.vec.y*scale);
                  mtxAFF[2]=miniv4d(0.0,0.0,1.0,center.vec.z*scale);

                  mlt_mtx(mtxAFF,invFLT,mtxAFF);
                  mtxAFF[2].x=mtxAFF[2].y=mtxAFF[2].z=0.0;
                  mlt_mtx(mtxAFF,mtxFLT,mtxAFF);
                  }
               }
            }
      }
   else if (LPARAMS.warpmode==WARPMODE_AFFINE || LPARAMS.warpmode==WARPMODE_AFFINE_REF) // non-flat warp modes
      {
      center=WARP->getcenter();
      center.convert2(minicoord::MINICOORD_ECEF);

      north=WARP->getnorth();
      north.convert2(minicoord::MINICOORD_ECEF);

      normal=center.vec;

      if (center.vec.getlength()>0.0)
         {
         if (REFERENCE==NULL) scale=1.0/scaleLOC;
         else scale=1.0/REFERENCE->getwarp()->getscaleloc();

         pointwarp(center,north,normal,scale,mtxAFF);
         }
      else
         {
         mtxAFF[0]=miniv4d(1.0,0.0,0.0);
         mtxAFF[1]=miniv4d(0.0,1.0,0.0);
         mtxAFF[2]=miniv4d(0.0,0.0,1.0);
         }
      }

   WARP->def_2affine(mtxAFF);

   // define reference coordinates:

   setreference(REFERENCE);
   }

// construct a point warp matrix
void minilayer::pointwarp(const minicoord &center,const minicoord &north,const miniv3d &normal,
                          double scale,miniv4d mtx[3])
   {
   miniv3d dir,up,right;

   dir=normal;
   dir.normalize();

   up=north.vec-center.vec;
   up.normalize();

   right=up/dir;
   right.normalize();
   up=dir/right;

   mtx[0]=miniv4d(right.x,up.x,dir.x,center.vec.x*scale);
   mtx[1]=miniv4d(right.y,up.y,dir.y,center.vec.y*scale);
   mtx[2]=miniv4d(right.z,up.z,dir.z,center.vec.z*scale);
   }

// update the coordinate transformations
void minilayer::updatecoords()
   {
   // create warp object for each exposed coordinate transformation:

   WARP_G2L=*WARP;
   WARP_G2L.setwarp(miniwarp::MINIWARP_METRIC,miniwarp::MINIWARP_LOCAL);

   WARP_L2G=*WARP;
   WARP_L2G.setwarp(miniwarp::MINIWARP_LOCAL,miniwarp::MINIWARP_METRIC);

   WARP_G2I=*WARP;
   WARP_G2I.setwarp(miniwarp::MINIWARP_METRIC,miniwarp::MINIWARP_INTERNAL);

   WARP_I2G=*WARP;
   WARP_I2G.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_METRIC);

   WARP_G2O=*WARP;
   WARP_G2O.setwarp(miniwarp::MINIWARP_METRIC,miniwarp::MINIWARP_FINAL);

   WARP_O2G=*WARP;
   WARP_O2G.setwarp(miniwarp::MINIWARP_FINAL,miniwarp::MINIWARP_METRIC);

   WARP_G2T=*WARP;
   WARP_G2T.setwarp(miniwarp::MINIWARP_METRIC,miniwarp::MINIWARP_TILESET);

   WARP_T2G=*WARP;
   WARP_T2G.setwarp(miniwarp::MINIWARP_TILESET,miniwarp::MINIWARP_METRIC);

   // compute tileset center and axis:

   CENTER=map_i2g(miniv3d(0.0,0.0,0.0));

   NORMAL=rot_i2g(miniv3d(0.0,1.0,0.0),miniv3d(0.0,0.0,0.0));
   NORTH=rot_i2g(miniv3d(0.0,0.0,-1.0),miniv3d(0.0,0.0,0.0));
   EAST=rot_i2g(miniv3d(1.0,0.0,0.0),miniv3d(0.0,0.0,0.0));

   // copy warp object to encapsulated tileset
   if (TERRAIN!=NULL)
      {
      miniray::lock();

      TERRAIN->getminitile()->copywarp(WARP);
      TERRAIN->getminitile()->getwarp()->setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_FINAL);

      createwarps(LPARAMS.cols,LPARAMS.rows,
                  LPARAMS.offsetDAT,LPARAMS.extentDAT,
                  LPARAMS.scale);

      miniray::unlock();
      }
   }

// create the per-tile warps
void minilayer::createwarps(int cols,int rows,
                            minicoord offsetDAT,minicoord extentDAT,
                            double scaleLOC)
   {
   int i,j,k;

   int mode;

   minicoord fcenter;
   miniv3d fnormal;

   miniwarp twarp;

   double u,v,w;

   minicoord p;
   miniv3d n;

   miniv3d crnr[8];
   miniv3d nrml[8];

   mode=LPARAMS.warpmode;

   if (WARP->gettls()==minicoord::MINICOORD_LINEAR) mode=WARPMODE_LINEAR;

   if (mode==WARPMODE_FLAT || mode==WARPMODE_FLAT_REF)
      {
      if (REFERENCE==NULL) mode=WARPMODE_LINEAR;
      else if (REFERENCE->getwarp()->getgeo()==minicoord::MINICOORD_LINEAR) mode=WARPMODE_LINEAR;
      }

   fcenter=minicoord(miniv3d(0.0),minicoord::MINICOORD_LINEAR);
   fnormal=miniv3d(0.0,0.0,1.0);

   if (mode==WARPMODE_FLAT || mode==WARPMODE_FLAT_REF)
      if (REFERENCE!=NULL)
         {
         fcenter=REFERENCE->getcenter();
         fnormal=REFERENCE->getnormal();
         }

   for (i=0; i<cols; i++)
      for (j=0; j<rows; j++)
         {
         twarp=*WARP;

         for (k=0; k<8; k++)
            {
            switch (k)
               {
               case 0:
                  u=(double)i/cols-0.5;
                  v=(double)j/rows-0.5;
                  w=-0.5;
                  break;
               case 1:
                  u=(double)(i+1)/cols-0.5;
                  v=(double)j/rows-0.5;
                  w=-0.5;
                  break;
               case 2:
                  u=(double)i/cols-0.5;
                  v=(double)(j+1)/rows-0.5;
                  w=-0.5;
                  break;
               case 3:
                  u=(double)(i+1)/cols-0.5;
                  v=(double)(j+1)/rows-0.5;
                  w=-0.5;
                  break;
               case 4:
                  u=(double)i/cols-0.5;
                  v=(double)j/rows-0.5;
                  w=0.5;
                  break;
               case 5:
                  u=(double)(i+1)/cols-0.5;
                  v=(double)j/rows-0.5;
                  w=0.5;
                  break;
               case 6:
                  u=(double)i/cols-0.5;
                  v=(double)(j+1)/rows-0.5;
                  w=0.5;
                  break;
               case 7:
                  u=(double)(i+1)/cols-0.5;
                  v=(double)(j+1)/rows-0.5;
                  w=0.5;
                  break;
               default:
                  u=v=w=0.0;
                  break;
               }

            if (mode==WARPMODE_LINEAR)
               {
               p=offsetDAT;
               p.vec+=miniv4d(u*extentDAT.vec.x,v*extentDAT.vec.y,w*extentDAT.vec.z);

               n=miniv3d(0.0,0.0,1.0);

               crnr[k]=map_t2g(p).vec;
               nrml[k]=rot_t2g(n,p);
               }
            else if (mode==WARPMODE_FLAT || mode==WARPMODE_FLAT_REF)
               {
               p=offsetDAT;
               p.vec+=miniv4d(u*extentDAT.vec.x,v*extentDAT.vec.y,0.0);
               p.convert2(minicoord::MINICOORD_ECEF);

               n=p.vec;
               n.normalize();

               p.vec+=(miniv3d(fcenter.vec-p.vec)*fnormal)*fnormal;
               p.vec+=w*extentDAT.vec.z*fnormal;
               }
            else
               {
               p=offsetDAT;
               p.vec+=miniv4d(u*extentDAT.vec.x,v*extentDAT.vec.y,w*extentDAT.vec.z);
               p.convert2(minicoord::MINICOORD_LLH);
               p.vec.z+=sample_geoid(p.vec.y/(60*60),p.vec.x/(60*60)); // add geoid height
               p.convert2(minicoord::MINICOORD_ECEF);

               n=p.vec;
               n.normalize();
               }

            crnr[k]=map_g2o(p).vec;
            nrml[k]=rot_g2o(n,p);
            }

         twarp.settile(miniv3d(cols,rows,1.0),miniv3d(-i+0.5*(cols-1),-j+0.5*(rows-1),0.0));
         twarp.setcorners(crnr,nrml,extentDAT.vec.z/scaleLOC);

         twarp.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_WARP);

         TERRAIN->getminitile()->copywarp(&twarp,i,rows-1-j);
         }
   }

// non-linear mapper
minicoord minilayer::nonlin_map_g2t(const minicoord &p)
   {
   minicoord p2;

   p2=p;

   if (LPARAMS.nonlin)
      if (LPARAMS.warpmode==WARPMODE_AFFINE || LPARAMS.warpmode==WARPMODE_AFFINE_REF)
         {
         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);

         p2=WARP_G2T.warp(p2);

         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);
         }

   return(p2);
   }

// non-linear mapper
minicoord minilayer::nonlin_map_t2g(const minicoord &p)
   {
   minicoord p2;

   p2=p;

   if (LPARAMS.nonlin)
      if (LPARAMS.warpmode==WARPMODE_AFFINE || LPARAMS.warpmode==WARPMODE_AFFINE_REF)
         {
         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);

         p2=WARP_T2G.warp(p2);

         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);
         }

   return(p2);
   }

// non-linear mapper
miniv3d minilayer::nonlin_rot_g2t(const miniv3d &v,const minicoord &p)
   {
   miniv3d v2;
   minicoord p2;

   v2=v;

   if (LPARAMS.nonlin)
      if (LPARAMS.warpmode==WARPMODE_AFFINE || LPARAMS.warpmode==WARPMODE_AFFINE_REF)
         {
         p2=p;

         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);

         v2=WARP_G2T.invtra(v2,p2);
         }

   return(v2);
   }

// non-linear mapper
miniv3d minilayer::nonlin_rot_t2g(const miniv3d &v,const minicoord &p)
   {
   miniv3d v2;
   minicoord p2;

   v2=v;

   if (LPARAMS.nonlin)
      if (LPARAMS.warpmode==WARPMODE_AFFINE || LPARAMS.warpmode==WARPMODE_AFFINE_REF)
         {
         p2=p;

         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=minicoord::MINICOORD_ECEF;
         p2.convert2(minicoord::MINICOORD_ECEF);

         v2=WARP_T2G.invtra(v2,p2);
         }

   return(v2);
   }

// get columns of tileset
int minilayer::getcols()
   {return(LPARAMS.cols);}

// get rows of tileset
int minilayer::getrows()
   {return(LPARAMS.rows);}

// get extent of tileset
miniv3d minilayer::getextent()
   {return(miniv3d(LPARAMS.extent)*len_i2g(1.0));}

// get center of tileset
minicoord minilayer::getcenter()
   {return(CENTER);}

// get geographic extent of tileset
miniv3d minilayer::getgeoextent()
   {return(LPARAMS.extentDAT.vec);}

// get geographic center of tileset
minicoord minilayer::getgeocenter()
   {return(LPARAMS.offsetDAT);}

// get normal of tileset
miniv3d minilayer::getnormal()
   {return(NORMAL);}

// get north vector of tileset
miniv3d minilayer::getnorth()
   {return(NORTH);}

// get east vector of tileset
miniv3d minilayer::geteast()
   {return(EAST);}

// get the elevation at position (x,y,z)
double minilayer::getheight(const minicoord &p,int approx)
   {
   float elev;
   minicoord pi;

   if (!LOADED || TERRAIN==NULL) return(-MAXFLOAT);

   pi=map_g2i(p);

   elev=TERRAIN->getheight(pi.vec.x,pi.vec.z,approx);
   if (elev==-MAXFLOAT) return(elev);

   return(len_i2g(elev));
   }

// get the normal at position (x,y,z)
miniv3d minilayer::getnormal(const minicoord &p,int approx)
   {
   float nx,ny,nz;
   minicoord pi;

   if (!LOADED || TERRAIN==NULL) return(miniv3d(0.0));

   pi=map_g2i(p);

   TERRAIN->getnormal(pi.vec.x,pi.vec.z,&nx,&ny,&nz,approx);

   return(rot_i2g(miniv3d(nx,ny,nz),pi));
   }

// get initial view point
minicoord minilayer::getinitial()
   {
   if (!LOADED || POINTS==NULL) return(getcenter());

   if (POINTS->getfirst()==NULL) return(getcenter());
   else return(map_l2g(miniv3d(POINTS->getfirst()->x,POINTS->getfirst()->y,POINTS->getfirst()->elev)));
   }

// set initial eye point
void minilayer::initeyepoint(const minicoord &e)
   {
   minicoord ei;

   // save eye point
   LPARAMS.eye=e;

   if (!LOADED || TERRAIN==NULL || !VISIBLE) return;

   ei=map_g2i(e);

   // restrict loaded area
   TERRAIN->restrictroi(ei.vec.x,ei.vec.z,LPARAMS.load*len_g2i(LPARAMS.farp));

   // load smallest LODs
   TERRAIN->updateroi(LPARAMS.res*LPARAMS.relres1*LPARAMS.relres2,
                      ei.vec.x,ei.vec.y+1000*len_g2i(LPARAMS.farp),ei.vec.z,
                      ei.vec.x,ei.vec.z,len_g2i(LPARAMS.farp));

   // unmark loaded area
   TERRAIN->restrictroi(ei.vec.x,ei.vec.z,0.0f);

   // mark scene for complete update
   update();
   }

// define region of interest
void minilayer::defineroi(double radius)
   {
   minicoord ei;

   if (!LOADED || TERRAIN==NULL || !VISIBLE) return;

   ei=map_g2i(LPARAMS.eye);

   // restrict loaded area
   TERRAIN->restrictroi(ei.vec.x,ei.vec.z,len_g2i(radius));
   }

// enable a specific focus point
void minilayer::enablefocus(const minicoord &f)
   {
   minicoord fi;

   if (!LOADED || TERRAIN==NULL) return;

   // transform focus point
   fi=map_g2i(f);

   // enable internal focus point
   TERRAIN->setfocus(1,fi.vec.x,fi.vec.y,fi.vec.z);
   }

// disable the focus point
void minilayer::disablefocus()
   {
   if (!LOADED || TERRAIN==NULL) return;
   TERRAIN->setfocus(0);
   }

// trigger complete render buffer update at next frame
void minilayer::update()
   {UPD=1;}

// generate and cache scene for a particular eye point
void minilayer::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time)
   {
   minicoord ei;
   miniv3d di,ui;

   // save actual eye point, viewing direction, and up vector
   LPARAMS.eye=e;
   LPARAMS.dir=d;
   LPARAMS.up=u;

   // save actual aspect ratio
   LPARAMS.aspect=aspect;

   // also save actual local time
   LPARAMS.time=time;

   if (!LOADED || TERRAIN==NULL || !VISIBLE) return;

   // transform coordinates
   ei=map_g2i(e);
   di=rot_g2i(d,e);
   ui=rot_g2i(u,e);

   // define resolution reduction of invisible tiles
   TERRAIN->setreduction(LPARAMS.reduction1,LPARAMS.reduction2);

   // update vertex arrays
   TERRAIN->draw(LPARAMS.res*LPARAMS.relres1*LPARAMS.relres2,
                 ei.vec.x,ei.vec.y,ei.vec.z,
                 di.x,di.y,di.z,
                 ui.x,ui.y,ui.z,
                 LPARAMS.fovy,aspect,
                 len_g2i(LPARAMS.nearp),len_g2i(LPARAMS.farp),
                 UPD);

   // revert to normal render buffer update
   UPD=ftrc(ffloor(LPARAMS.spu*LPARAMS.fps))+1;

   // update visibility of back-facing tilesets
   CACHE->display(TERRAIN->getminitile(),VISIBLE && !isculled());
   }

// check whether or not the layer is a tileset
BOOLINT minilayer::istileset()
   {return(TERRAIN!=NULL);}

// check whether or not the layer is a sub-tileset
BOOLINT minilayer::issubtileset()
   {return(TERRAIN!=NULL && LPARAMS.level!=LPARAMS.baselevel);}

// determine whether or not the layer is displayed
void minilayer::display(BOOLINT visible)
   {
   VISIBLE=visible;

   if (TERRAIN!=NULL)
      CACHE->display(TERRAIN->getminitile(),VISIBLE);
   }

// check whether or not the layer is displayed
BOOLINT minilayer::isdisplayed()
   {return(VISIBLE);}

// check whether or not the layer is culled
BOOLINT minilayer::isculled()
   {
   double length;
   double extent,height;

   minicoord eye,ctr;
   miniv3d nrm,dir;

   if (!LOADED || TERRAIN==NULL) return(FALSE);

   length=EARTH->getorbradius()*PI/2.0;

   extent=dmax(LPARAMS.extent[0],LPARAMS.extent[1])/2.0;
   height=LPARAMS.extent[2]/2.0;

   eye=map_g2o(LPARAMS.eye);
   ctr=map_g2o(getcenter());

   nrm=rot_g2o(getnormal(),getcenter());
   ctr-=len_g2o(height)*nrm;

   dir=eye.vec-ctr.vec;
   dir.normalize();

   if (dir*nrm<-extent/length-LPARAMS.cullslope) return(TRUE);

   return(FALSE);
   }

// set the layer level
void minilayer::setlevel(int level,int baselevel)
   {
   LPARAMS.level=level;
   LPARAMS.baselevel=baselevel;
   }

// get the layer level
int minilayer::getlevel()
   {
   if (!LOADED || TERRAIN==NULL) return(0);
   return(LPARAMS.level);
   }

// get the base layer level
int minilayer::getbaselevel()
   {
   if (!LOADED || TERRAIN==NULL) return(0);
   return(LPARAMS.baselevel);
   }

// flatten the terrain by a relative scaling factor (in the range [0-1])
void minilayer::flatten(float relscale)
   {
   if (!LOADED || TERRAIN==NULL) return;
   TERRAIN->setrelscale(relscale);
   }

// get the flattening factor
float minilayer::getflattening()
   {
   if (!LOADED || TERRAIN==NULL) return(1.0f);
   else return(TERRAIN->getrelscale());
   }

// get the internal cache id
int minilayer::getcacheid()
   {
   if (!LOADED || TERRAIN==NULL) return(-1);
   else if (TERRAIN->getminitile()==NULL) return(-1);
   return(TERRAIN->getminitile()->getid());
   }

// attach detail texture
void minilayer::attachdetailtex(int texid,int width,int height,int mipmaps,BOOLINT owner,
                                minicoord center,minicoord west,minicoord north,
                                float alpha)
   {
   if (center.type!=minicoord::MINICOORD_LINEAR) center.convert2(minicoord::MINICOORD_ECEF);
   if (west.type!=minicoord::MINICOORD_LINEAR) west.convert2(minicoord::MINICOORD_ECEF);
   if (north.type!=minicoord::MINICOORD_LINEAR) north.convert2(minicoord::MINICOORD_ECEF);

   if (DETAILOWNER)
      if (DETAILTEXID!=0) deletetexmap(DETAILTEXID);

   DETAILTEXID=texid;
   DETAILWIDTH=width;
   DETAILHEIGHT=height;
   DETAILMIPMAPS=mipmaps;

   DETAILOWNER=owner;

   LPARAMS.detailcenter=center;
   LPARAMS.detailwest=west;
   LPARAMS.detailnorth=north;

   LPARAMS.detailalpha=alpha;
   }

// load detail texture (db format)
void minilayer::loaddetailtex(const char *detailname,
                              float alpha)
   {
   char *dtname;

   databuf buf;

   int texid;
   int width,height;
   int mipmaps;

   minicoord sw,nw,ne,se;

   dtname=TILECACHE->getfile(detailname,LPARAMS.altpath);

   if (dtname!=NULL)
      {
      if (buf.loaddata(dtname))
         {
         // resample to next power of 2
         buf.resample2(LPARAMS.maxdetailsize);

         // automatically create mipmaps
         buf.automipmap();

         // convert db data into texture map
         mipmaps=0;
         texid=db2texid(&buf,&width,&height,&mipmaps);
         buf.release();

         // extract corners
         if (buf.crs!=databuf::DATABUF_CRS_LINEAR)
            {
            sw=minicoord(miniv3d(buf.LLWGS84_swx*3600.0,buf.LLWGS84_swy*3600.0,0.0),minicoord::MINICOORD_LLH);
            nw=minicoord(miniv3d(buf.LLWGS84_nwx*3600.0,buf.LLWGS84_nwy*3600.0,0.0),minicoord::MINICOORD_LLH);
            ne=minicoord(miniv3d(buf.LLWGS84_nex*3600.0,buf.LLWGS84_ney*3600.0,0.0),minicoord::MINICOORD_LLH);
            se=minicoord(miniv3d(buf.LLWGS84_sex*3600.0,buf.LLWGS84_sey*3600.0,0.0),minicoord::MINICOORD_LLH);

            sw.convert2(minicoord::MINICOORD_ECEF);
            nw.convert2(minicoord::MINICOORD_ECEF);
            ne.convert2(minicoord::MINICOORD_ECEF);
            se.convert2(minicoord::MINICOORD_ECEF);
            }
         else
            {
            sw=minicoord(miniv3d(buf.swx,buf.swy,0.0),minicoord::MINICOORD_LINEAR);
            nw=minicoord(miniv3d(buf.nwx,buf.nwy,0.0),minicoord::MINICOORD_LINEAR);
            ne=minicoord(miniv3d(buf.nex,buf.ney,0.0),minicoord::MINICOORD_LINEAR);
            se=minicoord(miniv3d(buf.sex,buf.sey,0.0),minicoord::MINICOORD_LINEAR);
            }

         // attach texture at center
         attachdetailtex(texid,width,height,mipmaps,TRUE,
                         0.25*(sw+nw+ne+se),0.5*(se+ne),0.5*(nw+ne),
                         alpha);
         }

      free(dtname);
      }
   }

// get detail texture
void minilayer::getdetailtex(int &texid,int &width,int &height,int &mipmaps,BOOLINT &owner,
                             minicoord &center,minicoord &west,minicoord &north,
                             float &alpha)
   {
   texid=DETAILTEXID;
   width=DETAILWIDTH;
   height=DETAILHEIGHT;
   mipmaps=DETAILMIPMAPS;

   owner=DETAILOWNER;

   center=LPARAMS.detailcenter;
   west=LPARAMS.detailwest;
   north=LPARAMS.detailnorth;

   alpha=LPARAMS.detailalpha;
   }

// get detail texture in planar representation
void minilayer::getdetailtex(int &texid,int &width,int &height,int &mipmaps,
                             miniv4d &planeu,miniv4d &planev,
                             float &alpha)
   {
   minicoord center;
   minicoord west,north;
   minicoord east,south;
   minicoord position,right,left,front,back;
   miniv3d pos,vecu,vecv;

   center=LPARAMS.detailcenter;
   west=LPARAMS.detailwest;
   north=LPARAMS.detailnorth;

   east=center-(west-center);
   south=center-(north-center);

   if (center.type!=minicoord::MINICOORD_LINEAR) center.convert2(minicoord::MINICOORD_ECEF);
   if (west.type!=minicoord::MINICOORD_LINEAR) west.convert2(minicoord::MINICOORD_ECEF);
   if (north.type!=minicoord::MINICOORD_LINEAR) north.convert2(minicoord::MINICOORD_ECEF);
   if (east.type!=minicoord::MINICOORD_LINEAR) east.convert2(minicoord::MINICOORD_ECEF);
   if (south.type!=minicoord::MINICOORD_LINEAR) south.convert2(minicoord::MINICOORD_ECEF);

   position=map_g2o(center);
   right=map_g2o(west);
   front=map_g2o(north);
   left=map_g2o(east);
   back=map_g2o(south);

   pos=position.vec;
   vecu=right.vec-left.vec;
   vecv=front.vec-back.vec;

   vecu/=vecu.getlength2();
   vecv/=-vecv.getlength2();

   texid=DETAILTEXID;
   width=DETAILWIDTH;
   height=DETAILHEIGHT;
   mipmaps=DETAILMIPMAPS;

   planeu=miniv4d(vecu.x,vecu.y,vecu.z,-pos*vecu);
   planev=miniv4d(vecv.x,vecv.y,vecv.z,-pos*vecv);

   alpha=LPARAMS.detailalpha;
   }

// render waypoints
void minilayer::renderpoints()
   {
   minicoord ei;
   miniv3d di;

   miniv4d mtx[3];
   double oglmtx[16];

   minipointopts global;

   if (!LOADED || POINTS==NULL || !VISIBLE) return;

   if (LPARAMS.level!=LPARAMS.baselevel) return;

   if (LPARAMS.usewaypoints)
      {
      ei=map_g2i(LPARAMS.eye);
      di=rot_g2i(LPARAMS.dir,LPARAMS.eye);

      mtxpush();

      TERRAIN->getminitile()->getwarp()->getwarp(mtx);

      oglmtx[0]=mtx[0].x;
      oglmtx[4]=mtx[0].y;
      oglmtx[8]=mtx[0].z;
      oglmtx[12]=mtx[0].w;

      oglmtx[1]=mtx[1].x;
      oglmtx[5]=mtx[1].y;
      oglmtx[9]=mtx[1].z;
      oglmtx[13]=mtx[1].w;

      oglmtx[2]=mtx[2].x;
      oglmtx[6]=mtx[2].y;
      oglmtx[10]=mtx[2].z;
      oglmtx[14]=mtx[2].w;

      oglmtx[3]=0.0;
      oglmtx[7]=0.0;
      oglmtx[11]=0.0;
      oglmtx[15]=1.0;

      mtxmult(oglmtx);

      global.signpostsize=len_g2i(LPARAMS.signpostheight);
      global.signpostheight=len_g2i(LPARAMS.signpostheight);
      global.signpostrange=LPARAMS.signpostrange*len_g2i(LPARAMS.farp);
      global.signpostturn=LPARAMS.signpostturn;
      global.signpostincline=LPARAMS.signpostincline;
      global.signpostalpha=0.5f;

      if (POINTS->getbrick()==NULL) global.brickfile=NULL;
      else global.brickfile=strdup(POINTS->getbrick());

      global.bricksize=len_g2i(LPARAMS.bricksize);
      global.brickradius=len_g2i(LPARAMS.brickradius);
      global.brickalpha=0.5f;
      global.brickceiling=LPARAMS.brickceiling;
      global.bricklods=16;
      global.brickstagger=1.25f;
      global.brickstripes=LPARAMS.brickscroll*LPARAMS.time;

      POINTS->draw(ei.vec.x,ei.vec.y,-ei.vec.z,
                   di.x,di.y,-di.z,
                   len_g2i(LPARAMS.nearp),len_g2i(LPARAMS.farp),LPARAMS.fovy,LPARAMS.aspect,
                   LPARAMS.time,&global,
                   !LPARAMS.usebricks?minipoint::getrndr_signpost():minipoint::getrndr_brick(LPARAMS.brickpasses),
                   LPARAMS.nonlin);

      mtxpop();
      }
   }

// get the nearest waypoint
minipointdata *minilayer::getnearestpoint(int type)
   {
   minicoord ei;

   minipointdata *nearest;

   if (!LOADED || POINTS==NULL || !VISIBLE) return(NULL);

   nearest=NULL;

   if (LPARAMS.usewaypoints)
      {
      ei=map_g2i(LPARAMS.eye);
      nearest=POINTS->getnearest(ei.vec.x,-ei.vec.z,ei.vec.y,type);
      }

   return(nearest);
   }
