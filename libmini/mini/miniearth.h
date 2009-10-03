// (c) by Stefan Roettger

#ifndef MINIEARTH_H
#define MINIEARTH_H

#include "minibase.h"

#include "database.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "minidyna.h"

#include "miniterrain.h"

#include "minisky.h"
#include "miniglobe.h"

#include "datagrid.h"

//! base class for viewing tilesets
class miniearth
   {
   public:

   struct MINIEARTH_PARAMS_STRUCT
      {
      // configurable parameters:

      int warpmode;   // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4
      BOOLINT nonlin; // use non-linear warp

      float fps;      // frames per second (target frame rate)

      float fovy;     // field of view (degrees)
      float nearp;    // near plane (meters)
      float farp;     // far plane (meters)

      // feature switches:

      BOOLINT usefog;
      BOOLINT useshaders;
      BOOLINT usediffuse;
      BOOLINT usedetail;
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

      // optional void display:

      float voidstart;
      float voidcolor[3];

      // optional abyss display:

      float abyssstart;
      float abysscolor[3];

      // optional sky-dome:

      const char *skydome;

      // optional earth globe:

      miniv3d lightdir;

      float lightbias;
      float lightoffset;

      float transbias;
      float transoffset;

      // earth textures as a PPM file
      const char *frontname;
      const char *backname;

      // earth textures as a databuf object
      databuf *frontbuf;
      databuf *backbuf;

      // earth textures as a DB file
      const char *frontbufname;
      const char *backbufname;
      };

   typedef MINIEARTH_PARAMS_STRUCT MINIEARTH_PARAMS;

   //! default constructor
   miniearth();

   //! destructor
   ~miniearth();

   //! get parameters
   void get(MINIEARTH_PARAMS &eparams);

   //! set parameters
   void set(MINIEARTH_PARAMS &eparams);

   //! get parameters
   MINIEARTH_PARAMS *get() {return(&EPARAMS);}

   //! set parameters
   void set(MINIEARTH_PARAMS *eparams) {set(*eparams);}

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

   //! load detail texture (db format)
   void loaddetail(const char *path,
                   float alpha=1.0f);

   //! load optional features
   void loadopts();

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! clear scene
   void clear();

   //! generate and cache scene for a particular eye point and time step
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time=0.0);

   //! render scene
   void render();

   //! check for static scene
   BOOLINT checkpending();

   //! make scene static
   void makestatic(BOOLINT flag);

   //! check whether the scene is static or not
   BOOLINT isstatic();

   //! freeze scene
   void freeze(BOOLINT flag);

   //! check whether the scene is frozen or not
   BOOLINT isfrozen();

   //! get the relative elevation above sea level at position (x,y,z)
   double getrelheight(const minicoord &p);

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d,double hitdist=0.0);

   //! extract triangles that [possibly] intersect a plane
   minidyna<miniv3d> extract(const minicoord &p,const miniv3d &v,double radius);

   //! set locking callbacks for ray shooting (and plane extraction)
   static void setraycallbacks(void (*lock)(void *data),void *data,
                               void (*unlock)(void *data));

   //! add datagrid object
   void addgrid(datagrid *obj,BOOLINT sort=FALSE);

   //! get null layer
   minilayer *getnull() {return(TERRAIN->getlayer(TERRAIN->getnull()));}

   //! get earth layer
   minilayer *getearth() {return(TERRAIN->getlayer(TERRAIN->getearth()));}

   //! get reference layer
   minilayer *getreference() {return(TERRAIN->getlayer(TERRAIN->getreference()));}

   //! get nearest layer
   minilayer *getnearest(const minicoord &p) {return(TERRAIN->getlayer(TERRAIN->getnearest(p)));}

   //! get texture id of z-buffer rectangle
   int getztexid() {return(ZTEXID);}

   protected:

   MINIEARTH_PARAMS EPARAMS;

   private:

   miniterrain *TERRAIN;

   minisky *SKYDOME;
   miniglobe *EARTH;

   BOOLINT LOADED;

   BOOLINT CLEAR;

   BOOLINT STATIC;

   BOOLINT FREEZE;
   BOOLINT GRABBED;

   int RGBTEXID,ZTEXID;
   int BUFWIDTH,BUFHEIGHT;
   unsigned char *RGBBUF;
   float *ZBUF;

   datagrid *DATAGRID;
   BOOLINT SORT;

   void initOGL();

   char *getfile(const char *src_file,const char *altpath=NULL);

   void rendercache();
   void renderdgrid();

   void grabbuffers();
   void drawbuffers();
   void freebuffers();
   };

#endif
