// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICOORD_H
#define MINICOORD_H

#include "miniv3d.h"
#include "miniv4d.h"

#include "minicrs.h"

#include "ministring.h"

//! geo-referenced coordinates
class minicoord
   {
   public:

   //! supported coordinate systems
   enum MINICOORD
      {
      MINICOORD_LINEAR=0, // linear coordinate space
      MINICOORD_LLH=1,    // Lat/Lon/H WGS84
      MINICOORD_UTM=2,    // Universal Transverse Mercator (zones [+-] 1-60)
      MINICOORD_MERC=3,   // Mercator WGS84
      MINICOORD_OGH=4,    // Oblique Gnomonic (zones 1-6)
      MINICOORD_ECEF=5    // Earth Centered Earth Fixed
      };

   //! supported coordinate datums
   enum MINICOORD_DATUM
      {
      MINICOORD_DATUM_NONE=0,
      // USGS datum codes
      MINICOORD_DATUM_NAD27=1,
      MINICOORD_DATUM_WGS72=2,
      MINICOORD_DATUM_WGS84=3,
      MINICOORD_DATUM_NAD83=4, // utilizes the GRS80 ellipsoid
      // libMini datum codes
      MINICOORD_DATUM_SPHERE=5,
      MINICOORD_DATUM_ED50=6,
      MINICOORD_DATUM_ED87=7,
      MINICOORD_DATUM_OldHawaiian=8,
      MINICOORD_DATUM_Luzon=9,
      MINICOORD_DATUM_Tokyo=10,
      MINICOORD_DATUM_OSGB1936=11,
      MINICOORD_DATUM_Australian1984=12,
      MINICOORD_DATUM_NewZealand1949=13,
      MINICOORD_DATUM_SouthAmerican1969=14,
      };

   //! supported orbitals
   enum MINICOORD_ORB
      {
      MINICOORD_ORB_NONE=0,
      MINICOORD_ORB_UNIT_SPHERE=1,
      MINICOORD_ORB_SUN=2,
      MINICOORD_ORB_MERCURY=3,
      MINICOORD_ORB_VENUS=4,
      MINICOORD_ORB_EARTH=5,
      MINICOORD_ORB_MARS=6,
      MINICOORD_ORB_JUPITER=7,
      MINICOORD_ORB_SATURN=8,
      MINICOORD_ORB_URANUS=9,
      MINICOORD_ORB_NEPTUNE=10,
      MINICOORD_ORB_CERES=11,
      MINICOORD_ORB_PLUTO=12,
      MINICOORD_ORB_ERIS=13,
      MINICOORD_ORB_MOON=14
      };

   //! get approximate radius of orb
   static double getorbradius(int orb);

   //! get semi-major and minor axis of orb
   static void getorbaxis(int orb,double &r_major,double &r_minor);

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
   //! if coordinate system is geographic (LatLon) then corners are given in arc-seconds
   //! else corners are given in meters (1 degree equals 3600 arc-seconds or roughly 111 km on the equator)
   minicoord(const miniv3d &v);
   minicoord(const miniv3d &v,const MINICOORD t);
   minicoord(const miniv3d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb=MINICOORD_ORB_EARTH);
   minicoord(const miniv4d &v);
   minicoord(const miniv4d &v,const MINICOORD t);
   minicoord(const miniv4d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb=MINICOORD_ORB_EARTH);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const MINICOORD_DATUM datum,const int orb=MINICOORD_ORB_EARTH);

   //! destructor
   ~minicoord();

   //! associated operators
   inline minicoord& operator += (const minicoord &v);
   inline minicoord& operator += (const miniv4d &v);
   inline minicoord& operator += (const miniv3d &v);
   inline minicoord& operator -= (const minicoord &v);
   inline minicoord& operator -= (const miniv4d &v);
   inline minicoord& operator -= (const miniv3d &v);
   inline minicoord& operator *= (const double c);
   inline minicoord& operator /= (const double c);

   //! set lat/lon/height in degrees/meters
   void set_llh(double lat,double lon,double height=0.0,double t=0.0,int orb=MINICOORD_ORB_NONE);

   //! set polar coordinates on unit sphere
   void set_polar(double alpha,double beta,double height=0.0,double t=0.0,int orb=MINICOORD_ORB_UNIT_SPHERE);

   //! set time (unix time since 1.1.1970)
   void set_time(double t);

   //! set time from utc
   void set_time(int year,unsigned int month,unsigned int day,
                 unsigned int hour=0,unsigned int minute=0,unsigned int second=0,
                 double milliseconds=0.0);

   //! set time from utc string
   void set_time(const ministring &utc);

   //! set meters above sea level (m.a.s.l.)
   void set_masl(double masl);

   //! get meters above sea level (m.a.s.l.)
   double get_masl();

   //! get latitude
   double get_latitude();

   //! get longitude
   double get_longitude();

   //! convert from 1 coordinate system 2 another
   void convert2(MINICOORD t,int zone=0,MINICOORD_DATUM datum=MINICOORD_DATUM_NONE,int orb=MINICOORD_ORB_NONE);

   //! convert 2 ecef
   void convert2ecef();

   //! convert 2 llh
   void convert2llh();

   //! convert 2 utm
   void convert2utm();

   //! linear conversion defined by 3x3 matrix and offset
   void convert(const miniv3d mtx[3],const miniv3d offset);

   //! linear conversion defined by 4x3 matrix
   void convert(const miniv4d mtx[3]); // fourth row is assumed to be (0,0,0,1)

   //! tri-linear conversion defined by point 2 point correspondences
   void convert(const miniv3d src[2], // bounding box in original domain
                const miniv3d dst[8]); // 8 points in warp domain

   //! normalize wraparound coordinates
   minicoord &normalize(BOOLINT symmetric=TRUE);

   //! normalize wraparound coordinates
   minicoord &normalize(const minicoord &ref,BOOLINT symmetric=TRUE);

   //! normalize coordinates symmetrically
   minicoord &symm() {return(this->normalize(TRUE));}

   //! normalize coordinates asymmetrically
   minicoord &asymm() {return(this->normalize(FALSE));}

   //! get approximate orb radius
   double getorbradius() const;

   //! get euclidean distance
   double getdist(const minicoord &v) const;

   //! get distance on orthodrome
   double getorthodist(const minicoord &v) const;

   //! left-of comparison
   BOOLINT leftof(const minicoord &v) const;

   //! right-of comparison
   BOOLINT rightof(const minicoord &v) const;

   //! bottom-of comparison
   BOOLINT bottomof(const minicoord &v) const;

   //! top-of comparison
   BOOLINT topof(const minicoord &v) const;

   //! interpolate coordinates linearily
   static minicoord lerp(double w,const minicoord &a,const minicoord &b,BOOLINT symmetric=TRUE);

   //! mean of two coordinates
   static minicoord mean(const minicoord &a,const minicoord &b,BOOLINT symmetric=TRUE);

   //! get crs type description
   ministring getcrs() const;
   static ministring getcrs(MINICOORD t,int zone);

   //! get crs datum description
   ministring getdatum() const;
   static ministring getdatum(MINICOORD_DATUM d);

   //! get crs orb description
   ministring getorb() const;
   static ministring getorb(int o);

   //! get 3D position vector
   miniv3d getpos() const {return(vec);}

   //! get scalar time component
   double gettime() const {return(vec.w);}

   //! create affine transformation from local north-up coordinates to global ECEF coordinates
   miniv3d local2ecef(miniv4d mtx[3]);

   //! string cast operator
   operator ministring() const;

   //! serialization
   ministring to_string() const;

   //! deserialization
   void from_string(ministring &info);

   miniv4d vec; // geo-referenced coordinates (plus time)
   MINICOORD type; // actual coordinate reference system type

   int crs_zone; // actual crs zone
   MINICOORD_DATUM crs_datum; // actual crs datum
   int crs_orb; // actual crs orb

   protected:

   //! datum to datum scaling
   void scale2(int orb);

   private:

   static minicrs UTM;
   };

// associated arithmetic operator +=
inline minicoord& minicoord::operator += (const minicoord &v)
   {
   vec+=v.vec;
   return(*this);
   }

// associated arithmetic operator +=
inline minicoord& minicoord::operator += (const miniv4d &v)
   {
   vec+=v;
   return(*this);
   }

// associated arithmetic operator +=
inline minicoord& minicoord::operator += (const miniv3d &v)
   {
   vec+=v;
   return(*this);
   }

// associated arithmetic operator -=
inline minicoord& minicoord::operator -= (const minicoord &v)
   {
   vec-=v.vec;
   return(*this);
   }

// associated arithmetic operator -=
inline minicoord& minicoord::operator -= (const miniv4d &v)
   {
   vec-=v;
   return(*this);
   }

// associated arithmetic operator -=
inline minicoord& minicoord::operator -= (const miniv3d &v)
   {
   vec-=v;
   return(*this);
   }

// associated arithmetic operator *=
inline minicoord& minicoord::operator *= (const double c)
   {
   vec*=c;
   return(*this);
   }

// associated arithmetic operator /=
inline minicoord& minicoord::operator /= (const double c)
   {
   vec/=c;
   return(*this);
   }

// arithmetic inline operators
inline minicoord operator + (const minicoord &a,const minicoord &b);
inline minicoord operator + (const minicoord &a,const miniv4d &b);
inline minicoord operator + (const minicoord &a,const miniv3d &b);
inline minicoord operator - (const minicoord &a,const minicoord &b);
inline minicoord operator - (const minicoord &a,const miniv4d &b);
inline minicoord operator - (const minicoord &a,const miniv3d &b);
inline minicoord operator - (const minicoord &c);
inline minicoord operator * (const double a,const minicoord &b);
inline minicoord operator * (const minicoord &a,const double b);
inline minicoord operator / (const minicoord &a,const double b);
inline int operator == (const minicoord &a,const minicoord &b);
inline int operator != (const minicoord &a,const minicoord &b);
inline int operator < (const minicoord &a,const minicoord &b);

// output stream operators
inline std::ostream& operator << (std::ostream &out,const minicoord &c);
inline std::ostream& operator << (std::ostream &out,const minicoord::MINICOORD &t);
inline std::ostream& operator << (std::ostream &out,const minicoord::MINICOORD_DATUM &d);

// implementation of inline operators:

inline minicoord operator + (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec+b.vec,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator + (const minicoord &a,const miniv4d &b)
   {return(minicoord(a.vec+b,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator + (const minicoord &a,const miniv3d &b)
   {return(minicoord(a.vec+miniv4d(b),a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator - (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec-b.vec,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator - (const minicoord &a,const miniv4d &b)
   {return(minicoord(a.vec-b,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator - (const minicoord &a,const miniv3d &b)
   {return(minicoord(a.vec-miniv4d(b),a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator - (const minicoord &c)
   {return(minicoord(-c.vec,c.type,c.crs_zone,c.crs_datum));}

inline minicoord operator * (const double a,const minicoord &b)
   {return(minicoord(a*b.vec,b.type,b.crs_zone,b.crs_datum));}

inline minicoord operator * (const minicoord &a,const double b)
   {return(minicoord(a.vec*b,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator / (const minicoord &a,const double b)
   {return(minicoord(a.vec/b,a.type,a.crs_zone,a.crs_datum));}

inline int operator == (const minicoord &a,const minicoord &b)
   {return(a.vec==b.vec && a.type==b.type && a.crs_zone==b.crs_zone && a.crs_datum==b.crs_datum);}

inline int operator != (const minicoord &a,const minicoord &b)
   {return(a.vec!=b.vec || a.type!=b.type || a.crs_zone!=b.crs_zone || a.crs_datum!=b.crs_datum);}

inline int operator < (const minicoord &a,const minicoord &b)
   {return(a.vec.w<b.vec.w);}

inline std::ostream& operator << (std::ostream &out,const minicoord &c)
   {return(out << "[ (" << c.vec.x << "," << c.vec.y << "," << c.vec.z << ") t=" << c.vec.w << " crs=" << c.getcrs() << " datum=" << c.getdatum() << " orb=" << c.getorb() << " ]");}

#endif
