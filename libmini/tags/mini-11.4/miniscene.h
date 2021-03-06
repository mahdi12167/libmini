// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISCENE_H
#define MINISCENE_H

#include <mini/minibase.h>

#include <mini/dataconv.h>

#include <mini/miniv3d.h>
#include <mini/miniv4d.h>

#include <mini/minidyna.h>

#include <mini/miniearth.h>
#include <mini/miniterrain.h>

#include <mini/threadbase.h>
#include <mini/curlbase.h>

//! base class for viewing tilesets
class miniscene
   {
   public:

   //! get approximate orb radius
   double getorbradius();

   struct MINISCENE_PARAMS_STRUCT
      {
      // configurable parameters:

      int winwidth;  // window width
      int winheight; // window height

      float fps;     // frames per second (target frame rate)

      float fovy;    // field of view (degrees)
      float nearp;   // near plane (meters)
      float farp;    // far plane (meters)

      // auto-adaption:

      BOOLINT autoadapt; // auto-adaption switch
      BOOLINT autores;   // adapt global resolution
      BOOLINT autorange; // adapt texture paging range
      float autotarget;  // targeted cpu load (percent)
      float automemory;  // targeted texture memory (mb)
      float autoseconds; // adaption period (seconds)

      // feature switches:

      BOOLINT usewireframe;

      // image conversion parameters:

      dataconv::MINI_CONVERSION_PARAMS conversion_params;
      };

   typedef MINISCENE_PARAMS_STRUCT MINISCENE_PARAMS;

   //! default constructor
   miniscene();

   //! destructor
   virtual ~miniscene();

   //! get parameters
   void get(MINISCENE_PARAMS &params);

   //! set parameters
   void set(MINISCENE_PARAMS &params);

   //! get parameters
   MINISCENE_PARAMS *get() {return(&PARAMS);}

   //! set parameters
   void set(MINISCENE_PARAMS *params) {set(*params);}

   // propagate parameters
   void propagate();

   //! get the encapsulated earth object
   miniearth *getearth() {return(EARTH);}

   //! get the encapsulated null layer
   minilayer *getnull() {return(EARTH->getnull());}

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! clear scene
   void clear();

   //! enable a specific focus point other than the eye point
   void enablefocus(const minicoord &f);

   //! disable the focus point
   void disablefocus();

   //! generate and cache scene for a particular eye point
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect);

   //! render scene
   void render();

   //! get time
   double time();

   //! start timer
   void starttimer();

   //! read out timer
   double gettimer();

   //! idle for the remainder of the frame
   //! dt is the time spent for rendering the last frame
   void idle(double dt);

   //! adapt quality parameters
   //! dt is the time spent for rendering the last frame
   void adapt(double dt);

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d,double mindist=0.0);

   //! shoot a ray at the surface
   double shoot_surface(const minicoord &o,const miniv3d &d,double mindist=0.0);

   //! shoot a ray at the orb
   double shoot_orb(const minicoord &o,const miniv3d &d,double mindist=0.0);

   //! extract triangles that [possibly] intersect a plane
   minidyna<miniv3d> extract(const minicoord &p,const miniv3d &v,double radius);

   //! set locking callbacks for ray shooting (and plane extraction)
   static void setraycallbacks(void (*lock)(void *data),void *data,
                               void (*unlock)(void *data));

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

   protected:

   MINISCENE_PARAMS PARAMS;

   virtual void check_ecef_geometry(miniv3d &center,double &radius);
   virtual void render_ecef_geometry(double t);

   miniearth *EARTH;
   miniterrain *TERRAIN;

   private:

   double START,TIMER;

   threadbase *THREADBASE;
   curlbase *CURLBASE;

   void inithooks();

   static void receive_callback(const char *src_url,const char *src_id,const char *src_file,const char *dst_file,int background,void *data);
   static int check_callback(const char *src_url,const char *src_id,const char *src_file,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,void *data);
   };

#endif
