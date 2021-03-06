// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIRAY_H
#define MINIRAY_H

#include "miniv3d.h"
#include "minidyna.h"

#include "miniwarp.h"

//! class for intersecting a ray with a set of triangles
//! the triangles are stored as a set of references to vertex arrays
class miniray
   {
   public:

   //! default constructor
   miniray();

   //! destructor
   ~miniray();

   //! clear triangle reference buffers
   void clear();

   //! clear back triangle reference buffer
   void clearbuffer();

   //! add reference to triangles to the back buffer
   void addtriangles(float **array,int index,int num,int stride=0,
                     miniv3d *scaling=0,miniv3d *offset=0,
                     int swapyz=0,miniwarpbase *warp=0,
                     int nonlin=0,miniv3d *crdgen=0,
                     int calcbounds=0);

   //! add reference to triangle fans to the back buffer
   void addtrianglefans(float **array,int index,int num,int stride=0,
                        miniv3d *scaling=0,miniv3d *offset=0,
                        int swapyz=0,miniwarpbase *warp=0,
                        int nonlin=0,miniv3d *crdgen=0,
                        int calcbounds=0);

   //! swap front and back triangle reference buffer
   void swapbuffer();

   //! swap and clear back triangle reference buffer
   void swapandclear();

   //! shoot a ray and return the distance to the closest triangle
   //! only the triangles referenced in the front buffer are considered
   //! only the triangles with a positive distance are considered
   //! o is the origin of the ray, d is the ray direction
   //! a valid hit distance to be returned has to be no less than mindist
   //! a return value of MAXFLOAT indicates that there was no hit
   double shoot(const miniv3d &o,const miniv3d &d,double mindist=0.0);

   //! extract triangles that [possibly] intersect a plane
   //! o is the origin of the plane
   //! n is the normal of the plane
   //! radius is the maximum allowed distance to the origin
   //! extracted triangles are likely but not guaranteed to intersect the plane
   minidyna<miniv3d> extract(const miniv3d &o,const miniv3d &n,double radius);

   //! get triangle bounds
   void getbounds(miniv3d &bmin,miniv3d &bmax);

   //! set locking callbacks
   //! when the callbacks are set ray shooting and plane extraction can be triggered safely from a separate thread
   static void setcallbacks(void (*lock)(void *data),void *data,
                            void (*unlock)(void *data));

   //! lock ray shooting
   static void lock();

   //! unlock ray shooting
   static void unlock();

   //! configuring
   void configure_maxchunksize_triangles(int maxchunksize=100); // set maximum chunk size for triangle arrays
   void configure_maxchunksize_trianglefans(int maxchunksize=20); // set maximum chunk size for triangle fan arrays

   private:

   struct TRIANGLEREF
      {
      float **array;
      int index;
      int num;

      int stride;

      int isfan;

      miniv3d scaling;
      miniv3d offset;

      int swapyz;

      miniwarpbase *warp;

      int nonlin;
      miniv3d crdgen;

      int hasbound;

      miniv3d b,r;
      double r2;

      TRIANGLEREF *next;
      };

   TRIANGLEREF *FRONT,*BACK;

   static void (*LOCK_CALLBACK)(void *data);
   static void (*UNLOCK_CALLBACK)(void *data);
   static void *LOCK_DATA;

   int CONFIGURE_MAXCHUNKSIZE_TRIANGLES;
   int CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS;

   void addtriangles_chunked(float **array,int index,int num,int stride=0,
                             miniv3d *scaling=0,miniv3d *offset=0,
                             int swapyz=0,miniwarpbase *warp=0,
                             int nonlin=0,miniv3d *crdgen=0,
                             int calcbounds=0);

   void addtrianglefans_chunked(float **array,int index,int num,int stride=0,
                                miniv3d *scaling=0,miniv3d *offset=0,
                                int swapyz=0,miniwarpbase *warp=0,
                                int nonlin=0,miniv3d *crdgen=0,
                                int calcbounds=0);

   void calcbound(TRIANGLEREF *ref);

   double calcdist(const TRIANGLEREF *ref,
                   const miniv3d &o,const miniv3d &d,
                   double dist,double mindist);

   minidyna<miniv3d> calcmesh(const TRIANGLEREF *ref);

   inline miniv3d calcpoint(const TRIANGLEREF *ref,const miniv3d &p);
   };

#endif
