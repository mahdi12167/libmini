// (c) by Stefan Roettger

#ifndef MINIVEC_H
#define MINIVEC_H

#include <iostream>

#include "minibase.h"

#include "miniv3f.h"
#include "miniv3d.h"
#include "miniv4f.h"
#include "miniv4d.h"

#include "minimpfp.h"

//! templated 3D vector
template <class Scalar>
class minivec
   {
   public:

   //! default constructor
   minivec() {}

   //! copy constructor
   minivec(const minivec &v) {x=v.x; y=v.y; z=v.z;}

   //! constructors
   minivec(const Scalar v) {x=y=z=v;}
   minivec(const Scalar vx,const Scalar vy,const Scalar vz) {x=vx; y=vy; z=vz;}

   //! conversion constructors
   minivec(const miniv3f &v) {x=v.x; y=v.y; z=v.z;}
   minivec(const miniv3d &v) {x=v.x; y=v.y; z=v.z;}
   minivec(const miniv4d &v) {x=v.x; y=v.y; z=v.z;}
   minivec(const miniv4f &v) {x=v.x; y=v.y; z=v.z;}

   //! conversion operator
   operator miniv3d() {return(miniv3d(x,y,z));}

   //! destructor
   ~minivec() {}

   void setcoords(const Scalar vx,const Scalar vy,const Scalar vz) {x=vx; y=vy; z=vz;}

   Scalar getlength() const {return(sqrt(x*x+y*y+z*z));}
   Scalar getlength2() const {return(x*x+y*y+z*z);}

   inline minivec& operator += (const minivec &v);
   inline minivec& operator -= (const minivec &v);

   inline minivec& operator *= (const Scalar c);
   inline minivec& operator /= (const Scalar c);

   Scalar normalize()
      {
      Scalar length=getlength();
      if (length>Scalar(0)) *this/=length;
      return(length);
      }

   Scalar x,y,z;
   };

template <class Scalar>
inline minivec<Scalar>& minivec<Scalar>::operator += (const minivec<Scalar> &v)
   {
   x+=v.x; y+=v.y; z+=v.z;
   return(*this);
   }

template <class Scalar>
inline minivec<Scalar>& minivec<Scalar>::operator -= (const minivec<Scalar> &v)
   {
   x-=v.x; y-=v.y; z-=v.z;
   return(*this);
   }

template <class Scalar>
inline minivec<Scalar>& minivec<Scalar>::operator *= (const Scalar c)
   {
   x*=c; y*=c; z*=c;
   return(*this);
   }

template <class Scalar>
inline minivec<Scalar>& minivec<Scalar>::operator /= (const Scalar c)
   {
   Scalar rc;
   if (c!=Scalar(0)) rc=Scalar(1)/c;
   else rc=1;
   x=x*rc; y=y*rc; z=z*rc;
   return(*this);
   }

template <class Scalar>
inline minivec<Scalar> operator + (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline minivec<Scalar> operator - (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline minivec<Scalar> operator - (const minivec<Scalar> &v);

template <class Scalar>
inline minivec<Scalar> operator * (const Scalar a,const minivec<Scalar> &b);
template <class Scalar>
inline minivec<Scalar> operator * (const minivec<Scalar> &a,const Scalar b);
template <class Scalar>
inline minivec<Scalar> operator / (const minivec<Scalar> &a,const Scalar b);

template <class Scalar>
inline Scalar operator * (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline minivec<Scalar> operator / (const minivec<Scalar> &a,const minivec<Scalar> &b);

template <class Scalar>
inline int operator == (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline int operator != (const minivec<Scalar> &a,const minivec<Scalar> &b);

template <class Scalar>
inline int operator < (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline int operator > (const minivec<Scalar> &a,const minivec<Scalar> &b);

template <class Scalar>
inline int operator <= (const minivec<Scalar> &a,const minivec<Scalar> &b);
template <class Scalar>
inline int operator >= (const minivec<Scalar> &a,const minivec<Scalar> &b);

template <class Scalar>
inline std::ostream& operator << (std::ostream &out,const minivec<Scalar> &v);

template <class Scalar>
inline minivec<Scalar> operator + (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(minivec<Scalar>(a.x+b.x,a.y+b.y,a.z+b.z));}

template <class Scalar>
inline minivec<Scalar> operator - (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(minivec<Scalar>(a.x-b.x,a.y-b.y,a.z-b.z));}

template <class Scalar>
inline minivec<Scalar> operator - (const minivec<Scalar> &v)
   {return(minivec<Scalar>(-v.x,-v.y,-v.z));}

template <class Scalar>
inline minivec<Scalar> operator * (const Scalar a,const minivec<Scalar> &b)
   {return(minivec<Scalar>(a*b.x,a*b.y,a*b.z));}

template <class Scalar>
inline minivec<Scalar> operator * (const minivec<Scalar> &a,const Scalar b)
   {return(minivec<Scalar>(a.x*b,a.y*b,a.z*b));}

template <class Scalar>
inline minivec<Scalar> operator / (const minivec<Scalar> &a,const Scalar b)
   {return(minivec<Scalar>(a.x/b,a.y/b,a.z/b));}

template <class Scalar>
inline Scalar operator * (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x*b.x+a.y*b.y+a.z*b.z);}

// (0,0,-1)/(-1,0,0)=(0,1,0)
template <class Scalar>
inline minivec<Scalar> operator / (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(minivec<Scalar>(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x));}

template <class Scalar>
inline int operator == (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z);}

template <class Scalar>
inline int operator != (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z);}

template <class Scalar>
inline int operator < (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x*a.x+a.y*a.y+a.z*a.z<b.x*b.x+b.y*b.y+b.z*b.z);}

template <class Scalar>
inline int operator > (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x*a.x+a.y*a.y+a.z*a.z>b.x*b.x+b.y*b.y+b.z*b.z);}

template <class Scalar>
inline int operator <= (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x*a.x+a.y*a.y+a.z*a.z<=b.x*b.x+b.y*b.y+b.z*b.z);}

template <class Scalar>
inline int operator >= (const minivec<Scalar> &a,const minivec<Scalar> &b)
   {return(a.x*a.x+a.y*a.y+a.z*a.z>=b.x*b.x+b.y*b.y+b.z*b.z);}

template <class Scalar>
inline std::ostream& operator << (std::ostream &out,const minivec<Scalar> &v)
   {return(out << '(' << round2(v.x) << ',' << round2(v.y) << ',' << round2(v.z) << ')');}

typedef minivec<float> minivecf;
typedef minivec<double> minivecd;
typedef minivec<minimf> minivecmf;

#endif
