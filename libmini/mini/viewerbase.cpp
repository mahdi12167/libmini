// (c) by Stefan Roettger

#undef GREYCSTORATION // enable this for greycstoration support

#include "minitime.h"

#include "database.h"
#include "datacloud.h"
#include "datacache.h"

#include "threadbase.h"
#include "curlbase.h"
#include "jpegbase.h"
#include "squishbase.h"
#include "shaderbase.h"

#include "greycbase.h"

#include "miniOGL.h"

#include "miniutm.h"

#include "viewerbase.h"

// default constructor
viewerbase::viewerbase()
   {
   // auto-determined parameters upon load:

   PARAMS.cols=0;           // number of columns per tileset
   PARAMS.rows=0;           // number of rows per tileset

   PARAMS.basesize=0;       // base size of texture maps

   PARAMS.usepnm=FALSE;     // use either PNM or DB loader

   PARAMS.extent[0]=0.0f;   // x-extent of tileset
   PARAMS.extent[1]=0.0f;   // y-extent of tileset
   PARAMS.extent[2]=0.0f;   // z-extent of tileset

   PARAMS.offset1[0]=0.0f;  // external2local x-offset of tileset center
   PARAMS.offset1[1]=0.0f;  // external2local y-offset of tileset center
   PARAMS.offset1[2]=0.0f;  // external2local z-offset of tileset center

   PARAMS.scaling1[0]=0.0f; // external2local x-scaling factor of tileset
   PARAMS.scaling1[1]=0.0f; // external2local y-scaling factor of tileset
   PARAMS.scaling1[2]=0.0f; // external2local z-scaling factor of tileset

   PARAMS.offset2[0]=0.0f;  // local2internal x-offset of tileset center
   PARAMS.offset2[1]=0.0f;  // local2internal y-offset of tileset center
   PARAMS.offset2[2]=0.0f;  // local2internal z-offset of tileset center

   PARAMS.scaling2[0]=0.0f; // local2internal x-scaling factor of tileset
   PARAMS.scaling2[1]=0.0f; // local2internal y-scaling factor of tileset
   PARAMS.scaling2[2]=0.0f; // local2internal z-scaling factor of tileset

   PARAMS.maxelev=15000.0f; // absolute maximum of expected elevations

   // configurable parameters:

   PARAMS.scale=100.0f;           // scaling of scene
   PARAMS.exaggeration=1.0f;      // exaggeration of elevation

   PARAMS.load=0.1f;              // initially loaded area relative to far plane
   PARAMS.preload=1.25f;          // continuously preloaded area relative to far plane

   PARAMS.minres=16.0f;           // minimum resolution of triangulation
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

   PARAMS.res=1.0E6f;             // global resolution of triangulation

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
   PARAMS.fogdensity=1.0f; // relative fog density

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
   PARAMS.nprseagrey=0.25f;     // NPR greyness of bathymetry

   // optional sky-dome:

   PARAMS.skydome=strdup("SkyDome.ppm"); // skydome file

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

   // optional earth globe:

   PARAMS.lightdir[0]=0.0f; // directional light
   PARAMS.lightdir[1]=1.0f; // directional light
   PARAMS.lightdir[2]=0.0f; // directional light

   PARAMS.transition=4.0f;  // transition gradient between night and day

   PARAMS.frontname=strdup("EarthDay.ppm");  // file name of front earth texture
   PARAMS.backname=strdup("EarthNight.ppm"); // file name of back earth texture

   // image processing parameters:

   PARAMS.jpeg_quality=75.0f; // jpeg quality in percent

   PARAMS.usegreycstoration=FALSE; // use greycstoration for image denoising

   PARAMS.greyc_p=0.8f; // greycstoration sharpness, useful range=[0.7-0.9]
   PARAMS.greyc_a=0.4f; // greycstoration anisotropy, useful range=[0.1-0.5]

   // initialize state:

   PARAMS0=PARAMS;

   LOADED=FALSE;

   TERRAIN=NULL;
   CACHE=NULL;

   TILECACHE=NULL;

   WINWIDTH=0;
   WINHEIGHT=0;

   UPD=1;

   START=minigettime();
   TIMER=0.0;

   IEX=IEY=IEZ=0.0f;
   IDX=IDY=IDZ=0.0f;
   }

// destructor
viewerbase::~viewerbase()
   {
   if (LOADED)
      {
      // delete the tile cache
      delete TILECACHE;

      // delete the terrain and its render cache
      delete TERRAIN;
      delete CACHE;

      // clean-up pthreads and libcurl
      threadbase::threadexit();
      curlbase::curlexit();
      }

   // delete strings:

   if (PARAMS.tilesetfile!=NULL) free(PARAMS.tilesetfile);
   if (PARAMS.vtbinisuffix!=NULL) free(PARAMS.vtbinisuffix);
   if (PARAMS.startupfile!=NULL) free(PARAMS.startupfile);

   if (PARAMS.localpath!=NULL) free(PARAMS.localpath);
   if (PARAMS.altpath!=NULL) free(PARAMS.altpath);

   if (PARAMS.skydome!=NULL) free(PARAMS.skydome);

   if (PARAMS.frontname!=NULL) free(PARAMS.frontname);
   if (PARAMS.backname!=NULL) free(PARAMS.backname);
   }

// get parameters
void viewerbase::get(VIEWER_PARAMS &params)
   {params=PARAMS;}

// set parameters
void viewerbase::set(VIEWER_PARAMS &params)
   {
   // set new state
   PARAMS=params;

   // delete unused strings:

   if (PARAMS.tilesetfile!=PARAMS0.tilesetfile) if (PARAMS0.tilesetfile!=NULL) free(PARAMS0.tilesetfile);
   if (PARAMS.vtbinisuffix!=PARAMS0.vtbinisuffix) if (PARAMS0.vtbinisuffix!=NULL) free(PARAMS0.vtbinisuffix);
   if (PARAMS.startupfile!=PARAMS0.startupfile) if (PARAMS0.startupfile!=NULL) free(PARAMS0.startupfile);

   if (PARAMS.localpath!=PARAMS0.localpath) if (PARAMS0.localpath!=NULL) free(PARAMS0.localpath);
   if (PARAMS.altpath!=PARAMS0.altpath) if (PARAMS0.altpath!=NULL) free(PARAMS0.altpath);

   if (PARAMS.skydome!=PARAMS0.skydome) if (PARAMS0.skydome!=NULL) free(PARAMS0.skydome);

   if (PARAMS.frontname!=PARAMS0.frontname) if (PARAMS0.frontname!=NULL) free(PARAMS0.frontname);
   if (PARAMS.backname!=PARAMS0.backname) if (PARAMS0.backname!=NULL) free(PARAMS0.backname);

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

   shaderbase::setVISbathymap(PARAMS.bathymap,PARAMS.bathywidth,PARAMS.bathyheight,PARAMS.bathycomps);
   shaderbase::setNPRbathymap(PARAMS.nprbathymap,PARAMS.nprbathywidth,PARAMS.nprbathyheight,PARAMS.nprbathycomps);

   // overwrite old state
   PARAMS0=PARAMS;
   }

// get the elevation at position (x,y) in external coords
float viewerbase::getheight(double x,double y)
   {
   double h;
   float lx,ly,lz;
   float ix,iy,iz;

   map_e2l(x,y,0.0f,lx,ly,lz);
   map_l2i(lx,ly,lz,ix,iy,iz);

   iy=TERRAIN->getheight(ix,iz);

   if (iy==-MAXFLOAT) return(iy);

   map_i2l(ix,iy,iz,lx,ly,lz);
   map_l2e(lx,ly,lz,x,y,h);

   return(h);
   }

// load requested data
void viewerbase::request_callback(char *file,int istexture,databuf *buf,void *data)
   {
   viewerbase *obj=(viewerbase *)data;

   if (!obj->PARAMS.usepnm) buf->loaddata(file);
   else buf->loadPNMdata(file);
   }

// http receiver
void viewerbase::receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data)
   {curlbase::getURL(src_url,src_id,src_file,dst_file,background);}

// http checker
int viewerbase::check_callback(char *src_url,char *src_id,char *src_file,void *data)
   {return(curlbase::checkURL(src_url,src_id,src_file));}

// conversion hook for external formats (e.g. JPEG/PNG)
void viewerbase::conversionhook(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,
                                unsigned char **newdata,unsigned int *newbytes,
                                databuf *obj,void *data)
   {
   viewerbase *viewer=(viewerbase *)data;

   switch (extformat)
      {
      case 1: // JPEG

         if (israwdata==0)
            {
            int width,height,components;

            *newdata=jpegbase::decompressJPEGimage(srcdata,bytes,&width,&height,&components);
            if ((unsigned int)width!=obj->xsize || (unsigned int)height!=obj->ysize) ERRORMSG();

            switch (components)
               {
               case 3:
                  if (obj->type!=3) ERRORMSG();
                  break;
               case 4:
                  if (obj->type!=4) ERRORMSG();
                  break;
               default: ERRORMSG();
               }

            *newbytes=width*height*components;
            }
         else
            {
            int components;

            switch (obj->type)
               {
               case 1:
                  components=1;
                  break;
               case 3:
                  components=3;
                  break;
               case 4:
                  components=4;
                  break;
               default: ERRORMSG();
               }

            if (viewer->PARAMS.usegreycstoration)
               greycbase::denoiseGREYCimage(srcdata,obj->xsize,obj->ysize,viewer->PARAMS.greyc_p,viewer->PARAMS.greyc_a);

            jpegbase::compressJPEGimage(srcdata,obj->xsize,obj->ysize,components,viewer->PARAMS.jpeg_quality/100.0f,newdata,newbytes);
            }

         break;

      case 2: // PNG

         if (israwdata==0)
            ERRORMSG(); //!! not yet implemented
         else
            switch (obj->type)
               {
               case 1:
                  ERRORMSG(); //!! not yet implemented
                  break;
               case 2:
                  ERRORMSG(); //!! not yet implemented
                  break;
               case 3:
                  ERRORMSG(); //!! not yet implemented
                  break;
               case 4:
                  ERRORMSG(); //!! not yet implemented
                  break;
               default: ERRORMSG();
               }

         break;

      default: ERRORMSG();
      }
   }

// S3TC auto-compression hook
void viewerbase::autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,
                              unsigned char **s3tcdata,unsigned int *s3tcbytes,
                              databuf *obj,void *data)
   {squishbase::compressS3TC(isrgbadata,rawdata,bytes,s3tcdata,s3tcbytes,obj->xsize,obj->ysize);}

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
         baseid=concat(++lastslash,"/");
         *lastslash='\0';
         }
      else
         {
         baseid=concat(++lastbslash,"\\");
         *lastbslash='\0';
         }

      // load tileset
      success=load(baseurl,baseid,"elev","imag",reset);

      free(baseid);
      }

   free(baseurl);

   return(success);
   }

// load tileset (long version)
BOOLINT viewerbase::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                         BOOLINT reset)
   {
   int success;

   float outparams[5];
   float outscale[2];

   char *vtbelevinifile,*vtbimaginifile;

   if (LOADED) return(FALSE);

   LOADED=TRUE;

   // register conversion hook (JPEG/PNG decompression)
   databuf::setconversion(conversionhook,this);

   // register auto-compression hook
   databuf::setautocompress(autocompress,NULL);

   // turn on on-the-fly OpenGL mipmap generation
   miniOGL::configure_generatemm(1);

   // turn off on-the-fly OpenGL texture compression
   miniOGL::configure_compression(0);

   // create the terrain and its render cache
   TERRAIN=new miniload;
   CACHE=new minicache;

   // concatenate vtb ini file names
   vtbelevinifile=concat(basepath1,PARAMS.vtbinisuffix);
   vtbimaginifile=concat(basepath2,PARAMS.vtbinisuffix);

   // attach the tile cache
   TILECACHE=new datacache(TERRAIN);
   TILECACHE->settilesetfile(PARAMS.tilesetfile);
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
   TILECACHE->getcloud()->setthread(threadbase::startthread,NULL,threadbase::jointhread,threadbase::lock_cs,threadbase::unlock_cs,threadbase::lock_io,threadbase::unlock_io);
   TILECACHE->getcloud()->configure_autocompress(PARAMS.autocompress);
   TILECACHE->getcloud()->configure_lod0uncompressed(PARAMS.lod0uncompressed);
   TILECACHE->getcloud()->configure_keepalive(PARAMS.keepalive);
   TILECACHE->getcloud()->configure_timeslice(PARAMS.timeslice);
   TILECACHE->configure_locthreads(PARAMS.locthreads);
   TILECACHE->configure_netthreads(PARAMS.numthreads);
   TILECACHE->setremoteid(baseid);
   TILECACHE->setremoteurl(baseurl);
   TILECACHE->setlocalpath(PARAMS.localpath);
   TILECACHE->setreceiver(receive_callback,NULL,check_callback);

   // free vtb ini file names
   free(vtbelevinifile);
   free(vtbimaginifile);

   // initialize pthreads and libcurl
   threadbase::threadinit(PARAMS.numthreads);
   curlbase::curlinit(PARAMS.numthreads);

   // load persistent startup file
   TILECACHE->load();

   // reset startup state
   if (reset)
      {
      TILECACHE->reset();
      TILECACHE->load();
      }

   // check tileset info
   if (TILECACHE->hasinfo())
      {
      // set size of tileset
      PARAMS.cols=TILECACHE->getinfo_tilesx();
      PARAMS.rows=TILECACHE->getinfo_tilesy();

      // set local offset of tileset center
      PARAMS.offset1[0]=TILECACHE->getinfo_centerx();
      PARAMS.offset1[1]=TILECACHE->getinfo_centery();

      // set base size of textures
      PARAMS.basesize=TILECACHE->getinfo_maxsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(PARAMS.basesize);

      // use PNM loader
      PARAMS.usepnm=TRUE;
      }
   // check imag tileset ini
   else if (TILECACHE->hasimagini())
      {
      // set size of tileset
      PARAMS.cols=TILECACHE->getimagini_tilesx();
      PARAMS.rows=TILECACHE->getimagini_tilesy();

      // set local offset of tileset center
      PARAMS.offset1[0]=TILECACHE->getimagini_centerx();
      PARAMS.offset1[1]=TILECACHE->getimagini_centery();

      // set base size of textures
      PARAMS.basesize=TILECACHE->getimagini_maxtexsize();
      TILECACHE->getcloud()->getterrain()->setbasesize(PARAMS.basesize);

      // use DB loader
      PARAMS.usepnm=FALSE;
      }
   // check elev tileset ini
   else if (TILECACHE->haselevini())
      {
      // set size of tileset
      PARAMS.cols=TILECACHE->getelevini_tilesx();
      PARAMS.rows=TILECACHE->getelevini_tilesy();

      // set local offset of tileset center
      PARAMS.offset1[0]=TILECACHE->getelevini_centerx();
      PARAMS.offset1[1]=TILECACHE->getelevini_centery();

      // set base size of textures
      TILECACHE->getcloud()->getterrain()->setbasesize(PARAMS.basesize);

      // use DB loader
      PARAMS.usepnm=FALSE;
      }

   // check the size of the tileset to detect load errors
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
                         -PARAMS.offset1[0],-PARAMS.offset1[1], // horizontal offset
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
   PARAMS.extent[2]=outparams[4];

   // set local offset of tileset center
   PARAMS.offset1[0]+=outparams[2];
   PARAMS.offset1[1]+=-outparams[3];

   // set local scaling factor of tileset
   PARAMS.scaling1[0]=outscale[0];
   PARAMS.scaling1[1]=outscale[1];
   PARAMS.scaling1[2]=1.0f/PARAMS.scale;

   // set internal scaling factor of tileset
   PARAMS.scaling2[0]=1.0f;
   PARAMS.scaling2[1]=-1.0f;
   PARAMS.scaling2[2]=1.0f;

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
                       outparams[2],0.0f,-outparams[3]); // origin with negative Z

   // set pre and post sea surface render callbacks
   CACHE->setseacb(preseacb,postseacb,this);

   // turn on ray object
   CACHE->configure_enableray(1);

   return(TRUE);
   }

// load optional features
void viewerbase::loadopts()
   {
   // load skydome:

   char *skyname=TILECACHE->getfile(PARAMS.skydome,PARAMS.altpath);

   if (skyname!=NULL)
      {
      skydome.loadskydome(skyname);
      free(skyname);
      }

   // load waypoints:

   char *wpname=TILECACHE->getfile(PARAMS.waypoints,PARAMS.altpath);

   if (wpname!=NULL)
      {
      if (!PARAMS.usepnm) points.configure_automap(1);

      points.load(wpname,-PARAMS.offset1[1],-PARAMS.offset1[0],PARAMS.scaling1[0],PARAMS.scaling1[1],PARAMS.exaggeration/PARAMS.scale,TERRAIN->getminitile());
      free(wpname);

      points.configure_brickceiling(PARAMS.brickceiling*points.getfirst()->elev*PARAMS.scale/PARAMS.exaggeration);
      points.configure_brickpasses(PARAMS.brickpasses);
      }

   // load brick data:

   char *bname=TILECACHE->getfile(PARAMS.brick,PARAMS.altpath);

   if (bname==NULL) PARAMS.usebricks=FALSE;
   else
      {
      points.setbrick(bname);
      free(bname);
      }

   // load earth textures:

   char *ename1=TILECACHE->getfile(PARAMS.frontname,PARAMS.altpath);

   if (ename1!=NULL)
      {
      earth.configure_frontname(ename1);
      free(ename1);
      }

   char *ename2=TILECACHE->getfile(PARAMS.backname,PARAMS.altpath);

   if (ename2!=NULL)
      {
      earth.configure_backname(ename2);
      free(ename2);
      }
   }

// set render window
void viewerbase::setwinsize(int width,int height)
   {
   WINWIDTH=width;
   WINHEIGHT=height;
   }

// set initial eye point in external coords
void viewerbase::initeyepoint(double ex,double ey,double ez)
   {
   float lex,ley,lez;
   float iex,iey,iez;

   map_e2l(ex,ey,ez,lex,ley,lez);
   map_l2i(lex,ley,lez,iex,iey,iez);

   TERRAIN->restrictroi(iex,iez,PARAMS.load*PARAMS.farp/PARAMS.scale);

   TERRAIN->updateroi(PARAMS.res,
                      iex,iey+1000*PARAMS.farp/PARAMS.scale,iez,
                      iex,iez,PARAMS.farp/PARAMS.scale);

   update();
   }

// trigger complete render buffer update at next frame
void viewerbase::update()
   {UPD=1;}

// generate and cache scene for a particular eye point in external coords
void viewerbase::cache(double ex,double ey,double ez,
                       float dx,float dy,float dz,
                       float ux,float uy,float uz)
   {
   float lex,ley,lez;
   float ldx,ldy,ldz;
   float lux,luy,luz;

   float iex,iey,iez;
   float idx,idy,idz;
   float iux,iuy,iuz;

   if (!LOADED) return;

   if (WINWIDTH<=0 || WINHEIGHT<=0) return;

   // transform coordinates
   map_e2l(ex,ey,ez,lex,ley,lez);
   map_l2i(lex,ley,lez,iex,iey,iez);
   rot_e2l(dx,dy,dz,ldx,ldy,ldz);
   rot_l2i(ldx,ldy,ldz,idx,idy,idz);
   rot_e2l(ux,uy,uz,lux,luy,luz);
   rot_l2i(lux,luy,luz,iux,iuy,iuz);

   // update vertex arrays
   TERRAIN->draw(PARAMS.res,
                 iex,iey,iez,
                 idx,idy,idz,
                 iux,iuy,iuz,
                 PARAMS.fovy,(float)WINWIDTH/WINHEIGHT,
                 PARAMS.nearp/PARAMS.scale,PARAMS.farp/PARAMS.scale,
                 UPD);

   // revert to normal render buffer update
   UPD=ftrc(ffloor(PARAMS.spu*PARAMS.fps))+1;

   // save actual internal eye point
   IEX=iex;
   IEY=iey;
   IEZ=iez;

   // save actual internal viewing direction
   IDX=idx;
   IDY=idy;
   IDZ=idz;
   }

// render cached scene
void viewerbase::render()
   {
   float time;

   GLfloat color[4];

   double mtx[16]={1,0,0,0,
                   0,1,0,0,
                   0,0,1,0,
                   0,-miniutm::WGS84_r_minor/PARAMS.scale,0,1};

   float light0[3]={0.0f,0.0f,0.0f};

   if (!LOADED) return;

   if (WINWIDTH<=0 || WINHEIGHT<=0) return;

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
      glFogf(GL_FOG_START,PARAMS.fogstart*PARAMS.farp/PARAMS.scale);
      glFogf(GL_FOG_END,PARAMS.farp/PARAMS.scale);

      glEnable(GL_FOG);
      }

   // draw skydome
   if (PARAMS.useskydome)
      {
      skydome.setpos(IEX,IEY,IEZ,
                     1.9f*PARAMS.farp/PARAMS.scale);

      skydome.drawskydome();
      }

   // render earth globe (without Z writing)
   if (PARAMS.useearth)
      {
      earth.setscale(PARAMS.scale);
      earth.setmatrix(mtx);

      if (PARAMS.usediffuse) earth.settexturedirectparams(PARAMS.lightdir,PARAMS.transition);
      else earth.settexturedirectparams(light0,PARAMS.transition);

      earth.setfogparams((PARAMS.usefog)?PARAMS.fogstart/2.0f*PARAMS.farp/PARAMS.scale:0.0f,(PARAMS.usefog)?PARAMS.farp/PARAMS.scale:0.0f,
                         PARAMS.fogdensity,
                         PARAMS.fogcolor);

      earth.render(MINIGLOBE_FIRST_RENDER_PHASE);
      }

   // enable shaders
   if (PARAMS.useshaders)
      if (!PARAMS.usenprshader)
         shaderbase::setVISshader(CACHE,
                                  PARAMS.scale,PARAMS.exaggeration,
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
                                  PARAMS.scale,PARAMS.exaggeration,
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

   // draw waypoints
   if (PARAMS.usewaypoints)
      if (!PARAMS.usebricks)
         points.drawsignposts(IEX,IEY,-IEZ,PARAMS.signpostheight/PARAMS.scale,PARAMS.signpostrange*PARAMS.farp/PARAMS.scale,PARAMS.signpostturn,PARAMS.signpostincline);
      else
         {
         time=PARAMS.brickscroll*gettime();
         points.configure_brickstripes(time-floor(time));
         points.drawbricks(IEX,IEY,-IEZ,PARAMS.brickrad/PARAMS.scale,PARAMS.farp/PARAMS.scale,PARAMS.fovy,(float)WINWIDTH/WINHEIGHT,PARAMS.bricksize/PARAMS.scale);
         }

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
void viewerbase::render_presea() {}

// post sea render function
void viewerbase::render_postsea() {}

// get time
float viewerbase::gettime()
   {return(minigettime()-START);}

// start timer
void viewerbase::starttimer()
   {TIMER=minigettime()-START;}

// measure timer
float viewerbase::gettimer()
   {return(minigettime()-START-TIMER);}

// idle for the remainder of the frame
void viewerbase::idle(float dt)
   {miniwaitfor(1.0f/PARAMS.fps-dt);}

// get extent of tileset in local coords
void viewerbase::getextent(float &sx,float &sy)
   {
   sx=PARAMS.extent[0];
   sy=PARAMS.extent[1];
   }

// get extent of tileset in local coords
void viewerbase::getextent(float &sx,float &sy,float &sz)
   {
   sx=PARAMS.extent[0];
   sy=PARAMS.extent[1];
   sz=2.0f*fmin(PARAMS.extent[2],PARAMS.maxelev*PARAMS.exaggeration/PARAMS.scale);
   }

// get center of tileset in external coords
void viewerbase::getcenter(double &cx,double &cy)
   {
   cx=PARAMS.offset1[0];
   cy=PARAMS.offset1[1];
   }

// get initial view point in external coords
void viewerbase::getinitial(double &vx,double &vy)
   {
   if (points.getfirst()==NULL)
      {
      vx=PARAMS.offset1[0];
      vy=PARAMS.offset1[1];
      }
   else
      {
      vx=(double)points.getfirst()->x/PARAMS.scaling1[0]+PARAMS.offset1[0];
      vy=(double)points.getfirst()->y/PARAMS.scaling1[1]+PARAMS.offset1[1];
      }
   }

// flatten the scene by a relative scaling factor (in the range [0-1])
void viewerbase::flatten(float relscale)
   {TERRAIN->setrelscale(relscale);}

// shoot a ray at the scene
float viewerbase::shoot(double ox,double oy,double oz,
                        float dx,float dy,float dz)
   {
   float dist;

   float lox,loy,loz;
   float ldx,ldy,ldz;

   float iox,ioy,ioz;
   float idx,idy,idz;

   // transform coordinates
   map_e2l(ox,oy,oz,lox,loy,loz);
   map_l2i(lox,loy,loz,iox,ioy,ioz);
   rot_e2l(dx,dy,dz,ldx,ldy,ldz);
   rot_l2i(ldx,ldy,ldz,idx,idy,idz);

   dist=CACHE->getray()->shoot(miniv3f(iox,ioy,ioz),miniv3f(idx,idy,idz));

   if (dist!=MAXFLOAT) dist=len_i2e(dist);

   return(dist);
   }

// move a point on a ray
void viewerbase::move(float dist,
                      double ox,double oy,double oz,
                      float dx,float dy,float dz,
                      double &mx,double &my,double &mz)
   {
   float lox,loy,loz;
   float ldx,ldy,ldz;

   // transform coordinates
   map_e2l(ox,oy,oz,lox,loy,loz);
   rot_e2l(dx,dy,dz,ldx,ldy,ldz);

   // transform length
   dist=len_e2l(dist);

   // move point
   lox+=dist*ldx;
   loy+=dist*ldy;
   loz+=dist*ldz;

   // transform back
   map_l2e(lox,loy,loz,mx,my,mz);
   }

// map point from external to local coordinates
void viewerbase::map_e2l(double ext_x,double ext_y,double ext_z,float &loc_x,float &loc_y,float &loc_z)
   {
   loc_x=(ext_x-PARAMS.offset1[0])*PARAMS.scaling1[0];
   loc_y=(ext_y-PARAMS.offset1[1])*PARAMS.scaling1[1];
   loc_z=(ext_z-PARAMS.offset1[2])*PARAMS.scaling1[2];
   }

// map point from local to external coordinates
void viewerbase::map_l2e(float loc_x,float loc_y,float loc_z,double &ext_x,double &ext_y,double &ext_z)
   {
   ext_x=(double)loc_x/PARAMS.scaling1[0]+PARAMS.offset1[0];
   ext_y=(double)loc_y/PARAMS.scaling1[1]+PARAMS.offset1[1];
   ext_z=(double)loc_z/PARAMS.scaling1[2]+PARAMS.offset1[2];
   }

// map point from local to internal coordinates
void viewerbase::map_l2i(float loc_x,float loc_y,float loc_z,float &int_x,float &int_y,float &int_z)
   {
   int_x=(loc_x-PARAMS.offset2[0])*PARAMS.scaling2[0];
   int_y=(loc_z-PARAMS.offset2[2])*PARAMS.scaling2[2];
   int_z=(loc_y-PARAMS.offset2[1])*PARAMS.scaling2[1];
   }

// map point from internal to local coordinates
void viewerbase::map_i2l(float int_x,float int_y,float int_z,float &loc_x,float &loc_y,float &loc_z)
   {
   loc_x=int_x/PARAMS.scaling2[0]+PARAMS.offset2[0];
   loc_y=int_z/PARAMS.scaling2[1]+PARAMS.offset2[1];
   loc_z=int_y/PARAMS.scaling2[2]+PARAMS.offset2[2];
   }

// rotate vector from external to local coordinates
void viewerbase::rot_e2l(float ext_dx,float ext_dy,float ext_dz,float &loc_dx,float &loc_dy,float &loc_dz)
   {
   loc_dx=ext_dx;
   loc_dy=ext_dy;
   loc_dz=ext_dz;
   }

// rotate vector from local to external coordinates
void viewerbase::rot_l2e(float loc_dx,float loc_dy,float loc_dz,float &ext_dx,float &ext_dy,float &ext_dz)
   {
   ext_dx=loc_dx;
   ext_dy=loc_dy;
   ext_dz=loc_dz;
   }

// rotate vector from local to internal coordinates
void viewerbase::rot_l2i(float loc_dx,float loc_dy,float loc_dz,float &int_dx,float &int_dy,float &int_dz)
   {
   int_dx=loc_dx;
   int_dy=loc_dz;
   int_dz=-loc_dy;
   }

// rotate vector from internal to local coordinates
void viewerbase::rot_i2l(float int_dx,float int_dy,float int_dz,float &loc_dx,float &loc_dy,float &loc_dz)
   {
   loc_dx=int_dx;
   loc_dy=-int_dz;
   loc_dz=int_dy;
   }

// map length from external to local coordinates
float viewerbase::len_e2l(float l)
   {return(l/PARAMS.scale);}

// map length from local to external coordinates
float viewerbase::len_l2e(float l)
   {return(l*PARAMS.scale);}

// map length from local to internal coordinates
float viewerbase::len_l2i(float l)
   {return(l);}

// map length from internal to local coordinates
float viewerbase::len_i2l(float l)
   {return(l);}

// map length from external to internal coordinates
float viewerbase::len_e2i(float l)
   {return(l/PARAMS.scale);}

// map length from internal to external coordinates
float viewerbase::len_i2e(float l)
   {return(l*PARAMS.scale);}

// concatenate two strings
char *viewerbase::concat(const char *str1,const char *str2)
   {
   char *str;

   if (str1==NULL && str2==NULL) ERRORMSG();

   if (str1==NULL) return(strdup(str2));
   if (str2==NULL) return(strdup(str1));

   if ((str=(char *)malloc(strlen(str1)+strlen(str2)+1))==NULL) ERRORMSG();

   memcpy(str,str1,strlen(str1));
   memcpy(str+strlen(str1),str2,strlen(str2)+1);

   return(str);
   }
