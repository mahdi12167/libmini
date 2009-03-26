// (c) by Stefan Roettger

template <class Scalar>
const double minigeom_base<Scalar>::delta=1E-3;

template <class Scalar>
const double minigeom_base<Scalar>::alpha=1E-7;
template <class Scalar>
const double minigeom_base<Scalar>::beta=1.0-alpha;

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
   if (dabs(dot)>B::alpha)
      {
      // project minimum distance into segment space
      if (dabs(halfspace.minlambda)!=MAXFLOAT) lambda1=(halfspace.minlambda-lambda)/dot;
      else if (dot<0) lambda1=-halfspace.minlambda;
      else lambda1=halfspace.minlambda;

      // clamp minimum distance
      if (lambda1<-MAXFLOAT) lambda1=-MAXFLOAT;
      else if (lambda1>MAXFLOAT) lambda1=MAXFLOAT;

      // project maximum distance into segment space
      if (dabs(halfspace.maxlambda)!=MAXFLOAT) lambda2=(halfspace.maxlambda-lambda)/dot;
      else if (dot<0.0) lambda2=-halfspace.maxlambda;
      else lambda2=halfspace.maxlambda;

      // clamp maximum distance
      if (lambda2<-MAXFLOAT) lambda2=-MAXFLOAT;
      else if (lambda2>MAXFLOAT) lambda2=MAXFLOAT;

      // intersect half space range with segment range
      if (dot<0)
         {
         if (lambda1<B::maxlambda-B::delta) {B::maxlambda=lambda1; cut=TRUE;}
         if (lambda2>B::minlambda+B::delta) {B::minlambda=lambda2; cut=TRUE;}
         }
      else
         {
         if (lambda1>B::minlambda+B::delta) {B::minlambda=lambda1; cut=TRUE;}
         if (lambda2<B::maxlambda-B::delta) {B::maxlambda=lambda2; cut=TRUE;}
         }
      }
   else
      // check if segment lies outside of half space
      if (lambda<halfspace.minlambda-B::delta || lambda>halfspace.maxlambda+B::delta) {B::setnull(); cut=TRUE;}

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
   if (dabs(dot)>B::alpha)
      {
      orig1=B::pnt+B::minlambda*B::vec; // plane origin
      orig2=halfspace.pnt+halfspace.minlambda*halfspace.vec; // intersecting plane origin

      lambda=(orig1-orig2)*halfspace.vec; // distance of plane origin to intersecting plane
      lambda/=dot; // distance to line origin

      // clamp distance
      if (lambda<-MAXFLOAT) lambda=-MAXFLOAT;
      else if (lambda>MAXFLOAT) lambda=MAXFLOAT;

      orig=orig1+lambda*dir; // line origin

      // create intersection line
      line=minigeom_line<Scalar>(orig,cross);
      line.setfull();
      }

   return(line);
   }

// default constructor
template <class Scalar>
minigeom_polyhedron<Scalar>::minigeom_polyhedron(const Scalar range)
   {
   // pre-define a closed bounding box with maximum possible size
   half.append(minigeom_halfspace<Scalar>(Vector(-range,0,0),Vector(1,0,0)));
   half.append(minigeom_halfspace<Scalar>(Vector(range,0,0),Vector(-1,0,0)));
   half.append(minigeom_halfspace<Scalar>(Vector(0,-range,0),Vector(0,1,0)));
   half.append(minigeom_halfspace<Scalar>(Vector(0,range,0),Vector(0,-1,0)));
   half.append(minigeom_halfspace<Scalar>(Vector(0,0,-range),Vector(0,0,1)));
   half.append(minigeom_halfspace<Scalar>(Vector(0,0,range),Vector(0,0,-1)));
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

// clear half spaces
template <class Scalar>
void minigeom_polyhedron<Scalar>::clear()
   {half.setnull();}

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
         if (!segment.isnull()) segments.append(segment);
         }

   return(segments);
   }
