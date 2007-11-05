// (c) by Stefan Roettger

#ifndef MINIRAY_H
#define MINIRAY_H

#include "miniv3f.h"

//! class for intersecting a ray with a set of triangles
//! the triangles are stored as a set of references to vertex arrays
class miniray
   {
   public:

   //! default constructor
   miniray();

   //! destructor
   ~miniray();

   //! clear back triangle reference buffer
   void clearbuffer();

   //! add reference to triangles to the back buffer
   void addtriangles(float **array,int index,int num,int stride=0,
                     miniv3f *scaling=0,miniv3f *offset=0,
                     int swapyz=0);

   //! add reference to triangle fans to the back buffer
   void addtrianglefans(float **array,int index,int num,int stride=0,
                        miniv3f *scaling=0,miniv3f *offset=0,
                        int swapyz=0);

   //! swap front and back triangle reference buffer
   void swapbuffer();

   //! shoot a ray and return the distance to the closest triangle
   //! only the triangles referenced in the front buffer are considered
   //! o is the origin of the ray, d is the ray direction
   //! a return value of MAXFLOAT indicates that there was no hit
   float shoot(const miniv3f &o,const miniv3f &d);

   //! render triangles as wireframe for debugging purposes
   void renderwire();

   protected:

   private:

   struct TRIANGLEREF
      {
      float **array;
      int index;
      int num;

      int stride;

      int isfan;

      miniv3f scaling;
      miniv3f offset;

      int swapyz;

      miniv3f b;
      float r2;

      TRIANGLEREF *next;
      };

   TRIANGLEREF *FRONT,*BACK;

   void calcbound(TRIANGLEREF *ref);

   float calcdist(TRIANGLEREF *ref,
                  const miniv3f &o,const miniv3f &d,
                  float dist);

   void renderwire(TRIANGLEREF *ref);

   inline int checkbound(const miniv3f &o,const miniv3f &d,
                         const miniv3f &b,const float r2);

   inline float checkdist(const miniv3f &o,const miniv3f &d,
                          const miniv3f &v1,const miniv3f &v2,const miniv3f &v3);

   // Moeller-Trumbore ray/triangle intersection
   inline int intersect(const miniv3f &o,const miniv3f &d,
                        const miniv3f &v0,const miniv3f &v1,const miniv3f &v2,
                        miniv3f *tuv);
   };

#endif
