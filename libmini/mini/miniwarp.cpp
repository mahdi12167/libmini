// (c) by Stefan Roettger

#include "minibase.h"

#include "miniutm.h"

#include "miniwarp.h"

// default constructor
minicoord::minicoord()
   {
   type=MINICOORD_NONE;
   vec=miniv3d(0.0,0.0,0.0);
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

minicoord::minicoord(const miniv3d &v,const int t,const int zone,const int datum)
   {
   vec=v;
   type=t;

   utm_zone=zone;
   utm_datum=datum;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const int t,const int zone,const int datum)
   {minicoord(miniv3d(cx,cy,cz),t,zone,datum);}

// destructor
minicoord::~minicoord() {}

// convert from one coordinate system 2 another:

void minicoord::convert2(const int t,const int zone,const int datum)
   {
   double xyz[3];

   switch (type)
      {
      case MINICOORD_LLH:
         switch (t)
            {
            case MINICOORD_UTM:
               LL2UTM(vec.y,vec.x,zone,datum,&xyz[0],&xyz[1]);
               vec=miniv3d(xyz[0],xyz[1],vec.z);
               type=t;
               utm_zone=zone;
               utm_datum=datum;
               break;
            case MINICOORD_ECEF:
               LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv3d(xyz);
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
               UTM2LL(vec.x,vec.y,utm_zone,utm_datum,&xyz[1],&xyz[0]);
               vec=miniv3d(xyz[0],xyz[1],vec.z);
               type=t;
               utm_zone=0;
               utm_datum=0;
               break;
            case MINICOORD_ECEF:
               UTM2LL(vec.x,vec.y,utm_zone,utm_datum,&xyz[1],&xyz[0]);
               LLH2ECEF(xyz[1],xyz[0],vec.z,xyz);
               vec=miniv3d(xyz);
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
               ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               type=t;
               utm_zone=0;
               utm_datum=0;
               break;
            case MINICOORD_UTM:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               LL2UTM(vec.y,vec.x,zone,datum,&xyz[0],&xyz[1]);
               vec=miniv3d(xyz[0],xyz[1],vec.z);
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

void minicoord::convert2(const miniv3d mtx[3],const miniv3d offset)
   {
   if (type==MINICOORD_NONE) ERRORMSG();

   type=MINICOORD_LINEAR;
   }

void minicoord::convert2(const miniv3d src[8],const miniv3d dst[8])
   {
   if (type==MINICOORD_NONE) ERRORMSG();

   type=MINICOORD_NONLIN;
   }

// default constructor
miniwarp::miniwarp()
   {
   }

// destructor
miniwarp::~miniwarp()
   {
   }
