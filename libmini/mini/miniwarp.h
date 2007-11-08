// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "miniv3d.h"
#include "miniv4d.h"

//! geo-referenced coordinates
class minicoord
   {
   public:

   //! generic coordinate systems
   enum MINICOORD
      {
      MINICOORD_NONE,   // undefined
      MINICOORD_ECEF,   // Earth Centered Earth Fixed
      MINICOORD_LLH,    // Lat/Lon/H WGS84
      MINICOORD_UTM,    // Universal Transverse Mercator
      MINICOORD_LINEAR, // linear space
      MINICOORD_NONLIN  // non-linear space
      };

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
   minicoord(const miniv3d &v,const int t);
   minicoord(const miniv3d &v,const int t,const int zone,const int datum);
   minicoord(const miniv4d &v,const int t);
   minicoord(const miniv4d &v,const int t,const int zone,const int datum);
   minicoord(const double cx,const double cy,const double cz,const int t);
   minicoord(const double cx,const double cy,const double cz,const int t,const int zone,const int datum);

   //! destructor
   ~minicoord();

   //! convert from 1 coordinate system 2 another
   void convert2(const int t,const int zone=0,const int datum=0);

   //! linear conversion defined by 3x3 matrix and offset
   void convert(const miniv3d mtx[3],const miniv3d offset);

   //! linear conversion defined by 4x3 matrix
   void convert(const miniv4d mtx[3]);

   //! non-linear conversion defined by point 2 point correspondences
   void convert(const miniv3d src[2], // bounding box in original domain
                const miniv3d dst[8]); // 8 points in mapped domain

   miniv4d vec; // geo-referenced coordinates (plus time)
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
   enum MINIWARP
      {
      MINIWARP_PLAIN,  // plain coordinates
      MINIWARP_DATA,   // data coordinates
      MINIWARP_LOCAL,  // local coordinates
      MINIWARP_AFFINE, // affine coordinates
      MINIWARP_TILE,   // tile coordinates
      MINIWARP_WARP    // warp coordinates
      };

   //! default constructor
   miniwarp();

   //! destructor
   ~miniwarp();

   //! define data coordinates
   void def_data(const miniv3d bboxDAT[2], // bounding box in data domain
                 const minicoord bboxGEO[2]); // bounding box in geo-referenced domain

   //! define conversion to local coordinates
   void def_2local(const miniv3d bboxLOC[2]); // bounding box in ortho-normal domain

   //! define conversion to affine coordinates
   void def_2affine(const miniv4d mtx[3]); // affine transformation

   //! define warp coordinates
   void def_warp(const minicoord::MINICOORD sys);

   //! set actual warp
   void setwarp(MINIWARP from,MINIWARP to);

   //! get actual warp matrix
   void getwarp(miniv4d mtx[3]);

   //! perform warp
   miniv3d warp(miniv3d v);
   miniv4d warp(miniv4d v);

   protected:

   miniv3d BBOXDAT[2],BBOXGEO[2];
   miniv3d BBOXLOC[2];
   miniv4d MTXAFF[3];
   minicoord::MINICOORD SYSWRP;

   miniv4d MTX_2DAT[3];
   miniv4d MTX_2LOC[3];
   miniv4d MTX_2AFF[3];
   miniv4d MTX_2TIL[3];
   miniv4d MTX_2WRP[3];

   MINIWARP FROM,TO;

   miniv4d MTX[3];

   private:
   };

#endif
