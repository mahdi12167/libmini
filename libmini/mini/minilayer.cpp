// (c) by Stefan Roettger

#include "minitime.h"

#include "database.h"
#include "datacloud.h"
#include "datacache.h"

#include "miniOGL.h"

#include "minilayer.h"

// default constructor
minilayer::minilayer(minicache *cache)
   {
   // auto-determined parameters upon load:

   LPARAMS.cols=0;           // number of columns per tileset
   LPARAMS.rows=0;           // number of rows per tileset

   LPARAMS.basesize=0;       // base size of texture maps

   LPARAMS.usepnm=FALSE;     // use either PNM or DB loader

   LPARAMS.extent[0]=0.0f;   // x-extent of tileset
   LPARAMS.extent[1]=0.0f;   // y-extent of tileset
   LPARAMS.extent[2]=0.0f;   // z-extent of tileset

   LPARAMS.offset[0]=0.0f;   // x-offset of tileset center
   LPARAMS.offset[1]=0.0f;   // y-offset of tileset center
   LPARAMS.offset[2]=0.0f;   // z-offset of tileset center

   LPARAMS.scaling[0]=0.0f;  // x-scaling factor of tileset
   LPARAMS.scaling[1]=0.0f;  // y-scaling factor of tileset
   LPARAMS.scaling[2]=0.0f;  // z-scaling factor of tileset

   // auto-set parameters during rendering:

   LPARAMS.eye=minicoord(miniv3d(0.0),minicoord::MINICOORD_LINEAR); // eye point
   LPARAMS.dir=miniv3d(0.0,0.0,-1.0); // viewing direction
   LPARAMS.up=miniv3d(0.0,1.0,0.0f); // up vector

   LPARAMS.aspect=1.0f; // aspect ratio

   LPARAMS.time=0.0; // local time

   // configurable parameters:

   LPARAMS.warpmode=0;             // warp mode: plain=0 affine=1 non-linear=2

   LPARAMS.shift[0]=0.0f;          // manual scene x-shift (lon)
   LPARAMS.shift[1]=0.0f;          // manual scene y-shift (lat)
   LPARAMS.shift[2]=0.0f;          // manual scene z-shift (alt)

   LPARAMS.scale=100.0f;           // scaling of scene
   LPARAMS.exaggeration=1.0f;      // exaggeration of elevations
   LPARAMS.maxelev=15000.0f;       // absolute maximum of expected elevations

   LPARAMS.load=0.1f;              // initially loaded area relative to far plane
   LPARAMS.preload=1.25f;          // continuously preloaded area relative to far plane

   LPARAMS.minres=9.0f;            // minimum resolution of triangulation
   LPARAMS.fastinit=2;             // fast initialization level
   LPARAMS.avgd2value=0.1f;        // average d2value for fast initialization

   LPARAMS.lazyness=1;             // lazyness of tileset paging
   LPARAMS.update=1.0f;            // update period for tileset paging in seconds
   LPARAMS.expire=60.0f;           // tile expiration time in seconds

   LPARAMS.upload=0.25f;           // tile upload time per frame relative to 1/fps
   LPARAMS.keep=0.25f;             // time to keep tiles in the cache in minutes
   LPARAMS.maxdelay=1.0f;          // time after which tiles are regarded as delayed relative to update time
   LPARAMS.cache=128.0f;           // memory footprint of the cache in mega bytes

   LPARAMS.keepalive=10.0f;        // time for which idling threads are kept alive in seconds
   LPARAMS.timeslice=0.001f;       // time for which idling threads sleep in seconds

   LPARAMS.fps=25.0f;              // frames per second (target frame rate)
   LPARAMS.spu=0.5f;               // update period for render buffer in seconds

   LPARAMS.res=1.0E3f;             // global resolution of triangulation

   LPARAMS.fovy=60.0f;             // field of view (degrees)
   LPARAMS.nearp=10.0f;            // near plane (meters)
   LPARAMS.farp=10000.0f;          // far plane (meters)

   LPARAMS.reduction1=2.0f;        // reduction parameter #1 for invisible tiles
   LPARAMS.reduction2=3.0f;        // reduction parameter #2 for invisible tiles

   LPARAMS.range=0.001f;           // texture paging range relative to far plane
   LPARAMS.radius=3.0f;            // non-linear kick-in distance relative to texture range
   LPARAMS.dropoff=1.0f;           // non-linear lod dropoff at kick-in distance

   LPARAMS.sealevel=-MAXFLOAT;     // sea-level height (meters, off=-MAXFLOAT)

   LPARAMS.autocompress=FALSE;     // auto-compress raw textures with S3TC
   LPARAMS.lod0uncompressed=FALSE; // keep LOD0 textures uncompressed

   LPARAMS.locthreads=1;           // number of local threads
   LPARAMS.numthreads=10;          // number of net threads

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

   LPARAMS.altpath="data/"; // alternative data path

   // optional feature switches:

   LPARAMS.usewaypoints=FALSE;
   LPARAMS.usebricks=FALSE;

   // optional way-points:

   LPARAMS.waypoints="Waypoints.txt"; // waypoint file

   LPARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   LPARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   LPARAMS.signpostheight=100.0f; // height of signposts in meters
   LPARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   LPARAMS.brick="Cone.db"; // brick file

   LPARAMS.bricksize=100.0f;  // brick size in meters
   LPARAMS.brickrad=1000.0f;  // brick viewing radius in meters

   LPARAMS.brickpasses=4;     // brick render passes
   LPARAMS.brickceiling=3.0f; // upper boundary for brick color mapping relative to first waypoint elevation
   LPARAMS.brickscroll=0.5f;  // scroll period of striped bricks in seconds

   // initialize state:

   CACHE=cache;

   TERRAIN=NULL;
   TILECACHE=NULL;

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
      // detach tileset from render cache
      CACHE->detach(TERRAIN->getminitile());

      // delete the tile cache
      delete TILECACHE;

      // delete the terrain
      delete TERRAIN;

      // delete the warp
      delete WARP;

      // delete the waypoints
      if (POINTS!=NULL) delete POINTS;

      // clean-up pthreads and libcurl
      threadexit();
      curlexit();
      }
   }

// get parameters
void minilayer::get(MINILAYER_PARAMS &lparams)
   {lparams=LPARAMS;}

// set parameters
void minilayer::set(MINILAYER_PARAMS &lparams)
   {
   // set new state
   LPARAMS=lparams;

   // pass parameters that need to be treated explicitly
   if (LOADED)
      {
      TILECACHE->getcloud()->getterrain()->setpreload(LPARAMS.preload*LPARAMS.farp/LPARAMS.scale,ftrc(fceil(LPARAMS.update*LPARAMS.fps)));
      TILECACHE->getcloud()->getterrain()->setexpire(ftrc(fceil(LPARAMS.expire*LPARAMS.fps)));

      TILECACHE->getcloud()->getterrain()->setrange(LPARAMS.range*LPARAMS.farp/LPARAMS.scale);
      TILECACHE->getcloud()->getterrain()->setradius(LPARAMS.radius*LPARAMS.range*LPARAMS.farp/LPARAMS.scale,LPARAMS.dropoff);

      TILECACHE->getcloud()->getterrain()->setsealevel((LPARAMS.sealevel==-MAXFLOAT)?LPARAMS.sealevel:LPARAMS.sealevel*LPARAMS.exaggeration/LPARAMS.scale);

      TILECACHE->getcloud()->setschedule(LPARAMS.upload/LPARAMS.fps,LPARAMS.keep,LPARAMS.maxdelay*LPARAMS.update);
      TILECACHE->getcloud()->setmaxsize(LPARAMS.cache);

      TILECACHE->getcloud()->configure_keepalive(LPARAMS.keepalive);
      TILECACHE->getcloud()->configure_timeslice(LPARAMS.timeslice);
      }
   }

// load requested data
void minilayer::request_callback(char *file,int istexture,databuf *buf,void *data)
   {
   minilayer *obj=(minilayer *)data;

   if (!obj->LPARAMS.usepnm) buf->loaddata(file);
   else buf->loadPNMdata(file);

   if (istexture==0 && (buf->type==3 || buf->type==4 || buf->type==5 || buf->type==6)) ERRORMSG();
   if (istexture!=0 && (buf->type==0 || buf->type==1 || buf->type==2)) ERRORMSG();
   }

// wrappers for internal callbacks:

void minilayer::threadinit(int threads) {THREADINIT(threads);}
void minilayer::threadexit() {THREADEXIT();}

void minilayer::startthread(void *(*thread)(void *background),backarrayelem *background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->STARTTHREAD(thread,background,obj->THREADDATA);
   }

void minilayer::jointhread(backarrayelem *background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->JOINTHREAD(background,obj->THREADDATA);
   }

void minilayer::lock_cs(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->LOCK_CS(obj->THREADDATA);
   }

void minilayer::unlock_cs(void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->UNLOCK_CS(obj->THREADDATA);
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

void minilayer::curlinit(int threads,char *proxyname,char *proxyport) {CURLINIT(threads,proxyname,proxyport);}
void minilayer::curlexit() {CURLEXIT();}

void minilayer::getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data)
   {
   minilayer *obj=(minilayer *)data;
   obj->GETURL(src_url,src_id,src_file,dst_file,background,obj->CURLDATA);
   }

int minilayer::checkURL(char *src_url,char *src_id,char *src_file,void *data)
   {
   minilayer *obj=(minilayer *)data;
   return(obj->CHECKURL(src_url,src_id,src_file,obj->CURLDATA));
   }

// set internal callbacks
void minilayer::setcallbacks(void *threaddata,
                             void (*threadinit)(int threads),void (*threadexit)(),
                             void (*startthread)(void *(*thread)(void *background),backarrayelem *background,void *data),
                             void (*jointhread)(backarrayelem *background,void *data),
                             void (*lock_cs)(void *data),void (*unlock_cs)(void *data),
                             void (*lock_io)(void *data),void (*unlock_io)(void *data),
                             void *curldata,
                             void (*curlinit)(int threads,char *proxyname,char *proxyport),void (*curlexit)(),
                             void (*geturl)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data),
                             int (*checkurl)(char *src_url,char *src_id,char *src_file,void *data))
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
BOOLINT minilayer::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                        BOOLINT reset)
   {
   int success;

   float outparams[6];
   float outscale[2];

   char *elevtilesetfile,*imagtilesetfile;
   char *vtbelevinifile,*vtbimaginifile;

   minicoord offsetDAT,extentDAT;

   if (LOADED) return(FALSE);

   LOADED=TRUE;

   // create the terrain
   TERRAIN=new miniload;

   // concatenate tileset info file names
   elevtilesetfile=strcct(LPARAMS.elevprefix,LPARAMS.tilesetfile);
   imagtilesetfile=strcct(LPARAMS.imagprefix,LPARAMS.tilesetfile);

   // concatenate vtb ini file names
   vtbelevinifile=strcct(basepath1,LPARAMS.vtbinisuffix);
   vtbimaginifile=strcct(basepath2,LPARAMS.vtbinisuffix);

   // attach the tile cache
   TILECACHE=new datacache(TERRAIN);
   TILECACHE->setelevtilesetfile(elevtilesetfile);
   TILECACHE->setimagtilesetfile(imagtilesetfile);
   TILECACHE->setvtbelevinifile(vtbelevinifile);
   TILECACHE->setvtbimaginifile(vtbimaginifile);
   TILECACHE->setvtbelevpath(basepath1);
   TILECACHE->setvtbimagpath(basepath2);
   TILECACHE->setstartupfile(LPARAMS.startupfile);
   TILECACHE->setloader(minilayer::request_callback,this,1,LPARAMS.preload*LPARAMS.farp/LPARAMS.scale,LPARAMS.range*LPARAMS.farp/LPARAMS.scale,LPARAMS.basesize,LPARAMS.lazyness,ftrc(fceil(LPARAMS.update*LPARAMS.fps)),ftrc(fceil(LPARAMS.expire*LPARAMS.fps)));
   TILECACHE->getcloud()->getterrain()->setradius(LPARAMS.radius*LPARAMS.range*LPARAMS.farp/LPARAMS.scale,LPARAMS.dropoff);
   TILECACHE->getcloud()->getterrain()->setsealevel((LPARAMS.sealevel==-MAXFLOAT)?LPARAMS.sealevel:LPARAMS.sealevel*LPARAMS.exaggeration/LPARAMS.scale);
   TILECACHE->getcloud()->setschedule(LPARAMS.upload/LPARAMS.fps,LPARAMS.keep,LPARAMS.maxdelay*LPARAMS.update);
   TILECACHE->getcloud()->setmaxsize(LPARAMS.cache);
   TILECACHE->getcloud()->setthread(minilayer::startthread,this,minilayer::jointhread,minilayer::lock_cs,minilayer::unlock_cs,minilayer::lock_io,minilayer::unlock_io);
   TILECACHE->getcloud()->configure_autocompress(LPARAMS.autocompress);
   TILECACHE->getcloud()->configure_lod0uncompressed(LPARAMS.lod0uncompressed);
   TILECACHE->getcloud()->configure_keepalive(LPARAMS.keepalive);
   TILECACHE->getcloud()->configure_timeslice(LPARAMS.timeslice);
   TILECACHE->configure_locthreads(LPARAMS.locthreads);
   TILECACHE->configure_netthreads(LPARAMS.numthreads);
   TILECACHE->setremoteid(baseid);
   TILECACHE->setremoteurl(baseurl);
   TILECACHE->setlocalpath(LPARAMS.localpath);
   TILECACHE->setreceiver(minilayer::getURL,this,minilayer::checkURL);

   // free tileset info file names
   free(elevtilesetfile);
   free(imagtilesetfile);

   // free vtb ini file names
   free(vtbelevinifile);
   free(vtbimaginifile);

   // initialize pthreads and libcurl
   threadinit(LPARAMS.numthreads);
   curlinit(LPARAMS.numthreads);

   // load persistent startup file
   TILECACHE->load();

   // reset startup state
   if (reset)
      {
      TILECACHE->reset();
      TILECACHE->load();
      }

   // check tileset info
   if (TILECACHE->haselevinfo())
      {
      // set size of tileset
      LPARAMS.cols=TILECACHE->getelevinfo_tilesx();
      LPARAMS.rows=TILECACHE->getelevinfo_tilesy();

      // set local offset of tileset center
      LPARAMS.offset[0]=TILECACHE->getelevinfo_centerx();
      LPARAMS.offset[1]=TILECACHE->getelevinfo_centery();

      // set base size of textures
      if (TILECACHE->hasimaginfo()) LPARAMS.basesize=TILECACHE->getimaginfo_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(LPARAMS.basesize);

      // use PNM loader
      LPARAMS.usepnm=TRUE;

      // get data coordinates
      offsetDAT=minicoord(miniv3d(TILECACHE->getelevinfo_centerx(),TILECACHE->getelevinfo_centery(),0.0),minicoord::MINICOORD_LLH);
      extentDAT=minicoord(miniv3d(TILECACHE->getelevinfo_sizex(),TILECACHE->getelevinfo_sizey(),2.0*TILECACHE->getelevinfo_maxelev()),minicoord::MINICOORD_LLH);
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

      // set base size of textures
      if (TILECACHE->hasimagini()) LPARAMS.basesize=TILECACHE->getimagini_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(LPARAMS.basesize);

      // use DB loader
      LPARAMS.usepnm=FALSE;

      // get data coordinates
      offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_LINEAR);
      extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*fmax(TILECACHE->getelevini_maxelev(),-TILECACHE->getelevini_minelev())),minicoord::MINICOORD_LINEAR);
      }

   // check the size of the tileset to detect load failures
   if (LPARAMS.cols==0 || LPARAMS.rows==0) return(FALSE);

   // use .db file numbering starting with zero for compatibility with vtp
   if (!LPARAMS.usepnm) TERRAIN->configure_usezeronumbering(1);

   // turn on mip-mapping
   TERRAIN->configure_mipmaps(1);

   // select either PNM or DB loader
   TERRAIN->configure_usepnm(LPARAMS.usepnm);

   // load tiles
   success=TERRAIN->load(LPARAMS.cols,LPARAMS.rows, // number of columns and rows
                         basepath1,basepath2,NULL, // directories for tiles and textures (and no fogmaps)
                         LPARAMS.shift[0]-LPARAMS.offset[0],LPARAMS.shift[1]-LPARAMS.offset[1], // horizontal offset
                         LPARAMS.shift[2], // vertical offset
                         LPARAMS.exaggeration,LPARAMS.scale, // vertical exaggeration and global scale
                         0.0f,0.0f, // no fog parameters required
                         0.0f, // choose default minimum resolution
                         0.0f, // disable base offset safety
                         outparams, // geometric parameters
                         outscale); // scaling parameters

   // check for load errors
   if (success==0) return(FALSE);

   // set extent of tileset
   LPARAMS.extent[0]=LPARAMS.cols*outparams[0];
   LPARAMS.extent[1]=LPARAMS.rows*outparams[1];
   LPARAMS.extent[2]=2.0f*fmin(outparams[4],LPARAMS.maxelev*LPARAMS.exaggeration/LPARAMS.scale);

   // set offset of tileset center
   LPARAMS.offset[0]+=outparams[2];
   LPARAMS.offset[1]+=-outparams[3];
   LPARAMS.offset[2]+=outparams[5];

   // set scaling factor of tileset
   LPARAMS.scaling[0]=outscale[0];
   LPARAMS.scaling[1]=outscale[1];
   LPARAMS.scaling[2]=1.0f/LPARAMS.scale;

   // create the warp
   createwarp(miniv3d(LPARAMS.offset),miniv3d(LPARAMS.scaling),
              offsetDAT,extentDAT);

   // set minimum resolution
   TERRAIN->configure_minres(LPARAMS.minres);

   // enable fast initialization
   TERRAIN->setfastinit(LPARAMS.fastinit,LPARAMS.avgd2value);

   // define resolution reduction of invisible tiles
   TERRAIN->setreduction(LPARAMS.reduction1,LPARAMS.reduction2);

   // attach tileset to render cache
   CACHE->attach(TERRAIN->getminitile());

   // success
   return(TRUE);
   }

// load optional features
void minilayer::loadopts()
   {
   if (!LOADED) return;

   // load waypoints:

   char *wpname=TILECACHE->getfile(LPARAMS.waypoints,LPARAMS.altpath);

   if (wpname!=NULL)
      {
      POINTS=new minipoint;

      if (!LPARAMS.usepnm) POINTS->configure_automap(1);

      POINTS->load(wpname,-LPARAMS.offset[1],-LPARAMS.offset[0],LPARAMS.scaling[0],LPARAMS.scaling[1],LPARAMS.exaggeration/LPARAMS.scale,TERRAIN->getminitile());
      free(wpname);

      POINTS->configure_brickceiling(LPARAMS.brickceiling*POINTS->getfirst()->elev*LPARAMS.scale/LPARAMS.exaggeration);
      POINTS->configure_brickpasses(LPARAMS.brickpasses);
      }

   // load brick data:

   char *bname=TILECACHE->getfile(LPARAMS.brick,LPARAMS.altpath);

   if (bname==NULL) LPARAMS.usebricks=FALSE;
   else
      {
      if (POINTS!=NULL) POINTS->setbrick(bname);
      free(bname);
      }
   }

// create the warp
void minilayer::createwarp(miniv3d offsetLOC,miniv3d scalingLOC,
                           minicoord offsetDAT,minicoord extentDAT)
   {
   minicoord bboxDAT[2];

   miniv4d mtxAFF[3];

   // create warp object
   WARP=new miniwarp();

   // define data coordinates:

   bboxDAT[0]=offsetDAT-extentDAT/2.0;
   bboxDAT[1]=offsetDAT+extentDAT/2.0;

   WARP->def_data(bboxDAT);

   // define local coordinates:

   WARP->def_2local(-offsetLOC,scalingLOC);

   // define affine coordinates:

   if (LPARAMS.warpmode==0)
      {
      mtxAFF[0]=miniv3d(1.0,0.0,0.0);
      mtxAFF[1]=miniv3d(0.0,1.0,1.0);
      mtxAFF[2]=miniv3d(0.0,0.0,1.0);
      }

   WARP->def_2affine(mtxAFF);

   // define warp coordinates:

   WARP->def_warp(minicoord::MINICOORD_ECEF);

   // create warp object for each exposed coordinate transformation:

   WARP_E2L=*WARP;
   WARP_E2L.setwarp(miniwarp::MINIWARP_GLOBAL,miniwarp::MINIWARP_AFFINE);

   WARP_L2E=*WARP;
   WARP_L2E.setwarp(miniwarp::MINIWARP_AFFINE,miniwarp::MINIWARP_GLOBAL);

   WARP_L2I=*WARP;
   WARP_L2I.setwarp(miniwarp::MINIWARP_AFFINE,miniwarp::MINIWARP_FINAL);

   WARP_I2L=*WARP;
   WARP_I2L.setwarp(miniwarp::MINIWARP_FINAL,miniwarp::MINIWARP_AFFINE);

   WARP_E2I=*WARP;
   WARP_E2I.setwarp(miniwarp::MINIWARP_GLOBAL,miniwarp::MINIWARP_FINAL);

   WARP_I2E=*WARP;
   WARP_I2E.setwarp(miniwarp::MINIWARP_FINAL,miniwarp::MINIWARP_GLOBAL);
   }

// get extent of tileset
miniv3d minilayer::getextent()
   {return(miniv3d(LPARAMS.extent)*len_l2e(1.0));}

// get center of tileset
minicoord minilayer::getcenter()
   {return(map_l2e(miniv3d(0.0,0.0,0.0)));}

// get the elevation at position (x,y,z)
double minilayer::getheight(const minicoord &p)
   {
   float elev;
   minicoord pi;

   if (!LOADED) return(-MAXFLOAT);

   pi=map_e2i(p);

   elev=TERRAIN->getheight(pi.vec.x,pi.vec.z);
   if (elev==-MAXFLOAT) return(elev);

   return(len_i2e(elev));
   }

// get initial view point
minicoord minilayer::getinitial()
   {
   if (POINTS==NULL) return(getcenter());

   if (POINTS->getfirst()==NULL) return(getcenter());
   else return(map_l2e(miniv3d(POINTS->getfirst()->x,POINTS->getfirst()->y,POINTS->getfirst()->elev)));
   }

// set initial eye point
void minilayer::initeyepoint(const minicoord &e)
   {
   minicoord ei;

   if (!LOADED) return;

   ei=map_e2i(e);

   // restrict loaded area
   TERRAIN->restrictroi(ei.vec.x,ei.vec.z,len_e2i(LPARAMS.load*LPARAMS.farp));

   // load smallest LODs
   TERRAIN->updateroi(LPARAMS.res,
                      ei.vec.x,ei.vec.y+1000*len_e2i(LPARAMS.farp),ei.vec.z,
                      ei.vec.x,ei.vec.z,len_e2i(LPARAMS.farp));

   // mark scene for complete update
   update();

   // save eye point
   LPARAMS.eye=e;
   }

// trigger complete render buffer update at next frame
void minilayer::update()
   {UPD=1;}

// generate and cache scene for a particular eye point
void minilayer::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time)
   {
   minicoord ei;
   miniv3d di,ui;

   if (!LOADED || !VISIBLE) return;

   // transform coordinates
   ei=map_e2i(e);
   di=rot_e2i(d,e);
   ui=rot_e2i(u,e);

   // update vertex arrays
   TERRAIN->draw(LPARAMS.res,
                 ei.vec.x,ei.vec.y,ei.vec.z,
                 di.x,di.y,di.z,
                 ui.x,ui.y,ui.z,
                 LPARAMS.fovy,aspect,
                 len_e2i(LPARAMS.nearp),len_e2i(LPARAMS.farp),
                 UPD);

   // revert to normal render buffer update
   UPD=ftrc(ffloor(LPARAMS.spu*LPARAMS.fps))+1;

   // save actual eye point, viewing direction, and up vector
   LPARAMS.eye=e;
   LPARAMS.dir=d;
   LPARAMS.up=u;

   // save actual aspect ratio
   LPARAMS.aspect=aspect;

   // also save actual local time
   LPARAMS.time=time;
   }

// determine whether or not the layer is displayed
void minilayer::display(BOOLINT yes)
   {VISIBLE=yes;}

// flatten the scene by a relative scaling factor (in the range [0-1])
void minilayer::flatten(float relscale)
   {TERRAIN->setrelscale(relscale);}

// render waypoints
void minilayer::renderpoints()
   {
   minicoord ei;

   if (POINTS!=NULL)
      if (LPARAMS.usewaypoints)
         {
         ei=map_e2i(LPARAMS.eye);

         if (!LPARAMS.usebricks)
            POINTS->drawsignposts(ei.vec.x,ei.vec.y,-ei.vec.z,len_e2i(LPARAMS.signpostheight),LPARAMS.signpostrange*len_e2i(LPARAMS.farp),LPARAMS.signpostturn,LPARAMS.signpostincline);
         else
            {
            POINTS->configure_brickstripes(FTRC(LPARAMS.brickscroll*LPARAMS.time));
            POINTS->drawbricks(ei.vec.x,ei.vec.y,-ei.vec.z,len_e2i(LPARAMS.brickrad),len_e2i(LPARAMS.farp),LPARAMS.fovy,LPARAMS.aspect,len_e2i(LPARAMS.bricksize));
            }
         }
   }
