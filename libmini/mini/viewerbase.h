// (c) by Stefan Roettger

#ifndef VIEWERBASE_H
#define VIEWERBASE_H

#include "minibase.h"

#undef GREYCSTORATION // enable this for greycstoration support
#include "convbase.h"

#include "minicache.h"

#include "miniv3d.h"
#include "minilayer.h"

#include "minisky.h"
#include "miniglobe.h"

//! aggregate class for viewing a tileset
class viewerbase
   {
   public:

   struct VIEWER_PARAMS_STRUCT
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

      char *elevdir;            //+ default elev directory
      char *imagdir;            //+ default imag directory

      // optional features:

      BOOLINT usefog;
      BOOLINT useshaders;
      BOOLINT usebathymap;
      BOOLINT usecontours;
      BOOLINT usenprshader;
      BOOLINT usediffuse;
      BOOLINT usewireframe;
      BOOLINT useskydome;
      BOOLINT usewaypoints;
      BOOLINT usebricks;
      BOOLINT useearth;

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

      // optional sky-dome:

      char *skydome;

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

      // optional earth globe:

      float lightdir[3];
      float transition;

      char *frontname;
      char *backname;

      // image conversion parameters:

      MINI_CONVERSION_PARAMS conversion_params;
      };

   typedef VIEWER_PARAMS_STRUCT VIEWER_PARAMS;

   //! default constructor
   viewerbase();

   //! destructor
   ~viewerbase();

   //! get parameters
   void get(VIEWER_PARAMS &params);

   //! set parameters
   void set(VIEWER_PARAMS &params);

   //! get parameters
   VIEWER_PARAMS *get() {return(&PARAMS);}

   //! set parameters
   void set(VIEWER_PARAMS *params) {set(*params);}

   //! get the encapsulated terrain object
   miniload *getterrain() {return(LAYER->getterrain());}

   //! get the encapsulated render buffer
   minicache *getbuffer() {return(CACHE);}

   //! get the encapsulated cache object
   datacache *getcache() {return(LAYER->getcache());}

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

   //! set render window
   void setwinsize(int width,int height);

   //! trigger complete render buffer update at next frame
   void update();

   //! generate and cache scene for a particular eye point
   void cache(const miniv3d &e,const miniv3d &d,const miniv3d &u);

   //! render cached scene
   void render();

   //! get time
   double gettime();

   //! start timer
   void starttimer();

   //! measure timer
   double gettimer();

   //! idle for the remainder of the frame
   //! dt is the time spent for rendering the last frame
   void idle(double dt);

   //! flatten the scene by a relative scaling factor (in the range [0-1])
   void flatten(float relscale);

   //! shoot a ray at the scene
   double shoot(const miniv3d &o,const miniv3d &d);

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

   VIEWER_PARAMS PARAMS,PARAMS0;

   void render_presea();
   void render_postsea();

   private:

   minilayer *LAYER;

   minicache *CACHE;

   int WINWIDTH;
   int WINHEIGHT;

   double START,TIMER;

   minisky skydome;
   miniglobe earth;

   static void receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int check_callback(char *src_url,char *src_id,char *src_file,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);

   static void preseacb(void *data);
   static void postseacb(void *data);
   };

#endif
