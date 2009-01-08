// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "minicoord.h"

class miniwarp;

//! warp kernel base
class miniwarpbase
   {
   public:

   //! default constructor
   miniwarpbase();

   //! copy constructor
   miniwarpbase(const miniwarp &warp);

   //! destructor
   ~miniwarpbase();

   //! get actual warp matrix
   void getwarp(miniv4d mtx[3]); // fourth row is assumed to be (0,0,0,1)

   //! get corners of warp box
   void getcorners(miniv3d p[8]);

   protected:

   miniv4d MTX[3];
   miniv3d CORNER[8];
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

   //! get inverse of actual warp matrix
   void getinv(miniv4d inv[3]);

   //! get transpose of actual warp matrix
   void gettra(miniv3d tra[3]);

   //! get inverse transpose of actual warp matrix
   void getinvtra(miniv3d invtra[3]);

   //! get actual scaling factor
   double getscale();

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

   //! get utm zone of data
   int getutmzone();

   //! get utm datum of data
   int getutmdatum();

   //! get local down-scaling factor
   double getscaleloc();

   //! set tile selection window
   void settile(const miniv3d &scale,const miniv3d &bias);

   //! set corners of warp box
   void setcorners(const miniv3d p[8]);

   //! perform warp of a point
   minicoord warp(const miniv4d &p); // fourth component is time

   //! perform warp of a coordinate
   minicoord warp(const minicoord &p);

   //! perform warp of a vector v at position p using the inverse transpose
   miniv3d invtra(const miniv3d &v,const minicoord &p);

   protected:

   miniv3d MTX_ZERO[3];
   miniv3d MTX_ONE[3];

   minicoord::MINICOORD SYSTLS;

   minicoord BBOXDAT[2];

   minicoord::MINICOORD SYSDAT;
   int UTMZONE,UTMDATUM;

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

   miniv4d INV[3],TRA[3];
   miniv4d INVTRA[3];
   float SCALE;

   private:

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
