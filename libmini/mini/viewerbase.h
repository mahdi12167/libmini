// (c) by Stefan Roettger

#ifndef VIEWERBASE_H
#define VIEWERBASE_H

#include "minibase.h"

#include "convbase.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "miniterrain.h"

#include "minisky.h"
#include "miniglobe.h"

#include "threadbase.h"
#include "curlbase.h"

//! base class for viewing tilesets
class viewerbase
   {
   public:

   struct VIEWER_PARAMS_STRUCT
      {
      // configurable parameters:

      int warpmode; // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4

      float fps;    // frames per second (target frame rate)

      float fovy;   //  field of view (degrees)
      float nearp;  //  near plane (meters)
      float farp;   //  far plane (meters)

      // feature switches:

      BOOLINT usefog;
      BOOLINT useshaders;
      BOOLINT usediffuse;
      BOOLINT usevisshader;
      BOOLINT usebathymap;
      BOOLINT usecontours;
      BOOLINT usenprshader;
      BOOLINT usewireframe;
      BOOLINT useskydome;
      BOOLINT usewaypoints;
      BOOLINT usebricks;
      BOOLINT useearth;
      BOOLINT useflat;

      // optional spherical fog:

      float fogcolor[3];

      float fogstart;
      float fogdensity;

      // optional sky-dome:

      char *skydome;

      // optional earth globe:

      miniv3d lightdir;

      float lightbias;
      float lightoffset;

      float transbias;
      float transoffset;

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
   miniterrain *getterrain() {return(TERRAIN);}

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

   //! get null layer
   minilayer *getnull() {return(TERRAIN->getlayer(TERRAIN->getnull()));}

   //! get earth layer
   minilayer *getearth() {return(TERRAIN->getlayer(TERRAIN->getearth()));}

   //! get reference layer
   minilayer *getreference() {return(TERRAIN->getlayer(TERRAIN->getreference()));}

   //! get nearest layer
   minilayer *getnearest(const minicoord &p) {return(TERRAIN->getlayer(TERRAIN->getnearest(p)));}

   protected:

   VIEWER_PARAMS PARAMS;

   private:

   miniterrain *TERRAIN;

   minisky *SKYDOME;
   miniglobe *EARTH;

   BOOLINT LOADED;

   double START,TIMER;

   threadbase *THREADBASE;
   curlbase *CURLBASE;

   void initOGL();
   void inithooks();

   char *getfile(const char *src_file,const char *altpath=NULL);

   static void receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int check_callback(char *src_url,char *src_id,char *src_file,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);
   };

#endif
