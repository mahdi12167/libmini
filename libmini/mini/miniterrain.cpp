// (c) by Stefan Roettger

#include "datacache.h"

#include "miniOGL.h"
#include "minishader.h"

#include "miniterrain.h"

// default constructor
miniterrain::miniterrain()
   {
   // configurable parameters:

   TPARAMS.warpmode=0;             // warp mode: plain=0 affine=1 non-linear=2

   TPARAMS.scale=100.0f;           // scaling of scene
   TPARAMS.exaggeration=1.0f;      // exaggeration of elevations
   TPARAMS.maxelev=15000.0f;       // absolute maximum of expected elevations

   TPARAMS.load=0.1f;              // initially loaded area relative to far plane
   TPARAMS.preload=1.25f;          // continuously preloaded area relative to far plane

   TPARAMS.minres=9.0f;            // minimum resolution of triangulation
   TPARAMS.fastinit=2;             // fast initialization level
   TPARAMS.avgd2value=0.1f;        // average d2value for fast initialization

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

   TPARAMS.fovy=60.0f;             // field of view (degrees)
   TPARAMS.nearp=10.0f;            // near plane (meters)
   TPARAMS.farp=10000.0f;          // far plane (meters)

   TPARAMS.reduction1=2.0f;        // reduction parameter #1 for invisible tiles
   TPARAMS.reduction2=3.0f;        // reduction parameter #2 for invisible tiles

   TPARAMS.range=0.001f;           // texture paging range relative to far plane
   TPARAMS.radius=3.0f;            // non-linear kick-in distance relative to texture range
   TPARAMS.dropoff=1.0f;           // non-linear lod dropoff at kick-in distance

   TPARAMS.overlap=0.001f;         // tile overlap in texels

   TPARAMS.genmipmaps=TRUE;        // enable on-the-fly generation of mipmaps

   TPARAMS.sealevel=-MAXFLOAT;     // sea-level height in meters (off=-MAXFLOAT)

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

   TPARAMS.altpath="data/"; // alternative data path

   // optional feature switches:

   TPARAMS.usefog=FALSE;
   TPARAMS.useshaders=FALSE;
   TPARAMS.usebathymap=FALSE;
   TPARAMS.usecontours=FALSE;
   TPARAMS.usenprshader=FALSE;
   TPARAMS.usewaypoints=FALSE;
   TPARAMS.usebricks=FALSE;

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

   TPARAMS.seacolor[0]=0.0f;     // color of sea surface
   TPARAMS.seacolor[1]=0.5f;     // color of sea surface
   TPARAMS.seacolor[2]=0.75f;    // color of sea surface

   TPARAMS.seatrans=0.25f;       // transparency of sea surface
   TPARAMS.seamodulate=0.0f;     // modulation factor of sea surface texture
   TPARAMS.seabottom=-10.0f;     // depth where sea transparency gets saturated
   TPARAMS.bottomtrans=1.0f;     // saturated transparency at sea bottom

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

   TPARAMS.nprseacolor[0]=0.5f;  // NPR color of sea surface
   TPARAMS.nprseacolor[1]=0.75f; // NPR color of sea surface
   TPARAMS.nprseacolor[2]=1.0f;  // NPR color of sea surface

   TPARAMS.nprseatrans=0.25f;    // NPR transparency of sea surface
   TPARAMS.nprseagrey=0.5f;      // NPR greyness of bathymetry

   // optional way-points:

   TPARAMS.waypoints="Waypoints.txt"; // waypoint file

   TPARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   TPARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   TPARAMS.signpostheight=100.0f; // height of signposts in meters
   TPARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   TPARAMS.brick="Cone.db"; // brick file

   TPARAMS.bricksize=100.0f;  // brick size in meters
   TPARAMS.brickrad=1000.0f;  // brick viewing radius in meters

   TPARAMS.brickpasses=4;     // brick render passes
   TPARAMS.brickceiling=3.0f; // upper boundary for brick color mapping relative to first waypoint elevation
   TPARAMS.brickscroll=0.5f;  // scroll period of striped bricks in seconds

   // initialize state:

   LAYER=NULL;
   LNUM=LMAX=0;

   setreference(0);

   CACHE=NULL;

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
      delete LAYER;
      }

   // delete the render cache
   if (CACHE!=NULL) delete CACHE;
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

         lparams.scale=TPARAMS.scale;
         lparams.exaggeration=TPARAMS.exaggeration;
         lparams.maxelev=TPARAMS.maxelev;

         lparams.load=TPARAMS.load;
         lparams.preload=TPARAMS.preload;

         lparams.minres=TPARAMS.minres;
         lparams.fastinit=TPARAMS.fastinit;
         lparams.avgd2value=TPARAMS.avgd2value;

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

         lparams.fovy=TPARAMS.fovy;
         lparams.nearp=TPARAMS.nearp;
         lparams.farp=TPARAMS.farp;

         lparams.reduction1=TPARAMS.reduction1;
         lparams.reduction2=TPARAMS.reduction2;

         lparams.range=TPARAMS.range;
         lparams.radius=TPARAMS.radius;
         lparams.dropoff=TPARAMS.dropoff;

         lparams.genmipmaps=TPARAMS.genmipmaps;

         lparams.sealevel=TPARAMS.sealevel;

         lparams.autocompress=TPARAMS.autocompress;
         lparams.lod0uncompressed=TPARAMS.lod0uncompressed;

         lparams.lod0uncompressed=TPARAMS.lod0uncompressed;

         lparams.proxyname=TPARAMS.proxyname;
         lparams.proxyport=TPARAMS.proxyport;

         lparams.localpath=TPARAMS.localpath;

         lparams.altpath=TPARAMS.altpath;

         // finally pass the updated layer state
         LAYER[n]->set(lparams);
         }

   // propagate waypoint parameters
   propagate_wp();

   // update color maps
   minishader::setVISbathymap(TPARAMS.bathymap,TPARAMS.bathywidth,TPARAMS.bathyheight,TPARAMS.bathycomps);
   minishader::setNPRbathymap(TPARAMS.nprbathymap,TPARAMS.nprbathywidth,TPARAMS.nprbathyheight,TPARAMS.nprbathycomps);
   }

// propagate parameters
void miniterrain::propagate()
   {set(TPARAMS);}

// propagate waypoint parameters
void miniterrain::propagate_wp()
   {
   int n;

   minilayer::MINILAYER_PARAMS *lparams;

   // propagate waypoint parameters
   if (LAYER!=NULL)
      for (n=0; n<LNUM; n++)
         {
         // get a reference to the actual layer state
         lparams=LAYER[n]->get();

         // update the waypoint state:

         lparams->usewaypoints=TPARAMS.usewaypoints;
         lparams->usebricks=TPARAMS.usebricks;

         lparams->waypoints=TPARAMS.waypoints;

         lparams->signpostheight=TPARAMS.signpostheight;
         lparams->signpostrange=TPARAMS.signpostrange;

         lparams->signpostturn=TPARAMS.signpostturn;
         lparams->signpostincline=TPARAMS.signpostincline;

         lparams->brick=TPARAMS.brick;

         lparams->bricksize=TPARAMS.bricksize;
         lparams->brickrad=TPARAMS.brickrad;

         lparams->brickpasses=TPARAMS.brickpasses;
         lparams->brickceiling=TPARAMS.brickceiling;
         lparams->brickscroll=TPARAMS.brickscroll;
         }
   }

// set internal callbacks
void miniterrain::setcallbacks(void *threaddata,
                               void (*threadinit)(int threads,int id,void *data),
                               void (*threadexit)(int id,void *data),
                               void (*startthread)(void *(*thread)(void *background),backarrayelem *background,int id,void *data),
                               void (*jointhread)(backarrayelem *background,int id,void *data),
                               void (*lock_cs)(int id,void *data),void (*unlock_cs)(int id,void *data),
                               void (*lock_io)(int id,void *data),void (*unlock_io)(int id,void *data),
                               void *curldata,
                               void (*curlinit)(int threads,int id,char *proxyname,char *proxyport,void *data),
                               void (*curlexit)(int id,void *data),
                               void (*geturl)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,int id,void *data),
                               int (*checkurl)(char *src_url,char *src_id,char *src_file,int id,void *data))
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
BOOLINT miniterrain::load(const char *url,
                          BOOLINT loadopts,BOOLINT reset)
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
      success=load(baseurl,baseid,TPARAMS.elevdir,TPARAMS.imagdir,loadopts,reset);

      free(baseid);
      }

   free(baseurl);

   return(success);
   }

// load tileset (long version)
BOOLINT miniterrain::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                          BOOLINT loadopts,BOOLINT reset)
   {
   // create layer array
   if (LAYER==NULL)
      {
      LMAX=1;
      if ((LAYER=(minilayer **)malloc(LMAX*sizeof(minilayer *)))==NULL) ERRORMSG();
      }

   // enlarge layer array
   if (LNUM>=LMAX)
      {
      LMAX*=2;
      if ((LAYER=(minilayer **)realloc(LAYER,LMAX*sizeof(minilayer *)))==NULL) ERRORMSG();
      }

   // create the render cache
   if (CACHE!=NULL) CACHE=new minicache;

   // create the tileset layer
   LAYER[LNUM++]=new minilayer(CACHE);

   // propagate parameters
   propagate();

   // register callbacks
   LAYER[LNUM-1]->setcallbacks(THREADDATA,
                               THREADINIT,THREADEXIT,
                               STARTTHREAD,JOINTHREAD,
                               LOCK_CS,UNLOCK_CS,
                               LOCK_IO,UNLOCK_IO,
                               CURLDATA,
                               CURLINIT,CURLEXIT,
                               GETURL,CHECKURL);

   // load the tileset layer
   if (!LAYER[LNUM-1]->load(baseurl,baseid,basepath1,basepath2,reset))
      {
      delete LAYER[--LNUM];
      return(FALSE);
      }

   // load optional features
   if (loadopts) LAYER[LNUM-1]->loadopts();

   // set reference coordinate system
   LAYER[LNUM-1]->setreference(LAYER[getreference()]);

   // set tile overlap
   CACHE->configure_overlap(TPARAMS.overlap);

   // set pre and post sea surface render callbacks
   CACHE->setseacb(preseacb,postseacb,this);

   // turn on ray object
   CACHE->configure_enableray(1);

   // turn on hw-accelerated OpenGL mipmap generation
   miniOGL::configure_generatemm(1);

   // turn off on-the-fly OpenGL texture compression
   miniOGL::configure_compression(0);

   // success
   return(TRUE);
   }

// get the nth terrain layer
minilayer *miniterrain::getlayer(int n)
   {
   if (n<0 || n>=LNUM) return(NULL);
   else return(LAYER[n]);
   }

// remove the nth terrain layer
void miniterrain::remove(int n)
   {
   if (n<0 || n>=LNUM) return;

   delete LAYER[n];

   if (n<LNUM-1) LAYER[n]=LAYER[--LNUM];
   else LNUM--;
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
double miniterrain::getheight(const minicoord &p)
   {
   int n;

   int nearest;

   double elev;

   if (LNUM>0)
      {
      nearest=getnearest(p);

      elev=LAYER[nearest]->getheight(p);
      if (elev!=-MAXFLOAT) return(LAYER[getreference()]->len_l2g(LAYER[nearest]->len_g2l(elev)));

      for (n=0; n<LNUM; n++)
         if (n!=nearest)
            {
            elev=LAYER[n]->getheight(p);
            if (elev!=-MAXFLOAT) return(LAYER[getreference()]->len_l2g(LAYER[n]->len_g2l(elev)));
            }
      }

   return(-MAXFLOAT);
   }

// set reference layer
void miniterrain::setreference(int ref)
   {
   int n;

   // set new reference
   LREF=ref;

   // set new reference layer for default coordinate conversions
   if (LNUM>0) REFERENCE=LAYER[getreference()];
   else REFERENCE=NULL;

   // propagate new reference coordinate system
   for (n=0; n<LNUM; n++)
      LAYER[n]->setreference(LAYER[getreference()]);
   }

// get reference layer
int miniterrain::getreference()
   {return(min(LREF,LNUM-1));}

// get initial view point
minicoord miniterrain::getinitial()
   {
   if (LNUM<1) return(minicoord());
   else return(LAYER[getreference()]->getinitial());
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

   int nearest;
   double dist,mindist;
   minicoord offset;

   nearest=-1;
   mindist=MAXFLOAT;

   for (n=0; n<LNUM; n++)
      {
      offset=LAYER[n]->getcenter()-e;
      dist=offset.vec.getLength();

      if (dist<mindist)
         {
         mindist=dist;
         nearest=n;
         }
      }

   return(nearest);
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

   for (n=0; n<LNUM; n++)
      LAYER[n]->cache(e,d,u,aspect,time);
   }

// render cached scene
void miniterrain::render()
   {
   if (LNUM>0)
      {
      // enable shaders
      if (TPARAMS.useshaders)
         if (!TPARAMS.usenprshader)
            minishader::setVISshader(CACHE,
                                     LAYER[getreference()]->len_o2g(1.0),TPARAMS.exaggeration,
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
         else
            minishader::setNPRshader(CACHE,
                                     LAYER[getreference()]->len_o2g(1.0),TPARAMS.exaggeration,
                                     (TPARAMS.usefog)?TPARAMS.fogstart/2.0f*TPARAMS.farp:0.0f,(TPARAMS.usefog)?TPARAMS.farp:0.0f,
                                     TPARAMS.fogdensity,
                                     TPARAMS.fogcolor,
                                     TPARAMS.nprbathystart,TPARAMS.nprbathyend,
                                     (TPARAMS.usecontours)?TPARAMS.nprcontours:0.0f,
                                     TPARAMS.nprfadefactor,
                                     fmax(TPARAMS.sealevel,0.0f),
                                     TPARAMS.nprseacolor,TPARAMS.nprseatrans,
                                     TPARAMS.nprseagrey);

      // render vertex arrays
      CACHE->rendercache();

      // disable shaders
      if (TPARAMS.useshaders)
         minishader::unsetshaders(CACHE);
      }
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

   minilayer::MINILAYER_PARAMS lparams;

   for (n=0; n<LNUM; n++)
      {
      LAYER[n]->get(lparams);

      el=LAYER[n]->map_g2l(lparams.eye);

      if (el.vec.z<lparams.sealevel)
         LAYER[n]->renderpoints(); // render waypoints before sea surface
      }
   }

// post sea render function
void miniterrain::render_postsea()
   {
   int n;

   minicoord el;

   minilayer::MINILAYER_PARAMS lparams;

   for (n=0; n<LNUM; n++)
      {
      LAYER[n]->get(lparams);

      el=LAYER[n]->map_g2l(lparams.eye);

      if (el.vec.z>=lparams.sealevel)
         LAYER[n]->renderpoints(); // render waypoints after sea surface
      }
   }

// determine whether or not a layer is displayed
void miniterrain::display(int n,BOOLINT yes)
   {
   if (n>=0 && n<LNUM)
      LAYER[n]->display(yes);
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
   if (LNUM>0) return(LAYER[getreference()]->getflattening());
   else return(1.0f);
   }

// shoot a ray at the scene
double miniterrain::shoot(const minicoord &o,const miniv3d &d)
   {
   int nearest;

   double dist;
   minicoord ogl;
   miniv3d dgl;

   if (LNUM<1) return(MAXFLOAT);

   // get nearest layer for highest precision
   nearest=getnearest(o);

   // transform coordinates
   ogl=LAYER[nearest]->map_g2o(o);
   dgl=LAYER[nearest]->rot_g2o(d,o);

   // shoot a ray and get the traveled distance
   dist=CACHE->getray()->shoot(ogl.vec,dgl);

   if (dist!=MAXFLOAT) dist=LAYER[nearest]->len_o2g(dist);

   return(dist);
   }

// return memory consumption
double miniterrain::getmem()
   {
   int n;

   double mem;

   mem=0.0;

   for (n=0; n<LNUM; n++)
      mem+=LAYER[n]->getterrain()->getmem();

   return(mem);
   }

// return raw texture memory consumption
double miniterrain::gettexmem()
   {
   int n;

   double texmem;

   texmem=0.0;

   for (n=0; n<LNUM; n++)
      texmem+=LAYER[n]->getterrain()->gettexmem();

   return(texmem);
   }

// get total number of pending tiles
int miniterrain::getpending()
   {
   int n;

   int pending;

   pending=0;

   for (n=0; n<LNUM; n++)
      pending+=LAYER[n]->getcache()->getpending();

   return(pending);
   }

// get total amount of cache memory
double miniterrain::getcachemem()
   {
   int n;

   double cachemem;

   cachemem=0.0;

   for (n=0; n<LNUM; n++)
      cachemem+=LAYER[n]->getcache()->getmem();

   return(cachemem);
   }
