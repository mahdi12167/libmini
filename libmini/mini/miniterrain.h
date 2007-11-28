// (c) by Stefan Roettger

#ifndef MINITERRAIN_H
#define MINITERRAIN_H

#include "minibase.h"

#include "minicache.h"

#include "miniv3d.h"
#include "minilayer.h"

//! aggregate class for rendering multiple layers
class miniterrain
   {
   public:

   struct MINITERRAIN_PARAMS_STRUCT
      {
      // configurable parameters:
      // [parameters marked with * must be changed via set()]
      // [parameters marked with + must not be changed after calling load()]

      int warpmode;             //+ warp mode: plain=0 affine=1 non-linear=2

      float scale;              //+ scaling of scene
      float exaggeration;       //+ exaggeration of elevations
      float maxelev;            //+ absolute maximum of expected elevations

      float load;               //+ initially loaded area relative to far plane
      float preload;            //+ continuously preloaded area relative to far plane

      float minres;             //+ minimum resolution of triangulation
      int fastinit;             //+ fast initialization level
      float avgd2value;         //+ average d2value for fast initialization

      int lazyness;             //+ lazyness of tileset paging
      float update;             //* update period for tileset paging in seconds
      float expire;             //* tile expiration time in seconds

      float upload;             //* tile upload time per frame relative to 1/fps
      float keep;               //* time to keep tiles in the cache in minutes
      float maxdelay;           //* time after which tiles are regarded as delayed relative to update time
      float cache;              //* memory footprint of the cache in mega bytes

      float keepalive;          //* time for which idling threads are kept alive in seconds
      float timeslice;          //* time for which idling threads sleep in seconds

      float fps;                //* frames per second (target frame rate)
      float spu;                //* update period for render buffer in seconds

      float res;                //  global resolution of triangulation

      float fovy;               //  field of view (degrees)
      float nearp;              //  near plane (meters)
      float farp;               //  far plane (meters)

      float reduction1;         //+ reduction parameter #1 for invisible tiles
      float reduction2;         //+ reduction parameter #2 for invisible tiles

      float range;              //* texture paging range relative to far plane
      float radius;             //* non-linear kick-in distance relative to texture range
      float dropoff;            //* non-linear lod dropoff at kick-in distance

      BOOLINT genmipmaps;       //* enable on-the-fly generation of mipmaps

      float sealevel;           //* sea-level height (meters, off=-MAXFLOAT)

      BOOLINT autocompress;     //+ auto-compress raw textures with S3TC
      BOOLINT lod0uncompressed; //+ keep LOD0 textures uncompressed

      char *elevdir;            //+ default elev directory
      char *imagdir;            //+ default imag directory

      // optional feature switches:

      BOOLINT usefog;
      BOOLINT useshaders;
      BOOLINT usebathymap;
      BOOLINT usecontours;
      BOOLINT usenprshader;
      BOOLINT usewaypoints;
      BOOLINT usebricks;

      // optional spherical fog:

      float fogcolor[3];

      float fogstart;
      float fogdensity;

      // optional color mapping:

      float bathystart;
      float bathyend;

      unsigned char *bathymap;
      int bathywidth,bathyheight,bathycomps;

      float contours;

      float seacolor[3];

      float seatrans;
      float seamodulate;
      float seabottom;
      float bottomtrans;

      float bottomcolor[3];

      // optional NPR rendering:

      float nprfadefactor;

      float nprbathystart;
      float nprbathyend;

      unsigned char *nprbathymap;
      int nprbathywidth,nprbathyheight,nprbathycomps;

      float nprcontours;

      float nprseacolor[3];

      float nprseatrans;
      float nprseagrey;

      // optional way-points:

      char *waypoints;

      float signpostheight;
      float signpostrange;

      float signpostturn;
      float signpostincline;

      char *brick;

      float bricksize;
      float brickrad;

      int brickpasses;
      float brickceiling;
      float brickscroll;
      };

   typedef MINITERRAIN_PARAMS_STRUCT MINITERRAIN_PARAMS;

   //! default constructor
   miniterrain();

   //! destructor
   ~miniterrain();

   //! get parameters
   void get(MINITERRAIN_PARAMS &tparams);

   //! set parameters
   void set(MINITERRAIN_PARAMS &tparams);

   //! get parameters
   MINITERRAIN_PARAMS *get() {return(&TPARAMS);}

   //! set parameters
   void set(MINITERRAIN_PARAMS *tparams) {set(*tparams);}

   //! propagate parameters
   void propagate();

   //! propagate waypoint parameters
   void propagate_wp();

   //! get the encapsulated render buffer
   minicache *getbuffer() {return(CACHE);}

   //! set internal callbacks
   void setcallbacks(void *threaddata,
                     void (*threadinit)(int threads),void (*threadexit)(),
                     void (*startthread)(void *(*thread)(void *background),backarrayelem *background,void *data),
                     void (*jointhread)(backarrayelem *background,void *data),
                     void (*lock_cs)(void *data),void (*unlock_cs)(void *data),
                     void (*lock_io)(void *data),void (*unlock_io)(void *data),
                     void *curldata,
                     void (*curlinit)(int threads,char *proxyname,char *proxyport),void (*curlexit)(),
                     void (*geturl)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data),
                     int (*checkurl)(char *src_url,char *src_id,char *src_file,void *data));

   //! load tileset (short version)
   BOOLINT load(const char *url,
                BOOLINT loadopts=FALSE,BOOLINT reset=FALSE);

   //! load tileset (long version)
   BOOLINT load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                BOOLINT loadopts=FALSE,BOOLINT reset=FALSE);

   //! get the number of terrain layers
   int getlnum() {return(LNUM);}

   //! get the nth terrain layer
   minilayer *getlayer(int n=0);

   //! remove the nth terrain layer
   void remove(int n=0);

   //! get extent of a tileset
   miniv3d getextent(int n=0);

   //! get center of a tileset
   minicoord getcenter(int n=0);

   //! get the elevation at position (x,y,z)
   double getheight(const minicoord &p);

   //! set reference layer
   void setreference(int ref=0);

   //! get reference layer
   int getreference();

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! get nearest layer
   int getnearest(const minicoord &e);

   //! trigger complete render buffer update at next frame
   void update();

   //! generate and cache scene for a particular eye point
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time);

   //! render cached scene
   void render();

   //! determine whether or not a layer is displayed
   void display(int n,BOOLINT yes=TRUE);

   //! flatten the terrain by a relative scaling factor (in the range [0-1])
   void flatten(float relscale);

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d);

   //! return memory consumption
   double getmem();

   //! return raw texture memory consumption
   double gettexmem();

   //! get total number of pending tiles
   int getpending();

   //! get total amount of cache memory
   double getcachemem();

   protected:

   MINITERRAIN_PARAMS TPARAMS;

   private:

   minilayer **LAYER;
   int LNUM,LMAX;
   int LREF;

   minicache *CACHE;

   //!! does not work for multiple layers yet
   void *THREADDATA;
   void (*THREADINIT)(int threads);
   void (*THREADEXIT)();
   void (*STARTTHREAD)(void *(*thread)(void *background),backarrayelem *background,void *data);
   void (*JOINTHREAD)(backarrayelem *background,void *data);
   void (*LOCK_CS)(void *data);
   void (*UNLOCK_CS)(void *data);
   void (*LOCK_IO)(void *data);
   void (*UNLOCK_IO)(void *data);

   void *CURLDATA;
   void (*CURLINIT)(int threads,char *proxyname,char *proxyport);
   void (*CURLEXIT)();
   void (*GETURL)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   int (*CHECKURL)(char *src_url,char *src_id,char *src_file,void *data);

   void render_presea();
   void render_postsea();

   static void preseacb(void *data);
   static void postseacb(void *data);
   };

#endif
