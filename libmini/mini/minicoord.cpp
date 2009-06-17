// (c) by Stefan Roettger

#include "minibase.h"

#include "minicrs.h"

#include "minicoord.h"

// default constructor
minicoord::minicoord()
   {
   vec=miniv4d(0.0);
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

// copy constructor
minicoord::minicoord(const minicoord &c)
   {
   vec=c.vec;
   type=c.type;

   crs_zone=c.crs_zone;
   crs_datum=c.crs_datum;
   }

// constructors:

minicoord::minicoord(const miniv3d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum)
   {
   vec=v;
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   }

minicoord::minicoord(const miniv4d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum)
   {
   vec=v;
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const MINICOORD_DATUM datum)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   }

// destructor
minicoord::~minicoord() {}

// convert from 1 coordinate system 2 another
void minicoord::convert2(const MINICOORD t,const int zone,const MINICOORD_DATUM datum)
   {
   double xyz[3];

   switch (type)
      {
      case MINICOORD_LLH:
         switch (t)
            {
            case MINICOORD_LLH: break;
            case MINICOORD_MERC:
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_UTM:
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               crs_zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,crs_zone);
               type=t;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_ECEF:
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            default: ERRORMSG();
            }
         break;
      case MINICOORD_MERC:
         switch (t)
            {
            case MINICOORD_LLH:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_MERC: break;
            case MINICOORD_UTM:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x);
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               crs_zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,crs_zone);
               type=t;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_ECEF:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            default: ERRORMSG();
            }
         break;
      case MINICOORD_UTM:
         switch (t)
            {
            case MINICOORD_LLH:
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_MERC:
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_UTM:
               if (zone==crs_zone && datum==crs_datum) break;
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               crs_zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,crs_zone);
               type=t;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_ECEF:
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            default: ERRORMSG();
            }
         break;
      case MINICOORD_OGH:
         switch (t)
            {
            case MINICOORD_LLH:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_MERC:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_UTM:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               if (zone==crs_zone) break;
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               crs_zone=zone;
               break;
            case MINICOORD_ECEF:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
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
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_MERC:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_OGH:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               crs_zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,crs_zone);
               type=t;
               crs_datum=MINICOORD_DATUM_NONE;
               break;
            case MINICOORD_UTM:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_ECEF: break;
            default: ERRORMSG();
            }
         break;
      default: ERRORMSG();
      }
   }

// linear conversion defined by 3x3 matrix and offset
void minicoord::convert(const miniv3d mtx[3],const miniv3d offset)
   {
   miniv3d v;

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

   v1=miniv4d(vec,1.0);

   vec.x=mtx[0]*v1;
   vec.y=mtx[1]*v1;
   vec.z=mtx[2]*v1;

   type=MINICOORD_LINEAR;
   }

// tri-linear conversion defined by point 2 point correspondences
void minicoord::convert(const miniv3d src[2],const miniv3d dst[8])
   {
   miniv3d u,v;

   if (src[0]==src[1]) ERRORMSG();

   u.x=(vec.x-src[0].x)/(src[1].x-src[0].x);
   u.y=(vec.y-src[0].y)/(src[1].y-src[0].y);
   u.z=(vec.z-src[0].z)/(src[1].z-src[0].z);

   v=(1.0-u.z)*((1.0-u.y)*((1.0-u.x)*dst[0]+u.x*dst[1])+
                u.y*((1.0-u.x)*dst[2]+u.x*dst[3]))+
     u.z*((1.0-u.y)*((1.0-u.x)*dst[4]+u.x*dst[5])+
          u.y*((1.0-u.x)*dst[6]+u.x*dst[7]));

   vec=miniv4d(v,vec.w);

   type=MINICOORD_LINEAR;
   }
