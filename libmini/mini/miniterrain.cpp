// (c) by Stefan Roettger

#include "datacache.h"

#include "minimath.h"

#include "miniOGL.h"
#include "minishader.h"

#include "minicoord.h"
#include "miniwarp.h"

#include "miniterrain.h"

// default constructor
miniterrain::miniterrain()
   {
   // auto-determined parameters:

   TPARAMS.time=0.0; // actual time

   // configurable parameters:

   TPARAMS.warpmode=0;             // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4
   TPARAMS.nonlin=FALSE;           // use non-linear warp

   TPARAMS.fademode=0;             // spherical fade mode: off=0 single=1 double=2
   TPARAMS.fadeout=0.01f;          // fadeout out distance relative to far plane

   TPARAMS.submode=0;              // spherical subduction mode: off=0 on=1
   TPARAMS.subfactor=0.1f;         // spherical subduction factor relative to distance

   TPARAMS.scale=1.0f;             // scaling of scene
   TPARAMS.exaggeration=1.0f;      // exaggeration of elevations
   TPARAMS.maxelev=15000.0f;       // absolute maximum of expected elevations

   TPARAMS.load=1E-9f;             // initially loaded area relative to far plane
   TPARAMS.preload=1.5f;           // continuously preloaded area relative to far plane

   TPARAMS.minres=9.0f;            // minimum resolution of triangulation
   TPARAMS.fastinit=1;             // fast initialization level
   TPARAMS.avgd2value=0.5f;        // average d2value for fast initialization

   TPARAMS.sead2=0.5f;             // influence of sea level on d2-values
   TPARAMS.seamin=-1.0f;           // lower boundary of sea level influence range
   TPARAMS.seamax=1.0f;            // upper boundary of sea level influence range

   TPARAMS.lazyness=1;             // lazyness of tileset paging
   TPARAMS.update=1.0f;            // update period for tileset paging in seconds
   TPARAMS.expire=60.0f;           // tile expiration time in seconds

   TPARAMS.upload=0.25f;           // tile upload time per frame relative to 1/fps
   TPARAMS.keep=0.25f;             // time to keep tiles in the cache in minutes
   TPARAMS.maxdelay=1.0f;          // time after which tiles are regarded as delayed relative to update time
   TPARAMS.cache=128.0f;           // memory footprint of the cache in mega bytes

   TPARAMS.keepalive=10.0f;        // time for which idling threads are kept alive in seconds
   TPARAMS.timeslice=0.001f;       // time for which idling threads sleep in seconds

   TPARAMS.fps=25.0f;              // frames per second (target frame rate)
   TPARAMS.spu=0.5f;               // update period for render buffer in seconds

   TPARAMS.res=1.0E3f;             // global resolution of triangulation
   TPARAMS.relres1=1.0f;           // relative adjustment factor #1 for global resolution
   TPARAMS.relres2=1.0f;           // relative adjustment factor #2 for global resolution

   TPARAMS.fovy=60.0f;             // field of view (degrees)
   TPARAMS.nearp=10.0f;            // near plane (meters)
   TPARAMS.farp=10000.0f;          // far plane (meters)

   TPARAMS.reduction1=2.0f;        // reduction parameter #1 for invisible tiles
   TPARAMS.reduction2=3.0f;        // reduction parameter #2 for invisible tiles

   TPARAMS.cullslope=0.05f;        // slope under which the terrain is culled

   TPARAMS.range=0.001f;           // texture paging range relative to far plane
   TPARAMS.relrange1=1.0f;         // relative adjustment factor #1 for texture paging range
   TPARAMS.relrange2=1.0f;         // relative adjustment factor #2 for texture paging range
   TPARAMS.refres=1.0f;            // reference resolution for texture paging in meters
   TPARAMS.radius=3.0f;            // non-linear kick-in distance relative to texture range
   TPARAMS.dropoff=1.0f;           // non-linear lod dropoff at kick-in distance

   TPARAMS.overlap=0.001f;         // tile overlap in texels

   TPARAMS.sealevel=-MAXFLOAT;     // sea-level height in meters (off=-MAXFLOAT)

   TPARAMS.alphathres=254.0f/255;  // alpha test threshold
   TPARAMS.seaalphathres=0.0f;     // sea alpha test threshold

   TPARAMS.omitsea=FALSE;          // omit sea level when shooting rays

   TPARAMS.genmipmaps=FALSE;       // enable on-the-fly generation of mipmaps
   TPARAMS.automipmap=FALSE;       // auto mip-map raw textures

   TPARAMS.autocompress=FALSE;     // auto-compress raw textures with S3TC
   TPARAMS.lod0uncompressed=FALSE; // keep LOD0 textures uncompressed

   TPARAMS.elevdir="elev";         // default elev directory
   TPARAMS.imagdir="imag";         // default imag directory

   TPARAMS.proxyname=NULL;         // proxy server name
   TPARAMS.proxyport=NULL;         // proxy server port

#ifndef _WIN32
   TPARAMS.localpath="/var/tmp/";           // local directory
#else
   TPARAMS.localpath="C:\\Windows\\Temp\\"; // local directory for Windows
#endif

   TPARAMS.altpath="data/";             // alternative data path
   TPARAMS.instpath="/usr/share/mini/"; // installation data path

   // optional feature switches:

   TPARAMS.usefog=FALSE;
   TPARAMS.useshaders=FALSE;
   TPARAMS.usediffuse=FALSE;
   TPARAMS.usedimming=FALSE;
   TPARAMS.usedetail=FALSE;
   TPARAMS.usevisshader=FALSE;
   TPARAMS.usebathymap=FALSE;
   TPARAMS.usecontours=FALSE;
   TPARAMS.usenprshader=FALSE;
   TPARAMS.usewaypoints=FALSE;
   TPARAMS.usebricks=FALSE;

   // optional lighting:

   TPARAMS.lightdir=miniv3d(0.0,0.0,1.0); // directional light

   TPARAMS.lightbias=0.75f;   // lighting bias
   TPARAMS.lightoffset=0.25f; // lighting offset

   // optional spherical fog:

   TPARAMS.fogcolor[0]=0.65f;
   TPARAMS.fogcolor[1]=0.7f;
   TPARAMS.fogcolor[2]=0.7f;

   TPARAMS.fogstart=0.5f;   // start of fog relative to far plane
   TPARAMS.fogdensity=0.5f; // relative fog density

   // optional color mapping:

   TPARAMS.bathystart=0.0f;      // start of colormapped elevation range
   TPARAMS.bathyend=0.0f;        // end of colormapped elevation range

   TPARAMS.bathymap=NULL;        // RGB[A] colormap
   TPARAMS.bathywidth=0;         // width of colormap
   TPARAMS.bathyheight=0;        // height of colormap
   TPARAMS.bathycomps=0;         // components of colormap (either 3 or 4)

   TPARAMS.contours=10.0f;       // contour distance in meters
   TPARAMS.contourmode=0;        // contour line mode (0=full 1=bathy)

   TPARAMS.seacolor[0]=0.0f;     // color of sea surface (matches with Blue Marble default sea color)
   TPARAMS.seacolor[1]=0.0f;     // color of sea surface (matches with Blue Marble default sea color)
   TPARAMS.seacolor[2]=0.2f;     // color of sea surface (matches with Blue Marble default sea color)

   TPARAMS.seatrans=0.25f;       // transparency of sea surface
   TPARAMS.seamodulate=0.0f;     // modulation factor of sea surface texture
   TPARAMS.seabottom=-10.0f;     // depth where sea transparency gets saturated
   TPARAMS.bottomtrans=1.0f;     // saturated transparency at sea bottom
   TPARAMS.seamode=0;            // sea surface mode (0=normal 1=stipple)

   TPARAMS.bottomcolor[0]=0.0f;  // saturated color at sea bottom
   TPARAMS.bottomcolor[1]=0.1f;  // saturated color at sea bottom
   TPARAMS.bottomcolor[2]=0.25f; // saturated color at sea bottom

   // optional NPR rendering:

   TPARAMS.nprfadefactor=0.25f;  // NPR fade factor

   TPARAMS.nprbathystart=0.0f;   // start of colormapped elevation range
   TPARAMS.nprbathyend=0.0f;     // end of colormapped elevation range

   TPARAMS.nprbathymap=NULL;     // RGB[A] colormap
   TPARAMS.nprbathywidth=0;      // width of colormap
   TPARAMS.nprbathyheight=0;     // height of colormap
   TPARAMS.nprbathycomps=0;      // components of colormap (either 3 or 4)

   TPARAMS.nprcontours=100.0f;   // NPR contour distance in meters
   TPARAMS.nprcontourmode=1;     // contour line mode (0=full 1=bathy)

   TPARAMS.nprseacolor[0]=0.5f;  // NPR color of sea surface
   TPARAMS.nprseacolor[1]=0.75f; // NPR color of sea surface
   TPARAMS.nprseacolor[2]=1.0f;  // NPR color of sea surface

   TPARAMS.nprseatrans=0.25f;    // NPR transparency of sea surface
   TPARAMS.nprseagray=0.5f;      // NPR grayness of bathymetry

   // optional detail textures:

   TPARAMS.detailtexmode=1;     // detail texture mode (0=off 1=overlay 2=modulate)
   TPARAMS.detailtexalpha=1.0f; // detail texture opacity
   TPARAMS.detailtexmask=1;     // detail texture mask (0=off 1=on)

   // optional way-points:

   TPARAMS.waypoints="Waypoints.txt"; // waypoint file
   TPARAMS.extrawps="Waypoints2.txt"; // extra waypoint file

   TPARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   TPARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   TPARAMS.signpostheight=100.0f; // height of signposts in meters
   TPARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   TPARAMS.brick="Cone.db";     // brick file

   TPARAMS.bricksize=100.0f;    // brick size in meters
   TPARAMS.brickradius=1000.0f; // brick display radius in meters

   TPARAMS.brickpasses=4;       // brick render passes
   TPARAMS.brickceiling=3.0f;   // upper boundary for brick color mapping relative to elevation of first waypoint
   TPARAMS.brickscroll=0.5f;    // scroll period of striped bricks in seconds

   // create the render cache
   CACHE=new minicache;

   // initialize state:

   LAYER=NULL;
   LNUM=LMAX=0;

   NULL_LAYER=setnull();
   EARTH_LAYER=setearth();

   DEFAULT_LAYER=LNUM;
   setreference(DEFAULT_LAYER);

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
   }

// destructor
miniterrain::~miniterrain()
   {
   int n;

   // delete the tileset layers
   if (LAYER!=NULL)
      {
      for (n=0; n<LNUM; n++) delete LAYER[n];
      delete[] LAYER;
      }

   // delete the render cache
   delete CACHE;
   }

// reserve space in layer array
int miniterrain::reserve()
   {
   // create layer array
   if (LAYER==NULL)
      {
      LMAX=1;
      if ((LAYER=(minilayer **)malloc(LMAX*sizeof(minilayer *)))==NULL) MEMERROR();
      }

   // enlarge layer array
   if (LNUM>=LMAX)
      {
      LMAX*=2;
      if ((LAYER=(minilayer **)realloc(LAYER,LMAX*sizeof(minilayer *)))==NULL) MEMERROR();
      }

   LAYER[LNUM]=NULL;

   return(LNUM++);
   }

// get parameters
void miniterrain::get(MINITERRAIN_PARAMS &tparams)
   {tparams=TPARAMS;}

// set parameters
void miniterrain::set(MINITERRAIN_PARAMS &tparams)
   {
   int n;

   minilayer::MINILAYER_PARAMS lparams;

   // set new state
   TPARAMS=tparams;

   // propagate layer parameters
   if (LAYER!=NULL)
      for (n=0; n<LNUM; n++)
         {
         // get the actual layer state
         LAYER[n]->get(lparams);

         // update the layer state:

         lparams.warpmode=TPARAMS.warpmode;
         lparams.nonlin=TPARAMS.nonlin;

         lparams.scale=TPARAMS.scale;
         lparams.exaggeration=TPARAMS.exaggeration;
         lparams.maxelev=TPARAMS.maxelev;

         lparams.load=TPARAMS.load;
         lparams.preload=TPARAMS.preload;

         lparams.minres=TPARAMS.minres;
         lparams.fastinit=TPARAMS.fastinit;
         lparams.avgd2value=TPARAMS.avgd2value;

         lparams.sead2=TPARAMS.sead2;
         lparams.seamin=TPARAMS.seamin;
         lparams.seamax=TPARAMS.seamax;

         lparams.lazyness=TPARAMS.lazyness;
         lparams.update=TPARAMS.update;
         lparams.expire=TPARAMS.expire;

         lparams.upload=TPARAMS.upload;
         lparams.keep=TPARAMS.keep;
         lparams.maxdelay=TPARAMS.maxdelay;
         lparams.cache=TPARAMS.cache;

         lparams.keepalive=TPARAMS.keepalive;
         lparams.timeslice=TPARAMS.timeslice;

         lparams.fps=TPARAMS.fps;
         lparams.spu=TPARAMS.spu;

         lparams.res=TPARAMS.res;
         lparams.relres1=TPARAMS.relres1;
         lparams.relres2=TPARAMS.relres2;

         lparams.fovy=TPARAMS.fovy;
         lparams.nearp=TPARAMS.nearp;
         lparams.farp=TPARAMS.farp;

         lparams.reduction1=TPARAMS.reduction1;
         lparams.reduction2=TPARAMS.reduction2;

         lparams.cullslope=TPARAMS.cullslope;

         if (TPARAMS.fademode==0) lparams.range=TPARAMS.range;
         else lparams.range=0.5f*TPARAMS.farp*fpow(2.0f,LAYER[n]->getlevel());

         lparams.relrange1=TPARAMS.relrange1;
         lparams.relrange2=TPARAMS.relrange2;
         lparams.refres=TPARAMS.refres;
         lparams.radius=TPARAMS.radius;
         lparams.dropoff=TPARAMS.dropoff;

         lparams.sealevel=TPARAMS.sealevel;

         lparams.genmipmaps=TPARAMS.genmipmaps;
         lparams.automipmap=TPARAMS.automipmap;

         lparams.autocompress=TPARAMS.autocompress;
         lparams.lod0uncompressed=TPARAMS.lod0uncompressed;

         lparams.proxyname=TPARAMS.proxyname;
         lparams.proxyport=TPARAMS.proxyport;

         lparams.localpath=TPARAMS.localpath;

         lparams.altpath=TPARAMS.altpath;
         lparams.instpath=TPARAMS.instpath;

         // update the waypoint state:

         lparams.usewaypoints=TPARAMS.usewaypoints;
         lparams.usebricks=TPARAMS.usebricks;

         lparams.waypoints=TPARAMS.waypoints;
         lparams.extrawps=TPARAMS.extrawps;

         lparams.signpostheight=TPARAMS.signpostheight;
         lparams.signpostrange=TPARAMS.signpostrange;

         lparams.signpostturn=TPARAMS.signpostturn;
         lparams.signpostincline=TPARAMS.signpostincline;

         lparams.brick=TPARAMS.brick;

         lparams.bricksize=TPARAMS.bricksize;
         lparams.brickradius=TPARAMS.brickradius;

         lparams.brickpasses=TPARAMS.brickpasses;
         lparams.brickceiling=TPARAMS.brickceiling;
         lparams.brickscroll=TPARAMS.brickscroll;

         // finally pass the updated layer state
         LAYER[n]->set(lparams);
         }

   // update color maps
   minishader::setVISbathymap(TPARAMS.bathymap,TPARAMS.bathywidth,TPARAMS.bathyheight,TPARAMS.bathycomps);
   minishader::setNPRbathymap(TPARAMS.nprbathymap,TPARAMS.nprbathywidth,TPARAMS.nprbathyheight,TPARAMS.nprbathycomps);

   // reset reference layer
   setreference(LREF);
   }

// propagate parameters
void miniterrain::propagate()
   {set(TPARAMS);}

// set internal callbacks
void miniterrain::setcallbacks(void *threaddata,
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

// load tileset (regular version)
minilayer *miniterrain::load(const char *elev_ini,const char *imag_ini,
                             BOOLINT loadopts,BOOLINT reset,
                             int level,int baselevel)
   {
   char *elev,*imag;

   minilayer *layer;

   elev=strdup(elev_ini);
   imag=strdup(imag_ini);

   // remove trailing .ini
   if (strlen(elev)>4)
      if (strstr(elev,".ini")==&elev[strlen(elev)-4]) elev[strlen(elev)-4]='\0';
   if (strlen(imag)>4)
      if (strstr(imag,".ini")==&imag[strlen(imag)-4]) imag[strlen(imag)-4]='\0';

   // remove trailing slash
   if (strlen(elev)>1)
      if (elev[strlen(elev)-1]=='/') elev[strlen(elev)-1]='\0';
   if (strlen(imag)>1)
      if (imag[strlen(imag)-1]=='/') imag[strlen(imag)-1]='\0';

   // remove trailing backslash
   if (strlen(elev)>1)
      if (elev[strlen(elev)-1]=='\\') elev[strlen(elev)-1]='\0';
   if (strlen(imag)>1)
      if (imag[strlen(imag)-1]=='\\') imag[strlen(imag)-1]='\0';

   // load tileset
   layer=load("","",elev,imag,loadopts,reset,level,baselevel);

   free(elev);
   free(imag);

   return(layer);
   }

// load tileset (short version)
minilayer *miniterrain::load(const char *url,
                             BOOLINT loadopts,BOOLINT reset,
                             int level,int baselevel)
   {
   char *baseurl;
   char *lastslash,*lastbslash;
   char *baseid;

   minilayer *layer;

   baseurl=strdup(url);

   // remove trailing .ini
   if (strlen(baseurl)>4)
      if (strstr(baseurl,".ini")==&baseurl[strlen(baseurl)-4]) baseurl[strlen(baseurl)-4]='\0';

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

   // decompose url into baseurl and baseid
   if (lastslash!=NULL)
      {
      baseid=strdup2(++lastslash,"/");
      *lastslash='\0';
      }
   else if (lastbslash!=NULL)
      {
      baseid=strdup2(++lastbslash,"\\");
      *lastbslash='\0';
      }
   else baseid=strdup("/");

   // load tileset
   layer=load(baseurl,baseid,TPARAMS.elevdir,TPARAMS.imagdir,loadopts,reset,level,baselevel);

   free(baseurl);
   free(baseid);

   return(layer);
   }

// load tileset (long version)
minilayer *miniterrain::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                             BOOLINT loadopts,BOOLINT reset,
                             int level,int baselevel)
   {
   int n;

   // reserve space in layer array
   n=reserve();

   // create the tileset layer
   LAYER[n]=new minilayer(CACHE);

   // propagate parameters
   propagate();

   // set reference coordinate system
   LAYER[n]->setreference(getlayer(getreference()));

   // register callbacks
   LAYER[n]->setcallbacks(THREADDATA,
                          THREADINIT,THREADEXIT,
                          STARTTHREAD,JOINTHREAD,
                          LOCK_CS,UNLOCK_CS,
                          LOCK_IO,UNLOCK_IO,
                          CURLDATA,
                          CURLINIT,CURLEXIT,
                          GETURL,CHECKURL);

   // load the tileset layer
   if (!LAYER[n]->load(baseurl,baseid,basepath1,basepath2,reset,level,baselevel))
      {
      remove(n);
      return(NULL);
      }

   // load optional features
   if (loadopts) LAYER[n]->loadopts();

   // propagate parameters
   propagate();

   // reset reference layer
   setreference(LREF);

   // set pre and post sea surface render callbacks
   CACHE->setseacb(preseacb,postseacb,this);

   // enable alpha test
   CACHE->setalphatest(TPARAMS.alphathres);

   // enable sea alpha test
   CACHE->setseaalphatest(TPARAMS.seaalphathres);

   // turn on ray object
   CACHE->setshooting(1);
   CACHE->configure_omitsea(TPARAMS.omitsea);

   // set tile overlap
   CACHE->configure_overlap(TPARAMS.overlap);

   // recompute whether or not a layer is a patch
   check4patches();

   // mark scene for complete update
   update();

   // success
   return(LAYER[n]);
   }

// load layered tileset
minilayer *miniterrain::loadLTS(const char *url,
                                BOOLINT loadopts,BOOLINT reset,
                                int levels)
   {
   int l;

   char *layerurl;
   char layerlevel[10];

   minilayer *layer;
   minilayer *toplevel;

   toplevel=NULL;

   // enable alpha test for multiple levels
   if (levels>1)
      {
      // set alpha test threshold to full transparency
      TPARAMS.alphathres=0.0f;

      // set sea alpha test threshold to sea opacity
      TPARAMS.seaalphathres=ftrc(255.0f*(TPARAMS.usevisshader?TPARAMS.seatrans:TPARAMS.nprseatrans)-1)/255.0f;
      }

   // load tileset levels
   for (l=0; l<levels; l++)
      {
      // append layer level to url
      if (l==0) layerurl=strdup(url);
      else
         {
         snprintf(layerlevel,10,"%d",l);
         layerurl=strdup2(url,layerlevel);
         }

      // load layer with negative levels
      layer=load(layerurl,loadopts,reset,l-levels+1,-levels+1);

      // free layer url
      free(layerurl);

      if (!layer) break;

      if (l==0) toplevel=layer;
      }

   // enable fade for multiple levels
   if (l>1)
      {
      // enable spherical fade
      TPARAMS.fademode=1;

      // enable spherical subduction
      TPARAMS.submode=1;
      }
   else
      if (toplevel!=NULL) toplevel->setlevel(0,0);

   return(toplevel);
   }

// set null layer
int miniterrain::setnull()
   {
   int n;

   // reserve space in layer array
   n=reserve();

   // create the layer
   LAYER[n]=new minilayer(NULL);

   // setup the earth layer
   LAYER[n]->setnull();

   return(n);
   }

// set earth layer
int miniterrain::setearth()
   {
   int n;

   // reserve space in layer array
   n=reserve();

   // create the layer
   LAYER[n]=new minilayer(NULL);

   // setup the earth layer
   LAYER[n]->setearth();

   return(n);
   }

// create empty layer
minilayer *miniterrain::create(minicoord &center,minicoord &north)
   {
   int n;

   // reserve space
   n=reserve();

   // create the layer
   LAYER[n]=new minilayer(NULL);

   // setup the earth layer
   LAYER[n]->setempty(center,north);

   return(LAYER[n]);
   }

// get null layer
int miniterrain::getnull()
   {return(NULL_LAYER);}

// get earth layer
int miniterrain::getearth()
   {return(EARTH_LAYER);}

// get default layer
int miniterrain::getdefault()
   {return(DEFAULT_LAYER);}

// set reference layer
void miniterrain::setreference(int ref)
   {
   int n;

   // set new reference
   LREF=ref;

   // set new reference layer for default coordinate conversions
   REFERENCE=getlayer(LREF);

   // propagate new reference coordinate system
   for (n=0; n<LNUM; n++)
      LAYER[n]->setreference(REFERENCE);
   }

// get reference layer
int miniterrain::getreference()
   {return(LREF);}

// get the nth terrain layer
minilayer *miniterrain::getlayer(int n)
   {
   if (n<0 || n>=LNUM) return(NULL);
   else return(LAYER[n]);
   }

// get the serial number of a terrain layer
int miniterrain::getnum(minilayer *layer)
   {
   int n;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]==layer) return(n);

   return(-1);
   }

// remove the nth terrain layer
void miniterrain::remove(int n)
   {
   if (n<0 || n>=LNUM) return;

   delete LAYER[n];

   if (n<LNUM-1) LAYER[n]=LAYER[--LNUM];
   else LNUM--;

   if (n==LREF) setreference(getdefault());
   }

// get extent of a tileset
miniv3d miniterrain::getextent(int n)
   {
   if (n<0 || n>=LNUM) return(miniv3d(0.0));
   else return(LAYER[n]->getextent());
   }

// get center of a tileset
minicoord miniterrain::getcenter(int n)
   {
   if (n<0 || n>=LNUM) return(minicoord());
   else return(LAYER[n]->getcenter());
   }

// get the elevation at position (x,y,z)
double miniterrain::getheight(const minicoord &p,int approx)
   {
   int n;

   int nst;

   double elev;

   if (LNUM>0)
      {
      nst=getnearest(p);

      if (isdisplayed(nst) && !isculled(nst))
         {
         elev=LAYER[nst]->getheight(p,approx);
         if (elev!=-MAXFLOAT) return(len_l2g(LAYER[nst]->len_g2l(elev)));
         }

      for (n=LNUM-1; n>=0; n--)
         if (n!=nst)
            if (isdisplayed(n) && !isculled(n))
               {
               elev=LAYER[n]->getheight(p,approx);
               if (elev!=-MAXFLOAT) return(len_l2g(LAYER[n]->len_g2l(elev)));
               }
      }

   return(-MAXFLOAT);
   }

// get the normal at position (x,y,z)
miniv3d miniterrain::getnormal(const minicoord &p,int approx)
   {
   int n;

   int nst;

   miniv3d nrml;

   if (LNUM>0)
      {
      nst=getnearest(p);

      if (isdisplayed(nst) && !isculled(nst))
         {
         nrml=LAYER[nst]->getnormal(p,approx);
         if (nrml!=miniv3d(0.0)) return(nrml);
         }

      for (n=LNUM-1; n>=0; n--)
         if (n!=nst)
            if (isdisplayed(n) && !isculled(n))
               {
               nrml=LAYER[n]->getnormal(p,approx);
               if (nrml!=miniv3d(0.0)) return(nrml);
               }
      }

   return(miniv3d(0.0));
   }

// get initial view point
minicoord miniterrain::getinitial()
   {
   if (REFERENCE==NULL) return(minicoord());
   else return(REFERENCE->getinitial());
   }

// set initial eye point
void miniterrain::initeyepoint(const minicoord &e)
   {
   int n;

   for (n=0; n<LNUM; n++)
      LAYER[n]->initeyepoint(e);
   }

// get nearest layer
int miniterrain::getnearest(const minicoord &e)
   {
   int n;

   int nst;
   double rad,dist,mindist;

   nst=-1;
   mindist=MAXFLOAT;

   for (n=0; n<LNUM; n++)
      if (isdisplayed(n) && !issubtileset(n))
         {
         rad=LAYER[n]->getextent().getlength()/2.0;

         dist=LAYER[n]->getcenter().getdist(e);
         dist-=rad;

         if (dist<rad)
            if (dist<mindist)
               {
               mindist=dist;
               nst=n;
               }
         }

   return(nst);
   }

// enable a specific focus point
void miniterrain::enablefocus(const minicoord &f)
   {
   int n;

   for (n=0; n<LNUM; n++)
      LAYER[n]->enablefocus(f);
   }

// disable the focus point
void miniterrain::disablefocus()
   {
   int n;

   for (n=0; n<LNUM; n++)
      LAYER[n]->disablefocus();
   }

// trigger complete render buffer update at next frame
void miniterrain::update()
   {
   int n;

   for (n=0; n<LNUM; n++)
      LAYER[n]->update();
   }

// generate and cache scene for a particular eye point
void miniterrain::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time)
   {
   int n;

   TPARAMS.time=time;

   // make vertex cache current
   CACHE->makecurrent();

   // update non-linear warp
   CACHE->usenonlinear(TPARAMS.nonlin);

   // update each layer
   for (n=0; n<LNUM; n++)
      LAYER[n]->cache(e,d,u,aspect,time);
   }

// render cached scene
void miniterrain::render()
   {
   int n;

   int detailtexid;
   int detailwidth,detailheight;
   int detailmipmaps;

   miniv4d detailu;
   miniv4d detailv;

   float detailalpha;

   double mvmtx[16];
   miniv4d invmtx[4];

   miniv3d lgl;

   if (LNUM>0)
      {
      // enable shaders
      if (TPARAMS.useshaders)
         {
         // compute inverse transpose modelview to transform into eye linear coordinates
         if (TPARAMS.usedetail && TPARAMS.detailtexmode!=0 && TPARAMS.detailtexalpha!=0.0f)
            {
            mtxgetmodel(mvmtx);

            invmtx[0]=miniv4d(mvmtx[0],mvmtx[4],mvmtx[8],mvmtx[12]);
            invmtx[1]=miniv4d(mvmtx[1],mvmtx[5],mvmtx[9],mvmtx[13]);
            invmtx[2]=miniv4d(mvmtx[2],mvmtx[6],mvmtx[10],mvmtx[14]);
            invmtx[3]=miniv4d(mvmtx[3],mvmtx[7],mvmtx[11],mvmtx[15]);

            inv_mtx4(invmtx,invmtx);
            tra_mtx4(invmtx,invmtx);
            }

         // set detail texture parameters in eye linear coordinates
         for (n=0; n<LNUM; n++)
            if (LAYER[n]->istileset())
               {
               if (!TPARAMS.usedetail || TPARAMS.detailtexmode==0 || TPARAMS.detailtexalpha==0.0f)
                  {
                  detailtexid=0;
                  detailwidth=detailheight=0;
                  detailmipmaps=0;

                  detailu=detailv=miniv4d(0.0);

                  detailalpha=0.0f;
                  }
               else
                  {
                  LAYER[n]->getdetailtex(detailtexid,detailwidth,detailheight,detailmipmaps,
                                         detailu,detailv,
                                         detailalpha);

                  detailu=miniv4d(detailu*invmtx[0],detailu*invmtx[1],detailu*invmtx[2],detailu*invmtx[3]);
                  detailv=miniv4d(detailv*invmtx[0],detailv*invmtx[1],detailv*invmtx[2],detailv*invmtx[3]);
                  }

               CACHE->setpixshadertexgen(LAYER[n]->getterrain()->getminitile(),
                                         detailu.x,detailu.y,detailu.z,detailu.w,
                                         detailv.x,detailv.y,detailv.z,detailv.w);

               CACHE->setpixshaderdetailtexid(LAYER[n]->getterrain()->getminitile(),
                                              detailtexid,detailwidth,detailheight,detailmipmaps);

               CACHE->setpixshadertexalpha(LAYER[n]->getterrain()->getminitile(),
                                           detailalpha);
               }

         // set detail texture mode
         minishader::setdetailtexmode(TPARAMS.detailtexmode,TPARAMS.detailtexalpha,TPARAMS.detailtexmask);

         // set sea surface mode
         minishader::setseamode(TPARAMS.seamode);

         // set fade mode
         minishader::setfademode(TPARAMS.fademode,(1.0-TPARAMS.fadeout)*TPARAMS.farp,TPARAMS.farp);

         // choose shader
         if (TPARAMS.usevisshader)
            {
            // set contour line mode
            minishader::setcontourmode(TPARAMS.contourmode);

            // use standard VIS shader
            minishader::setVISshader(CACHE,
                                     len_o2g(1.0),TPARAMS.exaggeration,
                                     (TPARAMS.usefog)?TPARAMS.fogstart/2.0f*TPARAMS.farp:0.0f,(TPARAMS.usefog)?TPARAMS.farp:0.0f,
                                     TPARAMS.fogdensity,
                                     TPARAMS.fogcolor,
                                     (TPARAMS.usebathymap)?TPARAMS.bathystart:TPARAMS.bathyend,TPARAMS.bathyend,
                                     (TPARAMS.usecontours)?TPARAMS.contours:0.0f,
                                     TPARAMS.sealevel,TPARAMS.seabottom,
                                     TPARAMS.seacolor,
                                     TPARAMS.seatrans,TPARAMS.bottomtrans,
                                     TPARAMS.bottomcolor,
                                     TPARAMS.seamodulate);
            }
         else if (TPARAMS.usenprshader)
            {
            // set contour line mode
            minishader::setcontourmode((TPARAMS.usecontours)?0:TPARAMS.nprcontourmode);

            // use alternative NPR shader
            minishader::setNPRshader(CACHE,
                                     len_o2g(1.0),TPARAMS.exaggeration,
                                     (TPARAMS.usefog)?TPARAMS.fogstart/2.0f*TPARAMS.farp:0.0f,(TPARAMS.usefog)?TPARAMS.farp:0.0f,
                                     TPARAMS.fogdensity,
                                     TPARAMS.fogcolor,
                                     TPARAMS.nprbathystart,TPARAMS.nprbathyend,
                                     TPARAMS.nprcontours,
                                     TPARAMS.nprfadefactor,
                                     fmax(TPARAMS.sealevel,0.0f),
                                     TPARAMS.nprseacolor,TPARAMS.nprseatrans,
                                     TPARAMS.nprseagray);
            }
         else
            {
            // use default shader
            CACHE->setvtxshader();
            CACHE->usevtxshader(1);
            CACHE->setpixshader();
            CACHE->usepixshader(1);
            CACHE->setseashader();
            CACHE->useseashader(1);
            }

         // set lighting parameters
         for (n=0; n<LNUM; n++)
            if (LAYER[n]->istileset())
               if (TPARAMS.usediffuse)
                  {
                  lgl=LAYER[getnull()]->rot_g2o(TPARAMS.lightdir,LAYER[getnull()]->getcenter());

                  if (TPARAMS.usedimming)
                     CACHE->setlight(LAYER[n]->getterrain()->getminitile(),
                                     lgl.x,lgl.y,lgl.z,0.5f*TPARAMS.lightbias,0.5f*TPARAMS.lightoffset);
                  else
                     CACHE->setlight(LAYER[n]->getterrain()->getminitile(),
                                     lgl.x,lgl.y,lgl.z,TPARAMS.lightbias,TPARAMS.lightoffset);
                  }
               else
                  if (TPARAMS.usedimming)
                     CACHE->setlight(LAYER[n]->getterrain()->getminitile(),
                                     0.0f,0.0f,0.0f,0.0f,0.5f);
                  else
                     CACHE->setlight(LAYER[n]->getterrain()->getminitile(),
                                     0.0f,0.0f,0.0f,0.0f,1.0f);

         // set level parameters
         for (n=0; n<LNUM; n++)
            if (LAYER[n]->istileset())
               CACHE->setpixshaderlayerlevel(LAYER[n]->getterrain()->getminitile(),
                                             LAYER[n]->getlevel(),LAYER[n]->getbaselevel());

         // set subduction parameters
         CACHE->usesubduction(TPARAMS.submode,TPARAMS.farp/TPARAMS.scale,TPARAMS.subfactor);
         }
      else
         {
         // set primary sea color
         CACHE->setseacolor(TPARAMS.seacolor[0],TPARAMS.seacolor[1],TPARAMS.seacolor[2],TPARAMS.seatrans);

         // disable sea texturing
         CACHE->configure_seaenabletex(0);
         }

      // render vertex arrays
      CACHE->rendercache();

      // disable shaders
      if (TPARAMS.useshaders)
         if (TPARAMS.usevisshader || TPARAMS.usenprshader)
            minishader::unsetshaders(CACHE);
         else
            {
            CACHE->usevtxshader(0);
            CACHE->usepixshader(0);
            CACHE->useseashader(0);
            }
      }
   }

// recompute whether or not a layer is a patch
void miniterrain::check4patches()
   {
   int n;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->istileset())
         CACHE->setpatch(LAYER[n]->getterrain()->getminitile(),checkpatch(n));
   }

// check whether or not a layer is a patch
int miniterrain::checkpatch(int n)
   {
   int i;

   double ext2;
   minicoord midp;

   ext2=LAYER[n]->getextent().getlength2();
   midp=LAYER[n]->getcenter();

   if (!LAYER[n]->issubtileset())
      for (i=0; i<LNUM; i++)
         if (i!=n)
            if (LAYER[i]->isdisplayed())
               if (!LAYER[i]->issubtileset())
                  if (LAYER[i]->getextent().getlength2()>ext2)
                     if (LAYER[i]->getheight(midp)!=-MAXFLOAT) return(1);

   return(0);
   }

// pre sea render callback
void miniterrain::preseacb(void *data)
   {
   miniterrain *viewer=(miniterrain *)data;
   viewer->render_presea();
   }

// post sea render callback
void miniterrain::postseacb(void *data)
   {
   miniterrain *viewer=(miniterrain *)data;
   viewer->render_postsea();
   }

// pre sea render function
void miniterrain::render_presea()
   {
   int n;

   minicoord el;

   minilayer::MINILAYER_PARAMS *lparams;

   for (n=0; n<LNUM; n++)
      if (isdisplayed(n) && !isculled(n))
         {
         lparams=LAYER[n]->get();

         el=LAYER[n]->map_g2l(lparams->eye);

         // render waypoints before sea surface
         if (el.vec.z>=lparams->sealevel/lparams->scale) LAYER[n]->renderpoints();
         }
   }

// post sea render function
void miniterrain::render_postsea()
   {
   int n;

   minicoord el;

   minilayer::MINILAYER_PARAMS *lparams;

   for (n=0; n<LNUM; n++)
      if (isdisplayed(n) && !isculled(n))
         {
         lparams=LAYER[n]->get();

         el=LAYER[n]->map_g2l(lparams->eye);

         // render waypoints after sea surface
         if (el.vec.z<lparams->sealevel/lparams->scale) LAYER[n]->renderpoints();
         }
   }

// check whether or not a layer is a tileset
BOOLINT miniterrain::istileset(int n)
   {
   if (n>=0 && n<LNUM) return(LAYER[n]->istileset());
   return(FALSE);
   }

// check whether or not a layer is a sub-tileset
BOOLINT miniterrain::issubtileset(int n)
   {
   if (n>=0 && n<LNUM) return(LAYER[n]->issubtileset());
   return(FALSE);
   }

// determine whether or not a layer is displayed
void miniterrain::display(int n,BOOLINT visible)
   {
   if (n>=0 && n<LNUM)
      if (LAYER[n]->istileset())
         {
         LAYER[n]->display(visible);
         check4patches();
         }
   }

// check whether or not a layer is displayed
BOOLINT miniterrain::isdisplayed(int n)
   {
   if (n>=0 && n<LNUM) return(LAYER[n]->isdisplayed());
   return(FALSE);
   }

// check whether or not a layer is culled
BOOLINT miniterrain::isculled(int n)
   {
   if (n>=0 && n<LNUM) return(LAYER[n]->isculled());
   return(TRUE);
   }

// flatten the terrain by a relative scaling factor (in the range [0-1])
void miniterrain::flatten(float relscale)
   {
   int n;

   for (n=0; n<LNUM; n++)
      LAYER[n]->flatten(relscale);
   }

// get the flattening factor
float miniterrain::getflattening()
   {
   if (REFERENCE!=NULL) return(REFERENCE->getflattening());
   else return(1.0f);
   }

// get the nearest waypoint
minipointdata *miniterrain::getnearestpoint(minicoord &e,int type)
   {
   int nst;

   minipointdata *nearest;

   nearest=NULL;

   if (LNUM>0)
      {
      // get nearest layer
      nst=getnearest(e);

      // get nearest waypoint from nearest layer
      if (isdisplayed(nst) && !isculled(nst)) nearest=LAYER[nst]->getnearestpoint(type);
      }

   return(nearest);
   }

// shoot a ray at the scene
double miniterrain::shoot(const minicoord &o,const miniv3d &d,double hitdist,int *id)
   {
   int n;

   int nst;

   double dist;
   double dn;

   minicoord ogl;
   miniv3d dgl;

   int id_hit;

   dist=MAXFLOAT;
   id_hit=-1;

   if (LNUM>0)
      {
      // transform coordinates
      ogl=map_g2o(o);
      dgl=rot_g2o(d,o);

      // get nearest layer
      nst=getnearest(o);

      // shoot a ray at the nearest layer
      if (isdisplayed(nst) && !isculled(nst))
         dist=CACHE->getray(LAYER[nst]->getcacheid())->shoot(ogl.vec,dgl,hitdist);
      else dist=MAXFLOAT;

      // check for valid hit
      if (dist!=MAXFLOAT)
         {
         dist=len_o2g(dist);
         id_hit=nst;
         }
      else
         for (n=LNUM-1; n>=0; n--)
            if (n!=nst)
               if (isdisplayed(n) && !isculled(n))
                  {
                  // shoot a ray and get the traveled distance
                  dn=CACHE->getray(LAYER[n]->getcacheid())->shoot(ogl.vec,dgl,hitdist);

                  // check for valid hit
                  if (dn!=MAXFLOAT) dn=len_o2g(dn);

                  // remember nearest hit
                  if (dn<dist)
                     {
                     dist=dn;
                     id_hit=n;
                     }

                  // stop if actual distance is already lower than required hitdist
                  if (dist<hitdist) break;
                  }
      }

   if (id!=NULL) *id=id_hit;

   return(dist);
   }

// extract triangles that [possibly] intersect a plane
minidyna<miniv3d> miniterrain::extract(const minicoord &p,const miniv3d &v,double radius)
   {
   int n;

   minidyna<miniv3d> result;

   minicoord pgl;
   miniv3d vgl;

   if (LNUM>0)
      {
      // transform coordinates
      pgl=map_g2o(p);
      vgl=rot_g2o(v,p);

      // gather [potentially] intersecting triangles
      for (n=0; n<LNUM; n++)
         if (isdisplayed(n) && !isculled(n))
            result.append(CACHE->getray(LAYER[n]->getcacheid())->extract(pgl.vec,vgl,radius));
      }

   return(result);
   }

// set locking callbacks
void miniterrain::setraycallbacks(void (*lock)(void *data),void *data,
                                  void (*unlock)(void *data))
   {miniray::setcallbacks(lock,data,unlock);}

// return memory consumption
double miniterrain::getmem()
   {
   int n;

   double mem;

   mem=0.0;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->istileset()) mem+=LAYER[n]->getterrain()->getmem();

   return(mem);
   }

// return raw texture memory consumption
double miniterrain::gettexmem()
   {
   int n;

   double texmem;

   texmem=0.0;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->istileset()) texmem+=LAYER[n]->getterrain()->gettexmem();

   return(texmem);
   }

// get total number of pending tiles
int miniterrain::getpending()
   {
   int n;

   int pending;

   pending=0;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->getcache()!=NULL) pending+=LAYER[n]->getcache()->getpending();

   return(pending);
   }

// get total amount of cache memory
double miniterrain::getcachemem()
   {
   int n;

   double cachemem;

   cachemem=0.0;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->getcache()!=NULL) cachemem+=LAYER[n]->getcache()->getmem();

   return(cachemem);
   }

// attach detail texture
void miniterrain::attachdetailtex(int n,
                                  int texid,int width,int height,int mipmaps,BOOLINT owner,
                                  minicoord center,minicoord west,minicoord north,
                                  float alpha)
   {
   if (n>=0 && n<LNUM)
      if (LAYER[n]->istileset())
         {
         LAYER[n]->attachdetailtex(texid,width,height,mipmaps,owner,
                                   center,west,north,
                                   alpha);

         while (LAYER[n]->issubtileset())
            {
            n--;

            if (n<0 || n>=LNUM) break;
            if (!LAYER[n]->istileset()) break;

            LAYER[n]->attachdetailtex(texid,width,height,mipmaps,FALSE,
                                      center,west,north,
                                      alpha);
            }
         }
   }

// load detail texture (db format)
void miniterrain::loaddetailtex(int n,
                                const char *detailname,
                                float alpha)
   {
   int texid,width,height,mipmaps;
   BOOLINT owner;
   minicoord center,west,north;

   if (n>=0 && n<LNUM)
      if (LAYER[n]->istileset())
         {
         LAYER[n]->loaddetailtex(detailname,alpha);

         LAYER[n]->getdetailtex(texid,width,height,mipmaps,owner,
                                center,west,north,
                                alpha);

         while (LAYER[n]->issubtileset())
            {
            n--;

            if (n<0 || n>=LNUM) break;
            if (!LAYER[n]->istileset()) break;

            LAYER[n]->attachdetailtex(texid,width,height,mipmaps,FALSE,
                                      center,west,north,
                                      alpha);
            }
         }
   }

// register waypoint renderer
void miniterrain::registerrndr(minipointrndr *rndr)
   {
   int n;

   for (n=0; n<LNUM; n++)
      if (LAYER[n]->getpoints()!=NULL) LAYER[n]->getpoints()->registerrndr(rndr);
   }
