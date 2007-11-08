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

   vec.x=mtx[0]*v+offset.x;
   vec.y=mtx[1]*v+offset.y;
   vec.z=mtx[2]*v+offset.z;

   type=MINICOORD_LINEAR;
   }

// linear conversion defined by 4x3 matrix
void minicoord::convert(const miniv4d mtx[3])
   {
   miniv4d v;

   if (type==MINICOORD_NONE) ERRORMSG();

   v=miniv4d(vec.x,vec.y,vec.z,1.0);

   vec.x=mtx[0]*v;
   vec.y=mtx[1]*v;
   vec.z=mtx[2]*v;

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

   MTX_2DAT[0]=MTX_2DAT[1]=MTX_2DAT[2]=miniv4d(0.0);
   MTX_2LOC[0]=MTX_2LOC[1]=MTX_2LOC[2]=miniv4d(0.0);
   MTX_2AFF[0]=MTX_2AFF[1]=MTX_2AFF[2]=miniv4d(0.0);
   MTX_2TIL[0]=MTX_2TIL[1]=MTX_2TIL[2]=miniv4d(0.0);
   MTX_2WRP[0]=MTX_2WRP[1]=MTX_2WRP[2]=miniv4d(0.0);

   FROM=TO=MINIWARP_PLAIN;

   MTX[0]=MTX[1]=MTX[2]=miniv4d(0.0);
   }

// destructor
miniwarp::~miniwarp()
   {
   //!! not yet implemented
   }

// define data coordinates
void miniwarp::def_data(const miniv3d bboxDAT[2],
                        const minicoord bboxGEO[2])
   {
   //!! not yet implemented
   }

// define conversion to local coordinates
void miniwarp::def_2local(const miniv3d bboxLOC[2])
   {
   //!! not yet implemented
   }

// define conversion to affine coordinates
void miniwarp::def_2affine(const miniv4d mtx[3])
   {
   //!! not yet implemented
   }

// define warp coordinates
void miniwarp::def_warp(const minicoord::MINICOORD sys)
   {
   //!! not yet implemented
   }

// set actual warp
void miniwarp::setwarp(MINIWARP from,MINIWARP to)
   {
   //!! not yet implemented
   }

// get actual warp matrix
void miniwarp::getwarp(miniv4d mtx[3])
   {
   //!! not yet implemented
   }

// perform warp
miniv3d miniwarp::warp(miniv3d v)
   {
   //!! not yet implemented
   return(miniv3d(0.0));
   }

// perform warp
miniv4d miniwarp::warp(miniv4d v)
   {
   //!! not yet implemented
   return(miniv4d(0.0));
   }
