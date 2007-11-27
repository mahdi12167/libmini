// (c) by Stefan Roettger

#ifndef VIEWERBASE_H
#define VIEWERBASE_H

#include "minibase.h"

#include "convbase.h"

#include "miniv3d.h"
#include "miniterrain.h"

#include "minisky.h"
#include "miniglobe.h"

//! base class for viewing tilesets
class viewerbase
   {
   public:

   struct VIEWER_PARAMS_STRUCT
      {
      // configurable parameters:

      float fps;   // frames per second (target frame rate)

      float fovy;  //  field of view (degrees)
      float nearp; //  near plane (meters)
      float farp;  //  far plane (meters)

      // feature switches:

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

      // optional sky-dome:

      char *skydome;

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

   // propagate parameters
   void propagate();

   //! get the encapsulated terrain object
   miniterrain *getterrain() {return(&TERRAIN);}

   //! load tileset (short version)
   BOOLINT load(const char *url,
                BOOLINT loadopts=FALSE,BOOLINT reset=FALSE);

   //! load tileset (long version)
   BOOLINT load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                BOOLINT loadopts=FALSE,BOOLINT reset=FALSE);

   //! load optional features
   void loadopts();

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! generate and cache scene for a particular eye point
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect);

   //! render scene
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

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d);

   // coordinate conversions (e=external, l=local, i=internal):

   //! map coordinates
   minicoord map_e2l(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_e2l(p));}
   minicoord map_l2e(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_l2e(p));}
   minicoord map_l2i(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_l2i(p));}
   minicoord map_i2l(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_i2l(p));}
   minicoord map_e2i(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_e2i(p));}
   minicoord map_i2e(const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->map_i2e(p));}

   //! rotate vector
   miniv3d rot_e2l(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_e2l(v,p));}
   miniv3d rot_l2e(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_l2e(v,p));}
   miniv3d rot_l2i(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_l2i(v,p));}
   miniv3d rot_i2l(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_i2l(v,p));}
   miniv3d rot_e2i(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_e2i(v,p));}
   miniv3d rot_i2e(const miniv3d &v,const minicoord &p) {return(TERRAIN.getlayer(TERRAIN.getnearest(p))->rot_i2e(v,p));}

   //! map length
   double len_e2l(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_e2l(l));}
   double len_l2e(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_l2e(l));}
   double len_l2i(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_l2i(l));}
   double len_i2l(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_i2l(l));}
   double len_e2i(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_e2i(l));}
   double len_i2e(double l) {return(TERRAIN.getlayer(TERRAIN.getreference())->len_i2e(l));}

   protected:

   VIEWER_PARAMS PARAMS;

   private:

   miniterrain TERRAIN;

   minisky SKYDOME;
   miniglobe EARTH;

   double START,TIMER;

   void inithooks();

   static void receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int check_callback(char *src_url,char *src_id,char *src_file,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);
   };

#endif
