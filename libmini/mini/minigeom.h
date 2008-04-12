// (c) by Stefan Roettger

#ifndef MINIGEOM_H
#define MINIGEOM_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"

class minigeom_base
   {
   public:

   static const double delta;

   //! default constructor
   minigeom_base() {}

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v,const double minl=0.0,const double maxl=MAXFLOAT)
      {
      pnt=p;
      vec=v;

      vec.normalize();

      minlambda=FMAX(minl,-MAXFLOAT);
      maxlambda=FMIN(maxl,MAXFLOAT);
      }

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const double minl=0.0,const double maxl=MAXFLOAT)
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=FMAX(minl,-MAXFLOAT);
      maxlambda=FMIN(maxl,MAXFLOAT);
      }

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &h,const double minl=0.0,const double maxl=MAXFLOAT)
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=FMAX(minl,-MAXFLOAT);
      maxlambda=FMIN(maxl,MAXFLOAT);

      flip(h);
      }

   //! destructor
   ~minigeom_base() {}

   miniv3d getpoint() const {return(pnt);}
   miniv3d getvector() const {return(vec);}

   miniv3d getpoint(const float lambda) const {return(pnt+lambda*vec);}

   miniv3d getminpoint() const {return(pnt+minlambda*vec);}
   miniv3d getmaxpoint() const {return(pnt+maxlambda*vec);}

   double getminlambda() const {return(minlambda);}
   double getmaxlambda() const {return(maxlambda);}

   BOOLINT isnull() const {return(minlambda>maxlambda);}
   BOOLINT iszero() const {return(minlambda==maxlambda);}
   BOOLINT ishalf() const {return(maxlambda==MAXFLOAT);}
   BOOLINT isfull() const {return(minlambda==-MAXFLOAT && maxlambda==MAXFLOAT);}

   void setnull() {minlambda=MAXFLOAT; maxlambda=-MAXFLOAT;}
   void setzero() {minlambda=0.0; maxlambda=0.0;}
   void sethalf() {minlambda=0.0; maxlambda=MAXFLOAT;}
   void setfull() {minlambda=-MAXFLOAT; maxlambda=MAXFLOAT;}

   BOOLINT isincl(const miniv3d &p) const
      {
      double d;

      d=(p-pnt)*vec;
      return(d>minlambda-delta && d<maxlambda+delta);
      }

   BOOLINT isequal(const minigeom_base &b)
      {
      double d;

      if (vec==b.vec)
         if (isnull() && b.isnull()) return(TRUE);
         else if (isfull() && b.isfull()) return(TRUE);
         else if (ishalf() && b.ishalf())
            {
            d=(b.pnt-pnt)*vec;
            if (FABS(d+minlambda-b.minlambda)<delta) return(TRUE);
            }
         else
            {
            d=(b.pnt-pnt)*vec;
            if (FABS(d+minlambda-b.minlambda)<delta && FABS(d+maxlambda-b.maxlambda)<delta) return(TRUE);
            }

      return(FALSE);
      }

   void swap()
      {
      double tmp;

      tmp=minlambda;
      minlambda=maxlambda;
      maxlambda=tmp;
      }

   void flip(const miniv3d &h)
      {
      if (ishalf())
         if (!isincl(h)) {vec=-vec; minlambda=-minlambda;}
      }

   void invert()
      {
      if (isnull() || iszero()) setfull();
      else if (ishalf()) {vec=-vec; minlambda=-minlambda;}
      else if (isfull()) setnull();
      }

   protected:

   miniv3d pnt,vec;
   double minlambda,maxlambda;
   };

class minigeom_segment;
class minigeom_halfspace;

//! line segment
class minigeom_segment: public minigeom_base
   {
   public:

   //! default constructor
   minigeom_segment(): minigeom_base() {}

   //! conversion constructor
   minigeom_segment(const miniv3d &p,const miniv3d &v,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &h,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_segment() {}

   //! get point distance
   double getdistance(const miniv3d &p) const {return((p-((p-pnt)*vec)*vec-pnt).getlength());}

   //! intersect with half space
   BOOLINT intersect(const minigeom_halfspace &halfspace);
   };

typedef minigeom_segment minigeom_line;
typedef minidyna<minigeom_segment,10> minigeom_segments;

//! half space
class minigeom_halfspace: public minigeom_base
   {
   public:

   //! default constructor
   minigeom_halfspace(): minigeom_base() {}

   //! conversion constructor
   minigeom_halfspace(const miniv3d &p,const miniv3d &v,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &h,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_halfspace() {}

   //! get point distance
   double getdistance(const miniv3d &p) const {return((p-pnt)*vec-minlambda);}

   //! intersect with half space
   minigeom_line intersect(const minigeom_halfspace &halfspace) const;

   private:

   friend class minigeom_segment;
   friend class minigeom_polyhedron;
   };

typedef minigeom_halfspace minigeom_plane;
typedef minidyna<minigeom_halfspace,10> minigeom_halfspaces;

//! convex polyhedron
class minigeom_polyhedron
   {
   public:

   //! default constructor
   minigeom_polyhedron();

   //! destructor
   ~minigeom_polyhedron();

   //! get number of defining half spaces
   unsigned int getnumhalfspace() const {return(half.getsize());}

   //! get defining half space
   minigeom_halfspace gethalfspace(const unsigned int h) const {return(half[h]);}

   //! intersect with half space
   void intersect(const minigeom_halfspace &halfspace);

   //! clear half spaces
   void clear();

   //! get face segments of corresponding half space
   minigeom_segments getface(const unsigned int h) const;

   protected:

   minidyna<minigeom_halfspace,6> half;

   private:

   void remove(const unsigned int h);

   BOOLINT check4intersection(const minigeom_halfspace &halfspace,const BOOLINT omit=FALSE,const unsigned int h=0) const;
   BOOLINT check4redundancy(const unsigned int h) const;
   };

typedef minidyna<minigeom_polyhedron,10> minigeom_polyhedra;

#endif
