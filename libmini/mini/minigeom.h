// (c) by Stefan Roettger

#ifndef MINIGEOM_H
#define MINIGEOM_H

#include "minibase.h"

#include "miniv3d.h"

class minigeom_base
   {
   public:

   //! default constructor
   minigeom_base()
      {
      pnt=vec=miniv3d(0.0);
      setnull();
      }

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v,double minl=0.0,double maxl=MAXFLOAT)
      {
      pnt=p;
      vec=v;

      vec.normalize();

      minlambda=FMAX(minl,-MAXFLOAT);
      maxlambda=FMIN(maxl,MAXFLOAT);
      }

   //! destructor
   ~minigeom_base() {}

   miniv3d &getpoint() {return(pnt);}
   miniv3d &getvector() {return(vec);}

   miniv3d getpoint(const float lambda) {return(pnt+lambda*vec);}

   double getminlambda() {return(minlambda);}
   double getmaxlambda() {return(maxlambda);}

   BOOLINT isnull() {return(minlambda>maxlambda);}
   BOOLINT iszero() {return(minlambda==maxlambda);}
   BOOLINT ishalf() {return(maxlambda==MAXFLOAT);}
   BOOLINT isfull() {return(minlambda==-MAXFLOAT && maxlambda==MAXFLOAT);}

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
   minigeom_segment(const miniv3d &p,const miniv3d &v,double minl=0.0,double maxl=MAXFLOAT): minigeom_base(p,v,minl,maxl) {}

   //! intersect with half space
   BOOLINT intersect(minigeom_halfspace &halfspace);

   protected:

   private:
   };

//! half space
class minigeom_halfspace: public minigeom_base
   {
   public:

   //! default constructor
   minigeom_halfspace(): minigeom_base() {}

   //! conversion constructor
   minigeom_halfspace(const miniv3d &p,const miniv3d &v,double minl=0.0,double maxl=MAXFLOAT): minigeom_base(p,v,minl,maxl) {}

   //! intersect with half space
   minigeom_segment intersect(minigeom_halfspace &halfspace);

   friend BOOLINT minigeom_segment::intersect(minigeom_halfspace &halfspace);

   protected:

   private:
   };

//! convex polyhedron
class minigeom_polyhedron
   {
   public:

   //! default constructor
   minigeom_polyhedron();

   //! copy constructor
   minigeom_polyhedron(const minigeom_polyhedron &poly);

   //! destructor
   ~minigeom_polyhedron();

   //! get number of defining half spaces
   int getnumhalfspace() {return(numhalf);}

   //! get defining half space
   minigeom_halfspace gethalfspace(int h) {return(half[h]);}

   //! intersect with half space
   void intersect(minigeom_halfspace &halfspace);

   protected:

   int numhalf,maxhalf;
   minigeom_halfspace *half;

   private:

   void allocate(int n);

   void remove(int h);

   BOOLINT check4intersection(minigeom_halfspace &halfspace);
   };

#endif
