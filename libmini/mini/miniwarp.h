// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "minibase.h"

#include "miniv3d.h"
#include "miniv4d.h"
#include "minicoord.h"

class miniwarp;

//! warp kernel base
class miniwarpbase
   {
   public:

   //! default constructor
   miniwarpbase();

   //! destructor
   ~miniwarpbase();

   //! get actual warp matrix
   void getwarp(miniv4d mtx[3]) const; // fourth row is assumed to be (0,0,0,1)

   //! get inverse of actual warp matrix
   void getinv(miniv4d inv[3]) const;

   //! get transpose of actual warp matrix
   void gettra(miniv3d tra[3]) const;

   //! get inverse transpose of actual warp matrix
   void getinvtra(miniv3d invtra[3]) const;

   //! get actual scaling factor
   double getscale() const;

   //! linear warp
   miniv3d linwarp(const miniv3d &p) const;

   //! enable non-linear warp
   void usenonlin(BOOLINT on,miniv3d crdgen);

   //! check for non-linear warp
   BOOLINT getnonlin() const;

   //! get corners of warp box
   double getcorners(miniv3d p[8],miniv3d n[8]=NULL) const;

   //! tri-linear warp
   miniv3d triwarp(const miniv3d &c) const;

   protected:

   miniv4d MTX[3];
   miniv4d INV[3],TRA[3];
   miniv4d INVTRA[3];
   float SCALE;

   BOOLINT NONLIN;
   miniv3d CRDGEN;
   miniv3d CORNER[8];
   miniv3d NORMAL[8];
   double EXTENT;
   double EXT1;
   };

//! warp kernel for global coordinate systems
class miniwarp: public miniwarpbase
   {
   public:

   //! global coordinate systems
   enum MINIWARP
      {
      MINIWARP_TILESET=0,    // tileset coordinates
      MINIWARP_METRIC=1,     // metric coordinates
      MINIWARP_PLAIN=2,      // plain coordinates
      MINIWARP_CENTER=3,     // center coordinates
      MINIWARP_DATA=4,       // data coordinates
      MINIWARP_ORIGINAL=5,   // original coordinates
      MINIWARP_LOCAL=6,      // local coordinates
      MINIWARP_INTERNAL=7,   // internal coordinates
      MINIWARP_REVERTED=8,   // reverted coordinates
      MINIWARP_AFFINE=9,     // affine coordinates
      MINIWARP_REFERENCE=10, // reference coordinates
      MINIWARP_FINAL=11,     // final coordinates
      MINIWARP_NORMAL=12,    // normal coordinates
      MINIWARP_TILE=13,      // tile coordinates
      MINIWARP_WARP=14       // warp coordinates
      };

   //! default constructor
   miniwarp();

   //! destructor
   ~miniwarp();

   //! define tileset coordinates
   void def_tileset(const minicoord::MINICOORD sysTLS);

   //! define data coordinates
   void def_data(const minicoord bboxDAT[2]);

   //! define geo-graphic coordinates
   void def_geo(const minicoord centerGEO,const minicoord northGEO);

   //! define conversion to local coordinates
   void def_2local(const miniv3d &offsetLOC,const miniv3d &scalingLOC,double scaleLOC);

   //! define conversion to affine coordinates
   void def_2affine(const miniv4d mtxAFF[3]);

   //! define conversion to reference coordinates
   void def_2reference(const miniv4d mtxREF[3]);

   //! get inverse affine coordinate conversion
   void get_invaff(miniv4d invAFF[3]);

   //! set actual warp
   void setwarp(MINIWARP from,MINIWARP to);

   //! get tileset coordinate system
   minicoord::MINICOORD gettls();

   //! get data coordinate system
   minicoord::MINICOORD getdat();

   //! get geo-graphic center point
   minicoord getcenter();

   //! get geo-graphic north point
   minicoord getnorth();

   //! get geo-graphic coordinate system
   minicoord::MINICOORD getgeo();

   //! get crs zone of data
   int getcrszone();

   //! get crs datum of data
   int getcrsdatum();

   //! get local down-scaling factor
   double getscaleloc();

   //! set tile selection window
   void settile(const miniv3d &scale,const miniv3d &bias);

   //! set corners of warp box
   void setcorners(const miniv3d p[8],const miniv3d n[8],double e);

   //! perform warp of a point
   minicoord warp(const miniv4d &p); // fourth component is time

   //! perform warp of a coordinate
   minicoord warp(const minicoord &p);

   //! perform warp of a vector v at position p using the inverse transpose
   miniv3d invtra(const miniv3d &v,const minicoord &p);

   //! enable non-linear warp
   void usenonlin(BOOLINT on);

   protected:

   minicoord::MINICOORD SYSTLS;

   minicoord BBOXDAT[2];

   minicoord::MINICOORD SYSDAT;

   int CRSZONE;
   minicoord::MINICOORD_DATUM CRSDATUM;

   minicoord CENTERGEO;
   minicoord NORTHGEO;

   minicoord::MINICOORD SYSGEO;

   miniv3d OFFSETLOC,SCALINGLOC;
   double SCALELOC;

   miniv4d MTXAFF[3];
   miniv4d MTXREF[3];

   BOOLINT HAS_DATA;

   miniv4d MTX_2MET[3];
   miniv4d MTX_2PLN[3];
   miniv4d MTX_2CNT[3];
   miniv4d MTX_2DAT[3];
   miniv4d MTX_2ORG[3];
   miniv4d MTX_2LOC[3];
   miniv4d MTX_2INT[3];
   miniv4d MTX_2REV[3];
   miniv4d MTX_2AFF[3];
   miniv4d MTX_2REF[3];
   miniv4d MTX_2FIN[3];
   miniv4d MTX_2NRM[3];
   miniv4d MTX_2TIL[3];
   miniv4d MTX_2WRP[3];

   miniv4d INV_2MET[3];
   miniv4d INV_2PLN[3];
   miniv4d INV_2CNT[3];
   miniv4d INV_2DAT[3];
   miniv4d INV_2ORG[3];
   miniv4d INV_2LOC[3];
   miniv4d INV_2INT[3];
   miniv4d INV_2REV[3];
   miniv4d INV_2AFF[3];
   miniv4d INV_2REF[3];
   miniv4d INV_2FIN[3];
   miniv4d INV_2NRM[3];
   miniv4d INV_2TIL[3];
   miniv4d INV_2WRP[3];

   MINIWARP FROM,TO;

   private:

   miniv3d MTX_ZERO[3];
   miniv3d MTX_ONE[3];

   void update_mtx();
   void update_wrp();
   void update_inv();
   void update_tra();
   void update_invtra();
   void update_scl();

   void calc_til(const miniv3d &scale,const miniv3d &bias);
   void calc_wrp();
   };

#endif
