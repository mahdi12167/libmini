// (c) by Stefan Roettger

#ifndef MINICOMPLEX_H
#define MINICOMPLEX_H

#include <iostream>

#include "minibase.h"

//! complex vector
class minicomplex
   {
   public:

   //! default constructor
   minicomplex() {}

   //! copy constructor
   minicomplex(const minicomplex &z) {x=z.x; y=z.y;}

   //! constructors
   minicomplex(const double c) {x=c; y=0.0;}
   minicomplex(const double zx,const double zy) {x=zx; y=zy;}

   //! destructor
   ~minicomplex() {}

   double x,y;
   };

inline minicomplex operator + (const minicomplex &a,const minicomplex &b)
   {return(minicomplex(a.x+b.x,a.y+b.y));}

inline minicomplex operator - (const minicomplex &a,const minicomplex &b)
   {return(minicomplex(a.x-b.x,a.y-b.y));}

// conjugate
inline minicomplex operator - (const minicomplex &z)
   {return(minicomplex(z.x,-z.y));}

inline minicomplex operator * (const minicomplex &a,const minicomplex &b)
   {return(minicomplex(a.x*b.x-a.y*b.y,a.y*b.x+a.x*b.y));}

inline minicomplex operator / (const minicomplex &a,const minicomplex &b)
   {double l=b.x*b.x+b.y*b.y; return(minicomplex((a.x*b.x+a.y*b.y)/l,(a.y*b.x-a.x*b.y)/l));}

inline int operator == (const minicomplex &a,const minicomplex &b)
   {return(a.x==b.x && a.y==b.y);}

inline int operator != (const minicomplex &a,const minicomplex &b)
   {return(a.x!=b.x || a.y!=b.y);}

inline std::ostream& operator << (std::ostream &out,const minicomplex &z)
   {return(out << '(' << round2(z.x) << ',' << round2(z.y) << ')');}

#endif
