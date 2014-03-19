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
      : minicoord(),
        accuracy(0.0f), velocity(0.0f), heading(0.0f),
        start(FALSE),
        description(NULL), metadata(NULL)
      {}

   //! constructor
   minimeas(const minicoord &c,
            float a=0.0f,float v=0.0f,float h=0.0f,
            BOOLINT s=FALSE)
      : minicoord(c),
        accuracy(a), velocity(v), heading(h),
        start(s),
        description(NULL), metadata(NULL)
      {
      if (heading<-180.0) heading+=360.0;
      else if (heading>180.0) heading-=360.0;
      }

   //! copy constructor
   minimeas(const minimeas &m)
      : minicoord((minicoord)m),
        accuracy(m.accuracy), velocity(m.velocity), heading(m.heading),
        start(m.start)
      {
      if (m.description) description=new ministring(*m.description);
      else description=NULL;

      if (m.metadata) metadata=new ministring(*m.metadata);
      else metadata=NULL;
      }

   //! destructor
   ~minimeas()
      {
      if (description)
         delete description;

      if (metadata)
         delete metadata;
      }

   //! assignment operator
   minimeas& operator = (const minimeas &m)
      {
      *(minicoord *)this=(minicoord)m;

      accuracy=m.accuracy;
      velocity=m.velocity;
      heading=m.heading;

      start=m.start;

      if (m.description) description=new ministring(*m.description);
      else description=NULL;

      if (m.metadata) metadata=new ministring(*m.metadata);
      else metadata=NULL;

      return(*this);
      }

   //! set description
   void set_description(const ministring &desc)
      {
      if (description)
         {
         delete description;
         description=NULL;
         }

      if (!desc.empty())
         {
         description=new ministring(desc);

         description->remove_leading_white_space();
         description->remove_trailing_white_space();
         }
      }

   //! get description
   ministring get_description() const
      {
      if (description)
         return(*description);

      return("");
      }

   //! set metadata
   void set_metadata(const ministring &meta)
      {
      if (metadata)
         {
         delete metadata;
         metadata=NULL;
         }

      if (!meta.empty())
         {
         metadata=new ministring(meta);

         metadata->remove_leading_white_space();
         metadata->remove_trailing_white_space();
         }
      }

   //! get metadata
   ministring get_metadata() const
      {
      if (metadata)
         return(*metadata);

      return("");
      }

   //! serialization
   ministring to_string() const;

   //! deserialization
   void from_string(ministring &info);

   float accuracy;
   float velocity;
   float heading;

   BOOLINT start;

   protected:

   ministring *description;
   ministring *metadata;
   };

// arithmetic inline operators
inline minimeas operator + (const minimeas &a,const minimeas &b);
inline minimeas operator - (const minimeas &a,const minimeas &b);
inline minimeas operator * (const double a,const minimeas &b);
inline minimeas operator * (const minimeas &a,const double b);
inline minimeas operator / (const minimeas &a,const double b);
inline int operator < (const minimeas &a,const minimeas &b);

// output stream operator
inline std::ostream& operator << (std::ostream &out,const minimeas &m);

// implementation of inline operators:

inline minimeas operator + (const minimeas &a,const minimeas &b)
   {
   return(minimeas(minicoord(a.vec+b.vec,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy+b.accuracy,a.velocity+b.velocity,a.heading+b.heading,
                   a.start));
   }

inline minimeas operator - (const minimeas &a,const minimeas &b)
   {
   return(minimeas(minicoord(a.vec-b.vec,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy+b.accuracy,a.velocity+b.velocity,a.heading-b.heading,
                   a.start));
   }

inline minimeas operator * (const double a,const minimeas &b)
   {
   return(minimeas(minicoord(a*b.vec,b.type,b.crs_zone,b.crs_datum),
                   a*b.accuracy,a*b.velocity,a*b.heading,
                   b.start));
   }

inline minimeas operator * (const minimeas &a,const double b)
   {
   return(minimeas(minicoord(a.vec*b,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy*b,a.velocity*b,a.heading*b,
                   a.start));
   }

inline minimeas operator / (const minimeas &a,const double b)
   {
   return(minimeas(minicoord(a.vec/b,a.type,a.crs_zone,a.crs_datum),
                   a.accuracy/b,a.velocity/b,a.heading/b,
                   a.start));
   }

inline int operator < (const minimeas &a,const minimeas &b)
   {return(a.vec.w<b.vec.w);}

inline std::ostream& operator << (std::ostream &out,const minimeas &m)
   {return(out << m.to_string().c_str());}

#endif
