// (c) by Stefan Roettger

#include "database.h"
#include "datacloud.h"
#include "datacache.h"

#include "miniOGL.h"

#include "minilayer.h"

// default constructor
minilayer::minilayer(int id,minicache *cache)
   {
   // auto-determined parameters upon load:

   PARAMS.cols=0;           // number of columns per tileset
   PARAMS.rows=0;           // number of rows per tileset

   PARAMS.basesize=0;       // base size of texture maps

   PARAMS.usepnm=FALSE;     // use either PNM or DB loader

   PARAMS.extent[0]=0.0f;   // x-extent of tileset
   PARAMS.extent[1]=0.0f;   // y-extent of tileset
   PARAMS.extent[2]=0.0f;   // z-extent of tileset

   PARAMS.offset[0]=0.0f;   // x-offset of tileset center
   PARAMS.offset[1]=0.0f;   // y-offset of tileset center
   PARAMS.offset[2]=0.0f;   // z-offset of tileset center

   PARAMS.scaling[0]=0.0f;  // x-scaling factor of tileset
   PARAMS.scaling[1]=0.0f;  // y-scaling factor of tileset
   PARAMS.scaling[2]=0.0f;  // z-scaling factor of tileset

   // configurable parameters:

   PARAMS.warpmode=0;             // warp mode: plain=0 affine=1 non-linear=2

   PARAMS.shift[0]=0.0f;          // manual scene x-shift (lon)
   PARAMS.shift[1]=0.0f;          // manual scene y-shift (lat)
   PARAMS.shift[2]=0.0f;          // manual scene z-shift (alt)

   PARAMS.scale=100.0f;           // scaling of scene
   PARAMS.exaggeration=1.0f;      // exaggeration of elevations
   PARAMS.maxelev=15000.0f;       // absolute maximum of expected elevations

   PARAMS.load=0.1f;              // initially loaded area relative to far plane
   PARAMS.preload=1.25f;          // continuously preloaded area relative to far plane

   PARAMS.minres=9.0f;            // minimum resolution of triangulation
   PARAMS.fastinit=2;             // fast initialization level

   PARAMS.lazyness=1;             // lazyness of tileset paging
   PARAMS.update=1.0f;            // update period for tileset paging in seconds
   PARAMS.expire=60.0f;           // tile expiration time in seconds

   PARAMS.upload=0.25f;           // tile upload time per frame relative to 1/fps
   PARAMS.keep=0.25f;             // time to keep tiles in the cache in minutes
   PARAMS.maxdelay=1.0f;          // time after which tiles are regarded as delayed relative to update time
   PARAMS.cache=128.0f;           // memory footprint of the cache in mega bytes

   PARAMS.keepalive=10.0f;        // time for which idling threads are kept alive in seconds
   PARAMS.timeslice=0.001f;       // time for which idling threads sleep in seconds

   PARAMS.fps=25.0f;              // frames per second (target frame rate)
   PARAMS.spu=0.5f;               // update period for render buffer in seconds

   PARAMS.res=1.0E3f;             // global resolution of triangulation

   PARAMS.fovy=60.0f;             // field of view (degrees)
   PARAMS.nearp=10.0f;            // near plane (meters)
   PARAMS.farp=10000.0f;          // far plane (meters)

   PARAMS.reduction1=2.0f;        // reduction parameter #1 for invisible tiles
   PARAMS.reduction2=3.0f;        // reduction parameter #2 for invisible tiles

   PARAMS.range=0.001f;           // texture paging range relative to far plane
   PARAMS.radius=3.0f;            // non-linear kick-in distance relative to texture range
   PARAMS.dropoff=1.0f;           // non-linear lod dropoff at kick-in distance

   PARAMS.sealevel=-MAXFLOAT;     // sea-level height (meters, off=-MAXFLOAT)

   PARAMS.autocompress=FALSE;     // auto-compress raw textures with S3TC
   PARAMS.lod0uncompressed=FALSE; // keep LOD0 textures uncompressed

   PARAMS.locthreads=1;           // number of local threads
   PARAMS.numthreads=10;          // number of net threads

   PARAMS.elevdir=strdup("elev"); // default elev directory
   PARAMS.imagdir=strdup("imag"); // default imag directory

   PARAMS.elevprefix=strdup("elev.");        // elev tileset prefix
   PARAMS.imagprefix=strdup("imag.");        // imag tileset prefix
   PARAMS.tilesetfile=strdup("tileset.sav"); // tileset sav file
   PARAMS.vtbinisuffix=strdup(".ini");       // suffix of vtb ini file
   PARAMS.startupfile=strdup("startup.sav"); // startup sav file

#ifndef _WIN32
   PARAMS.localpath=strdup("/var/tmp/");           // local directory
#else
   PARAMS.localpath=strdup("C:\\Windows\\Temp\\"); // local directory for Windows
#endif

   PARAMS.altpath=strdup("data/"); // alternative data path

   // optional features:

   PARAMS.usewaypoints=FALSE;
   PARAMS.usebricks=FALSE;

   // optional way-points:

   PARAMS.waypoints=strdup("Waypoints.txt"); // waypoint file

   PARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   PARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   PARAMS.signpostheight=100.0f; // height of signposts in meters
   PARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   PARAMS.brick=strdup("Cone.db"); // brick file

   PARAMS.bricksize=100.0f;  // brick size in meters
   PARAMS.brickrad=1000.0f;  // brick viewing radius in meters

   PARAMS.brickpasses=4;     // brick render passes
   PARAMS.brickceiling=3.0f; // upper boundary for brick color mapping relative to first waypoint elevation
   PARAMS.brickscroll=0.5f;  // scroll period of striped bricks in seconds

   // initialize state:

   PARAMS0=PARAMS;

   LOADED=FALSE;

   ID=id;
   CACHE=cache;

   TERRAIN=NULL;
   TILECACHE=NULL;

   POINTS=NULL;

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

   EYE_INT=miniv3d(0.0);
   DIR_INT=miniv3d(0.0);
   }

// destructor
minilayer::~minilayer()
   {
   if (LOADED)
      {
      // delete the tile cache
      delete TILECACHE;

      // delete the terrain
      delete TERRAIN;

      // delete the waypoints
      if (POINTS!=NULL) delete POINTS;

      // clean-up pthreads and libcurl
      threadexit();
      curlexit();
      }

   // delete strings:

   if (PARAMS.elevdir!=NULL) free(PARAMS.elevdir);
   if (PARAMS.imagdir!=NULL) free(PARAMS.imagdir);

   if (PARAMS.elevprefix!=NULL) free(PARAMS.elevprefix);
   if (PARAMS.imagprefix!=NULL) free(PARAMS.imagprefix);
   if (PARAMS.tilesetfile!=NULL) free(PARAMS.tilesetfile);
   if (PARAMS.vtbinisuffix!=NULL) free(PARAMS.vtbinisuffix);
   if (PARAMS.startupfile!=NULL) free(PARAMS.startupfile);

   if (PARAMS.localpath!=NULL) free(PARAMS.localpath);
   if (PARAMS.altpath!=NULL) free(PARAMS.altpath);
   }

// get parameters
void minilayer::get(MINILAYER_PARAMS &params)
   {params=PARAMS;}

// set parameters
void minilayer::set(MINILAYER_PARAMS &params)
   {
   // set new state
   PARAMS=params;

   // delete unused strings:

   if (PARAMS.elevdir!=PARAMS0.elevdir) if (PARAMS0.elevdir!=NULL) free(PARAMS0.elevdir);
   if (PARAMS.imagdir!=PARAMS0.imagdir) if (PARAMS0.imagdir!=NULL) free(PARAMS0.imagdir);

   if (PARAMS.elevprefix!=PARAMS0.elevprefix) if (PARAMS0.elevprefix!=NULL) free(PARAMS0.elevprefix);
   if (PARAMS.imagprefix!=PARAMS0.imagprefix) if (PARAMS0.imagprefix!=NULL) free(PARAMS0.imagprefix);
   if (PARAMS.tilesetfile!=PARAMS0.tilesetfile) if (PARAMS0.tilesetfile!=NULL) free(PARAMS0.tilesetfile);
   if (PARAMS.vtbinisuffix!=PARAMS0.vtbinisuffix) if (PARAMS0.vtbinisuffix!=NULL) free(PARAMS0.vtbinisuffix);
   if (PARAMS.startupfile!=PARAMS0.startupfile) if (PARAMS0.startupfile!=NULL) free(PARAMS0.startupfile);

   if (PARAMS.localpath!=PARAMS0.localpath) if (PARAMS0.localpath!=NULL) free(PARAMS0.localpath);
   if (PARAMS.altpath!=PARAMS0.altpath) if (PARAMS0.altpath!=NULL) free(PARAMS0.altpath);

   // pass parameters that need to be treated explicitly:

   if (LOADED)
      {
      TILECACHE->getcloud()->getterrain()->setpreload(PARAMS.preload*PARAMS.farp/PARAMS.scale,ftrc(fceil(PARAMS.update*PARAMS.fps)));
      TILECACHE->getcloud()->getterrain()->setexpire(ftrc(fceil(PARAMS.expire*PARAMS.fps)));

      TILECACHE->getcloud()->getterrain()->setrange(PARAMS.range*PARAMS.farp/PARAMS.scale);
      TILECACHE->getcloud()->getterrain()->setradius(PARAMS.radius*PARAMS.range*PARAMS.farp/PARAMS.scale,PARAMS.dropoff);

      TILECACHE->getcloud()->getterrain()->setsealevel((PARAMS.sealevel==-MAXFLOAT)?PARAMS.sealevel:PARAMS.sealevel*PARAMS.exaggeration/PARAMS.scale);

      TILECACHE->getcloud()->setschedule(PARAMS.upload/PARAMS.fps,PARAMS.keep,PARAMS.maxdelay*PARAMS.update);
      TILECACHE->getcloud()->setmaxsize(PARAMS.cache);

      TILECACHE->getcloud()->configure_keepalive(PARAMS.keepalive);
      TILECACHE->getcloud()->configure_timeslice(PARAMS.timeslice);
      }

   // overwrite old state
   PARAMS0=PARAMS;
   }

// load requested data
void minilayer::request_callback(char *file,int istexture,databuf *buf,void *data)
   {
   minilayer *obj=(minilayer *)data;

   if (!obj->PARAMS.usepnm) buf->loaddata(file);
   else buf->loadPNMdata(file);
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

// load tileset (short version)
BOOLINT minilayer::load(const char *url,
                        BOOLINT reset)
   {
   char *baseurl;
   char *lastslash,*lastbslash;
   char *baseid;

   BOOLINT success;

   baseurl=strdup(url);

   // remove trailing slash
   if (strlen(baseurl)>1)
      if (baseurl[strlen(baseurl)-1]=='/') baseurl[strlen(baseurl)-1]='\0';

   // remove trailing backslash
   if (strlen(baseurl)>1)
      if (baseurl[strlen(baseurl)-1]=='\\') baseurl[strlen(baseurl)-1]='\0';

   // search for last slash
   lastslash=strrchr(baseurl,'/');

   // search for last backslash
   lastbslash=strrchr(baseurl,'\\');

   // give up if no slash or backslash was found
   if (lastslash==NULL && lastbslash==NULL) success=FALSE;
   else
      {
      // decompose url into baseurl and baseid
      if (lastslash!=NULL)
         {
         baseid=strcct(++lastslash,"/");
         *lastslash='\0';
         }
      else
         {
         baseid=strcct(++lastbslash,"\\");
         *lastbslash='\0';
         }

      // load tileset
      success=load(baseurl,baseid,PARAMS.elevdir,PARAMS.imagdir,reset);

      free(baseid);
      }

   free(baseurl);

   return(success);
   }

// load tileset (long version)
BOOLINT minilayer::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                        BOOLINT reset)
   {
   int success;

   float outparams[5];
   float outscale[2];

   char *elevtilesetfile,*imagtilesetfile;
   char *vtbelevinifile,*vtbimaginifile;

   minicoord offsetDAT,extentDAT;

   if (LOADED) return(FALSE);

   LOADED=TRUE;

   // create the terrain
   TERRAIN=new miniload;

   // concatenate tileset info file names
   elevtilesetfile=strcct(PARAMS.elevprefix,PARAMS.tilesetfile);
   imagtilesetfile=strcct(PARAMS.imagprefix,PARAMS.tilesetfile);

   // concatenate vtb ini file names
   vtbelevinifile=strcct(basepath1,PARAMS.vtbinisuffix);
   vtbimaginifile=strcct(basepath2,PARAMS.vtbinisuffix);

   // attach the tile cache
   TILECACHE=new datacache(TERRAIN);
   TILECACHE->setelevtilesetfile(elevtilesetfile);
   TILECACHE->setimagtilesetfile(imagtilesetfile);
   TILECACHE->setvtbelevinifile(vtbelevinifile);
   TILECACHE->setvtbimaginifile(vtbimaginifile);
   TILECACHE->setvtbelevpath(basepath1);
   TILECACHE->setvtbimagpath(basepath2);
   TILECACHE->setstartupfile(PARAMS.startupfile);
   TILECACHE->setloader(request_callback,this,1,PARAMS.preload*PARAMS.farp/PARAMS.scale,PARAMS.range*PARAMS.farp/PARAMS.scale,PARAMS.basesize,PARAMS.lazyness,ftrc(fceil(PARAMS.update*PARAMS.fps)),ftrc(fceil(PARAMS.expire*PARAMS.fps)));
   TILECACHE->getcloud()->getterrain()->setradius(PARAMS.radius*PARAMS.range*PARAMS.farp/PARAMS.scale,PARAMS.dropoff);
   TILECACHE->getcloud()->getterrain()->setsealevel((PARAMS.sealevel==-MAXFLOAT)?PARAMS.sealevel:PARAMS.sealevel*PARAMS.exaggeration/PARAMS.scale);
   TILECACHE->getcloud()->setschedule(PARAMS.upload/PARAMS.fps,PARAMS.keep,PARAMS.maxdelay*PARAMS.update);
   TILECACHE->getcloud()->setmaxsize(PARAMS.cache);
   TILECACHE->getcloud()->setthread(startthread,this,jointhread,lock_cs,unlock_cs,lock_io,unlock_io);
   TILECACHE->getcloud()->configure_autocompress(PARAMS.autocompress);
   TILECACHE->getcloud()->configure_lod0uncompressed(PARAMS.lod0uncompressed);
   TILECACHE->getcloud()->configure_keepalive(PARAMS.keepalive);
   TILECACHE->getcloud()->configure_timeslice(PARAMS.timeslice);
   TILECACHE->configure_locthreads(PARAMS.locthreads);
   TILECACHE->configure_netthreads(PARAMS.numthreads);
   TILECACHE->setremoteid(baseid);
   TILECACHE->setremoteurl(baseurl);
   TILECACHE->setlocalpath(PARAMS.localpath);
   TILECACHE->setreceiver(getURL,this,checkURL);

   // free tileset info file names
   free(elevtilesetfile);
   free(imagtilesetfile);

   // free vtb ini file names
   free(vtbelevinifile);
   free(vtbimaginifile);

   // initialize pthreads and libcurl
   threadinit(PARAMS.numthreads);
   curlinit(PARAMS.numthreads);

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
      PARAMS.cols=TILECACHE->getelevinfo_tilesx();
      PARAMS.rows=TILECACHE->getelevinfo_tilesy();

      // set local offset of tileset center
      PARAMS.offset[0]=TILECACHE->getelevinfo_centerx();
      PARAMS.offset[1]=TILECACHE->getelevinfo_centery();

      // set base size of textures
      if (TILECACHE->hasimaginfo()) PARAMS.basesize=TILECACHE->getimaginfo_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(PARAMS.basesize);

      // use PNM loader
      PARAMS.usepnm=TRUE;

      // get data coordinates
      offsetDAT=minicoord(miniv3d(TILECACHE->getelevinfo_centerx(),TILECACHE->getelevinfo_centery(),0.0),minicoord::MINICOORD_LLH);
      extentDAT=minicoord(miniv3d(TILECACHE->getelevinfo_sizex(),TILECACHE->getelevinfo_sizey(),2.0*TILECACHE->getelevinfo_maxelev()),minicoord::MINICOORD_LLH);
      }
   // check tileset ini
   else if (TILECACHE->haselevini())
      {
      // set size of tileset
      PARAMS.cols=TILECACHE->getelevini_tilesx();
      PARAMS.rows=TILECACHE->getelevini_tilesy();

      // set local offset of tileset center
      PARAMS.offset[0]=TILECACHE->getelevini_centerx();
      PARAMS.offset[1]=TILECACHE->getelevini_centery();

      // set base size of textures
      if (TILECACHE->hasimagini()) PARAMS.basesize=TILECACHE->getimagini_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(PARAMS.basesize);

      // use DB loader
      PARAMS.usepnm=FALSE;

      // get data coordinates
      offsetDAT=minicoord(miniv3d(TILECACHE->getelevini_centerx(),TILECACHE->getelevini_centery(),0.0),minicoord::MINICOORD_LINEAR);
      extentDAT=minicoord(miniv3d(TILECACHE->getelevini_sizex(),TILECACHE->getelevini_sizey(),2.0*fmax(TILECACHE->getelevini_maxelev(),-TILECACHE->getelevini_minelev())),minicoord::MINICOORD_LINEAR);
      }

   // check the size of the tileset to detect load failures
   if (PARAMS.cols==0 || PARAMS.rows==0) return(FALSE);

   // use .db file numbering starting with zero for compatibility with vtp
   if (!PARAMS.usepnm) TERRAIN->configure_usezeronumbering(1);

   // turn on mip-mapping
   TERRAIN->configure_mipmaps(1);

   // select either PNM or DB loader
   TERRAIN->configure_usepnm(PARAMS.usepnm);

   // load tiles
   success=TERRAIN->load(PARAMS.cols,PARAMS.rows, // number of columns and rows
                         basepath1,basepath2,NULL, // directories for tiles and textures (and no fogmaps)
                         PARAMS.shift[0]-PARAMS.offset[0],PARAMS.shift[1]-PARAMS.offset[1], // horizontal offset
                         PARAMS.exaggeration,PARAMS.scale, // vertical exaggeration and global scale
                         0.0f,0.0f, // no fog parameters required
                         0.0f, // choose default minimum resolution
                         0.0f, // disable base offset safety
                         outparams, // geometric parameters
                         outscale); // scaling parameters

   // check for load errors
   if (success==0) return(FALSE);

   // set extent of tileset
   PARAMS.extent[0]=PARAMS.cols*outparams[0];
   PARAMS.extent[1]=PARAMS.rows*outparams[1];
   PARAMS.extent[2]=2.0f*fmin(outparams[4],PARAMS.maxelev*PARAMS.exaggeration/PARAMS.scale);

   // set offset of tileset center
   PARAMS.offset[0]+=outparams[2];
   PARAMS.offset[1]-=outparams[3];
   PARAMS.offset[2]-=PARAMS.shift[2];

   // set scaling factor of tileset
   PARAMS.scaling[0]=outscale[0];
   PARAMS.scaling[1]=outscale[1];
   PARAMS.scaling[2]=1.0f/PARAMS.scale;

   // define warp
   defwarp(offsetDAT,extentDAT);

   // set minimum resolution
   TERRAIN->configure_minres(PARAMS.minres);

   // enable fast initialization
   TERRAIN->setfastinit(PARAMS.fastinit);

   // define resolution reduction of invisible tiles
   TERRAIN->setreduction(PARAMS.reduction1,PARAMS.reduction2);

   // use tile caching with vertex arrays
   CACHE->setcallbacks(TERRAIN->getminitile(), // the minitile object to be cached
                       PARAMS.cols,PARAMS.rows, // number of tile columns and rows
                       outparams[0],outparams[1], // tile extents
                       outparams[2],PARAMS.shift[2]/PARAMS.scale,-outparams[3]); // origin with negative Z

   // success
   return(TRUE);
   }

// load optional features
void minilayer::loadopts()
   {
   if (!LOADED) return;

   // load waypoints:

   char *wpname=TILECACHE->getfile(PARAMS.waypoints,PARAMS.altpath);

   if (wpname!=NULL)
      {
      POINTS=new minipoint;

      if (!PARAMS.usepnm) POINTS->configure_automap(1);

      POINTS->load(wpname,-PARAMS.offset[1],-PARAMS.offset[0],PARAMS.scaling[0],PARAMS.scaling[1],PARAMS.exaggeration/PARAMS.scale,TERRAIN->getminitile());
      free(wpname);

      POINTS->configure_brickceiling(PARAMS.brickceiling*POINTS->getfirst()->elev*PARAMS.scale/PARAMS.exaggeration);
      POINTS->configure_brickpasses(PARAMS.brickpasses);
      }

   // load brick data:

   char *bname=TILECACHE->getfile(PARAMS.brick,PARAMS.altpath);

   if (bname==NULL) PARAMS.usebricks=FALSE;
   else
      {
      POINTS->setbrick(bname);
      free(bname);
      }
   }

// define warp
void minilayer::defwarp(minicoord offsetDAT,minicoord extentDAT)
   {
   minicoord bboxDAT[2];

   miniv3d offsetLOC,scalingLOC;

   miniv4d mtxAFF[3];

   // define data coordinates:

   bboxDAT[0]=offsetDAT-extentDAT/2.0;
   bboxDAT[1]=offsetDAT+extentDAT/2.0;

   WARP.def_data(bboxDAT);

   // define local coordinates:

   offsetLOC=miniv3d(PARAMS.offset);
   scalingLOC=miniv3d(PARAMS.scaling);

   WARP.def_2local(-offsetLOC,scalingLOC);

   // define affine coordinates:

   if (PARAMS.warpmode==0)
      {
      mtxAFF[0]=miniv3d(1.0,0.0,0.0);
      mtxAFF[1]=miniv3d(0.0,1.0,1.0);
      mtxAFF[2]=miniv3d(0.0,0.0,1.0);
      }

   WARP.def_2affine(mtxAFF);

   // define warp coordinates:

   WARP.def_warp(minicoord::MINICOORD_ECEF);

   // create warp object for each exposed coordinate transformation:

   WARP_E2L=WARP;
   WARP_E2L.setwarp(miniwarp::MINIWARP_EXTERNAL,miniwarp::MINIWARP_LOCAL);

   WARP_L2E=WARP;
   WARP_L2E.setwarp(miniwarp::MINIWARP_LOCAL,miniwarp::MINIWARP_EXTERNAL);

   WARP_L2I=WARP;
   WARP_L2I.setwarp(miniwarp::MINIWARP_LOCAL,miniwarp::MINIWARP_INTERNAL);

   WARP_I2L=WARP;
   WARP_I2L.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_LOCAL);

   WARP_E2I=WARP;
   WARP_E2I.setwarp(miniwarp::MINIWARP_EXTERNAL,miniwarp::MINIWARP_INTERNAL);

   WARP_I2E=WARP;
   WARP_I2E.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_EXTERNAL);
   }

// get extent of tileset
miniv3d minilayer::getextent()
   {return(miniv3d(PARAMS.extent));}

// get center of tileset
miniv3d minilayer::getcenter()
   {return(map_l2e(miniv3d(0.0,0.0,0.0)));}

// get the elevation at position (x,y,z)
double minilayer::getheight(const miniv3d &p)
   {
   miniv3d pe,pi;

   if (!LOADED) return(-MAXFLOAT);

   pi=map_e2i(p);

   pi.y=TERRAIN->getheight(pi.x,pi.z);
   if (pi.y==-MAXFLOAT) return(pi.y);

   pe=map_i2e(pi);

   return(pe.z);
   }

// get initial view point
miniv3d minilayer::getinitial()
   {
   if (POINTS==NULL) return(getcenter());

   if (POINTS->getfirst()==NULL) return(getcenter());
   else return(map_l2e(miniv3d(POINTS->getfirst()->x,POINTS->getfirst()->y,POINTS->getfirst()->elev)));
   }

// set initial eye point
void minilayer::initeyepoint(const miniv3d &e)
   {
   miniv3d ei;

   if (!LOADED) return;

   ei=map_e2i(e);

   TERRAIN->restrictroi(ei.x,ei.z,len_e2i(PARAMS.load*PARAMS.farp));

   TERRAIN->updateroi(PARAMS.res,
                      ei.x,ei.y+1000*len_e2i(PARAMS.farp),ei.z,
                      ei.x,ei.z,len_e2i(PARAMS.farp));

   update();
   }

// trigger complete render buffer update at next frame
void minilayer::update()
   {UPD=1;}

// generate and cache scene for a particular eye point
void minilayer::cache(const miniv3d &e,const miniv3d &d,const miniv3d &u,float aspect)
   {
   miniv3d ei,di,ui;

   if (!LOADED) return;

   // transform coordinates
   ei=map_e2i(e);
   di=rot_e2i(d);
   ui=rot_e2i(u);

   // update vertex arrays
   TERRAIN->draw(PARAMS.res,
                 ei.x,ei.y,ei.z,
                 di.x,di.y,di.z,
                 ui.x,ui.y,ui.z,
                 PARAMS.fovy,aspect,
                 len_e2i(PARAMS.nearp),len_e2i(PARAMS.farp),
                 UPD);

   // revert to normal render buffer update
   UPD=ftrc(ffloor(PARAMS.spu*PARAMS.fps))+1;

   // save actual internal eye point and viewing direction
   EYE_INT=ei;
   DIR_INT=di;
   }

// flatten the scene by a relative scaling factor (in the range [0-1])
void minilayer::flatten(float relscale)
   {TERRAIN->setrelscale(relscale);}

// map point from external to local coordinates
miniv3d minilayer::map_e2l(const miniv3d &p)
   {
   minicoord c_ext=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_loc=WARP_E2L.warp(c_ext);
   return(c_loc.vec);
   }

// map point from local to external coordinates
miniv3d minilayer::map_l2e(const miniv3d &p)
   {
   minicoord c_loc=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_ext=WARP_L2E.warp(c_loc);
   return(c_ext.vec);
   }

// map point from local to internal coordinates
miniv3d minilayer::map_l2i(const miniv3d &p)
   {
   minicoord c_loc=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_int=WARP_L2I.warp(c_loc);
   return(c_int.vec);
   }

// map point from internal to local coordinates
miniv3d minilayer::map_i2l(const miniv3d &p)
   {
   minicoord c_int=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_loc=WARP_I2L.warp(c_int);
   return(c_loc.vec);
   }

// map point from external to internal coordinates
miniv3d minilayer::map_e2i(const miniv3d &p)
   {
   minicoord c_ext=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_int=WARP_E2I.warp(c_ext);
   return(c_int.vec);
   }

// map point from internal to external coordinates
miniv3d minilayer::map_i2e(const miniv3d &p)
   {
   minicoord c_int=minicoord(p,minicoord::MINICOORD_LINEAR);
   minicoord c_ext=WARP_I2E.warp(c_int);
   return(c_ext.vec);
   }

// rotate vector from external to local coordinates
miniv3d minilayer::rot_e2l(const miniv3d &v)
   {return(WARP_E2L.invtra(v));}

// rotate vector from local to external coordinates
miniv3d minilayer::rot_l2e(const miniv3d &v)
   {return(WARP_L2E.invtra(v));}

// rotate vector from local to internal coordinates
miniv3d minilayer::rot_l2i(const miniv3d &v)
   {return(WARP_L2I.invtra(v));}

// rotate vector from internal to local coordinates
miniv3d minilayer::rot_i2l(const miniv3d &v)
   {return(WARP_I2L.invtra(v));}

// rotate vector from external to internal coordinates
miniv3d minilayer::rot_e2i(const miniv3d &v)
   {return(WARP_E2I.invtra(v));}

// rotate vector from internal to external coordinates
miniv3d minilayer::rot_i2e(const miniv3d &v)
   {return(WARP_I2E.invtra(v));}

// map length from external to local coordinates
double minilayer::len_e2l(double l)
   {return(l*WARP_E2L.getscale());}

// map length from local to external coordinates
double minilayer::len_l2e(double l)
   {return(l*WARP_L2E.getscale());}

// map length from local to internal coordinates
double minilayer::len_l2i(double l)
   {return(l*WARP_L2I.getscale());}

// map length from internal to local coordinates
double minilayer::len_i2l(double l)
   {return(l*WARP_I2L.getscale());}

// map length from external to internal coordinates
double minilayer::len_e2i(double l)
   {return(l*WARP_E2I.getscale());}

// map length from internal to external coordinates
double minilayer::len_i2e(double l)
   {return(l*WARP_I2E.getscale());}
