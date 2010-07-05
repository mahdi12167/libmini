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
void minicoord::convert2(MINICOORD t,int zone,MINICOORD_DATUM datum)
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
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               if (datum==MINICOORD_DATUM_NONE) datum=MINICOORD_DATUM_WGS84;
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               type=t;
               crs_zone=zone;
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
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               if (datum==MINICOORD_DATUM_NONE) datum=MINICOORD_DATUM_WGS84;
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               type=t;
               crs_zone=zone;
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
               if (zone==0) zone=crs_zone;
               if (datum==MINICOORD_DATUM_NONE) datum=crs_datum;
               if (zone==crs_zone && datum==crs_datum) break;
               if (zone==crs_zone && datum==MINICOORD_DATUM_WGS84 && crs_datum==MINICOORD_DATUM_NAD83) {crs_datum=datum; break;}
               if (zone==crs_zone && datum==MINICOORD_DATUM_NAD83 && crs_datum==MINICOORD_DATUM_WGS84) {crs_datum=datum; break;}
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               minicrs::UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               type=t;
               crs_zone=zone;
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
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               if (datum==MINICOORD_DATUM_NONE) datum=MINICOORD_DATUM_WGS84;
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               if (zone==0) break;
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
            case MINICOORD_UTM:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               if (datum==MINICOORD_DATUM_NONE) datum=MINICOORD_DATUM_WGS84;
               minicrs::LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               type=t;
               crs_zone=zone;
               crs_datum=MINICOORD_DATUM_NONE;
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

// normalize wraparound coordinates
void minicoord::normalize(BOOLINT symmetric)
   {
   double wrap;

   switch (type)
      {
      case MINICOORD_LLH:
         wrap=vec.x/(360*60*60);
         if (symmetric)
            {if (wrap<-0.5 || wrap>0.5) vec.x-=floor(wrap+0.5)*360*60*60;}
         else
            {if (wrap<0.0 || wrap>1.0) vec.x-=floor(wrap)*360*60*60;}
         break;
      case MINICOORD_MERC:
         wrap=vec.x/(2*minicrs::WGS84_r_major);
         if (symmetric)
            {if (wrap<-0.5 || wrap>0.5) vec.x-=floor(wrap+0.5)*2*minicrs::WGS84_r_major;}
         else
            {if (wrap<0.0 || wrap>1.0) vec.x-=floor(wrap)*2*minicrs::WGS84_r_major;}
         break;
      default:
         break;
      }
   }

// get shortest distance on orthodrome
double minicoord::getdist(const minicoord &v) const
   {
   double dist;

   minicoord p1,p2;
   double h1,h2;

   minicoord p;
   double len,tmp;

   static const double maxdist=100000.0;

   if (type==MINICOORD_LINEAR &&
       v.type==MINICOORD_LINEAR)
      dist=(vec-v.vec).getlength();
   else
      {
      p1=*this;
      p2=v;

      p1.convert2(MINICOORD_LLH);
      p2.convert2(MINICOORD_LLH);

      h1=p1.vec.z;
      h2=p2.vec.z;

      p1.convert2(MINICOORD_ECEF);
      p2.convert2(MINICOORD_ECEF);

      dist=(p1.vec-p2.vec).getlength();

      if (dist>maxdist)
         {
         p=0.5*(p1+p2);
         len=p.vec.getlength();

         if (len>0.0) p*=minicrs::EARTH_radius/len;
         else
            if (p.vec.x>p.vec.y)
               if (p.vec.x>p.vec.z)
                  {
                  tmp=p.vec.x;
                  p.vec.x=-p.vec.z;
                  p.vec.z=tmp;
                  }
               else
                  {
                  tmp=p.vec.z;
                  p.vec.z=-p.vec.x;
                  p.vec.x=tmp;
                  }
            else
               if (p.vec.y>p.vec.z)
                  {
                  tmp=p.vec.y;
                  p.vec.y=-p.vec.z;
                  p.vec.z=tmp;
                  }
               else
                  {
                  tmp=p.vec.z;
                  p.vec.z=-p.vec.y;
                  p.vec.y=tmp;
                  }

         p.convert2(MINICOORD_LLH);
         p.vec.z=0.5*(h1+h2);

         dist=p1.getdist(p)+p2.getdist(p);
         }
      }

   return(dist);
   }

// get crs type description from object
const char *minicoord::getcrs() const
   {return(getcrs(this->type));}

// get crs type description
const char *minicoord::getcrs(const MINICOORD &t)
   {
   switch (t)
      {
      case MINICOORD_LINEAR: return("Linear");
      case MINICOORD_LLH: return("LLH");
      case MINICOORD_UTM: return("UTM");
      case MINICOORD_MERC: return("Mercator");
      case MINICOORD_OGH: return("OGH");
      case MINICOORD_ECEF: return("ECEF");
      default: return("Unkown");
      }
   }

// get crs datum description from object
const char *minicoord::getdatum() const
   {return(getdatum(this->crs_datum));}

// get crs datum description
const char *minicoord::getdatum(const MINICOORD_DATUM &d)
   {
   switch (d)
      {
      case MINICOORD_DATUM_NONE: return("None");
      case MINICOORD_DATUM_NAD27: return("NAD27");
      case MINICOORD_DATUM_WGS72: return("WGS72");
      case MINICOORD_DATUM_WGS84: return("WGS84");
      case MINICOORD_DATUM_NAD83: return("NAD83");
      case MINICOORD_DATUM_SPHERE: return("Sphere");
      case MINICOORD_DATUM_ED50: return("ED50");
      case MINICOORD_DATUM_ED87: return("ED87");
      case MINICOORD_DATUM_OldHawaiian: return("OldHawaiian");
      case MINICOORD_DATUM_Luzon: return("Luzon");
      case MINICOORD_DATUM_Tokyo: return("Tokyo");
      case MINICOORD_DATUM_OSGB1936: return("OSGB1936");
      case MINICOORD_DATUM_Australian1984: return("Australian1984");
      case MINICOORD_DATUM_NewZealand1949: return("NewZealand1949");
      case MINICOORD_DATUM_SouthAmerican1969: return("SouthAmerican1969");
      default: return("Unknown");
      }
   }
