// (c) by Stefan Roettger

#include "minibase.h"

#include "miniutm.h"

#include "miniwarp.h"

// default constructor
minicoord::minicoord()
   {
   vec=miniv4d(0.0);
   type=MINICOORD_NONE;

   utm_zone=utm_datum=0;
   }

// copy constructor
minicoord::minicoord(const minicoord &c)
   {
   vec=c.vec;
   type=c.type;

   utm_zone=c.utm_zone;
   utm_datum=c.utm_datum;
   }

// constructors:

minicoord::minicoord(const miniv3d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   utm_zone=utm_datum=0;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   utm_zone=utm_datum=0;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t,const int zone,const int datum)
   {
   vec=v;
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

minicoord::minicoord(const miniv4d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   utm_zone=utm_datum=0;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   utm_zone=utm_datum=0;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t,const int zone,const int datum)
   {
   vec=v;
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   utm_zone=utm_datum=0;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const int datum)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

// destructor
minicoord::~minicoord() {}

// convert from 1 coordinate system 2 another
void minicoord::convert2(const MINICOORD t,const int zone,const int datum)
   {
   double xyz[3];

   if (t==type) return;

   switch (type)
      {
      case MINICOORD_LLH:
         switch (t)
            {
            case MINICOORD_UTM:
               miniutm::LL2UTM(vec.y,vec.x,zone,datum,&xyz[0],&xyz[1]);
               vec=miniv4d(xyz[0],xyz[1],vec.z,vec.w);
               type=t;
               utm_zone=zone;
               utm_datum=datum;
               break;
            case MINICOORD_ECEF:
               miniutm::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               utm_zone=utm_datum=0;
               break;
            case MINICOORD_NONE:
            default: ERRORMSG();
            }
         break;
      case MINICOORD_UTM:
         switch (t)
            {
            case MINICOORD_LLH:
               miniutm::UTM2LL(vec.x,vec.y,utm_zone,utm_datum,&xyz[1],&xyz[0]);
               vec=miniv4d(xyz[0],xyz[1],vec.z,vec.w);
               type=t;
               utm_zone=utm_datum=0;
               break;
            case MINICOORD_ECEF:
               miniutm::UTM2LL(vec.x,vec.y,utm_zone,utm_datum,&xyz[1],&xyz[0]);
               miniutm::LLH2ECEF(xyz[1],xyz[0],vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               utm_zone=utm_datum=0;
               break;
            case MINICOORD_NONE:
            default: ERRORMSG();
            }
         break;
      case MINICOORD_ECEF:
         switch (t)
            {
            case MINICOORD_LLH:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               miniutm::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               type=t;
               utm_zone=utm_datum=0;
               break;
            case MINICOORD_UTM:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               miniutm::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               miniutm::LL2UTM(vec.y,vec.x,zone,datum,&xyz[0],&xyz[1]);
               vec=miniv4d(xyz[0],xyz[1],vec.z,vec.w);
               type=t;
               utm_zone=zone;
               utm_datum=datum;
               break;
            case MINICOORD_NONE:
            default: ERRORMSG();
            }
         break;
      case MINICOORD_NONE:
      default: ERRORMSG();
      }
   }

// linear conversion defined by 3x3 matrix and offset
void minicoord::convert(const miniv3d mtx[3],const miniv3d offset)
   {
   miniv3d v;

   if (type==MINICOORD_NONE) ERRORMSG();

   v=vec;

   vec.x=v*mtx[0]+offset.x;
   vec.y=v*mtx[1]+offset.y;
   vec.z=v*mtx[2]+offset.z;

   type=MINICOORD_LINEAR;
   }

// linear conversion defined by 4x3 matrix
void minicoord::convert(const miniv4d mtx[3])
   {
   miniv4d v1;

   if (type==MINICOORD_NONE) ERRORMSG();

   v1=miniv4d(vec,1.0);

   vec.x=mtx[0]*v1;
   vec.y=mtx[1]*v1;
   vec.z=mtx[2]*v1;

   type=MINICOORD_LINEAR;
   }

// non-linear conversion defined by point 2 point correspondences
void minicoord::convert(const miniv3d src[2],const miniv3d dst[8])
   {
   miniv3d u,v;

   if (type==MINICOORD_NONE) ERRORMSG();

   if (src[0]==src[1]) ERRORMSG();

   u.x=(vec.x-src[0].x)/(src[1].x-src[0].x);
   u.y=(vec.y-src[0].y)/(src[1].y-src[0].y);
   u.z=(vec.z-src[0].z)/(src[1].z-src[0].z);

   v=(1.0-u.z)*((1.0-u.y)*((1.0-u.x)*dst[0]+
                           u.x*dst[1])+
                u.y*((1.0-u.x)*dst[2]+
                     u.x*dst[3]))+
     u.z*((1.0-u.y)*((1.0-u.x)*dst[4]+
                     u.x*dst[5])+
          u.y*((1.0-u.x)*dst[6]+
               u.x*dst[7]));

   vec=miniv4d(v,vec.w);

   type=MINICOORD_LINEAR;
   }

// default constructor
miniwarp::miniwarp()
   {
   SYSGLB=minicoord::MINICOORD_ECEF;

   BBOXDAT[0]=BBOXDAT[1]=minicoord();

   SYSDAT=minicoord::MINICOORD_NONE;
   UTMZONE=UTMDATUM=0;

   OFFSETLOC=miniv3d(0.0);
   SCALINGLOC=miniv3d(1.0);

   MTXAFF[0]=miniv4d(1.0,0.0,0.0);
   MTXAFF[1]=miniv4d(0.0,1.0,0.0);
   MTXAFF[2]=miniv4d(0.0,0.0,1.0);

   SYSWRP=minicoord::MINICOORD_NONE;

   HAS_DATA=FALSE;

   MTX_2PLN[0]=MTX_2PLN[1]=MTX_2PLN[2]=miniv4d(0.0);
   MTX_2CNT[0]=MTX_2CNT[1]=MTX_2CNT[2]=miniv4d(0.0);
   MTX_2DAT[0]=MTX_2DAT[1]=MTX_2DAT[2]=miniv4d(0.0);
   MTX_2LOC[0]=MTX_2LOC[1]=MTX_2LOC[2]=miniv4d(0.0);
   MTX_2INT[0]=MTX_2INT[1]=MTX_2INT[2]=miniv4d(0.0);
   MTX_2AFF[0]=MTX_2AFF[1]=MTX_2AFF[2]=miniv4d(0.0);
   MTX_2TIL[0]=MTX_2TIL[1]=MTX_2TIL[2]=miniv4d(0.0);
   MTX_2WRP[0]=MTX_2WRP[1]=MTX_2WRP[2]=miniv4d(0.0);

   INV_2PLN[0]=INV_2PLN[1]=INV_2PLN[2]=miniv4d(0.0);
   INV_2CNT[0]=INV_2CNT[1]=INV_2CNT[2]=miniv4d(0.0);
   INV_2DAT[0]=INV_2DAT[1]=INV_2DAT[2]=miniv4d(0.0);
   INV_2LOC[0]=INV_2LOC[1]=INV_2LOC[2]=miniv4d(0.0);
   INV_2INT[0]=INV_2INT[1]=INV_2INT[2]=miniv4d(0.0);
   INV_2AFF[0]=INV_2AFF[1]=INV_2AFF[2]=miniv4d(0.0);
   INV_2TIL[0]=INV_2TIL[1]=INV_2TIL[2]=miniv4d(0.0);
   INV_2WRP[0]=INV_2WRP[1]=INV_2WRP[2]=miniv4d(0.0);

   MTX_MET2DAT[0]=MTX_MET2DAT[1]=MTX_MET2DAT[2]=miniv4d(0.0);
   MTX_DAT2MET[0]=MTX_DAT2MET[1]=MTX_DAT2MET[2]=miniv4d(0.0);

   FROM=TO=MINIWARP_PLAIN;

   MTX[0]=miniv4d(1.0,0.0,0.0);
   MTX[1]=miniv4d(0.0,1.0,0.0);
   MTX[2]=miniv4d(0.0,0.0,1.0);

   INVTRA[0]=miniv4d(1.0,0.0,0.0);
   INVTRA[1]=miniv4d(0.0,1.0,0.0);
   INVTRA[2]=miniv4d(0.0,0.0,1.0);

   SCALE=1.0;
   }

// destructor
miniwarp::~miniwarp() {}

// define global coordinates
void miniwarp::def_global(const minicoord::MINICOORD sysGLB)
   {
   if (sysGLB==minicoord::MINICOORD_LLH) ERRORMSG();
   if (sysGLB==minicoord::MINICOORD_UTM) ERRORMSG();

   SYSGLB=sysGLB;

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

// define conversion to local coordinates
void miniwarp::def_2local(const miniv3d &offsetLOC,const miniv3d &scalingLOC)
   {
   OFFSETLOC=offsetLOC;
   SCALINGLOC=scalingLOC;

   update_mtx();
   }

// define conversion to affine coordinates
void miniwarp::def_2affine(const miniv4d mtxAFF[3])
   {
   MTXAFF[0]=mtxAFF[0];
   MTXAFF[1]=mtxAFF[1];
   MTXAFF[2]=mtxAFF[2];

   update_mtx();
   }

// define warp coordinates
void miniwarp::def_warp(const minicoord::MINICOORD sysWRP)
   {
   SYSWRP=sysWRP;

   update_mtx();
   }

// set actual warp
void miniwarp::setwarp(MINIWARP from,MINIWARP to)
   {
   FROM=from;
   TO=to;

   update_wrp();
   update_inv();
   update_scl();
   }

// get actual warp matrix
void miniwarp::getwarp(miniv4d mtx[3])
   {
   mtx[0]=MTX[0];
   mtx[1]=MTX[1];
   mtx[2]=MTX[2];
   }

// get actual inverse transpose warp matrix
void miniwarp::getinvtra(miniv4d invtra[3])
   {
   invtra[0]=INVTRA[0];
   invtra[1]=INVTRA[1];
   invtra[2]=INVTRA[2];
   }

// get actual scaling factor
double miniwarp::getscale()
   {return(SCALE);}

// get data coordinate system
minicoord::MINICOORD miniwarp::getdat()
   {return(SYSDAT);}

// get global coordinate system
minicoord::MINICOORD miniwarp::getglb()
   {return(SYSGLB);}

// get utm zone of data
int miniwarp::getutmzone()
   {return(UTMZONE);}

// get utm datum of data
int miniwarp::getutmdatum()
   {return(UTMDATUM);}

// perform warp of a point
minicoord miniwarp::warp(const miniv4d &p)
   {
   if (FROM==MINIWARP_DATA) return(warp(minicoord(p,SYSDAT,UTMZONE,UTMDATUM)));
   if (FROM==MINIWARP_GLOBAL) return(warp(minicoord(p,SYSGLB)));
   else return(warp(minicoord(p,minicoord::MINICOORD_LINEAR)));
   }

// perform warp of a coordinate
minicoord miniwarp::warp(const minicoord &p)
   {
   miniv4d v1;
   minicoord p1,p2;

   if (FROM==MINIWARP_DATA)
      if (TO==MINIWARP_GLOBAL && SYSGLB!=minicoord::MINICOORD_LINEAR)
         {
         p2=p;
         p2.convert2(SYSGLB);
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
   else if (FROM==MINIWARP_GLOBAL)
      if (TO==MINIWARP_GLOBAL)
         {
         p2=p;
         p2.convert2(SYSGLB);
         }
      else if (TO==MINIWARP_DATA && SYSGLB!=minicoord::MINICOORD_LINEAR)
         {
         p2=p;
         p2.convert2(SYSDAT,UTMZONE,UTMDATUM);
         }
      else
         {
         p1=p;
         if (SYSGLB!=minicoord::MINICOORD_LINEAR) p1.convert2(SYSDAT,UTMZONE,UTMDATUM);
         v1=miniv4d(p1.vec,1.0);
         if (SYSGLB!=minicoord::MINICOORD_LINEAR) v1=miniv4d(MTX_DAT2MET[0]*v1,MTX_DAT2MET[1]*v1,MTX_DAT2MET[2]*v1,1.0);
         p2=minicoord(miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,p.vec.w),minicoord::MINICOORD_LINEAR);
         }
   else
      if (TO==MINIWARP_GLOBAL && SYSGLB!=minicoord::MINICOORD_LINEAR)
         {
         v1=miniv4d(p.vec,1.0);
         v1=miniv4d(MTX[0]*v1,MTX[1]*v1,MTX[2]*v1,1.0);
         p2=minicoord(miniv4d(MTX_MET2DAT[0]*v1,MTX_MET2DAT[1]*v1,MTX_MET2DAT[2]*v1,p.vec.w),SYSDAT,UTMZONE,UTMDATUM);
         p2.convert2(SYSGLB);
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
   miniv3d v2;

   if ((FROM!=MINIWARP_GLOBAL && TO!=MINIWARP_GLOBAL) || SYSGLB==minicoord::MINICOORD_LINEAR)
      {
      v1=miniv4d(v,1.0);
      return(miniv3d(INVTRA[0]*v1*SCALE,INVTRA[1]*v1*SCALE,INVTRA[2]*v1*SCALE));
      }
   else
      {
      p1=warp(p);
      p2=warp(minicoord(miniv3d(p.vec)+v,p.type,p.utm_zone,p.utm_datum));
      v2=p2.vec-p1.vec;
      v2.normalize();
      return(v2);
      }
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

      // conversion 2 affine coordinates:

      MTX_2AFF[0]=MTXAFF[0];
      MTX_2AFF[1]=MTXAFF[1];
      MTX_2AFF[2]=MTXAFF[2];

      inv_mtx(INV_2AFF,MTX_2AFF);

      // conversion 2 tile coordinates:

      mlt_mtx(MTX_2TIL,INV_2DAT,INV_2LOC,INV_2INT,INV_2AFF);

      inv_mtx(INV_2TIL,MTX_2TIL);

      // conversion 2 warp coordinates:

      calc_wrp();

      inv_mtx(INV_2WRP,MTX_2WRP);

      // conversion 2 metric coordinates:

      if (SYSDAT==minicoord::MINICOORD_LLH)
         {
         miniv4d mtx[3];

         mtx[0]=miniv4d(SCALINGLOC.z,0.0,0.0);
         mtx[1]=miniv4d(0.0,SCALINGLOC.z,0.0);
         mtx[2]=miniv4d(0.0,0.0,SCALINGLOC.z);

         mlt_mtx(MTX_2PLN,INV_2CNT,INV_2DAT,INV_2LOC,mtx);
         }
      else mlt_mtx(MTX_2PLN,INV_2CNT,INV_2DAT);

      inv_mtx(INV_2PLN,MTX_2PLN);

      // conversion 2 global coordinates:

      if (SYSGLB!=minicoord::MINICOORD_LINEAR &&
          SYSDAT==minicoord::MINICOORD_LINEAR)
         SYSGLB=minicoord::MINICOORD_LINEAR;

      mlt_mtx(MTX_MET2DAT,MTX_2DAT,MTX_2CNT,MTX_2PLN);
      mlt_mtx(MTX_DAT2MET,INV_2PLN,INV_2CNT,INV_2DAT);
      }
   }

// calculate the actual 4x3 warp matrix
void miniwarp::update_wrp()
   {
   int i;

   // initialize warp matrix with identity
   MTX[0]=miniv4d(1.0,0.0,0.0);
   MTX[1]=miniv4d(0.0,1.0,0.0);
   MTX[2]=miniv4d(0.0,0.0,1.0);

   if (FROM<TO)
      // warp forward by concatenating the respective conversion matrices
      for (i=FROM+1; i<=TO; i++)
         switch (i)
            {
            case MINIWARP_PLAIN: mlt_mtx(MTX,MTX_2PLN,MTX); break;
            case MINIWARP_CENTER: mlt_mtx(MTX,MTX_2CNT,MTX); break;
            case MINIWARP_DATA: mlt_mtx(MTX,MTX_2DAT,MTX); break;
            case MINIWARP_LOCAL: mlt_mtx(MTX,MTX_2LOC,MTX); break;
            case MINIWARP_INTERNAL: mlt_mtx(MTX,MTX_2INT,MTX); break;
            case MINIWARP_AFFINE: mlt_mtx(MTX,MTX_2AFF,MTX); break;
            case MINIWARP_TILE: mlt_mtx(MTX,MTX_2TIL,MTX); break;
            case MINIWARP_WARP: mlt_mtx(MTX,MTX_2WRP,MTX); break;
            }
   else if (FROM>TO)
      // warp backward by concatenating the respective inverse matrices
      for (i=FROM-1; i>=TO; i--)
         switch (i)
            {
            case MINIWARP_TILE: mlt_mtx(MTX,INV_2WRP,MTX); break;
            case MINIWARP_AFFINE: mlt_mtx(MTX,INV_2TIL,MTX); break;
            case MINIWARP_INTERNAL: mlt_mtx(MTX,INV_2AFF,MTX); break;
            case MINIWARP_LOCAL: mlt_mtx(MTX,INV_2INT,MTX); break;
            case MINIWARP_DATA: mlt_mtx(MTX,INV_2LOC,MTX); break;
            case MINIWARP_CENTER: mlt_mtx(MTX,INV_2DAT,MTX); break;
            case MINIWARP_PLAIN: mlt_mtx(MTX,INV_2CNT,MTX); break;
            case MINIWARP_METRIC: mlt_mtx(MTX,INV_2PLN,MTX); break;
            }
   }

// calculate the inverse transpose of the 4x3 warp matrix
void miniwarp::update_inv()
   {
   miniv3d inv[3];

   // extract 3x3 sub-matrix
   inv[0]=MTX[0];
   inv[1]=MTX[1];
   inv[2]=MTX[2];

   // it is sufficient to invert the 3x3 sub-matrix
   inv_mtx(inv,inv);

   // construct the inverse transpose matrix
   INVTRA[0]=miniv4d(inv[0].x,inv[1].x,inv[2].x);
   INVTRA[1]=miniv4d(inv[0].y,inv[1].y,inv[2].y);
   INVTRA[2]=miniv4d(inv[0].z,inv[1].z,inv[2].z);
   }

// calculate the scaling factor of the 4x3 warp matrix
void miniwarp::update_scl()
   {
   int i;

   double avg=0.0;

   for (i=0; i<3; i++)
      {
      // assume that the matrix is orthogonal
      avg+=MTX[i].x*MTX[i].x;
      avg+=MTX[i].y*MTX[i].y;
      avg+=MTX[i].z*MTX[i].z;
      }

   SCALE=sqrt(avg/3.0f);
   }

// calculate warp coordinate conversion
void miniwarp::calc_wrp()
   {
   int i;

   minicoord bboxGEO[2];

   double x1,x2,y1,y2,z1,z2;

   minicoord p[8];
   miniv4d b,e[3];

   // initialize warp matrix with identity
   MTX_2WRP[0]=miniv4d(1.0,0.0,0.0);
   MTX_2WRP[1]=miniv4d(0.0,1.0,0.0);
   MTX_2WRP[2]=miniv4d(0.0,0.0,1.0);

   // check if warp coordinate conversion is disabled
   if (SYSWRP==minicoord::MINICOORD_NONE) return;

   // check if warp coordinate conversion is possible
   if (SYSDAT!=minicoord::MINICOORD_LLH &&
       SYSDAT!=minicoord::MINICOORD_UTM &&
       SYSDAT!=minicoord::MINICOORD_ECEF) return;

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
   for (i=0; i<8; i++) p[i].convert2(SYSWRP);

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

// multiply two 4x3 matrices
void miniwarp::mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3])
   {
   int i;

   miniv4d m[3];

   for (i=0; i<3; i++)
      {
      // fourth row is assumed to be (0,0,0,1)
      m[i].x=mtx1[i].x*mtx2[0].x+mtx1[i].y*mtx2[1].x+mtx1[i].z*mtx2[2].x;
      m[i].y=mtx1[i].x*mtx2[0].y+mtx1[i].y*mtx2[1].y+mtx1[i].z*mtx2[2].y;
      m[i].z=mtx1[i].x*mtx2[0].z+mtx1[i].y*mtx2[1].z+mtx1[i].z*mtx2[2].z;
      m[i].w=mtx1[i].x*mtx2[0].w+mtx1[i].y*mtx2[1].w+mtx1[i].z*mtx2[2].w+mtx1[i].w;
      }

   mtx[0]=m[0];
   mtx[1]=m[1];
   mtx[2]=m[2];
   }

// multiply three 4x3 matrices
void miniwarp::mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx2,mtx3);
   mlt_mtx(mtx,mtx1,m);
   }

// multiply four 4x3 matrices
void miniwarp::mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx3,mtx4);
   mlt_mtx(mtx,mtx1,mtx2,m);
   }

// invert a 3x3 matrix
void miniwarp::inv_mtx(miniv3d inv[3],const miniv3d mtx[3])
   {
   double det;
   miniv3d m[3];

   // calculate determinant
   det=mtx[0].x*(mtx[2].z*mtx[1].y-mtx[1].z*mtx[2].y)+
       mtx[0].y*(mtx[1].z*mtx[2].x-mtx[2].z*mtx[1].x)+
       mtx[0].z*(mtx[2].y*mtx[1].x-mtx[1].y*mtx[2].x);

   // check determinant
   if (det==0.0) inv[0]=inv[1]=inv[2]=miniv3d(0.0);
   else
      {
      det=1.0/det;

      m[0]=mtx[0];
      m[1]=mtx[1];
      m[2]=mtx[2];

      inv[0].x=det*(m[2].z*m[1].y-m[1].z*m[2].y);
      inv[1].x=det*(m[1].z*m[2].x-m[2].z*m[1].x);
      inv[2].x=det*(m[2].y*m[1].x-m[1].y*m[2].x);
      inv[0].y=det*(m[0].z*m[2].y-m[2].z*m[0].y);
      inv[1].y=det*(m[2].z*m[0].x-m[0].z*m[2].x);
      inv[2].y=det*(m[0].y*m[2].x-m[2].y*m[0].x);
      inv[0].z=det*(m[1].z*m[0].y-m[0].z*m[1].y);
      inv[1].z=det*(m[0].z*m[1].x-m[1].z*m[0].x);
      inv[2].z=det*(m[1].y*m[0].x-m[0].y*m[1].x);
      }
   }

// invert a 4x3 matrix
void miniwarp::inv_mtx(miniv4d inv[3],const miniv4d mtx[3])
   {
   miniv3d m[3];
   miniv4d m1[3],m2[3];

   // extract 3x3 sub-matrix
   m[0]=mtx[0];
   m[1]=mtx[1];
   m[2]=mtx[2];

   // it is sufficient to invert the 3x3 sub-matrix
   inv_mtx(m,m);

   // decompose 4x3 inversion into matrix #1 = inverted 3x3 sub-matrix
   m1[0]=m[0];
   m1[1]=m[1];
   m1[2]=m[2];

   // decompose 4x3 inversion into matrix #2 = inverted 1x3 sub-matrix = negated offset
   m2[0]=miniv4d(1.0,0.0,0.0,-mtx[0].w);
   m2[1]=miniv4d(0.0,1.0,0.0,-mtx[1].w);
   m2[2]=miniv4d(0.0,0.0,1.0,-mtx[2].w);

   // compose the inverted matrix
   mlt_mtx(inv,m1,m2);
   }
