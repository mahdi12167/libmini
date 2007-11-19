// (c) by Stefan Roettger

#ifndef MINILAYER_H
#define MINILAYER_H

#include "minibase.h"

#include "miniload.h"
#include "minicache.h"
#include "datacache.h"

#include "minipoint.h"

#include "miniv3d.h"
#include "miniwarp.h"

//! aggregate class for viewing a layer
class minilayer
   {
   public:

   struct MINILAYER_PARAMS_STRUCT
      {
      // auto-determined parameters upon load:

      int cols;         // number of columns per tileset
      int rows;         // number of rows per tileset

      int basesize;     // base size of texture maps

      BOOLINT usepnm;   // use either PNM or DB loader

      float extent[3];  // extent of tileset

      float offset[3];  // offset of tileset center
      float scaling[3]; // scaling factor of tileset

      // configurable parameters:
      // [parameters marked with * must be changed via set()]
      // [parameters marked with + must not be changed after calling load()]

      int warpmode;             //+ warp mode: plain=0 affine=1 non-linear=2

      float shift[3];           //+ manual scene shift

      float scale;              //+ scaling of scene
      float exaggeration;       //+ exaggeration of elevations
      float maxelev;            //+ absolute maximum of expected elevations

      float load;               //+ initially loaded area relative to far plane
      float preload;            //+ continuously preloaded area relative to far plane

      float minres;             //+ minimum resolution of triangulation
      int fastinit;             //+ fast initialization level

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

      float sealevel;           //* sea-level height (meters, off=-MAXFLOAT)

      BOOLINT autocompress;     //+ auto-compress raw textures with S3TC
      BOOLINT lod0uncompressed; //+ keep LOD0 textures uncompressed

      int locthreads;           //+ number of local threads
      int numthreads;           //+ number of net threads

      char *elevdir;            //+ default elev directory
      char *imagdir;            //+ default imag directory

      char *elevprefix;         //+ elev tileset prefix
      char *imagprefix;         //+ imag tileset prefix

      char *tilesetfile;        //+ tileset sav file
      char *vtbinisuffix;       //+ suffix of vtb ini file
      char *startupfile;        //+ startup sav file

      char *localpath;          //+ local directory

      char *altpath;            //+ alternative data path

      // optional features:

      BOOLINT usewaypoints;
      BOOLINT usebricks;

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

   typedef MINILAYER_PARAMS_STRUCT MINILAYER_PARAMS;

   //! default constructor
   minilayer(int id,minicache *cache);

   //! destructor
   ~minilayer();

   //! get parameters
   void get(MINILAYER_PARAMS &params);

   //! set parameters
   void set(MINILAYER_PARAMS &params);

   //! get parameters
   MINILAYER_PARAMS *get() {return(&PARAMS);}

   //! set parameters
   void set(MINILAYER_PARAMS *params) {set(*params);}

   //! get the encapsulated terrain object
   miniload *getterrain() {return(TERRAIN);}

   //! get the encapsulated cache object
   datacache *getcache() {return(TILECACHE);}

   //! get the encapsulated waypoint object
   minipoint *getpoints() {return(POINTS);}

   //! get the encapsulated warp object
   miniwarp *getwarp() {return(&WARP);}

   //! set internal callbacks
   void setcallbacks(void *threaddata,
                     void (*threadinit)(int threads),void (*threadexit)(),
                     void (*startthread)(void *(*thread)(void *background),backarrayelem *background,void *data),
                     void (*jointhread)(backarrayelem *background,void *data),
                     void (*lock_cs)(void *data),void (*unlock_cs)(void *data),
                     void *curldata,
                     void (*curlinit)(int threads,char *proxyname,char *proxyport),void (*curlexit)(),
                     void (*geturl)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data),
                     int (*checkurl)(char *src_url,char *src_id,char *src_file,void *data));

   //! load tileset (short version)
   BOOLINT load(const char *url,
                BOOLINT reset=FALSE);

   //! load tileset (long version)
   BOOLINT load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                BOOLINT reset=FALSE);

   //! load optional features
   void loadopts();

   //! get extent of tileset
   miniv3d getextent();

   //! get center of tileset
   miniv3d getcenter();

   //! get the elevation at position (x,y,z)
   double getheight(const miniv3d &p);

   //! get initial view point
   miniv3d getinitial();

   //! set initial eye point
   void initeyepoint(const miniv3d &e);

   //! trigger complete render buffer update at next frame
   void update();

   //! generate and cache scene for a particular eye point
   void cache(const miniv3d &e,const miniv3d &d,const miniv3d &u,float aspect);

   //! flatten the scene by a relative scaling factor (in the range [0-1])
   void flatten(float relscale);

   //! map point from external to local coordinates
   miniv3d map_e2l(const miniv3d &p);

   //! map point from local to external coordinates
   miniv3d map_l2e(const miniv3d &p);

   //! map point from local to internal coordinates
   miniv3d map_l2i(const miniv3d &p);

   //! map point from internal to local coordinates
   miniv3d map_i2l(const miniv3d &p);

   //! map point from external to internal coordinates
   miniv3d map_e2i(const miniv3d &p);

   //! map point from internal to external coordinates
   miniv3d map_i2e(const miniv3d &p);

   //! rotate vector from external to local coordinates
   miniv3d rot_e2l(const miniv3d &v);

   //! rotate vector from local to external coordinates
   miniv3d rot_l2e(const miniv3d &v);

   //! rotate vector from local to internal coordinates
   miniv3d rot_l2i(const miniv3d &v);

   //! rotate vector from internal to local coordinates
   miniv3d rot_i2l(const miniv3d &v);

   //! rotate vector from external to internal coordinates
   miniv3d rot_e2i(const miniv3d &v);

   //! rotate vector from internal to external coordinates
   miniv3d rot_i2e(const miniv3d &v);

   //! map length from external to local coordinates
   double len_e2l(double l);

   //! map length from local to external coordinates
   double len_l2e(double l);

   //! map length from local to internal coordinates
   double len_l2i(double l);

   //! map length from internal to local coordinates
   double len_i2l(double l);

   //! map length from external to internal coordinates
   double len_e2i(double l);

   //! map length from internal to external coordinates
   double len_i2e(double l);

   protected:

   MINILAYER_PARAMS PARAMS,PARAMS0;

   BOOLINT LOADED;

   miniwarp WARP_E2L;
   miniwarp WARP_L2E;
   miniwarp WARP_L2I;
   miniwarp WARP_I2L;
   miniwarp WARP_E2I;
   miniwarp WARP_I2E;

   private:

   int ID;
   minicache *CACHE;

   miniload *TERRAIN;
   datacache *TILECACHE;

   minipoint *POINTS;

   miniwarp WARP;

   int UPD;

   miniv3d EYE_INT;
   miniv3d DIR_INT;

   void defwarp(minicoord offsetDAT,minicoord extentDAT);

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

   static void request_callback(char *file,int istexture,databuf *buf,void *data);

   void threadinit(int threads=1);
   void threadexit();

   static void startthread(void *(*thread)(void *background),backarrayelem *background,void *data);
   static void jointhread(backarrayelem *background,void *data);

   static void lock_cs(void *data);
   static void unlock_cs(void *data);

   static void lock_io(void *data);
   static void unlock_io(void *data);

   void curlinit(int threads=1,char *proxyname=NULL,char *proxyport=NULL);
   void curlexit();

   static void getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int checkURL(char *src_url,char *src_id,char *src_file,void *data);
   };

#endif
