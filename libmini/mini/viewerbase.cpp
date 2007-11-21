// (c) by Stefan Roettger

#include "minitime.h"

#include "threadbase.h"
#include "curlbase.h"
#include "squishbase.h"
#include "shaderbase.h"

#include "miniOGL.h"

#include "miniutm.h"

#include "viewerbase.h"

// default constructor
viewerbase::viewerbase()
   {
   // configurable parameters:

   PARAMS.warpmode=0;             // warp mode: plain=0 affine=1 non-linear=2

   PARAMS.scale=100.0f;           // scaling of scene
   PARAMS.exaggeration=1.0f;      // exaggeration of elevations
   PARAMS.maxelev=15000.0f;       // absolute maximum of expected elevations

   PARAMS.load=0.1f;              // initially loaded area relative to far plane
   PARAMS.preload=1.25f;          // continuously preloaded area relative to far plane

   PARAMS.minres=9.0f;            // minimum resolution of triangulation
   PARAMS.fastinit=2;             // fast initialization level
   PARAMS.avgd2value=0.1f;        // average d2value for fast initialization

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

   PARAMS.elevdir="elev";         // default elev directory
   PARAMS.imagdir="imag";         // default imag directory

   // optional features:

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

   // optional color mapping:

   PARAMS.bathystart=0.0f;      // start of colormapped elevation range
   PARAMS.bathyend=0.0f;        // end of colormapped elevation range

   PARAMS.bathymap=NULL;        // RGB[A] colormap
   PARAMS.bathywidth=0;         // width of colormap
   PARAMS.bathyheight=0;        // height of colormap
   PARAMS.bathycomps=0;         // components of colormap (either 3 or 4)

   PARAMS.contours=10.0f;       // contour distance in meters

   PARAMS.seacolor[0]=0.0f;     // color of sea surface
   PARAMS.seacolor[1]=0.5f;     // color of sea surface
   PARAMS.seacolor[2]=0.75f;    // color of sea surface

   PARAMS.seatrans=0.25f;       // transparency of sea surface
   PARAMS.seamodulate=0.0f;     // modulation factor of sea surface texture
   PARAMS.seabottom=-10.0f;     // depth where sea transparency gets saturated
   PARAMS.bottomtrans=1.0f;     // saturated transparency at sea bottom

   PARAMS.bottomcolor[0]=0.0f;  // saturated color at sea bottom
   PARAMS.bottomcolor[1]=0.1f;  // saturated color at sea bottom
   PARAMS.bottomcolor[2]=0.25f; // saturated color at sea bottom

   // optional NPR rendering:

   PARAMS.nprfadefactor=0.25f;  // NPR fade factor

   PARAMS.nprbathystart=0.0f;   // start of colormapped elevation range
   PARAMS.nprbathyend=0.0f;     // end of colormapped elevation range

   PARAMS.nprbathymap=NULL;     // RGB[A] colormap
   PARAMS.nprbathywidth=0;      // width of colormap
   PARAMS.nprbathyheight=0;     // height of colormap
   PARAMS.nprbathycomps=0;      // components of colormap (either 3 or 4)

   PARAMS.nprcontours=100.0f;   // NPR contour distance in meters

   PARAMS.nprseacolor[0]=0.5f;  // NPR color of sea surface
   PARAMS.nprseacolor[1]=0.75f; // NPR color of sea surface
   PARAMS.nprseacolor[2]=1.0f;  // NPR color of sea surface

   PARAMS.nprseatrans=0.25f;    // NPR transparency of sea surface
   PARAMS.nprseagrey=0.5f;      // NPR greyness of bathymetry

   // optional sky-dome:

   PARAMS.skydome="SkyDome.ppm"; // skydome file

   // optional way-points:

   PARAMS.waypoints="Waypoints.txt"; // waypoint file

   PARAMS.signpostturn=0.0f;     // horizontal orientation of signposts in degrees
   PARAMS.signpostincline=0.0f;  // vertical orientation of signposts in degrees

   PARAMS.signpostheight=100.0f; // height of signposts in meters
   PARAMS.signpostrange=0.1f;    // viewing range of signposts relative to far plane

   PARAMS.brick="Cone.db"; // brick file

   PARAMS.bricksize=100.0f;  // brick size in meters
   PARAMS.brickrad=1000.0f;  // brick viewing radius in meters

   PARAMS.brickpasses=4;     // brick render passes
   PARAMS.brickceiling=3.0f; // upper boundary for brick color mapping relative to first waypoint elevation
   PARAMS.brickscroll=0.5f;  // scroll period of striped bricks in seconds

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

   LAYER=NULL;

   CACHE=NULL;

   WINWIDTH=0;
   WINHEIGHT=0;

   START=minigettime();
   TIMER=0.0;
   }

// destructor
viewerbase::~viewerbase()
   {
   // delete the tileset layer
   if (LAYER!=NULL) delete LAYER;

   // delete the render cache
   if (CACHE!=NULL) delete CACHE;
   }

// get parameters
void viewerbase::get(VIEWER_PARAMS &params)
   {params=PARAMS;}

// set parameters
void viewerbase::set(VIEWER_PARAMS &params)
   {
   minilayer::MINILAYER_PARAMS lparams;

   // set new state
   PARAMS=params;

   // propagate parameters
   if (LAYER!=NULL)
      {
      // get the actual state
      LAYER->get(lparams);

      // update the state:

      lparams.warpmode=PARAMS.warpmode;

      lparams.scale=PARAMS.scale;
      lparams.exaggeration=PARAMS.exaggeration;
      lparams.maxelev=PARAMS.maxelev;

      lparams.load=PARAMS.load;
      lparams.preload=PARAMS.preload;

      lparams.minres=PARAMS.minres;
      lparams.fastinit=PARAMS.fastinit;
      lparams.avgd2value=PARAMS.avgd2value;

      lparams.lazyness=PARAMS.lazyness;
      lparams.update=PARAMS.update;
      lparams.expire=PARAMS.expire;

      lparams.upload=PARAMS.upload;
      lparams.keep=PARAMS.keep;
      lparams.maxdelay=PARAMS.maxdelay;
      lparams.cache=PARAMS.cache;

      lparams.keepalive=PARAMS.keepalive;
      lparams.timeslice=PARAMS.timeslice;

      lparams.fps=PARAMS.fps;
      lparams.spu=PARAMS.spu;

      lparams.res=PARAMS.res;

      lparams.fovy=PARAMS.fovy;
      lparams.nearp=PARAMS.nearp;
      lparams.farp=PARAMS.farp;

      lparams.reduction1=PARAMS.reduction1;
      lparams.reduction2=PARAMS.reduction2;

      lparams.range=PARAMS.range;
      lparams.radius=PARAMS.radius;
      lparams.dropoff=PARAMS.dropoff;

      lparams.sealevel=PARAMS.sealevel;

      lparams.autocompress=PARAMS.autocompress;
      lparams.lod0uncompressed=PARAMS.lod0uncompressed;

      // optional features:

      lparams.usewaypoints=PARAMS.usewaypoints;
      lparams.usebricks=PARAMS.usebricks;

      // optional way-points:

      lparams.waypoints=PARAMS.waypoints;

      lparams.signpostheight=PARAMS.signpostheight;
      lparams.signpostrange=PARAMS.signpostrange;

      lparams.signpostturn=PARAMS.signpostturn;
      lparams.signpostincline=PARAMS.signpostincline;

      lparams.brick=PARAMS.brick;

      lparams.bricksize=PARAMS.bricksize;
      lparams.brickrad=PARAMS.brickrad;

      lparams.brickpasses=PARAMS.brickpasses;
      lparams.brickceiling=PARAMS.brickceiling;
      lparams.brickscroll=PARAMS.brickscroll;

      // finally pass the updated state
      LAYER->set(lparams);
      }

   // update color maps
   shaderbase::setVISbathymap(PARAMS.bathymap,PARAMS.bathywidth,PARAMS.bathyheight,PARAMS.bathycomps);
   shaderbase::setNPRbathymap(PARAMS.nprbathymap,PARAMS.nprbathywidth,PARAMS.nprbathyheight,PARAMS.nprbathycomps);
   }

// http receiver
void viewerbase::receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data)
   {
   if (data!=NULL) ERRORMSG();
   curlbase::getURL(src_url,src_id,src_file,dst_file,background);
   }

// http checker
int viewerbase::check_callback(char *src_url,char *src_id,char *src_file,void *data)
   {
   if (data!=NULL) ERRORMSG();
   return(curlbase::checkURL(src_url,src_id,src_file));
   }

// S3TC auto-compression hook
void viewerbase::autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                              unsigned char **s3tcdata,unsigned int *s3tcbytes,
                              databuf *obj,void *data)
   {
   if (data!=NULL) ERRORMSG();

   squishbase::compressS3TC(isrgbadata,rawdata,bytes,
                            s3tcdata,s3tcbytes,obj->xsize,obj->ysize);
   }

// load tileset (short version)
BOOLINT viewerbase::load(const char *url,
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
BOOLINT viewerbase::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                         BOOLINT reset)
   {
   if (LAYER!=NULL) ERRORMSG();

   // register conversion hook (JPEG/PNG)
   convbase::setconversion(&PARAMS.conversion_params);

   // register auto-compression hook
   databuf::setautocompress(autocompress,NULL);

   // turn on on-the-fly OpenGL mipmap generation
   miniOGL::configure_generatemm(1);

   // turn off on-the-fly OpenGL texture compression
   miniOGL::configure_compression(0);

   // create the render cache
   CACHE=new minicache;

   // create the tileset layer
   LAYER=new minilayer(1,CACHE);

   // propagate parameters
   propagate();

   // register callbacks
   LAYER->setcallbacks(NULL,
                       threadbase::threadinit,threadbase::threadexit,
                       threadbase::startthread,threadbase::jointhread,
                       threadbase::lock_cs,threadbase::unlock_cs,
                       threadbase::lock_io,threadbase::unlock_io,
                       NULL,
                       curlbase::curlinit,curlbase::curlexit,
                       receive_callback,check_callback);

   // load the tileset layer
   if (!LAYER->load(baseurl,baseid,basepath1,basepath2,reset)) return(FALSE);

   // set pre and post sea surface render callbacks
   CACHE->setseacb(preseacb,postseacb,this);

   // turn on ray object
   CACHE->configure_enableray(1);

   // success
   return(TRUE);
   }

// load optional features
void viewerbase::loadopts()
   {
   minilayer::MINILAYER_PARAMS lparams;

   if (LAYER==NULL) ERRORMSG();

   LAYER->loadopts();

   LAYER->get(lparams);

   // load skydome:

   char *skyname=LAYER->getcache()->getfile(PARAMS.skydome,lparams.altpath);

   if (skyname!=NULL)
      {
      skydome.loadskydome(skyname);
      free(skyname);
      }

   // load earth textures:

   char *ename1=LAYER->getcache()->getfile(PARAMS.frontname,lparams.altpath);

   if (ename1!=NULL)
      {
      earth.configure_frontname(ename1);
      free(ename1);
      }

   char *ename2=LAYER->getcache()->getfile(PARAMS.backname,lparams.altpath);

   if (ename2!=NULL)
      {
      earth.configure_backname(ename2);
      free(ename2);
      }
   }

// get extent of tileset
miniv3d viewerbase::getextent()
   {
   if (LAYER==NULL) ERRORMSG();
   return(LAYER->getextent());
   }

// get center of tileset
minicoord viewerbase::getcenter()
   {
   if (LAYER==NULL) ERRORMSG();
   return(LAYER->getcenter());
   }

// get the elevation at position (x,y,z)
double viewerbase::getheight(const minicoord &p)
   {
   if (LAYER==NULL) ERRORMSG();
   return(LAYER->getheight(p));
   }

// get initial view point
minicoord viewerbase::getinitial()
   {
   if (LAYER==NULL) ERRORMSG();
   return(LAYER->getinitial());
   }

// set initial eye point
void viewerbase::initeyepoint(const minicoord &e)
   {
   if (LAYER==NULL) ERRORMSG();
   LAYER->initeyepoint(e);
   }

// set render window
void viewerbase::setwinsize(int width,int height)
   {
   WINWIDTH=width;
   WINHEIGHT=height;
   }

// trigger complete render buffer update at next frame
void viewerbase::update()
   {
   if (LAYER==NULL) return;
   LAYER->update();
   }

// generate and cache scene for a particular eye point
void viewerbase::cache(const minicoord &e,const miniv3d &d,const miniv3d &u)
   {
   if (LAYER==NULL) ERRORMSG();

   if (WINWIDTH>0 && WINHEIGHT>0)
      LAYER->cache(e,d,u,(float)WINWIDTH/WINHEIGHT,
                   gettime());
   }

// render cached scene
void viewerbase::render()
   {
   minicoord ei;

   minilayer::MINILAYER_PARAMS lparams;

   GLfloat color[4];

   double mtx[16]={1.0,0.0,0.0,0.0,
                   0.0,1.0,0.0,0.0,
                   0.0,0.0,1.0,0.0,
                   0.0,-len_e2i(miniutm::WGS84_r_minor),0.0,1.0};

   float light0[3]={0.0f,0.0f,0.0f};

   if (LAYER==NULL) ERRORMSG();

   if (WINWIDTH<=0 || WINHEIGHT<=0) return;

   LAYER->get(lparams);

   ei=map_e2i(lparams.eye);

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
      glFogf(GL_FOG_START,PARAMS.fogstart*len_e2i(PARAMS.farp));
      glFogf(GL_FOG_END,len_e2i(PARAMS.farp));

      glEnable(GL_FOG);
      }

   // draw skydome
   if (PARAMS.useskydome)
      {
      skydome.setpos(ei.vec.x,ei.vec.y,ei.vec.z,
                     1.9*len_e2i(PARAMS.farp));

      skydome.drawskydome();
      }

   // render earth globe (without Z writing)
   if (PARAMS.useearth)
      {
      earth.setscale(len_i2e(1.0));
      earth.setmatrix(mtx);

      if (PARAMS.usediffuse) earth.settexturedirectparams(PARAMS.lightdir,PARAMS.transition);
      else earth.settexturedirectparams(light0,PARAMS.transition);

      earth.setfogparams((PARAMS.usefog)?PARAMS.fogstart/2.0f*len_e2i(PARAMS.farp):0.0f,(PARAMS.usefog)?len_e2i(PARAMS.farp):0.0f,
                         PARAMS.fogdensity,
                         PARAMS.fogcolor);

      earth.render(MINIGLOBE_FIRST_RENDER_PHASE);
      }

   // enable shaders
   if (PARAMS.useshaders)
      if (!PARAMS.usenprshader)
         shaderbase::setVISshader(CACHE,
                                  len_i2e(1.0),PARAMS.exaggeration,
                                  (PARAMS.usefog)?PARAMS.fogstart/2.0f*PARAMS.farp:0.0f,(PARAMS.usefog)?PARAMS.farp:0.0f,
                                  PARAMS.fogdensity,
                                  PARAMS.fogcolor,
                                  (PARAMS.usebathymap)?PARAMS.bathystart:PARAMS.bathyend,PARAMS.bathyend,
                                  (PARAMS.usecontours)?PARAMS.contours:0.0f,
                                  PARAMS.sealevel,PARAMS.seabottom,
                                  PARAMS.seacolor,
                                  PARAMS.seatrans,PARAMS.bottomtrans,
                                  PARAMS.bottomcolor,
                                  PARAMS.seamodulate);
      else
         shaderbase::setNPRshader(CACHE,
                                  len_i2e(1.0),PARAMS.exaggeration,
                                  (PARAMS.usefog)?PARAMS.fogstart/2.0f*PARAMS.farp:0.0f,(PARAMS.usefog)?PARAMS.farp:0.0f,
                                  PARAMS.fogdensity,
                                  PARAMS.fogcolor,
                                  PARAMS.nprbathystart,PARAMS.nprbathyend,
                                  (PARAMS.usecontours)?PARAMS.nprcontours:0.0f,
                                  PARAMS.nprfadefactor,
                                  fmax(PARAMS.sealevel,0.0f),
                                  PARAMS.nprseacolor,PARAMS.nprseatrans,
                                  PARAMS.nprseagrey);

   // render vertex arrays
   // assuming that the OpenGL state is already entirely set
   CACHE->rendercache();

   // disable shaders
   if (PARAMS.useshaders)
      shaderbase::unsetshaders(CACHE);

   // render earth globe (without RGB writing)
   if (PARAMS.useearth) earth.render(MINIGLOBE_LAST_RENDER_PHASE);

   // disable fog
   if (PARAMS.usefog) glDisable(GL_FOG);

   // disable wireframe mode
   if (PARAMS.usewireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   }

// pre sea render callback
void viewerbase::preseacb(void *data)
   {
   viewerbase *viewer=(viewerbase *)data;
   viewer->render_presea();
   }

// post sea render callback
void viewerbase::postseacb(void *data)
   {
   viewerbase *viewer=(viewerbase *)data;
   viewer->render_postsea();
   }

// pre sea render function
void viewerbase::render_presea()
   {
   minicoord ei;

   minilayer::MINILAYER_PARAMS lparams;

   LAYER->get(lparams);

   ei=map_e2l(lparams.eye);

   if (ei.vec.z<lparams.sealevel)
      LAYER->renderpoints(); // render waypoints before sea surface
   }

// post sea render function
void viewerbase::render_postsea()
   {
   minicoord ei;

   minilayer::MINILAYER_PARAMS lparams;

   LAYER->get(lparams);

   ei=map_e2l(lparams.eye);

   if (ei.vec.z>=lparams.sealevel)
      LAYER->renderpoints(); // render waypoints after sea surface
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

// flatten the scene by a relative scaling factor (in the range [0-1])
void viewerbase::flatten(float relscale)
   {
   if (LAYER==NULL) ERRORMSG();
   LAYER->flatten(relscale);
   }

// shoot a ray at the scene
double viewerbase::shoot(const minicoord &o,const miniv3d &d)
   {
   double dist;
   minicoord oi;
   miniv3d di;

   // transform coordinates
   oi=map_e2i(o);
   di=rot_e2i(d,o);

   // shoot a ray and get the traveled distance
   dist=CACHE->getray()->shoot(oi.vec,di);

   if (dist!=MAXFLOAT) dist=len_i2e(dist);

   return(dist);
   }
