// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "miniv3d.h"

//! geo-referenced coordinates
class minicoord
   {
   public:

   enum
      {
      MINICOORD_NONE=0,   // undefined
      MINICOORD_ECEF=1,   // Earth Centered Earth Fixed
      MINICOORD_LLH=2,    // Lat/Lon/H WGS84
      MINICOORD_UTM=3,    // Universal Transverse Mercator
      MINICOORD_LINEAR=4, // linear space
      MINICOORD_NONLIN=5  // non-linear space
      };

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
   minicoord(const miniv3d &v,const int t,const int zone=0,const int datum=0);
   minicoord(const double cx,const double cy,const double cz,const int t,const int zone=0,const int datum=0);

   //! destructor
   ~minicoord();

   //! convert from one coordinate system 2 another
   void convert2(const int t,const int zone=0,const int datum=0);
   void convert2(const miniv3d mtx[3],const miniv3d offset);
   void convert2(const miniv3d src[8],const miniv3d dst[8]);

   miniv3d vec;
   int type;

   int utm_zone,utm_datum;

   protected:

   private:
   };

//! warp kernel for global coordinate systems
class miniwarp
   {
   public:

   //! default constructor
   miniwarp();

   //! destructor
   ~miniwarp();

   protected:

   private:
   };

#endif
