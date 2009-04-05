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
      MINICOORD_NONE,   // undefined
      MINICOORD_ECEF,   // Earth Centered Earth Fixed
      MINICOORD_LLH,    // Lat/Lon/H WGS84
      MINICOORD_MERC,   // Mercator WGS84
      MINICOORD_UTM,    // Universal Transverse Mercator
      MINICOORD_LINEAR, // linear coordinate space
      };

   //! supported coordinate datums
   enum MINICOORD_DATUM
      {
      MINICOORD_DATUM_NONE=0,
      // USGS datum codes
      MINICOORD_DATUM_NAD27=1,
      MINICOORD_DATUM_WGS72=2,
      MINICOORD_DATUM_WGS84=3,
      MINICOORD_DATUM_NAD83=4,
      // libMini datum codes
      MINICOORD_DATUM_SPHERE=5,
      MINICOORD_DATUM_ED50=6,
      MINICOORD_DATUM_ED79=7,
      MINICOORD_DATUM_OldHawaiian=8,
      MINICOORD_DATUM_Luzon=9,
      MINICOORD_DATUM_Tokyo=10,
      MINICOORD_DATUM_OSGB1936=11,
      MINICOORD_DATUM_Australian1984=12,
      MINICOORD_DATUM_Geodetic1949=13,
      MINICOORD_DATUM_SouthAmerican1969=14
      };

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
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
   MINICOORD type; // actual coordinate system type

   int utm_zone; // actual UTM zone
   MINICOORD_DATUM utm_datum; // actual UTM datum
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
   {return(minicoord(a.vec+b.vec,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator - (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec-b.vec,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator - (const minicoord &c)
   {return(minicoord(-c.vec,c.type,c.utm_zone,c.utm_datum));}

inline minicoord operator * (const double a,const minicoord &b)
   {return(minicoord(a*b.vec,b.type,b.utm_zone,b.utm_datum));}

inline minicoord operator * (const minicoord &a,const double b)
   {return(minicoord(a.vec*b,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator / (const minicoord &a,const double b)
   {return(minicoord(a.vec/b,a.type,a.utm_zone,a.utm_datum));}

inline int operator == (const minicoord &a,const minicoord &b)
   {return(a.vec==b.vec && a.type==b.type && a.utm_zone==b.utm_zone && a.utm_datum==b.utm_datum);}

inline int operator != (const minicoord &a,const minicoord &b)
   {return(a.vec!=b.vec || a.type!=b.type || a.utm_zone!=b.utm_zone || a.utm_datum!=b.utm_datum);}

inline std::ostream& operator << (std::ostream &out,const minicoord &c)
   {return(out << '(' << c.vec << ',' << c.type << ',' << c.utm_zone << ',' << c.utm_datum << ')');}

#endif
