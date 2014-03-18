// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIMEAS_H
#define MINIMEAS_H

#include "minicoord.h"

//! geo-referenced measurements
class minimeas: public minicoord
   {
   public:

   //! default constructor
   minimeas()
      : minicoord(), accuracy(0.0), velocity(0.0), heading(0.0)
      {}

   //! constructor
   minimeas(const minicoord &c,
            double a=0.0,double v=0.0,double h=0.0)
      : minicoord(c), accuracy(a), velocity(v), heading(h)
      {
      if (heading<-180.0) heading+=360.0;
      else if (heading>180.0) heading-=360.0;
      }

   //! destructor
   ~minimeas() {}

   //! serialization
   ministring to_string() const;

   //! deserialization
   void from_string(ministring &info);

   double accuracy;
   double velocity;
   double heading;
   };

// arithmetic inline operators
inline minimeas operator + (const minimeas &a,const minimeas &b);
inline minimeas operator - (const minimeas &a,const minimeas &b);
inline minimeas operator * (const double a,const minimeas &b);
inline minimeas operator * (const minimeas &a,const double b);
inline minimeas operator / (const minimeas &a,const double b);
inline int operator < (const minimeas &a,const minimeas &b);

// implementation of inline operators:

inline minimeas operator + (const minimeas &a,const minimeas &b)
   {
   return(minimeas(minicoord(a.vec+b.vec,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy+b.accuracy,a.velocity+b.velocity,a.heading+b.heading));
   }

inline minimeas operator - (const minimeas &a,const minimeas &b)
   {
   return(minimeas(minicoord(a.vec-b.vec,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy+b.accuracy,a.velocity+b.velocity,a.heading-b.heading));
   }

inline minimeas operator * (const double a,const minimeas &b)
   {
   return(minimeas(minicoord(a*b.vec,b.type,b.crs_zone,b.crs_datum),
                   a*b.accuracy,a*b.velocity,a*b.heading));
   }

inline minimeas operator * (const minimeas &a,const double b)
   {
   return(minimeas(minicoord(a.vec*b,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy*b,a.velocity*b,a.heading*b));
   }

inline minimeas operator / (const minimeas &a,const double b)
   {
   return(minimeas(minicoord(a.vec/b,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy/b,a.velocity/b,a.heading/b));
   }

inline int operator < (const minimeas &a,const minimeas &b)
   {return(a.vec.w<b.vec.w);}

#endif
