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

minicoord::minicoord(const miniv3d &v,const int t)
   {
   vec=miniv4d(v.x,v.y,v.z);
   type=t;

   utm_zone=0;
   utm_datum=0;
   }

minicoord::minicoord(const miniv3d &v,const int t,const int zone,const int datum)
   {
   vec=miniv4d(v.x,v.y,v.z);
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

minicoord::minicoord(const miniv4d &v,const int t)
   {
   vec=v;
   type=t;

   utm_zone=0;
   utm_datum=0;
   }

minicoord::minicoord(const miniv4d &v,const int t,const int zone,const int datum)
   {
   vec=v;
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const int t)
   {minicoord(miniv3d(cx,cy,cz),t);}

minicoord::minicoord(const double cx,const double cy,const double cz,const int t,const int zone,const int datum)
   {minicoord(miniv3d(cx,cy,cz),t,zone,datum);}

// destructor
minicoord::~minicoord() {}

// convert from 1 coordinate system 2 another
void minicoord::convert2(const int t,const int zone,const int datum)
   {
   double xyz[3];

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
               utm_zone=0;
               utm_datum=0;
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
               utm_zone=0;
               utm_datum=0;
               break;
            case MINICOORD_ECEF:
               miniutm::UTM2LL(vec.x,vec.y,utm_zone,utm_datum,&xyz[1],&xyz[0]);
               miniutm::LLH2ECEF(xyz[1],xyz[0],vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               utm_zone=0;
               utm_datum=0;
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
               utm_zone=0;
               utm_datum=0;
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

   v=miniv3d(vec.x,vec.y,vec.z);

   vec.x=v*mtx[0]+offset.x;
   vec.y=v*mtx[1]+offset.y;
   vec.z=v*mtx[2]+offset.z;

   type=MINICOORD_LINEAR;
   }

// linear conversion defined by 4x3 matrix
void minicoord::convert(const miniv4d mtx[3])
   {
   miniv4d v;

   if (type==MINICOORD_NONE) ERRORMSG();

   v=miniv4d(vec.x,vec.y,vec.z,1.0);

   vec.x=v*mtx[0];
   vec.y=v*mtx[1];
   vec.z=v*mtx[2];

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

   vec=miniv4d(v.x,v.y,v.z,vec.w);

   type=MINICOORD_NONLIN;
   }

// default constructor
miniwarp::miniwarp()
   {
   BBOXDAT[0]=BBOXDAT[1]=miniv3d(0.0);
   BBOXGEO[0]=BBOXGEO[1]=miniv3d(0.0);

   BBOXLOC[0]=BBOXLOC[1]=miniv3d(0.0);

   MTXAFF[0]=MTXAFF[1]=MTXAFF[2]=miniv4d(0.0);
   MTXAFF[0].x=MTXAFF[1].y=MTXAFF[2].z=1.0;

   SYSWRP=minicoord::MINICOORD_ECEF;

   HAS_DATA=HAS_LOCAL=FALSE;

   MTX_2DAT[0]=MTX_2DAT[1]=MTX_2DAT[2]=miniv4d(0.0);
   MTX_2LOC[0]=MTX_2LOC[1]=MTX_2LOC[2]=miniv4d(0.0);
   MTX_2AFF[0]=MTX_2AFF[1]=MTX_2AFF[2]=miniv4d(0.0);
   MTX_2TIL[0]=MTX_2TIL[1]=MTX_2TIL[2]=miniv4d(0.0);
   MTX_2WRP[0]=MTX_2WRP[1]=MTX_2WRP[2]=miniv4d(0.0);

   INV_2DAT[0]=INV_2DAT[1]=INV_2DAT[2]=miniv4d(0.0);
   INV_2LOC[0]=INV_2LOC[1]=INV_2LOC[2]=miniv4d(0.0);
   INV_2AFF[0]=INV_2AFF[1]=INV_2AFF[2]=miniv4d(0.0);
   INV_2TIL[0]=INV_2TIL[1]=INV_2TIL[2]=miniv4d(0.0);
   INV_2WRP[0]=INV_2WRP[1]=INV_2WRP[2]=miniv4d(0.0);

   FROM=TO=MINIWARP_PLAIN;

   MTX[0]=MTX[1]=MTX[2]=miniv4d(0.0);
   MTX[0].x=MTX[1].y=MTX[2].z=1.0;
   }

// destructor
miniwarp::~miniwarp() {}

// define data coordinates
void miniwarp::def_data(const miniv3d bboxDAT[2],
                        const minicoord bboxGEO[2])
   {
   minicoord bbox[2];

   BBOXDAT[0]=bboxDAT[0];
   BBOXDAT[1]=bboxDAT[1];

   bbox[0]=bboxGEO[0];
   bbox[1]=bboxGEO[1];

   bbox[0].convert2(minicoord::MINICOORD_LLH);
   bbox[1].convert2(minicoord::MINICOORD_LLH);

   BBOXGEO[0]=miniv3d(bbox[0].vec.x,bbox[0].vec.y,bbox[0].vec.z);
   BBOXGEO[1]=miniv3d(bbox[1].vec.x,bbox[1].vec.y,bbox[1].vec.z);

   HAS_DATA=TRUE;

   update_mtx();
   }

// define conversion to local coordinates
void miniwarp::def_2local(const miniv3d bboxLOC[2])
   {
   BBOXLOC[0]=bboxLOC[0];
   BBOXLOC[1]=bboxLOC[1];

   HAS_LOCAL=TRUE;

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
   }

// get actual warp matrix
void miniwarp::getwarp(miniv4d mtx[3])
   {
   mtx[0]=MTX[0];
   mtx[1]=MTX[1];
   mtx[2]=MTX[2];
   }

// perform warp
miniv3d miniwarp::warp(miniv3d v)
   {
   minicoord c;

   c=minicoord(v,minicoord::MINICOORD_LINEAR);
   c.convert(MTX);

   return(miniv3d(c.vec.x,c.vec.y,c.vec.z));
   }

// perform warp
miniv4d miniwarp::warp(miniv4d v)
   {
   minicoord c;

   c=minicoord(v,minicoord::MINICOORD_LINEAR);
   c.convert(MTX);

   return(miniv4d(c.vec.x,c.vec.y,c.vec.z,v.w));
   }

// update conversion matrices
void miniwarp::update_mtx()
   {
   if (HAS_DATA)
      {
      // conversion 2 data coordinates:

      MTX_2DAT[0]=miniv4d(BBOXDAT[1].x-BBOXDAT[0].x,0.0,0.0,BBOXDAT[0].x);
      MTX_2DAT[1]=miniv4d(0.0,BBOXDAT[1].y-BBOXDAT[0].y,0.0,BBOXDAT[0].y);
      MTX_2DAT[2]=miniv4d(0.0,0.0,BBOXDAT[1].z-BBOXDAT[0].z,BBOXDAT[0].z);

      inv_mtx(INV_2DAT,MTX_2DAT);
      }

   if (HAS_DATA && HAS_LOCAL)
      {
      // conversion 2 local coordinates:

      MTX_2LOC[0]=miniv4d(BBOXLOC[1].x-BBOXLOC[0].x,0.0,0.0,BBOXLOC[0].x);
      MTX_2LOC[1]=miniv4d(0.0,BBOXLOC[1].y-BBOXLOC[0].y,0.0,BBOXLOC[0].y);
      MTX_2LOC[2]=miniv4d(0.0,0.0,BBOXLOC[1].z-BBOXLOC[0].z,BBOXLOC[0].z);

      mlt_mtx(MTX_2LOC,INV_2DAT,MTX_2LOC);

      inv_mtx(INV_2LOC,MTX_2LOC);

      // conversion 2 affine coordinates:

      MTX_2AFF[0]=MTXAFF[0];
      MTX_2AFF[1]=MTXAFF[1];
      MTX_2AFF[2]=MTXAFF[2];

      inv_mtx(INV_2AFF,MTX_2AFF);

      // conversion 2 tile coordinates:

      //!! not yet implemented
      MTX_2TIL[0]=miniv4d(0.0,0.0,0.0,0.0);
      MTX_2TIL[1]=miniv4d(0.0,0.0,0.0,0.0);
      MTX_2TIL[2]=miniv4d(0.0,0.0,0.0,0.0);

      inv_mtx(INV_2TIL,MTX_2TIL);

      // conversion 2 warp coordinates:

      //!! not yet implemented
      MTX_2WRP[0]=miniv4d(0.0,0.0,0.0,0.0);
      MTX_2WRP[1]=miniv4d(0.0,0.0,0.0,0.0);
      MTX_2WRP[2]=miniv4d(0.0,0.0,0.0,0.0);

      inv_mtx(INV_2WRP,MTX_2WRP);
      }
   }

// update warp matrix
void miniwarp::update_wrp()
   {
   //!! not yet implemented
   }

// multiply two 4x3 matrices
void miniwarp::mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3])
   {
   int i;

   miniv4d m[3];

   for (i=0; i<3; i++)
      {
      m[i].x=mtx1[i].x*mtx2[0].x+mtx1[i].y*mtx2[1].x+mtx1[i].z*mtx2[2].x;
      m[i].y=mtx1[i].x*mtx2[0].y+mtx1[i].y*mtx2[1].y+mtx1[i].z*mtx2[2].y;
      m[i].z=mtx1[i].x*mtx2[0].z+mtx1[i].y*mtx2[1].z+mtx1[i].z*mtx2[2].z;
      m[i].w=mtx1[i].x*mtx2[0].w+mtx1[i].y*mtx2[1].w+mtx1[i].z*mtx2[2].w+mtx1[i].w;
      }

   mtx[0]=m[0];
   mtx[1]=m[1];
   mtx[2]=m[2];
   }

// invert a 3x3 matrix
void miniwarp::inv_mtx(miniv3d inv[3],const miniv3d mtx[3])
   {
   double det;
   miniv3d m[3];

   det=mtx[1].x*(mtx[3].z*mtx[2].y-mtx[2].z*mtx[3].y)+
       mtx[1].y*(mtx[2].z*mtx[3].x-mtx[3].z*mtx[2].x)+
       mtx[1].z*(mtx[3].y*mtx[2].x-mtx[2].y*mtx[3].x);

   if (det==0.0) inv[0]=inv[1]=inv[2]=miniv3d(0.0);
   else
      {
      det=1.0/det;

      m[0]=mtx[0];
      m[1]=mtx[1];
      m[2]=mtx[2];

      inv[1].x=det*(m[3].z*m[2].y-m[2].z*m[3].y);
      inv[2].x=det*(m[2].z*m[3].x-m[3].z*m[2].x);
      inv[3].x=det*(m[3].y*m[2].x-m[2].y*m[3].x);
      inv[1].y=det*(m[1].z*m[3].y-m[3].z*m[1].y);
      inv[2].y=det*(m[3].z*m[1].x-m[1].z*m[3].x);
      inv[3].y=det*(m[1].y*m[3].x-m[3].y*m[1].x);
      inv[1].z=det*(m[2].z*m[1].y-m[1].z*m[2].y);
      inv[2].z=det*(m[1].z*m[2].x-m[2].z*m[1].x);
      inv[3].z=det*(m[2].y*m[1].x-m[1].y*m[2].x);
      }
   }

// invert a 4x3 matrix
void miniwarp::inv_mtx(miniv4d inv[3],const miniv4d mtx[3])
   {
   miniv3d m[3];
   miniv4d m1[3],m2[3];

   m[0]=miniv3d(mtx[0].x,mtx[0].y,mtx[0].z);
   m[1]=miniv3d(mtx[1].x,mtx[1].y,mtx[1].z);
   m[2]=miniv3d(mtx[2].x,mtx[2].y,mtx[2].z);

   inv_mtx(m,m);

   m1[0]=miniv4d(1.0,0.0,0.0,-mtx[0].z);
   m1[1]=miniv4d(0.0,1.0,0.0,-mtx[1].z);
   m1[2]=miniv4d(0.0,0.0,1.0,-mtx[2].z);

   m2[0]=miniv4d(m[0].x,m[0].y,m[0].z);
   m2[1]=miniv4d(m[1].x,m[1].y,m[1].z);
   m2[2]=miniv4d(m[2].x,m[2].y,m[2].z);

   mlt_mtx(inv,m1,m2);
   }
