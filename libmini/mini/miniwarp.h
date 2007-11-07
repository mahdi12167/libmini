// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "miniv3d.h"

//! geo-referenced coordinates
class minicoord
   {
   public:

   //! generic coordinate systems
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

   //! linear conversion defined by 3x3 matrix and offset
   void convert2(const miniv3d mtx[3],const miniv3d offset);

   //! non-linear conversion defined by point 2 point correspondences
   void convert2(const miniv3d src[2], // bounding box in original domain
                 const miniv3d dst[8]); // 8 points in mapped domain

   miniv3d vec; // geo-referenced coordinates
   int type; // actual coordinate system type

   int utm_zone,utm_datum; // actual UTM zone and datum

   protected:

   private:
   };

//! warp kernel for global coordinate systems
class miniwarp
   {
   public:

   //! global coordinate systems
   enum
      {
      MINIWARP_PLAIN=0,  // plain coordinates
      MINIWARP_DATA=1,   // data coordinates
      MINIWARP_LOCAL=2,  // local coordinates
      MINIWARP_AFFINE=3, // affine coordinates
      MINIWARP_TILE=4,   // tile coordinates
      MINIWARP_WARP=5    // warp coordinates
      };

   //! default constructor
   miniwarp();

   //! destructor
   ~miniwarp();

   //! define data coordinates
   void def_data(const miniv3d bbox[2], // bounding box in data domain
                 const minicoord bboxGEO[2]); // bounding box in geo-referenced domain

   //! define conversion to local coordinates
   void def_2local(const miniv3d bbox[2]); // bounding box in orthonormal domain

   //! define conversion to afine coordinates
   void def_2affine(const miniv3d mtx[3]); // affine transformation

   //! define warp coordinates
   void def_warp(const minicoord &origin); // destination coordinate system

   protected:

   private:
   };

#endif
