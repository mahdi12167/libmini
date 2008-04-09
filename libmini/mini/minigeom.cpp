// (c) by Stefan Roettger

#include "minigeom.h"

const double minigeom_base::delta=1E-10;

// intersect with half space
BOOLINT minigeom_segment::intersect(const minigeom_halfspace &halfspace)
   {
   BOOLINT cut;

   double dot,lambda;
   double lambda1,lambda2;

   if (minlambda>maxlambda) return(FALSE);

   cut=FALSE;

   dot=vec*halfspace.vec; // dot product between line direction and plane normal
   lambda=(pnt-halfspace.pnt)*halfspace.vec; // distance of line origin to plane

   // check if line and plane are parallel
   if (dot!=0.0)
      {
      // project minimum distance into segment space
      if (FABS(halfspace.minlambda)!=MAXFLOAT) lambda1=(halfspace.minlambda-lambda)/dot;
      else if (dot<0.0) lambda1=-halfspace.minlambda;
      else lambda1=halfspace.minlambda;

      // clamp minimum distance
      if (lambda1<-MAXFLOAT) lambda1=-MAXFLOAT;
      else if (lambda1>MAXFLOAT) lambda1=MAXFLOAT;

      // project maximum distance into segment space
      if (FABS(halfspace.maxlambda)!=MAXFLOAT) lambda2=(halfspace.maxlambda-lambda)/dot;
      else if (dot<0.0) lambda2=-halfspace.maxlambda;
      else lambda2=halfspace.maxlambda;

      // clamp maximum distance
      if (lambda2<-MAXFLOAT) lambda2=-MAXFLOAT;
      else if (lambda2>MAXFLOAT) lambda2=MAXFLOAT;

      // intersect half space range with segment range
      if (dot<0.0)
         {
         if (lambda1<maxlambda-delta) {maxlambda=lambda1; cut=TRUE;}
         if (lambda2>minlambda+delta) {minlambda=lambda2; cut=TRUE;}
         }
      else
         {
         if (lambda1>minlambda+delta) {minlambda=lambda1; cut=TRUE;}
         if (lambda2<maxlambda-delta) {maxlambda=lambda2; cut=TRUE;}
         }
      }
   else
      // check if segment lies outside of half space
      if (lambda<halfspace.minlambda-delta ||
          lambda>halfspace.maxlambda+delta) {setnull(); cut=TRUE;}

   return(cut);
   }

// intersect with half space
minigeom_line minigeom_halfspace::intersect(const minigeom_halfspace &halfspace)
   {
   miniv3d cross,dir;
   double dot,lambda;
   miniv3d orig1,orig2;
   miniv3d orig;

   minigeom_line line;

   // check half space and plane condition
   if ((!ishalf() || !halfspace.ishalf()) &&
       (!iszero() || !halfspace.iszero())) return(line);

   cross=vec/halfspace.vec; // cross product of plane normals
   dir=cross/vec; // direction from plane origin towards line origin
   dot=-dir*halfspace.vec; // dot product with intersecting plane normal

   // check if planes are parallel
   if (dot!=0.0)
      {
      orig1=pnt+minlambda*vec; // plane origin
      orig2=halfspace.pnt+halfspace.minlambda*halfspace.vec; // intersecting plane origin

      lambda=(orig1-orig2)*halfspace.vec; // distance of plane origin to intersecting plane
      lambda/=dot; // distance to line origin

      // clamp distance
      if (lambda<-MAXFLOAT) lambda=-MAXFLOAT;
      else if (lambda>MAXFLOAT) lambda=MAXFLOAT;

      orig=orig1+lambda*dir; // line origin

      // create intersection line
      line=minigeom_line(orig,cross);
      line.setfull();
      }

   return(line);
   }

// default constructor
minigeom_polyhedron::minigeom_polyhedron()
   {
   // pre-define a closed bounding box with maximum possible size
   half.setsize(6);
   half[0]=minigeom_halfspace(miniv3d(-MAXFLOAT/8,0,0),miniv3d(1,0,0));
   half[1]=minigeom_halfspace(miniv3d(MAXFLOAT/8,0,0),miniv3d(-1,0,0));
   half[2]=minigeom_halfspace(miniv3d(0,-MAXFLOAT/8,0),miniv3d(0,1,0));
   half[3]=minigeom_halfspace(miniv3d(0,MAXFLOAT/8,0),miniv3d(0,-1,0));
   half[4]=minigeom_halfspace(miniv3d(0,0,-MAXFLOAT/8),miniv3d(0,0,1));
   half[5]=minigeom_halfspace(miniv3d(0,0,MAXFLOAT/8),miniv3d(0,0,-1));
   }

// destructor
minigeom_polyhedron::~minigeom_polyhedron() {}

// intersect with half space
void minigeom_polyhedron::intersect(const minigeom_halfspace &halfspace)
   {
   unsigned int i;

   if (halfspace.ishalf())
      if (check4intersection(halfspace)) // check if half space cuts the polyhedron
         {
         half.append(halfspace); // add half space to existing half spaces

         for (i=2; i<half.getsize(); i++)
            if (check4redundancy(half.getsize()-i)) remove(half.getsize()-i); // delete redundant half spaces
         }
   }

// clear half spaces
void minigeom_polyhedron::clear()
   {
   half.setnull();
   val.setnull();
   }

// remove half space
void minigeom_polyhedron::remove(const unsigned int h)
   {
   half[h]=half[half.getsize()-1];
   half.setsize(half.getsize()-1);
   }

// check if a half space intersects with the polyhedron
BOOLINT minigeom_polyhedron::check4intersection(const minigeom_halfspace &halfspace,const BOOLINT omit,const unsigned int h)
   {
   unsigned int i,j,k;

   minigeom_segment segment;

   // check for equal half space
   for (i=0; i<half.getsize(); i++)
      if (!omit || i!=h)
         if (half[i].vec==halfspace.vec)
            if (FABS((half[i].pnt-halfspace.pnt)*halfspace.vec+half[i].minlambda-halfspace.minlambda)<minigeom_base::delta) return(FALSE);

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

            // intersect edge segement with half space
            if (segment.intersect(halfspace)) return(TRUE);
            }

   return(FALSE);
   }

// check if a half space is redundant with respect to the other half spaces
BOOLINT minigeom_polyhedron::check4redundancy(const unsigned int h)
   {return(!check4intersection(half[h],TRUE,h));}
