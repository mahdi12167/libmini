// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"

#include "miniwarp.h"

miniwarp::miniwarp()
   {
   // set up zero matrix
   MTX_ZERO[0]=MTX_ZERO[1]=MTX_ZERO[2]=miniv3d(0.0);

   // set up identity matrix
   MTX_ONE[0]=miniv3d(1.0,0.0,0.0);
   MTX_ONE[1]=miniv3d(0.0,1.0,0.0);
   MTX_ONE[2]=miniv3d(0.0,0.0,1.0);

   SYSTLS=minicoord::MINICOORD_LINEAR;

   BBOXDAT[0]=BBOXDAT[1]=minicoord();

   SYSDAT=minicoord::MINICOORD_NONE;
   UTMZONE=UTMDATUM=0;

   CENTERGEO=minicoord(miniv3d(0.0,0.0,0.0),minicoord::MINICOORD_LINEAR);
   NORTHGEO=minicoord(miniv3d(0.0,1.0,0.0),minicoord::MINICOORD_LINEAR);

   SYSGEO=minicoord::MINICOORD_LINEAR;

   OFFSETLOC=miniv3d(0.0);
   SCALINGLOC=miniv3d(1.0);

   SCALELOC=1.0;

   cpy_mtx(MTXAFF,MTX_ONE);
   cpy_mtx(MTXREF,MTX_ONE);

   SYSWRP=minicoord::MINICOORD_NONE;

   HAS_DATA=FALSE;

   cpy_mtx(MTX_2MET,MTX_ZERO);
   cpy_mtx(MTX_2PLN,MTX_ZERO);
   cpy_mtx(MTX_2CNT,MTX_ZERO);
   cpy_mtx(MTX_2DAT,MTX_ZERO);
   cpy_mtx(MTX_2ORG,MTX_ZERO);
   cpy_mtx(MTX_2LOC,MTX_ZERO);
   cpy_mtx(MTX_2INT,MTX_ZERO);
   cpy_mtx(MTX_2REV,MTX_ZERO);
   cpy_mtx(MTX_2AFF,MTX_ZERO);
   cpy_mtx(MTX_2REF,MTX_ZERO);
   cpy_mtx(MTX_2FIN,MTX_ZERO);
   cpy_mtx(MTX_2TIL,MTX_ZERO);
   cpy_mtx(MTX_2WRP,MTX_ZERO);

   cpy_mtx(INV_2MET,MTX_ZERO);
   cpy_mtx(INV_2PLN,MTX_ZERO);
   cpy_mtx(INV_2CNT,MTX_ZERO);
   cpy_mtx(INV_2DAT,MTX_ZERO);
   cpy_mtx(INV_2ORG,MTX_ZERO);
   cpy_mtx(INV_2LOC,MTX_ZERO);
   cpy_mtx(INV_2INT,MTX_ZERO);
   cpy_mtx(INV_2REV,MTX_ZERO);
   cpy_mtx(INV_2AFF,MTX_ZERO);
   cpy_mtx(INV_2REF,MTX_ZERO);
   cpy_mtx(INV_2FIN,MTX_ZERO);
   cpy_mtx(INV_2TIL,MTX_ZERO);
   cpy_mtx(INV_2WRP,MTX_ZERO);

   FROM=TO=MINIWARP_PLAIN;

   cpy_mtx(MTX,MTX_ONE);
   cpy_mtx(INV,MTX_ONE);
   cpy_mtx(TRA,MTX_ONE);
   cpy_mtx(INVTRA,MTX_ONE);

   SCALE=1.0;
   }

// destructor
miniwarp::~miniwarp() {}

// define tileset coordinates
void miniwarp::def_tileset(const minicoord::MINICOORD sysTLS)
   {
   if (sysTLS==minicoord::MINICOORD_LLH ||
       sysTLS==minicoord::MINICOORD_MERC ||
       sysTLS==minicoord::MINICOORD_UTM) ERRORMSG();

   SYSTLS=sysTLS;

   update_mtx();
   }

// define data coordinates
void miniwarp::def_data(const minicoord bboxDAT[2])
   {
   if (bboxDAT[0].type!=bboxDAT[1].type ||
       bboxDAT[0].utm_zone!=bboxDAT[1].utm_zone ||
       bboxDAT[0].utm_datum!=bboxDAT[1].utm_datum) ERRORMSG();

   BBOXDAT[0]=bboxDAT[0];
   BBOXDAT[1]=bboxDAT[1];

   SYSDAT=BBOXDAT[0].type;
   UTMZONE=BBOXDAT[0].utm_zone;
   UTMDATUM=BBOXDAT[0].utm_datum;

   HAS_DATA=TRUE;

   update_mtx();
   }

// define geo-graphic coordinates
void miniwarp::def_geo(const minicoord centerGEO,const minicoord northGEO)
   {
   if (centerGEO.type!=northGEO.type) ERRORMSG();

   CENTERGEO=centerGEO;
   NORTHGEO=northGEO;

   SYSGEO=centerGEO.type;
   }

// define conversion to local coordinates
void miniwarp::def_2local(const miniv3d &offsetLOC,const miniv3d &scalingLOC,double scaleLOC)
   {
   OFFSETLOC=offsetLOC;
   SCALINGLOC=scalingLOC;

   SCALELOC=scaleLOC;

   update_mtx();
   }

// define conversion to affine coordinates
void miniwarp::def_2affine(const miniv4d mtxAFF[3])
   {
   cpy_mtx(MTXAFF,mtxAFF);

   update_mtx();
   }

// define conversion to reference coordinates
void miniwarp::def_2reference(const miniv4d mtxREF[3])
   {
   cpy_mtx(MTXREF,mtxREF);

   update_mtx();
   }

// define warp coordinates
void miniwarp::def_warp(const minicoord::MINICOORD sysWRP)
   {
   SYSWRP=sysWRP;

   update_mtx();
   }

// get inverse affine coordinate conversion
void miniwarp::get_invaff(miniv4d invAFF[3])
   {cpy_mtx(invAFF,INV_2AFF);}

// set actual warp
void miniwarp::setwarp(MINIWARP from,MINIWARP to)
   {
   FROM=from;
   TO=to;

   update_wrp();
   update_inv();
   update_tra();
   update_invtra();
   update_scl();
   }

// get actual warp matrix
void miniwarp::getwarp(miniv4d mtx[3])
   {cpy_mtx(mtx,MTX);}

// get inverse of actual warp matrix
void miniwarp::getinv(miniv4d inv[3])
   {cpy_mtx(inv,INV);}

// get transpose of actual warp matrix
void miniwarp::gettra(miniv3d tra[3])
   {cpy_mtx(tra,TRA);}

// get inverse transpose of actual warp matrix
void miniwarp::getinvtra(miniv3d invtra[3])
   {cpy_mtx(invtra,INVTRA);}

// get actual scaling factor
double miniwarp::getscale()
   {return(SCALE);}

// get nth corner of warp box
miniv3d miniwarp::getcorner(int n)
   {return(CORNER[n]);}

// get data coordinate system
minicoord::MINICOORD miniwarp::getdat()
   {return(SYSDAT);}

// get tileset coordinate system
minicoord::MINICOORD miniwarp::gettls()
   {return(SYSTLS);}

// get geo-graphic center point
minicoord miniwarp::getcenter()
   {return(CENTERGEO);}

// get geo-graphic north point
minicoord miniwarp::getnorth()
   {return(NORTHGEO);}

// get geo-graphic coordinate system
minicoord::MINICOORD miniwarp::getgeo()
   {return(SYSGEO);}

// get utm zone of data
int miniwarp::getutmzone()
   {return(UTMZONE);}

// get utm datum of data
int miniwarp::getutmdatum()
   {return(UTMDATUM);}

// get local down-scaling factor
double miniwarp::getscaleloc()
   {return(SCALELOC);}

// perform warp of a point
minicoord miniwarp::warp(const miniv4d &p)
   {
   if (FROM==MINIWARP_DATA) return(warp(minicoord(p,SYSDAT,UTMZONE,UTMDATUM)));
   else return(warp(minicoord(p,minicoord::MINICOORD_LINEAR)));
   }

// perform warp of a coordinate
minicoord miniwarp::warp(const minicoord &p)
   {
   miniv4d v1;
   minicoord p1,p2;

   if (FROM==MINIWARP_DATA)
      if (TO==MINIWARP_TILESET && SYSTLS!=minicoord::MINICOORD_LINEAR)
         {
         p2=p;
         p2.convert2(SYSTLS);
         }
      else if (TO==MINIWARP_DATA)
         {
         p2=p;
         p2.convert2(SYSDAT,UTMZONE,UTMDATUM);
         }
      else
         {
         p1=p;
         p1.convert2(SYSDAT,UTMZONE,UTMDATUM);
         v1=miniv4d(p1.vec,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),minicoord::MINICOORD_LINEAR);
         }
   else if (FROM==MINIWARP_TILESET)
      if (TO==MINIWARP_TILESET && SYSTLS!=minicoord::MINICOORD_LINEAR)
         {
         p2=p;
         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=SYSTLS;
         p2.convert2(SYSTLS);
         }
      else if (TO==MINIWARP_DATA && SYSTLS!=minicoord::MINICOORD_LINEAR)
         {
         p2=p;
         if (p2.type==minicoord::MINICOORD_LINEAR) p2.type=SYSTLS;
         p2.convert2(SYSDAT,UTMZONE,UTMDATUM);
         }
      else
         {
         p1=p;
         if (p1.type==minicoord::MINICOORD_LINEAR) p1.type=SYSTLS;
         if (SYSTLS!=minicoord::MINICOORD_LINEAR) p1.convert2(SYSDAT,UTMZONE,UTMDATUM);
         v1=miniv4d(p1.vec,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),minicoord::MINICOORD_LINEAR);
         }
   else
      if (TO==MINIWARP_TILESET && SYSTLS!=minicoord::MINICOORD_LINEAR)
         {
         v1=miniv4d(p.vec,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),SYSDAT,UTMZONE,UTMDATUM);
         p2.convert2(SYSTLS);
         }
      else if (TO==MINIWARP_DATA)
         {
         v1=miniv4d(p.vec,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),SYSDAT,UTMZONE,UTMDATUM);
         }
      else
         {
         v1=miniv4d(p.vec,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),minicoord::MINICOORD_LINEAR);
         }

   return(p2);
   }

// perform warp of a vector v at position p using the inverse transpose
miniv3d miniwarp::invtra(const miniv3d &v,const minicoord &p)
   {
   miniv4d v1;
   minicoord p1,p2;

   static const double scale=1000.0;

   if ((FROM!=MINIWARP_TILESET && TO!=MINIWARP_TILESET) || SYSTLS==minicoord::MINICOORD_LINEAR)
      {
      v1=miniv4d(v,1.0);
      return(miniv3d(INVTRA[0]*v1*SCALE,INVTRA[1]*v1*SCALE,INVTRA[2]*v1*SCALE));
      }

   p1=warp(p);
   p2=warp(minicoord(miniv3d(p.vec)+v*scale,p.type,p.utm_zone,p.utm_datum));
   v1=p2.vec-p1.vec;
   v1.normalize();

   return(v1);
   }

// update conversion matrices
void miniwarp::update_mtx()
   {
   if (HAS_DATA)
      {
      // conversion 2 center coordinates:

      MTX_2CNT[0]=miniv4d(1.0,0.0,0.0,-0.5);
      MTX_2CNT[1]=miniv4d(0.0,1.0,0.0,-0.5);
      MTX_2CNT[2]=miniv4d(0.0,0.0,1.0,-0.5);

      inv_mtx(INV_2CNT,MTX_2CNT);

      // conversion 2 data coordinates:

      MTX_2DAT[0]=miniv4d(BBOXDAT[1].vec.x-BBOXDAT[0].vec.x,0.0,0.0,BBOXDAT[0].vec.x);
      MTX_2DAT[1]=miniv4d(0.0,BBOXDAT[1].vec.y-BBOXDAT[0].vec.y,0.0,BBOXDAT[0].vec.y);
      MTX_2DAT[2]=miniv4d(0.0,0.0,BBOXDAT[1].vec.z-BBOXDAT[0].vec.z,BBOXDAT[0].vec.z);

      mlt_mtx(MTX_2DAT,MTX_2DAT,INV_2CNT);

      inv_mtx(INV_2DAT,MTX_2DAT);

      // conversion 2 original coordinates:

      MTX_2ORG[0]=miniv4d(1.0,0.0,0.0);
      MTX_2ORG[1]=miniv4d(0.0,1.0,0.0);
      MTX_2ORG[2]=miniv4d(0.0,0.0,1.0/(SCALELOC*SCALINGLOC.z));

      inv_mtx(INV_2ORG,MTX_2ORG);

      // conversion 2 local coordinates:

      MTX_2LOC[0]=miniv4d(SCALINGLOC.x,0.0,0.0,OFFSETLOC.x*SCALINGLOC.x);
      MTX_2LOC[1]=miniv4d(0.0,SCALINGLOC.y,0.0,OFFSETLOC.y*SCALINGLOC.y);
      MTX_2LOC[2]=miniv4d(0.0,0.0,SCALINGLOC.z,OFFSETLOC.z*SCALINGLOC.z);

      inv_mtx(INV_2LOC,MTX_2LOC);

      // conversion 2 internal coordinates:

      MTX_2INT[0]=miniv3d(1.0,0.0,0.0);
      MTX_2INT[1]=miniv3d(0.0,0.0,1.0);
      MTX_2INT[2]=miniv3d(0.0,-1.0,0.0);

      inv_mtx(INV_2INT,MTX_2INT);

      // conversion 2 reverted coordinates:

      MTX_2REV[0]=INV_2INT[0];
      MTX_2REV[1]=INV_2INT[1];
      MTX_2REV[2]=INV_2INT[2];

      inv_mtx(INV_2REV,MTX_2REV);

      // conversion 2 affine coordinates:

      MTX_2AFF[0]=MTXAFF[0];
      MTX_2AFF[1]=MTXAFF[1];
      MTX_2AFF[2]=MTXAFF[2];

      inv_mtx(INV_2AFF,MTX_2AFF);

      // conversion 2 reference coordinates:

      MTX_2REF[0]=MTXREF[0];
      MTX_2REF[1]=MTXREF[1];
      MTX_2REF[2]=MTXREF[2];

      inv_mtx(INV_2REF,MTX_2REF);

      // conversion 2 final coordinates:

      MTX_2FIN[0]=MTX_2INT[0];
      MTX_2FIN[1]=MTX_2INT[1];
      MTX_2FIN[2]=MTX_2INT[2];

      inv_mtx(INV_2FIN,MTX_2FIN);

      // conversion 2 tile coordinates:

      mlt_mtx(MTX_2TIL,INV_2DAT,INV_2ORG,INV_2LOC,INV_2INT,INV_2REV,INV_2AFF,INV_2FIN);

      inv_mtx(INV_2TIL,MTX_2TIL);

      // conversion 2 warp coordinates:

      calc_wrp();

      inv_mtx(INV_2WRP,MTX_2WRP);

      // conversion 2 metric coordinates:

      MTX_2PLN[0]=miniv4d(1.0/SCALELOC,0.0,0.0);
      MTX_2PLN[1]=miniv4d(0.0,1.0/SCALELOC,0.0);
      MTX_2PLN[2]=miniv4d(0.0,0.0,1.0/SCALELOC);

      mlt_mtx(MTX_2PLN,INV_2CNT,INV_2DAT,INV_2ORG,INV_2LOC,INV_2INT,INV_2REV,INV_2AFF,MTX_2PLN);

      inv_mtx(INV_2PLN,MTX_2PLN);

      // conversion 2 tileset coordinates:

      if (SYSDAT==minicoord::MINICOORD_LINEAR) SYSTLS=minicoord::MINICOORD_LINEAR;

      if (SYSTLS==minicoord::MINICOORD_LINEAR) cpy_mtx(MTX_2MET,MTX_ONE);
      else mlt_mtx(MTX_2MET,INV_2PLN,INV_2CNT,INV_2DAT);

      inv_mtx(INV_2MET,MTX_2MET);
      }
   }

// calculate the actual 4x3 warp matrix
void miniwarp::update_wrp()
   {
   int i;

   cpy_mtx(MTX,MTX_ONE);

   if (FROM<TO)
      // warp forward by concatenating the respective conversion matrices
      for (i=FROM+1; i<=TO; i++)
         switch (i)
            {
            case MINIWARP_METRIC: mlt_mtx(MTX,MTX_2MET,MTX); break;
            case MINIWARP_PLAIN: mlt_mtx(MTX,MTX_2PLN,MTX); break;
            case MINIWARP_CENTER: mlt_mtx(MTX,MTX_2CNT,MTX); break;
            case MINIWARP_DATA: mlt_mtx(MTX,MTX_2DAT,MTX); break;
            case MINIWARP_ORIGINAL: mlt_mtx(MTX,MTX_2ORG,MTX); break;
            case MINIWARP_LOCAL: mlt_mtx(MTX,MTX_2LOC,MTX); break;
            case MINIWARP_INTERNAL: mlt_mtx(MTX,MTX_2INT,MTX); break;
            case MINIWARP_REVERTED: mlt_mtx(MTX,MTX_2REV,MTX); break;
            case MINIWARP_AFFINE: mlt_mtx(MTX,MTX_2AFF,MTX); break;
            case MINIWARP_REFERENCE: mlt_mtx(MTX,MTX_2REF,MTX); break;
            case MINIWARP_FINAL: mlt_mtx(MTX,MTX_2FIN,MTX); break;
            case MINIWARP_TILE: mlt_mtx(MTX,MTX_2TIL,MTX); break;
            case MINIWARP_WARP: mlt_mtx(MTX,MTX_2WRP,MTX); break;
            }
   else if (FROM>TO)
      // warp backward by concatenating the respective inverse matrices
      for (i=FROM-1; i>=TO; i--)
         switch (i)
            {
            case MINIWARP_TILE: mlt_mtx(MTX,INV_2WRP,MTX); break;
            case MINIWARP_FINAL: mlt_mtx(MTX,INV_2TIL,MTX); break;
            case MINIWARP_REFERENCE: mlt_mtx(MTX,INV_2FIN,MTX); break;
            case MINIWARP_AFFINE: mlt_mtx(MTX,INV_2REF,MTX); break;
            case MINIWARP_REVERTED: mlt_mtx(MTX,INV_2AFF,MTX); break;
            case MINIWARP_INTERNAL: mlt_mtx(MTX,INV_2REV,MTX); break;
            case MINIWARP_LOCAL: mlt_mtx(MTX,INV_2INT,MTX); break;
            case MINIWARP_ORIGINAL: mlt_mtx(MTX,INV_2LOC,MTX); break;
            case MINIWARP_DATA: mlt_mtx(MTX,INV_2ORG,MTX); break;
            case MINIWARP_CENTER: mlt_mtx(MTX,INV_2DAT,MTX); break;
            case MINIWARP_PLAIN: mlt_mtx(MTX,INV_2CNT,MTX); break;
            case MINIWARP_METRIC: mlt_mtx(MTX,INV_2PLN,MTX); break;
            case MINIWARP_TILESET: mlt_mtx(MTX,INV_2MET,MTX); break;
            }
   }

// calculate the inverse of the 4x3 warp matrix
void miniwarp::update_inv()
   {inv_mtx(INV,MTX);}

// construct the 3x3 transpose of the 4x3 warp matrix
void miniwarp::update_tra()
   {tra_mtx(TRA,MTX);}

// calculate the 3x3 inverse transpose of the 4x3 warp matrix
void miniwarp::update_invtra()
   {
   cpy_mtx(INVTRA,MTX);

   // take care of tileset coordinates
   if (TO==MINIWARP_TILESET) mlt_mtx(INVTRA,MTX_2MET,INVTRA);
   else if (FROM==MINIWARP_TILESET) mlt_mtx(INVTRA,INVTRA,INV_2MET);

   // construct the inverse transpose matrix
   inv_mtx(INVTRA,INVTRA);
   tra_mtx(INVTRA,INVTRA);
   }

// calculate the scaling factor of the 4x3 warp matrix
void miniwarp::update_scl()
   {
   int i;

   double avg=0.0;

   for (i=0; i<3; i++)
      {
      // assume that the matrix is orthogonal
      avg+=INVTRA[i].x*INVTRA[i].x;
      avg+=INVTRA[i].y*INVTRA[i].y;
      avg+=INVTRA[i].z*INVTRA[i].z;
      }

   SCALE=sqrt(avg/3.0f);
   if (SCALE!=0.0) SCALE=1.0/SCALE;
   }

// calculate warp coordinate conversion
void miniwarp::calc_wrp()
   {
   int i;

   minicoord bboxGEO[2];

   double x1,x2,y1,y2,z1,z2;

   minicoord p[8];
   miniv4d b,e[3];

   cpy_mtx(MTX_2WRP,MTX_ONE);

   // check if warp coordinate conversion is disabled
   if (SYSWRP==minicoord::MINICOORD_NONE) return;

   // check if warp coordinate conversion is possible
   if (SYSDAT!=minicoord::MINICOORD_LLH &&
       SYSDAT!=minicoord::MINICOORD_MERC &&
       SYSDAT!=minicoord::MINICOORD_UTM) return;

   // fetch geo-referenced bounding box
   bboxGEO[0]=BBOXDAT[0];
   bboxGEO[1]=BBOXDAT[1];

   // convert bounding box to geographic coordinates
   bboxGEO[0].convert2(minicoord::MINICOORD_LLH);
   bboxGEO[1].convert2(minicoord::MINICOORD_LLH);

   // get extents of geo-referenced bbox
   x1=bboxGEO[0].vec.x;
   x2=bboxGEO[1].vec.x;
   y1=bboxGEO[0].vec.y;
   y2=bboxGEO[1].vec.y;
   z1=bboxGEO[0].vec.z;
   z2=bboxGEO[1].vec.z;

   // construct corners of geo-referenced bbox
   p[0]=minicoord(miniv3d(x1,y1,z1),minicoord::MINICOORD_LLH);
   p[1]=minicoord(miniv3d(x2,y1,z1),minicoord::MINICOORD_LLH);
   p[2]=minicoord(miniv3d(x1,y2,z1),minicoord::MINICOORD_LLH);
   p[3]=minicoord(miniv3d(x2,y2,z1),minicoord::MINICOORD_LLH);
   p[4]=minicoord(miniv3d(x1,y1,z2),minicoord::MINICOORD_LLH);
   p[5]=minicoord(miniv3d(x2,y1,z2),minicoord::MINICOORD_LLH);
   p[6]=minicoord(miniv3d(x1,y2,z2),minicoord::MINICOORD_LLH);
   p[7]=minicoord(miniv3d(x2,y2,z2),minicoord::MINICOORD_LLH);

   // warp geo-referenced corners
   for (i=0; i<8; i++)
      {
      p[i].convert2(SYSWRP);
      CORNER[i]=p[i].vec;
      }

   // calculate warped barycenter
   b=(p[0].vec+p[1].vec+p[2].vec+p[3].vec+p[4].vec+p[5].vec+p[6].vec+p[7].vec)/8.0;

   // average warped edges along each axis
   e[0]=(p[1].vec-p[0].vec+p[3].vec-p[2].vec+p[5].vec-p[4].vec+p[7].vec-p[6].vec)/4.0;
   e[1]=(p[2].vec-p[0].vec+p[3].vec-p[1].vec+p[6].vec-p[4].vec+p[7].vec-p[5].vec)/4.0;
   e[2]=(p[4].vec-p[0].vec+p[5].vec-p[1].vec+p[6].vec-p[2].vec+p[7].vec-p[3].vec)/4.0;

   // construct approxiate matrix for the warp
   MTX_2WRP[0]=miniv4d(e[0].x,e[1].x,e[2].x,b.x);
   MTX_2WRP[1]=miniv4d(e[0].y,e[1].y,e[2].y,b.y);
   MTX_2WRP[2]=miniv4d(e[0].z,e[1].z,e[2].z,b.z);
   }
