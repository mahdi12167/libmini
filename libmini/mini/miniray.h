// (c) by Stefan Roettger

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
                     int swapyz=0,miniwarp *warp=0,
                     int calcbounds=0);

   //! add reference to triangle fans to the back buffer
   void addtrianglefans(float **array,int index,int num,int stride=0,
                        miniv3d *scaling=0,miniv3d *offset=0,
                        int swapyz=0,miniwarp *warp=0,
                        int calcbounds=0);

   //! swap front and back triangle reference buffer
   void swapbuffer();

   //! swap and clear back triangle reference buffer
   void swapandclear();

   //! shoot a ray and return the distance to the closest triangle
   //! only the triangles referenced in the front buffer are considered
   //! only the triangles with a positive distance are considered
   //! o is the origin of the ray, d is the ray direction
   //! a return value of MAXFLOAT indicates that there was no hit
   //! the first hit with a smaller distance than hitdist will be returned
   double shoot(const miniv3d &o,const miniv3d &d,double firsthit=0.0);

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

   protected:

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

      miniwarp *warp;

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
                             int swapyz=0,miniwarp *warp=0,
                             int calcbounds=0);

   void addtrianglefans_chunked(float **array,int index,int num,int stride=0,
                                miniv3d *scaling=0,miniv3d *offset=0,
                                int swapyz=0,miniwarp *warp=0,
                                int calcbounds=0);

   void calcbound(TRIANGLEREF *ref);

   double calcdist(TRIANGLEREF *ref,
                   const miniv3d &o,const miniv3d &d,
                   double dist);

   inline miniv3d calcpoint(TRIANGLEREF *ref,miniwarp **lastwarp,miniv3d p);

   minidyna<miniv3d> calcmesh(TRIANGLEREF *ref);

   inline int checkbound(const miniv3d &o,const miniv3d &d,
                         const miniv3d &b,const double r2);

   inline int checkbbox(const miniv3d &o,const miniv3d &d,
                        const miniv3d &b,const miniv3d r);

   inline int checkplane(const miniv3d &o,const miniv3d &n,const double radius,
                         const miniv3d &b,const double r2);

   inline double checkdist(const miniv3d &o,const miniv3d &d,
                           const miniv3d &v1,const miniv3d &v2,const miniv3d &v3);

   // Moeller-Trumbore ray/triangle intersection
   inline int intersect(const miniv3d &o,const miniv3d &d,
                        const miniv3d &v0,const miniv3d &v1,const miniv3d &v2,
                        miniv3d *tuv);
   };

#endif
