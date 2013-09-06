// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "ministring.h"

#include "minicoord.h"

minicrs minicoord::UTM;

// get approximate radius of orb
double minicoord::getorbradius(int orb)
   {
   if (orb<0) return(-1E5*orb);

   switch (orb)
      {
      case MINICOORD_ORB_NONE:
      case MINICOORD_ORB_EARTH: return(minicrs::EARTH_radius);
      case MINICOORD_ORB_UNIT_SPHERE: return(0.5);
      case MINICOORD_ORB_SUN: return(getorbradius(-6955));
      case MINICOORD_ORB_MERCURY: return(getorbradius(-24));
      case MINICOORD_ORB_VENUS: return(getorbradius(-61));
      case MINICOORD_ORB_MARS: return(getorbradius(-34));
      case MINICOORD_ORB_JUPITER: return(getorbradius(-715));
      case MINICOORD_ORB_SATURN: return(getorbradius(-603));
      case MINICOORD_ORB_URANUS: return(getorbradius(-256));
      case MINICOORD_ORB_NEPTUNE: return(getorbradius(-248));
      case MINICOORD_ORB_CERES: return(getorbradius(-5));
      case MINICOORD_ORB_PLUTO: return(getorbradius(-12));
      case MINICOORD_ORB_ERIS: return(getorbradius(-12));
      case MINICOORD_ORB_MOON: return(getorbradius(-17));
      default: return(minicrs::EARTH_radius);
      }
   }

// get semi-major and minor axis of orb
void minicoord::getorbaxis(int orb,double &r_major,double &r_minor)
   {
   switch (orb)
      {
      case MINICOORD_ORB_NONE:
      case MINICOORD_ORB_EARTH: r_major=minicrs::WGS84_r_major; r_minor=minicrs::WGS84_r_minor; break;
      case MINICOORD_ORB_UNIT_SPHERE: r_major=r_minor=0.5; break;
      default: r_major=r_minor=getorbradius(orb); break;
      }
   }

// default constructor
minicoord::minicoord()
   {
   vec=miniv4d(0.0);
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   crs_orb=MINICOORD_ORB_UNIT_SPHERE;
   }

// copy constructor
minicoord::minicoord(const minicoord &c)
   {
   vec=c.vec;
   type=c.type;

   crs_zone=c.crs_zone;
   crs_datum=c.crs_datum;
   crs_orb=c.crs_orb;
   }

// constructors:

minicoord::minicoord(const miniv3d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   crs_orb=MINICOORD_ORB_UNIT_SPHERE;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_WGS84;
   crs_orb=MINICOORD_ORB_EARTH;
   }

minicoord::minicoord(const miniv3d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb)
   {
   vec=v;
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   crs_orb=orb;
   }

minicoord::minicoord(const miniv4d &v)
   {
   vec=v;
   type=MINICOORD_LINEAR;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   crs_orb=MINICOORD_ORB_UNIT_SPHERE;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t)
   {
   vec=v;
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_WGS84;
   crs_orb=MINICOORD_ORB_EARTH;
   }

minicoord::minicoord(const miniv4d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb)
   {
   vec=v;
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   crs_orb=orb;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_WGS84;
   crs_orb=MINICOORD_ORB_EARTH;
   }

minicoord::minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb)
   {
   vec=miniv4d(cx,cy,cz);
   type=t;

   crs_zone=zone;
   crs_datum=datum;
   crs_orb=orb;
   }

// destructor
minicoord::~minicoord() {}

// set lat/lon/height in degrees/meters
void minicoord::set_llh(double lat,double lon,double height)
   {
   vec=miniv4d(3600*lon,3600*lat,height);
   type=minicoord::MINICOORD_LLH;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_WGS84;
   crs_orb=MINICOORD_ORB_EARTH;
   }

// set polar coordinates on unit sphere
void minicoord::set_polar(double alpha,double beta,double height,double t)
   {
   vec=miniv4d(3600*alpha,3600*beta,height,t);
   type=minicoord::MINICOORD_LLH;

   crs_zone=0;
   crs_datum=MINICOORD_DATUM_NONE;
   crs_orb=MINICOORD_ORB_UNIT_SPHERE;
   }

// orb to orb scaling
void minicoord::scale2(int orb)
   {
   double scale;
   double radius0,radius1;
   double r_major0,r_minor0;
   double r_major1,r_minor1;

   if (crs_orb==MINICOORD_ORB_NONE) crs_orb=MINICOORD_ORB_EARTH;
   if (orb==MINICOORD_ORB_NONE) orb=crs_orb;

   if (orb!=crs_orb)
      {
      switch (type)
         {
         case MINICOORD_LLH:
            radius0=getorbradius(crs_orb);
            radius1=getorbradius(orb);
            if (radius1!=radius0) vec.z*=radius1/radius0;
            break;
         case MINICOORD_MERC:
         case MINICOORD_UTM:
         case MINICOORD_OGH:
            radius0=getorbradius(crs_orb);
            radius1=getorbradius(orb);
            if (radius1!=radius0)
               {
               scale=radius1/radius0;
               vec.x*=scale;
               vec.y*=scale;
               vec.z*=scale;
               }
            break;
         case MINICOORD_ECEF:
            getorbaxis(crs_orb,r_major0,r_minor0);
            getorbaxis(orb,r_major1,r_minor1);
            if (r_major1!=r_major0)
               {
               scale=r_major1/r_major0;
               vec.x*=scale;
               vec.y*=scale;
               }
            if (r_minor1!=r_minor0) vec.z*=r_minor1/r_minor0;
            break;
         default: ERRORMSG();
         }

      crs_orb=orb;
      }

   if (crs_datum==MINICOORD_DATUM_WGS84 && crs_orb!=MINICOORD_ORB_EARTH) crs_datum=MINICOORD_DATUM_NONE;
   }

// convert from 1 coordinate system 2 another
void minicoord::convert2(MINICOORD t,int zone,MINICOORD_DATUM datum,int orb)
   {
   double xyz[3];

   double r_major,r_minor;

   scale2(orb);
   getorbaxis(orb,r_major,r_minor);

   switch (type)
      {
      case MINICOORD_LINEAR:
         if (t!=MINICOORD_LINEAR) ERRORMSG();
         break;
      case MINICOORD_LLH:
         switch (t)
            {
            case MINICOORD_LLH: break;
            case MINICOORD_MERC:
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y,r_major,r_minor);
               type=t;
               break;
            case MINICOORD_UTM:
               scale2(MINICOORD_ORB_EARTH);
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               UTM.LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               scale2(orb);
               break;
            case MINICOORD_OGH:
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz,r_major,r_minor);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz,r_major,r_minor);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone,r_major,r_minor);
               type=t;
               crs_zone=zone;
               break;
            case MINICOORD_ECEF:
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz,r_major,r_minor);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               break;
            default: ERRORMSG();
            }
         break;
      case MINICOORD_MERC:
         switch (t)
            {
            case MINICOORD_LLH:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x,r_major,r_minor);
               type=t;
               break;
            case MINICOORD_MERC: break;
            case MINICOORD_UTM:
               scale2(MINICOORD_ORB_EARTH);
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x,r_major,r_minor);
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               UTM.LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               scale2(orb);
               break;
            case MINICOORD_OGH:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x,r_major,r_minor);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz,r_major,r_minor);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz,r_major,r_minor);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone,r_major,r_minor);
               type=t;
               crs_zone=zone;
               break;
            case MINICOORD_ECEF:
               minicrs::MERC2LL(vec.x,vec.y,&vec.y,&vec.x,r_major,r_minor);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz,r_major,r_minor);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               break;
            default: ERRORMSG();
            }
         break;
      case MINICOORD_UTM:
         scale2(MINICOORD_ORB_EARTH);
         switch (t)
            {
            case MINICOORD_LLH:
               UTM.UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_MERC:
               UTM.UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_UTM:
               if (zone==0) zone=crs_zone;
               if (datum==MINICOORD_DATUM_NONE) datum=crs_datum;
               if (zone==crs_zone && datum==crs_datum) break;
               if (zone==crs_zone && datum==MINICOORD_DATUM_WGS84 && crs_datum==MINICOORD_DATUM_NAD83) {crs_datum=datum; break;}
               if (zone==crs_zone && datum==MINICOORD_DATUM_NAD83 && crs_datum==MINICOORD_DATUM_WGS84) {crs_datum=datum; break;}
               UTM.UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               UTM.LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               UTM.UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz,r_major,r_minor);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone);
               type=t;
               crs_zone=zone;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            case MINICOORD_ECEF:
               UTM.UTM2LL(vec.x,vec.y,crs_zone,crs_datum,&vec.y,&vec.x);
               minicrs::LLH2ECEF(vec.y,vec.x,vec.z,xyz);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
               crs_datum=MINICOORD_DATUM_WGS84;
               break;
            default: ERRORMSG();
            }
         scale2(orb);
         break;
      case MINICOORD_OGH:
         switch (t)
            {
            case MINICOORD_LLH:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz,r_major,r_minor);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z,r_major,r_minor);
               type=t;
               crs_zone=0;
               break;
            case MINICOORD_MERC:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz,r_major,r_minor);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z,r_major,r_minor);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y,r_major,r_minor);
               type=t;
               crs_zone=0;
               break;
            case MINICOORD_UTM:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz);
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               UTM.LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               break;
            case MINICOORD_OGH:
               if (zone==0) break;
               if (zone==crs_zone) break;
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz,r_major,r_minor);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone,r_major,r_minor);
               crs_zone=zone;
               break;
            case MINICOORD_ECEF:
               minicrs::OGH2ECEF(vec.x,vec.y,vec.z,crs_zone,xyz,r_major,r_minor);
               vec=miniv4d(xyz[0],xyz[1],xyz[2],vec.w);
               type=t;
               crs_zone=0;
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
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z,r_major,r_minor);
               type=t;
               break;
            case MINICOORD_MERC:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z,r_major,r_minor);
               minicrs::LL2MERC(vec.y,vec.x,&vec.x,&vec.y,r_major,r_minor);
               type=t;
               break;
            case MINICOORD_UTM:
               scale2(MINICOORD_ORB_EARTH);
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               minicrs::ECEF2LLH(xyz,&vec.y,&vec.x,&vec.z);
               if (zone==0) zone=minicrs::LL2UTMZ(vec.y,vec.x);
               UTM.LL2UTM(vec.y,vec.x,zone,datum,&vec.x,&vec.y);
               type=t;
               crs_zone=zone;
               crs_datum=datum;
               scale2(orb);
               break;
            case MINICOORD_OGH:
               xyz[0]=vec.x;
               xyz[1]=vec.y;
               xyz[2]=vec.z;
               if (zone==0) zone=minicrs::ECEF2OGHZ(xyz,r_major,r_minor);
               minicrs::ECEF2OGH(xyz,&vec.x,&vec.y,&vec.z,zone,r_major,r_minor);
               type=t;
               crs_zone=zone;
               break;
            case MINICOORD_ECEF: break;
            default: ERRORMSG();
            }
         break;
      default: ERRORMSG();
      }

   if (type!=MINICOORD_LINEAR)
      if (crs_datum==MINICOORD_DATUM_NONE && crs_orb==MINICOORD_ORB_EARTH) crs_datum=MINICOORD_DATUM_WGS84;
   }

// convert 2 ecef
void minicoord::convert2ecef()
   {if (type!=MINICOORD_LINEAR) convert2(MINICOORD_ECEF);}

// convert 2 llh
void minicoord::convert2llh()
   {if (type!=MINICOORD_LINEAR) convert2(MINICOORD_LLH);}

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
minicoord &minicoord::normalize(BOOLINT symmetric)
   {
   double wrap;

   if (type==MINICOORD_LLH)
      {
      wrap=vec.x/(360*60*60);
      if (symmetric)
         {if (wrap<-0.5 || wrap>0.5) vec.x-=floor(wrap+0.5)*360*60*60;}
      else
         {if (wrap<0.0 || wrap>1.0) vec.x-=floor(wrap)*360*60*60;}
      }
   else if (type==MINICOORD_MERC)
      {
      wrap=vec.x/(2*minicrs::WGS84_r_major);
      if (symmetric)
         {if (wrap<-0.5 || wrap>0.5) vec.x-=floor(wrap+0.5)*2*minicrs::WGS84_r_major;}
      else
         {if (wrap<0.0 || wrap>1.0) vec.x-=floor(wrap)*2*minicrs::WGS84_r_major;}
      }

   return(*this);
   }

// get approximate orb radius
double minicoord::getorbradius() const
   {return(getorbradius(crs_orb));}

// get euclidean distance
double minicoord::getdist(const minicoord &v) const
   {
   double dist;

   minicoord p1,p2;

   if (type==MINICOORD_LINEAR &&
       v.type==MINICOORD_LINEAR)
      dist=(vec-v.vec).getlength();
   else
      {
      p1=*this;
      p2=v;

      if (p1.type!=MINICOORD_LINEAR) p1.convert2(MINICOORD_ECEF);
      if (p2.type!=MINICOORD_LINEAR) p2.convert2(MINICOORD_ECEF);

      dist=(p1.vec-p2.vec).getlength();
      }

   return(dist);
   }

// get distance on orthodrome
double minicoord::getorthodist(const minicoord &v) const
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

         if (len>0.0) p*=p.getorbradius()/len;
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

         dist=p1.getorthodist(p)+p2.getorthodist(p);
         }
      }

   return(dist);
   }

// left-of comparison
BOOLINT minicoord::leftof(const minicoord &v) const
   {return((*this-v).symm().vec.x<0.0);}

// right-of comparison
BOOLINT minicoord::rightof(const minicoord &v) const
   {return((*this-v).symm().vec.x>0.0);}

// bottom-of comparison
BOOLINT minicoord::bottomof(const minicoord &v) const
   {return(vec.y<v.vec.y);}

// top-of comparison
BOOLINT minicoord::topof(const minicoord &v) const
   {return(vec.y>v.vec.y);}

// interpolate coordinates linearily
minicoord minicoord::lerp(double w,const minicoord &a,const minicoord &b,BOOLINT symmetric)
   {return(a+w*(b-a).normalize(symmetric));}

// mean of two coordinates
minicoord minicoord::mean(const minicoord &a,const minicoord &b,BOOLINT symmetric)
   {return(lerp(0.5,a,b,symmetric));}

// get crs type description from object
ministring minicoord::getcrs() const
   {return(getcrs(this->type,this->crs_zone));}

// get crs type description
ministring minicoord::getcrs(MINICOORD t,int zone)
   {
   switch (t)
      {
      case MINICOORD_LINEAR: return("Linear");
      case MINICOORD_LLH: return("LLH");
      case MINICOORD_UTM: return(ministring("UTM[")+zone+"]");
      case MINICOORD_MERC: return("Mercator");
      case MINICOORD_OGH: return(ministring("OGH[")+zone+"]");
      case MINICOORD_ECEF: return("ECEF");
      default: return("Unkown");
      }
   }

// get crs datum description from object
ministring minicoord::getdatum() const
   {return(getdatum(this->crs_datum));}

// get crs datum description
ministring minicoord::getdatum(MINICOORD_DATUM d)
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

// get crs orb description from object
ministring minicoord::getorb() const
   {return(getorb(this->crs_orb));}

// get crs orb description
ministring minicoord::getorb(int o)
   {
   switch (o)
      {
      case MINICOORD_ORB_NONE: return("None");
      case MINICOORD_ORB_UNIT_SPHERE: return("Unit-Sphere");
      case MINICOORD_ORB_SUN: return("Sun");
      case MINICOORD_ORB_MERCURY: return("Mercury");
      case MINICOORD_ORB_VENUS: return("Venus");
      case MINICOORD_ORB_EARTH: return("Earth");
      case MINICOORD_ORB_JUPITER: return("Jupiter");
      case MINICOORD_ORB_SATURN: return("Saturn");
      case MINICOORD_ORB_URANUS: return("Uranus");
      case MINICOORD_ORB_NEPTUNE: return("Neptune");
      case MINICOORD_ORB_CERES: return("Ceres");
      case MINICOORD_ORB_PLUTO: return("Pluto");
      case MINICOORD_ORB_ERIS: return("Eris");
      case MINICOORD_ORB_MOON: return("Moon");
      default: return("Unknown");
      }
   }

// string cast operator
minicoord::operator ministring() const
   {return((ministring)"[ (" + vec.x + "," + vec.y + "," + vec.z + ") t=" + vec.w + " crs=" + getcrs() + " datum=" + getdatum() + " orb=" + getorb() + " ]");}

// serialization
ministring minicoord::to_string() const
   {
   ministring info("minicoord");

   info.append("(");
   info.append(vec.to_string());
   info.append(",");
   info.append_int(type);
   info.append(",");
   info.append_int(crs_zone);
   info.append(",");
   info.append_int(crs_datum);
   info.append(",");
   info.append_int(crs_orb);
   info.append(")");

   return(info);
   }

// deserialization
void minicoord::from_string(ministring &info)
   {
   if (info.startswith("minicoord"))
      {
      info=info.tail("minicoord");
      vec.from_string(info);
      info=info.tail(",");
      type=(minicoord::MINICOORD)info.prefix(",").value_int();
      info=info.tail(",");
      crs_zone=info.prefix(",").value_int();
      info=info.tail(",");
      crs_datum=(minicoord::MINICOORD_DATUM)info.prefix(",").value_int();
      info=info.tail(",");
      crs_orb=info.prefix(",").value_int();
      info=info.tail(")");
      }
   }
