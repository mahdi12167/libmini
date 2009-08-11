// (c) by Stefan Roettger

#ifndef MINICOORD_H
#define MINICOORD_H

#include "miniv3d.h"
#include "miniv4d.h"

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
      MINICOORD_DATUM_SouthAmerican1969=14
      };

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
   //! if coordinate system is geographic (LatLon) then corners are given in arc-seconds
   //! else corners are given in meters
   minicoord(const miniv3d &v);
   minicoord(const miniv3d &v,const MINICOORD t);
   minicoord(const miniv3d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum);
   minicoord(const miniv4d &v);
   minicoord(const miniv4d &v,const MINICOORD t);
   minicoord(const miniv4d &v,const MINICOORD t,const int zone,const MINICOORD_DATUM datum);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const MINICOORD_DATUM datum);

   //! destructor
   ~minicoord();

   //! associated operators
   inline minicoord& operator += (const minicoord &v);
   inline minicoord& operator -= (const minicoord &v);
   inline minicoord& operator *= (const double c);
   inline minicoord& operator /= (const double c);

   //! convert from 1 coordinate system 2 another
   void convert2(const MINICOORD t,const int zone=0,const MINICOORD_DATUM datum=MINICOORD_DATUM_NONE);

   //! linear conversion defined by 3x3 matrix and offset
   void convert(const miniv3d mtx[3],const miniv3d offset);

   //! linear conversion defined by 4x3 matrix
   void convert(const miniv4d mtx[3]); // fourth row is assumed to be (0,0,0,1)

   //! tri-linear conversion defined by point 2 point correspondences
   void convert(const miniv3d src[2], // bounding box in original domain
                const miniv3d dst[8]); // 8 points in warp domain

   miniv4d vec; // geo-referenced coordinates (plus time)
   MINICOORD type; // actual coordinate reference system type

   int crs_zone; // actual crs zone
   MINICOORD_DATUM crs_datum; // actual crs datum
   };

// associated arithmetic operator +=
inline minicoord& minicoord::operator += (const minicoord &v)
   {
   vec+=v.vec;
   return(*this);
   }

// associated arithmetic operator -=
inline minicoord& minicoord::operator -= (const minicoord &v)
   {
   vec-=v.vec;
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
inline minicoord operator - (const minicoord &a,const minicoord &b);
inline minicoord operator - (const minicoord &c);
inline minicoord operator * (const double a,const minicoord &b);
inline minicoord operator * (const minicoord &a,const double b);
inline minicoord operator / (const minicoord &a,const double b);
inline int operator == (const minicoord &a,const minicoord &b);
inline int operator != (const minicoord &a,const minicoord &b);

// output stream operator
inline std::ostream& operator << (std::ostream &out,const minicoord &c);

// implementation of inline operators:

inline minicoord operator + (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec+b.vec,a.type,a.crs_zone,a.crs_datum));}

inline minicoord operator - (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec-b.vec,a.type,a.crs_zone,a.crs_datum));}

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

inline std::ostream& operator << (std::ostream &out,const minicoord &c)
   {return(out << '(' << c.vec << ',' << c.type << ',' << c.crs_zone << ',' << c.crs_datum << ')');}

#endif
