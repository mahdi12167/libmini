// (c) by Stefan Roettger

#ifndef VIEWERBASE_H
#define VIEWERBASE_H

#include "minibase.h"

#include "miniload.h"
#include "minicache.h"
#include "datacache.h"

#include "minisky.h"
#include "minipoint.h"

#include "miniglobe.h"

//! aggregate class for viewing a tileset
class viewerbase
   {
   public:

   struct VIEWER_PARAMS_STRUCT
      {
      // auto-determined parameters upon load:

      int cols;          // number of columns per tileset
      int rows;          // number of rows per tileset

      int basesize;      // base size of texture maps

      BOOLINT usepnm;    // use either PNM or DB loader

      float extent[3];   // extent of tileset

      float offset1[3];  // external2local offset of tileset center
      float scaling1[3]; // external2local scaling factor of tileset

      float offset2[3];  // local2internal offset of tileset center
      float scaling2[3]; // local2internal scaling factor of tileset

      float maxelev;     // absolute maximum of expected elevations

      // configurable parameters:
      // [parameters marked with * must be changed via set()]
      // [parameters marked with + must not be changed after calling load()]

      float scale;              //+ scaling of scene
      float exaggeration;       //+ exaggeration of elevation

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

      char *tilesetfile;        //+ tileset sav file
      char *vtbinisuffix;       //+ suffix of vtb ini file
      char *startupfile;        //+ startup sav file

      char *localpath;          //+ local directory

      char *altpath;            //+ alternative data path

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

      // image processing parameters:

      float jpeg_quality;

      BOOLINT usegreycstoration;

      float greyc_p;
      float greyc_a;
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

   //! get the elevation at position (x,y) in external coords
   float getheight(double x,double y);

   //! get the encapsulated terrain object
   miniload *getterrain() {return(TERRAIN);}

   //! get the encapsulated render buffer
   minicache *getbuffer() {return(CACHE);}

   //! get the encapsulated cache object
   datacache *getcache() {return(TILECACHE);}

   //! load tileset (short version)
   BOOLINT load(const char *url,
                BOOLINT reset=FALSE);

   //! load tileset (long version)
   BOOLINT load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                BOOLINT reset=FALSE);

   //! load optional features
   void loadopts();

   //! set render window
   void setwinsize(int width,int height);

   //! set initial eye point in external coords
   void initeyepoint(double ex,double ey,double ez);

   //! trigger complete render buffer update at next frame
   void update();

   //! generate and cache scene for a particular eye point in external coords
   void cache(double ex,double ey,double ez,
              float dx,float dy,float dz,
              float ux,float uy,float uz);

   //! render cached scene
   void render();

   //! get time
   float gettime();

   //! start timer
   void starttimer();

   //! measure timer
   float gettimer();

   //! idle for the remainder of the frame
   //! dt is the time spent for rendering the last frame
   void idle(float dt);

   //! get extent of tileset in local coords
   void getextent(float &sx,float &sy);
   void getextent(float &sx,float &sy,float &sz);

   //! get center of tileset in external coords
   void getcenter(double &cx,double &cy);

   //! get initial view point in external coords
   void getinitial(double &vx,double &vy);

   //! flatten the scene by a relative scaling factor (in the range [0-1])
   void flatten(float relscale);

   //! shoot a ray at the scene
   float shoot(double ox,double oy,double oz,
               float dx,float dy,float dz);

   //! move a point on a ray
   void move(float dist,
             double ox,double oy,double oz,
             float dx,float dy,float dz,
             double &mx,double &my,double &mz);

   //! map point from external to local coordinates
   void map_e2l(double ext_x,double ext_y,double ext_z,float &loc_x,float &loc_y,float &loc_z);

   //! map point from local to external coordinates
   void map_l2e(float loc_x,float loc_y,float loc_z,double &ext_x,double &ext_y,double &ext_z);

   //! map point from local to internal coordinates
   void map_l2i(float loc_x,float loc_y,float loc_z,float &int_x,float &int_y,float &int_z);

   //! map point from internal to local coordinates
   void map_i2l(float int_x,float int_y,float int_z,float &loc_x,float &loc_y,float &loc_z);

   //! rotate vector from external to local coordinates
   void rot_e2l(float ext_dx,float ext_dy,float ext_dz,float &loc_dx,float &loc_dy,float &loc_dz);

   //! rotate vector from local to external coordinates
   void rot_l2e(float loc_dx,float loc_dy,float loc_dz,float &ext_dx,float &ext_dy,float &ext_dz);

   //! rotate vector from local to internal coordinates
   void rot_l2i(float loc_dx,float loc_dy,float loc_dz,float &int_dx,float &int_dy,float &int_dz);

   //! rotate vector from internal to local coordinates
   void rot_i2l(float int_dx,float int_dy,float int_dz,float &loc_dx,float &loc_dy,float &loc_dz);

   //! map length from external to local coordinates
   float len_e2l(float l);

   //! map length from local to external coordinates
   float len_l2e(float l);

   //! map length from local to internal coordinates
   float len_l2i(float l);

   //! map length from internal to local coordinates
   float len_i2l(float l);

   //! map length from external to internal coordinates
   float len_e2i(float l);

   //! map length from internal to external coordinates
   float len_i2e(float l);

   protected:

   VIEWER_PARAMS PARAMS,PARAMS0;

   BOOLINT LOADED;

   void render_presea();
   void render_postsea();

   private:

   static const int MAXSTR=1000;

   miniload *TERRAIN;
   minicache *CACHE;

   datacache *TILECACHE;

   int WINWIDTH;
   int WINHEIGHT;

   int UPD;

   float IEX,IEY,IEZ;
   float IDX,IDY,IDZ;

   double START,TIMER;

   minisky skydome;
   minipoint points;

   miniglobe earth;

   static char *concat(const char *str1,const char *str2);

   static void request_callback(char *file,int istexture,databuf *buf,void *data);

   static void receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int check_callback(char *src_url,char *src_id,char *src_file,void *data);

   static int conversionhook(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);

   static void preseacb(void *data);
   static void postseacb(void *data);
   };

#endif
