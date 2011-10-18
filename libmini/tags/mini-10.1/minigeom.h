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

   static const Scalar zero() {static Scalar v(0); return(v);}
   static const Scalar one() {static Scalar v(1); return(v);}

   static const Scalar maxscalar() {static Scalar v(MAXFLOAT); return(v);}

   protected:

   static const Scalar delta() {static Scalar v(1E-3); return(v);}

   static const Scalar alpha() {static Scalar v(1E-9); return(v);}
   static const Scalar beta() {static Scalar v=one()-alpha(); return(v);}

   public:

   //! default constructor
   minigeom_base() {setnull();}

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v,const Scalar minl=zero(),const Scalar maxl=maxscalar())
      {
      pnt=p;
      vec=v;

      vec.normalize();

      minlambda=dmax(minl,-maxscalar());
      maxlambda=dmin(maxl,maxscalar());
      }

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=zero(),const Scalar maxl=maxscalar())
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=dmax(minl,-maxscalar());
      maxlambda=dmin(maxl,maxscalar());
      }

   //! conversion constructor
   minigeom_base(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=zero(),const Scalar maxl=maxscalar())
      {
      pnt=p;
      vec=(v1-p)/(v2-p);

      vec.normalize();

      minlambda=dmax(minl,-maxscalar());
      maxlambda=dmin(maxl,maxscalar());

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

   Scalar getlength() const {return(maxlambda-minlambda);}

   BOOLINT isnull() const {return(minlambda>maxlambda);}
   BOOLINT iszero() const {return(minlambda==maxlambda);}
   BOOLINT isnonzero() const {return(maxlambda>minlambda);}
   BOOLINT ishalf() const {return(minlambda>-maxscalar() && maxlambda>=maxscalar());}
   BOOLINT isfull() const {return(minlambda<=-maxscalar() && maxlambda>=maxscalar());}

   void setnull() {minlambda=maxscalar(); maxlambda=-maxscalar();}
   void setzero() {minlambda=zero(); maxlambda=zero();}
   void sethalf() {minlambda=zero(); maxlambda=maxscalar();}
   void setfull() {minlambda=-maxscalar(); maxlambda=maxscalar();}

   BOOLINT isincl(const Vector &p) const
      {
      Scalar d;

      if (isnull()) return(FALSE);
      else if (isfull()) return(TRUE);
      else
         {
         d=(p-pnt)*vec;

         if (ishalf()) return(d+delta()>minlambda);
         else return(d+delta()>minlambda && d-delta()<maxlambda);
         }

      return(FALSE);
      }

   BOOLINT isequal(const minigeom_base &b) const
      {
      Scalar d;

      if (isnull() && b.isnull()) return(TRUE);
      else if (isfull() && b.isfull()) return(TRUE);
      else if (vec*b.vec>beta())
         {
         d=(b.pnt-pnt)*vec;

         if (ishalf() && b.ishalf()) return(dabs(d+b.minlambda-minlambda)<delta());
         else return(dabs(d+b.minlambda-minlambda)<delta() && dabs(d+b.maxlambda-maxlambda)<delta());
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
   minigeom_segment(const Vector &p,const Vector &v,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_segment(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_segment() {}

   //! get point distance
   Scalar getdistance(const Vector &p) const {return((p-((p-B::pnt)*B::vec)*B::vec-B::pnt).getlength());}

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
   minigeom_line(const Vector &p,const Vector &v,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v,minl,maxl) {}
   minigeom_line(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,minl,maxl) {}
   minigeom_line(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,h,minl,maxl) {}

   ~minigeom_line() {}
   };

//! dynamic polygon array
template <class Scalar>
class minigeom_polygon: public minidyna<minivec<Scalar>,10> {};

//! dynamic segment array
template <class Scalar>
class minigeom_segments: public minidyna<minigeom_segment<Scalar>,10>
   {
   public:

   typedef minidyna<minigeom_segment<Scalar>,10> B;
   typedef minivec<Scalar> Vector;

   minigeom_polygon<Scalar> polygonize();
   };

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
   minigeom_halfspace(const Vector &p,const Vector &v,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,minl,maxl) {}

   //! conversion constructor
   minigeom_halfspace(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,h,minl,maxl) {}

   //! destructor
   ~minigeom_halfspace() {}

   //! get point distance
   Scalar getdistance(const Vector &p) const {return((p-B::pnt)*B::vec-B::minlambda);}

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
   minigeom_plane(const Vector &p,const Vector &v,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v,minl,maxl) {}
   minigeom_plane(const Vector &p,const Vector &v1,const Vector &v2,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,minl,maxl) {}
   minigeom_plane(const Vector &p,const Vector &v1,const Vector &v2,const Vector &h,const Scalar minl=B::zero(),const Scalar maxl=B::maxscalar()): B(p,v1,v2,h,minl,maxl) {}

   ~minigeom_plane() {}
   };

//! dynamic halfspace array
template <class Scalar>
class minigeom_halfspaces: public minidyna<minigeom_halfspace<Scalar>,6> {};

//! forward declaration
template <class Scalar>
class minigeom_tetrahedra;

//! convex polyhedron
template <class Scalar>
class minigeom_polyhedron
   {
   public:

   typedef minigeom_halfspace<Scalar> B;
   typedef minivec<Scalar> Vector;

   protected:

   static const Scalar maxrange() {static Scalar v(1E9); return(v);}

   public:

   //! default constructor
   minigeom_polyhedron(const Scalar range=maxrange());

   //! destructor
   ~minigeom_polyhedron();

   //! get number of defining half spaces
   unsigned int getnumhalfspace() const {return(half.getsize());}

   //! get defining half space
   minigeom_halfspace<Scalar> gethalfspace(const unsigned int h) const {return(half[h]);}

   //! intersect with half space
   void intersect(const minigeom_halfspace<Scalar> &halfspace);

   //! intersect with polyhedron
   void intersect(const minigeom_polyhedron<Scalar> &poly);

   //! clear half spaces
   void clear();

   //! get face segments of corresponding half space
   minigeom_segments<Scalar> getface(const unsigned int h) const;

   //! check if point is included within polyhedron
   BOOLINT isincl(const Vector &p) const;

   //! check if polyhedron includes another one
   BOOLINT isincl(const minigeom_polyhedron<Scalar> poly) const;

   //! tetrahedralize a convex polyhedron
   minigeom_tetrahedra<Scalar> tetrahedralize() const;

   //! get volume
   Scalar getvolume() const;

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

//! tetrahedron
template <class Scalar>
class minigeom_tetrahedron
   {
   public:

   typedef minigeom_halfspace<Scalar> B;
   typedef minivec<Scalar> Vector;

   public:

   //! default constructor
   minigeom_tetrahedron() {}

   //! constructor
   minigeom_tetrahedron(const Vector &p1,const Vector &p2,const Vector &p3,const Vector &p4)
      {
      corner[0]=p1;
      corner[1]=p2;
      corner[2]=p3;
      corner[3]=p4;
      }

   //! conversion operator
   operator minigeom_polyhedron<Scalar>() const
      {
      minigeom_polyhedron<Scalar> poly;

      poly.intersect(B(corner[0],corner[1],corner[2],corner[3]));
      poly.intersect(B(corner[0],corner[3],corner[1],corner[2]));
      poly.intersect(B(corner[1],corner[3],corner[2],corner[0]));
      poly.intersect(B(corner[2],corner[3],corner[0],corner[1]));

      return(poly);
      }

   //! destructor
   ~minigeom_tetrahedron() {}

   //! get corner point
   Vector getcorner(const unsigned int c) const
      {return(corner[c]);}

   //! check if point is included within polyhedron
   BOOLINT isincl(const Vector &p) const
      {
      if (!B(corner[0],corner[1],corner[2],corner[3]).isincl(p)) return(FALSE);
      if (!B(corner[0],corner[3],corner[1],corner[2]).isincl(p)) return(FALSE);
      if (!B(corner[1],corner[3],corner[2],corner[0]).isincl(p)) return(FALSE);
      if (!B(corner[2],corner[3],corner[0],corner[1]).isincl(p)) return(FALSE);

      return(TRUE);
      }

   //! get volume
   Scalar getvolume() const
      {
      Vector mtx[3];
      Scalar det;

      mtx[0]=Vector(corner[0]-corner[3]);
      mtx[1]=Vector(corner[1]-corner[3]);
      mtx[2]=Vector(corner[2]-corner[3]);

      det=mtx[0].x*(mtx[1].y*mtx[2].z-mtx[2].y*mtx[1].z)+
          mtx[0].y*(mtx[2].x*mtx[1].z-mtx[1].x*mtx[2].z)+
          mtx[0].z*(mtx[1].x*mtx[2].y-mtx[2].x*mtx[1].y);

      return(dabs(det)/Scalar(6));
      }

   protected:

   Vector corner[4];
   };

//! stream output
template <class Scalar>
inline std::ostream& operator << (std::ostream &out,const minigeom_tetrahedron<Scalar> &tet)
   {
   unsigned int i;

   out << "minitet[ ";

   for (i=0; i<4; i++)
      {
      out << tet.getcorner(i);
      if (i<3) out << ", ";
      }

   out << " ]";

   return(out);
   }

//! dynamic tetrahedron array
template <class Scalar>
class minigeom_tetrahedra: public minidyna<minigeom_tetrahedron<Scalar>,10> {};

// template body:

// intersect with half space
template <class Scalar>
BOOLINT minigeom_segment<Scalar>::intersect(const minigeom_halfspace<Scalar> &halfspace)
   {
   BOOLINT cut;

   Scalar dot,lambda;
   Scalar lambda1,lambda2;

   if (B::isnull()) return(FALSE);

   cut=FALSE;

   dot=B::vec*halfspace.vec; // dot product between line direction and plane normal
   lambda=(B::pnt-halfspace.pnt)*halfspace.vec; // distance of line origin to plane

   // check if line and plane are parallel
   if (dabs(dot)>B::alpha())
      {
      // project minimum distance into segment space
      if (dabs(halfspace.minlambda)<B::maxscalar()) lambda1=(halfspace.minlambda-lambda)/dot;
      else if (dot<B::zero()) lambda1=-halfspace.minlambda;
      else lambda1=halfspace.minlambda;

      // project maximum distance into segment space
      if (dabs(halfspace.maxlambda)<B::maxscalar()) lambda2=(halfspace.maxlambda-lambda)/dot;
      else if (dot<B::zero()) lambda2=-halfspace.maxlambda;
      else lambda2=halfspace.maxlambda;

      // intersect half space range with segment range
      if (dot<B::zero())
         {
         if (lambda1<B::maxlambda-B::delta()) {B::maxlambda=lambda1; cut=TRUE;}
         if (lambda2>B::minlambda+B::delta()) {B::minlambda=lambda2; cut=TRUE;}
         }
      else
         {
         if (lambda1>B::minlambda+B::delta()) {B::minlambda=lambda1; cut=TRUE;}
         if (lambda2<B::maxlambda-B::delta()) {B::maxlambda=lambda2; cut=TRUE;}
         }
      }
   else
      // check if segment lies outside of half space
      if (lambda+B::delta()<halfspace.minlambda || lambda-B::delta()>halfspace.maxlambda) {B::setnull(); cut=TRUE;}

   return(cut);
   }

// intersect with half space
template <class Scalar>
minigeom_line<Scalar> minigeom_halfspace<Scalar>::intersect(const minigeom_halfspace<Scalar> &halfspace) const
   {
   Vector cross,dir;
   Scalar dot,lambda;
   Vector orig1,orig2;
   Vector orig;

   minigeom_line<Scalar> line;

   if (!B::iszero() && !B::ishalf()) return(line);
   if (!halfspace.iszero() && !halfspace.ishalf()) return(line);

   cross=B::vec/halfspace.vec; // cross product of plane normals
   dir=cross/B::vec; // direction from plane origin towards line origin
   dot=-dir*halfspace.vec; // dot product with intersecting plane normal

   // check if planes are parallel
   if (dabs(dot)>B::alpha())
      {
      orig1=B::pnt+B::minlambda*B::vec; // plane origin
      orig2=halfspace.pnt+halfspace.minlambda*halfspace.vec; // intersecting plane origin

      lambda=(orig1-orig2)*halfspace.vec; // distance of plane origin to intersecting plane
      lambda=lambda/dot; // distance to line origin

      orig=orig1+lambda*dir; // line origin

      // create intersection line
      line=minigeom_line<Scalar>(orig,cross);
      line.setfull();
      }

   return(line);
   }

// polygonize a set of line segments
template <class Scalar>
minigeom_polygon<Scalar> minigeom_segments<Scalar>::polygonize()
   {
   unsigned int i,j;

   Vector a,b,c,d,e;

   unsigned int idx;
   Scalar dist,d1,d2;

   minigeom_polygon<Scalar> gon;

   minigeom_segment<Scalar> tmp;

   if (B::getsize()<3) return(gon);

   e=B::get(0).getminpoint();

   for (i=0; i<B::getsize(); i++)
      {
      a=B::get(i).getminpoint();
      b=B::get(i).getmaxpoint();

      gon.append(a);

      idx=i;
      dist=minigeom_segment<Scalar>::maxscalar();

      for (j=i+1; j<B::getsize(); j++)
         {
         c=B::get(j).getminpoint();
         d=B::get(j).getmaxpoint();

         d1=(c-b).getlength2();
         d2=(d-b).getlength2();

         if (d1<dist)
            {
            idx=j;
            dist=d1;
            }

         if (d2<dist)
            {
            idx=j;
            dist=d2;

            B::ref(j).swap();
            }
         }

      if (dist>(b-e).getlength2()) break;

      if (i+2<B::getsize())
         {
         tmp=B::get(i+1);
         B::set(i+1,B::get(idx));
         B::set(idx,tmp);
         }
      }

   return(gon);
   }

// default constructor
template <class Scalar>
minigeom_polyhedron<Scalar>::minigeom_polyhedron(const Scalar range)
   {
   // pre-define a closed bounding box with maximum possible size
   half.append(B(Vector(-range,B::zero(),B::zero()),Vector(B::one(),B::zero(),B::zero())));
   half.append(B(Vector(range,B::zero(),B::zero()),Vector(-B::one(),B::zero(),B::zero())));
   half.append(B(Vector(B::zero(),-range,B::zero()),Vector(B::zero(),B::one(),B::zero())));
   half.append(B(Vector(B::zero(),range,B::zero()),Vector(B::zero(),-B::one(),B::zero())));
   half.append(B(Vector(B::zero(),B::zero(),-range),Vector(B::zero(),B::zero(),B::one())));
   half.append(B(Vector(B::zero(),B::zero(),range),Vector(B::zero(),B::zero(),-B::one())));
   }

// destructor
template <class Scalar>
minigeom_polyhedron<Scalar>::~minigeom_polyhedron() {}

// intersect with half space
template <class Scalar>
void minigeom_polyhedron<Scalar>::intersect(const minigeom_halfspace<Scalar> &halfspace)
   {
   unsigned int i;

   if (halfspace.ishalf()) // check validity of half space
      if (check4intersection(halfspace)) // check if half space cuts the polyhedron
         {
         half.append(halfspace); // add half space to existing half spaces

         for (i=1; i<half.getsize(); i++) // check for redundant half spaces
            if (check4redundancy(half.getsize()-i-1)) // check half spaces in reverse order
               {
               remove(half.getsize()-i-1); // delete redundant half space
               i--; // account for reduced size
               }
         }
   }

// intersect with polyhedron
template <class Scalar>
void minigeom_polyhedron<Scalar>::intersect(const minigeom_polyhedron<Scalar> &poly)
   {
   unsigned int i;

   for (i=0; i<poly.getnumhalfspace(); i++) intersect(poly.gethalfspace(i));
   }

// clear half spaces
template <class Scalar>
void minigeom_polyhedron<Scalar>::clear()
   {half.clear();}

// remove half space
template <class Scalar>
void minigeom_polyhedron<Scalar>::remove(const unsigned int h)
   {half.remove(h);}

// check if a half space intersects with the polyhedron
template <class Scalar>
BOOLINT minigeom_polyhedron<Scalar>::check4intersection(const minigeom_halfspace<Scalar> &halfspace,const BOOLINT omit,const unsigned int h) const
   {
   unsigned int i,j,k;

   minigeom_segment<Scalar> segment;

   // check for identical half space
   for (i=0; i<half.getsize(); i++)
      if (!omit || i!=h)
         if (half[i].isequal(halfspace)) return(FALSE);

   // check for intersection with all the edges of all faces
   for (i=0; i<half.getsize(); i++)
      for (j=i+1; j<half.getsize(); j++)
         if (!omit || (i!=h && j!=h))
            {
            // calculate a line from two shared faces
            segment=half[i].intersect(half[j]);

            // the faces were parallel
            if (segment.isnull()) continue;

            // shrink the line to its corresponding edge segment
            for (k=0; k<half.getsize(); k++)
               if (!omit || k!=h)
                  if (k!=i && k!=j) segment.intersect(half[k]);

            // test edge segment for intersection with half space
            if (segment.intersect(halfspace)) return(TRUE);
            }

   return(FALSE);
   }

// check if a half space is redundant with respect to the other half spaces
template <class Scalar>
BOOLINT minigeom_polyhedron<Scalar>::check4redundancy(const unsigned int h) const
   {return(!check4intersection(half[h],TRUE,h));}

// get face segments of corresponding half space
template <class Scalar>
minigeom_segments<Scalar> minigeom_polyhedron<Scalar>::getface(const unsigned int h) const
   {
   unsigned int i,j;

   minigeom_segment<Scalar> segment;
   minigeom_segments<Scalar> segments;

   // create a segment for each half space
   for (i=0; i<half.getsize(); i++)
      if (i!=h)
         {
         // calculate a line from two shared faces
         segment=half[h].intersect(half[i]);

         // the faces were parallel
         if (segment.isnull()) continue;

         // shrink the line to its corresponding edge segment
         for (j=0; j<half.getsize(); j++)
            if (j!=h && j!=i) segment.intersect(half[j]);

         // append one face segment
         if (segment.isnonzero()) segments.append(segment);
         }

   return(segments);
   }

// check if point is included within polyhedron
template <class Scalar>
BOOLINT minigeom_polyhedron<Scalar>::isincl(const Vector &p) const
   {
   unsigned int i;

   for (i=0; i<half.getsize(); i++)
      if (!half[i].isincl(p)) return(FALSE);

   return(TRUE);
   }

// check if polyhedron includes another one
template <class Scalar>
BOOLINT minigeom_polyhedron<Scalar>::isincl(const minigeom_polyhedron<Scalar> poly) const
   {
   unsigned int i,j;

   minigeom_polygon<Scalar> gon;

   for (i=0; i<poly.getnumhalfspace(); i++)
      {
      gon=poly.getface(i).polygonize();

      for (j=0; j<gon.getsize(); j++)
         if (!isincl(gon[j])) return(FALSE);
      }

   return(TRUE);
   }

// tetrahedralize a convex polyhedron
template <class Scalar>
minigeom_tetrahedra<Scalar> minigeom_polyhedron<Scalar>::tetrahedralize() const
   {
   unsigned int i,j;

   minigeom_polygon<Scalar> gon;
   Vector anchor,v1,v2,v3;

   minigeom_tetrahedra<Scalar> mesh;

   Scalar dist;

   if (getnumhalfspace()<4) return(mesh);

   gon=getface(0).polygonize();
   if (gon.getsize()==0) return(mesh);

   anchor=gon[0];

   for (i=1; i<getnumhalfspace(); i++)
      {
      gon=getface(i).polygonize();

      for (j=0; j+2<gon.getsize(); j++)
         {
         v1=gon[0];
         v2=gon[j+1];
         v3=gon[j+2];

         dist=minigeom_plane<Scalar>(v1,v2,v3).getdistance(anchor);

         if (dabs(dist)>B::delta())
            mesh.append(minigeom_tetrahedron<Scalar>(anchor,v1,v2,v3));
         }
      }

   return(mesh);
   }

// get volume
template <class Scalar>
Scalar minigeom_polyhedron<Scalar>::getvolume() const
   {
   unsigned int i;

   minigeom_tetrahedra<Scalar> mesh;

   Scalar vol;

   mesh=tetrahedralize();

   vol=0;

   for (i=0; i<mesh.getsize(); i++) vol=vol+mesh[i].getvolume();

   return(vol);
   }

#endif
