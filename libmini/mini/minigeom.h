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
   minigeom_base()
      {
      pnt=vec=miniv3d(0.0);
      setnull();
      }

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v,const double minl=0.0,const double maxl=MAXFLOAT)
      {
      pnt=p;
      vec=v;

      vec.normalize();

      minlambda=FMAX(minl,-MAXFLOAT);
      maxlambda=FMIN(maxl,MAXFLOAT);
      }

   //! destructor
   ~minigeom_base() {}

   miniv3d getpoint() const {return(pnt);}
   miniv3d getvector() const {return(vec);}

   miniv3d getpoint(const float lambda) const {return(pnt+lambda*vec);}

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

   void invert()
      {
      if (isnull() || iszero()) setfull();
      else if (ishalf()) {vec=-vec; minlambda=-minlambda;}
      else if (isfull()) setnull();
      }

   protected:

   miniv3d pnt,vec;
   double minlambda,maxlambda;

   private:
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

   //! get point distance
   double getdistance(const miniv3d &p) const {return((p-((p-pnt)*vec)*vec-pnt).getLength());}

   //! intersect with half space
   BOOLINT intersect(const minigeom_halfspace &halfspace);

   protected:

   private:
   };

typedef minigeom_segment minigeom_line;

//! half space
class minigeom_halfspace: public minigeom_base
   {
   public:

   //! default constructor
   minigeom_halfspace(): minigeom_base() {}

   //! conversion constructor
   minigeom_halfspace(const miniv3d &p,const miniv3d &v,const double minl=0.0,const double maxl=MAXFLOAT): minigeom_base(p,v,minl,maxl) {}

   //! get point distance
   double getdistance(const miniv3d &p) const {return((p-pnt)*vec-minlambda);}

   //! intersect with half space
   minigeom_line intersect(const minigeom_halfspace &halfspace);

   protected:

   private:

   friend class minigeom_segment;
   friend class minigeom_polyhedron;
   };

typedef minigeom_halfspace minigeom_plane;

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

   protected:

   minidyna<minigeom_halfspace> half;

   private:

   void remove(const unsigned int h);

   BOOLINT check4intersection(const minigeom_halfspace &halfspace,const BOOLINT omit=FALSE,const unsigned int h=0);
   BOOLINT check4redundancy(const unsigned int h);
   };

#endif
