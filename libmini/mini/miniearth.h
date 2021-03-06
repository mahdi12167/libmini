// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIEARTH_H
#define MINIEARTH_H

#include "minibase.h"

#include "database.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "minicoord.h"

#include "minidyna.h"

#include "miniterrain.h"

#include "minisky.h"
#include "miniglobe.h"

#include "datagrid.h"

//! base class for viewing the earth
class miniearth
   {
   public:

   //! get approximate orb radius
   virtual double getorbradius();

   //! get semi-major and minor axis of orb
   virtual void getorbaxis(double &r_major,double &r_minor);

   struct MINIEARTH_PARAMS_STRUCT
      {
      // configurable parameters:

      int warpmode;   // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4
      BOOLINT nonlin; // use non-linear warp

      float fps;      // frames per second (target frame rate)

      float fovy;     // field of view (degrees)
      float nearp;    // near plane (meters)
      float farp;     // far plane (meters)

      // data paths:

      const char *stdpath;
      const char *instpath;

      // feature switches:

      BOOLINT usefog;
      BOOLINT useshaders;
      BOOLINT usediffuse;
      BOOLINT usedetail;
      BOOLINT usevisshader;
      BOOLINT usebathymap;
      BOOLINT usecontours;
      BOOLINT usenprshader;
      BOOLINT useskydome;
      BOOLINT usewaypoints;
      BOOLINT usebricks;
      BOOLINT useearth;
      BOOLINT useflat;

      // optional spherical fog:

      float fogcolor[3];

      float fogstart;
      float fogend;
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
   virtual ~miniearth();

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

   //! load tileset (regular version)
   minilayer *load(const char *elev,const char *imag,
                   BOOLINT loadopts=FALSE,BOOLINT reset=FALSE,
                   int level=0,int baselevel=0);

   //! load tileset (short version)
   minilayer *load(const char *url,
                   BOOLINT loadopts=FALSE,BOOLINT reset=FALSE,
                   int level=0,int baselevel=0);

   //! load tileset (long version)
   minilayer *load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                   BOOLINT loadopts=FALSE,BOOLINT reset=FALSE,
                   int level=0,int baselevel=0);

   //! load layered tileset
   minilayer *loadLTS(const char *url,
                      BOOLINT loadopts=FALSE,BOOLINT reset=FALSE,
                      int levels=5);

   //! load detail texture (db format)
   void loaddetailtex(const char *path,
                      float alpha=1.0f);

   //! load optional features
   void loadopts();

   //! get file
   char *getfile(const char *src_file,const char *altpath=NULL);

   //! set reference layer
   void setreference(minilayer *layer);

   //! remove a layer
   void remove(minilayer *layer);

   //! remove a layer /w sub-layers
   void removeLTS(minilayer *layer);

   //! remove all terrain layers
   void remove();

   //! get extent of a tileset
   miniv3d getextent(minilayer *layer);

   //! get center of a tileset
   minicoord getcenter(minilayer *layer);

   //! get the elevation at position (x,y,z)
   double getheight(const minicoord &p);

   //! get the normal at position (x,y,z)
   miniv3d getnormal(const minicoord &p);

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! define region of interest
   void defineroi(double radius);

   //! clear scene
   void clear();

   //! generate and cache scene for a particular eye point and time step
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time=0.0);

   //! render scene
   void render();

   //! check for static scene
   BOOLINT checkpending();

   //! make scene static (render triangle cache, but do not update it)
   void makestatic(BOOLINT flag);

   //! check whether the scene is static or not
   BOOLINT isstatic();

   //! freeze scene (do not render triangle cache, but grab frame buffer)
   void freeze(BOOLINT flag);

   //! check whether the scene is frozen or not
   BOOLINT isfrozen();

   //! get the relative elevation above sea level at position (x,y,z)
   double getrelheight(const minicoord &p);

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d,double mindist=0.0);

   //! shoot a ray at the orb
   double shoot_orb(const minicoord &o,const miniv3d &d,double mindist=0.0);

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

   //! map coordinates
   minicoord map_g2l(const minicoord &p) {return(TERRAIN->map_g2l(p));}
   minicoord map_l2g(const minicoord &p) {return(TERRAIN->map_l2g(p));}
   minicoord map_g2o(const minicoord &p) {return(TERRAIN->map_g2o(p));}
   minicoord map_o2g(const minicoord &p) {return(TERRAIN->map_o2g(p));}

   //! rotate vector
   miniv3d rot_g2l(const miniv3d &v,const minicoord &p) {return(TERRAIN->rot_g2l(v,p));}
   miniv3d rot_l2g(const miniv3d &v,const minicoord &p) {return(TERRAIN->rot_l2g(v,p));}
   miniv3d rot_g2o(const miniv3d &v,const minicoord &p) {return(TERRAIN->rot_g2o(v,p));}
   miniv3d rot_o2g(const miniv3d &v,const minicoord &p) {return(TERRAIN->rot_o2g(v,p));}

   //! map length
   double len_g2l(double l) {return(TERRAIN->len_g2l(l));}
   double len_l2g(double l) {return(TERRAIN->len_l2g(l));}
   double len_g2o(double l) {return(TERRAIN->len_g2o(l));}
   double len_o2g(double l) {return(TERRAIN->len_o2g(l));}

   //! get transformation matrix from ecef to opengl
   void get_ecef_matrix(miniv4d matrix[3])
      {TERRAIN->get_ecef_matrix(matrix);}

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

   void rendercache();
   void renderdgrid();

   void grabbuffers();
   void drawbuffers();
   void freebuffers();
   };

//! base class for viewing planets
class miniplanet: public miniearth
   {
   public:

   //! get approximate planet radius
   virtual double getorbradius();

   //! get semi-major and minor axis of planet
   virtual void getorbaxis(double &r_major,double &r_minor);

   //! default constructor
   miniplanet(int orb=minicoord::MINICOORD_ORB_EARTH)
      : miniearth()
      {planet_orb=orb;}

   //! destructor
   virtual ~miniplanet() {}

   protected:

   int planet_orb;
   };

#endif
