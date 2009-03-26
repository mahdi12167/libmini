// (c) by Stefan Roettger

#ifndef MINIGEOM_H
#define MINIGEOM_H

#include "iostream"

#include "minibase.h"

#include "minivec.h"
#include "minidyna.h"

//! templated geometry base class
template <class Scalar>
class minigeom_base
   {
   public:

   typedef minivec<Scalar> Vector;

   protected:

   static const double delta;

   static const double alpha;
   static const double beta;

   public:

   //! default constructor
   minigeom_base() {setnull();}

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v,const Scalar minl=0,const Scalar maxl=MAXFLOAT)
      {
      pnt=p;
      vec=v;

      vec.normalize();

      minlambda=dmax(minl,-MAXFLOAT);
      maxlambda=dmin(maxl,MAXFLOAT);
      }

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=0,const Scalar maxl=MAXFLOAT)
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=dmax(minl,-MAXFLOAT);
      maxlambda=dmin(maxl,MAXFLOAT);
      }

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=0,const Scalar maxl=MAXFLOAT)
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=dmax(minl,-MAXFLOAT);
      maxlambda=dmin(maxl,MAXFLOAT);

      flip(h);
      }

   //! destructor
   ~minigeom_base() {}

   Vector getpoint() const {return(pnt);}
   Vector getvector() const {return(vec);}

   Vector getpoint(const Scalar lambda) const {return(pnt+lambda*vec);}

   Vector getminpoint() const {return(pnt+minlambda*vec);}
   Vector getmaxpoint() const {return(pnt+maxlambda*vec);}

   Scalar getminlambda() const {return(minlambda);}
   Scalar getmaxlambda() const {return(maxlambda);}

   BOOLINT isnull() const {return(minlambda>maxlambda);}
   BOOLINT iszero() const {return(minlambda==maxlambda);}
   BOOLINT ishalf() const {return(minlambda>-MAXFLOAT && maxlambda==MAXFLOAT);}
   BOOLINT isfull() const {return(minlambda==-MAXFLOAT && maxlambda==MAXFLOAT);}

   void setnull() {minlambda=MAXFLOAT; maxlambda=-MAXFLOAT;}
   void setzero() {minlambda=0; maxlambda=0;}
   void sethalf() {minlambda=0; maxlambda=MAXFLOAT;}
   void setfull() {minlambda=-MAXFLOAT; maxlambda=MAXFLOAT;}

   BOOLINT isincl(const Vector &p) const
      {
      Scalar d;

      if (isnull()) return(FALSE);
      else if (isfull()) return(TRUE);
      else
         {
         d=(p-pnt)*vec;

         if (ishalf()) return(d>minlambda-delta);
         else return(d>minlambda-delta && d<maxlambda+delta);
         }

      return(FALSE);
      }

   BOOLINT isequal(const minigeom_base &b) const
      {
      Scalar d;

      if (isnull() && b.isnull()) return(TRUE);
      else if (isfull() && b.isfull()) return(TRUE);
      else if (vec*b.vec>beta)
         {
         d=(b.pnt-pnt)*vec;

         if (ishalf() && b.ishalf()) return(dabs(d+b.minlambda-minlambda)<delta);
         else return(dabs(d+b.minlambda-minlambda)<delta && dabs(d+b.maxlambda-maxlambda)<delta);
         }

      return(FALSE);
      }

   void swap()
      {
      Scalar tmp;

      vec=-vec;
      tmp=-minlambda;
      minlambda=-maxlambda;
      maxlambda=tmp;
      }

   void flip(const Vector &h)
      {
      if (ishalf())
         if (!isincl(h))
            {
            vec=-vec;
            minlambda=-minlambda;
            }
      }

   void invert()
      {
      if (isnull()) setfull();
      else if (isfull()) setnull();
      else if (ishalf())
         {
         vec=-vec;
         minlambda=-minlambda;
         }
      }

   protected:

   Vector pnt,vec;
   Scalar minlambda,maxlambda;
   };

//! stream output
template <class Scalar>
inline std::ostream& operator << (std::ostream &out,const minigeom_base<Scalar> &g)
   {
   out << "minigeom( point=" << g.getpoint() << ", vector=" << g.getvector();
   if (!g.ishalf()) out << ", minlambda=" << g.getminlambda() << ", maxlambda=" << g.getmaxlambda();
   out << " )";

   return(out);
   }

//! forward declaration
template <class Scalar>
class minigeom_segment;

//! forward declaration
template <class Scalar>
class minigeom_halfspace;

//! line segment
template <class Scalar>
class minigeom_segment: public minigeom_base<Scalar>
   {
   public:

   typedef minigeom_base<Scalar> B;
   typedef minivec<Scalar> Vector;

   //! default constructor
   minigeom_segment(): B() {}

   //! conversion constructor
   minigeom_segment(const Vector &p,const Vector &v,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_segment() {}

   //! get point distance
   Scalar getdistance(const Vector &p) const {return((B::p-((B::p-B::pnt)*B::vec)*B::vec-B::pnt).getlength());}

   //! intersect with half space
   BOOLINT intersect(const minigeom_halfspace<Scalar> &halfspace);
   };

//! clone declaration
template <class Scalar>
class minigeom_line: public minigeom_segment<Scalar>
   {
   public:

   typedef minigeom_segment<Scalar> B;
   typedef minivec<Scalar> Vector;

   minigeom_line(): B() {}
   minigeom_line(const Vector &p,const Vector &v,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v,minl,maxl) {}
   minigeom_line(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,minl,maxl) {}
   minigeom_line(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,h,minl,maxl) {}

   ~minigeom_line() {}
   };

//! dynamic segment array
template <class Scalar>
class minigeom_segments: public minidyna<minigeom_segment<Scalar>,10> {};

//! forward declaration
template <class Scalar>
class minigeom_polyhedron;

//! half space
template <class Scalar>
class minigeom_halfspace: public minigeom_base<Scalar>
   {
   public:

   typedef minigeom_base<Scalar> B;
   typedef minivec<Scalar> Vector;

   //! default constructor
   minigeom_halfspace(): B() {}

   //! conversion constructor
   minigeom_halfspace(const Vector &p,const Vector &v,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_halfspace() {}

   //! get point distance
   Scalar getdistance(const Vector &p) const
      {return((p-B::pnt)*B::vec-B::minlambda);}

   //! intersect with half space
   minigeom_line<Scalar> intersect(const minigeom_halfspace<Scalar> &halfspace) const;

   private:

   friend class minigeom_segment<Scalar>;
   friend class minigeom_polyhedron<Scalar>;
   };

//! clone declaration
template <class Scalar>
class minigeom_plane: public minigeom_halfspace<Scalar>
   {
   public:

   typedef minigeom_halfspace<Scalar> B;
   typedef minivec<Scalar> Vector;

   minigeom_plane(): B() {}
   minigeom_plane(const Vector &p,const Vector &v,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v,minl,maxl) {}
   minigeom_plane(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,minl,maxl) {}
   minigeom_plane(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=0,const Scalar maxl=MAXFLOAT): B(p,v1,v2,h,minl,maxl) {}

   ~minigeom_plane() {}
   };

//! dynamic halfspace array
template <class Scalar>
class minigeom_halfspaces: public minidyna<minigeom_halfspace<Scalar>,6> {};

//! convex polyhedron
template <class Scalar>
class minigeom_polyhedron
   {
   public:

   typedef minivec<Scalar> Vector;

   //! default constructor
   minigeom_polyhedron(const Scalar range=1.0E9);

   //! destructor
   ~minigeom_polyhedron();

   //! get number of defining half spaces
   unsigned int getnumhalfspace() const {return(half.getsize());}

   //! get defining half space
   minigeom_halfspace<Scalar> gethalfspace(const unsigned int h) const {return(half[h]);}

   //! intersect with half space
   void intersect(const minigeom_halfspace<Scalar> &halfspace);

   //! clear half spaces
   void clear();

   //! get face segments of corresponding half space
   minigeom_segments<Scalar> getface(const unsigned int h) const;

   protected:

   minigeom_halfspaces<Scalar> half;

   private:

   void remove(const unsigned int h);

   BOOLINT check4intersection(const minigeom_halfspace<Scalar> &halfspace,const BOOLINT omit=FALSE,const unsigned int h=0) const;
   BOOLINT check4redundancy(const unsigned int h) const;
   };

//! stream output
template <class Scalar>
inline std::ostream& operator << (std::ostream &out,const minigeom_polyhedron<Scalar> &poly)
   {
   unsigned int i;

   out << "minipoly[ ";

   for (i=0; i<poly.getnumhalfspace(); i++)
      {
      out << poly.gethalfspace(i);
      if (i<poly.getnumhalfspace()-1) out << ", ";
      }

   out << " ]";

   return(out);
   }

//! dynamic polyhedron array
template <class Scalar>
class minigeom_polyhedra: public minidyna<minigeom_polyhedron<Scalar>,10> {};

#include "minigeom.cpp"

#endif
