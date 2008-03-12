// (c) by Stefan Roettger

#include "minigeom.h"

// intersect with half space
void minigeom_segment::intersect(minigeom_halfspace &halfspace)
   {
   double dot,lambda;
   double lambda1,lambda2;

   dot=vec*halfspace.vec; // dot product between line direction and plane normal
   lambda=(pnt-halfspace.pnt)*halfspace.vec; // distance of line origin to plane

   // check if line and plane are parallel
   if (dot!=0.0)
      {
      // project minimum distance into segment space
      if (FABS(halfspace.minlambda)!=MAXFLOAT) lambda1=(lambda+halfspace.minlambda)/dot;
      else if (dot<0.0) lambda1=-halfspace.minlambda;
      else lambda1=halfspace.minlambda;

      // clamp minimum distance
      if (lambda1<-MAXFLOAT) lambda1=-MAXFLOAT;
      else if (lambda1>MAXFLOAT) lambda1=MAXFLOAT;

      // project maximum distance into segment space
      if (FABS(halfspace.maxlambda)!=MAXFLOAT) lambda2=(lambda+halfspace.maxlambda)/dot;
      else if (dot<0.0) lambda2=-halfspace.maxlambda;
      else lambda2=halfspace.maxlambda;

      // clamp maximum distance
      if (lambda2<-MAXFLOAT) lambda2=-MAXFLOAT;
      else if (lambda2>MAXFLOAT) lambda2=MAXFLOAT;

      // intersect half space range with segment range
      if (lambda1>minlambda) minlambda=lambda1;
      if (lambda2<maxlambda) maxlambda=lambda1;
      }
   else
      // check if segment lies outside of half space
      if (lambda<halfspace.minlambda || lambda>halfspace.maxlambda)
         {
         minlambda=MAXFLOAT;
         maxlambda=-MAXFLOAT;
         }
   }

// intersect with half space
minigeom_segment minigeom_halfspace::intersect(minigeom_halfspace &halfspace)
   {return(minigeom_segment());}

// default constructor
minigeom_polyhedron::minigeom_polyhedron()
   {
   half=NULL;

   numhalf=0;
   maxhalf=0;
   }

// copy constructor
minigeom_polyhedron::minigeom_polyhedron(const minigeom_polyhedron &poly)
   {
   int i;

   allocate(poly.numhalf);

   for (i=0; i<poly.numhalf; i++) half[i]=poly.half[i];
   }

// destructor
minigeom_polyhedron::~minigeom_polyhedron()
   {if (half!=NULL) delete half;}

// intersect with half space
void minigeom_polyhedron::intersect(minigeom_halfspace &halfspace)
   {
   allocate(numhalf+1);

   half[numhalf]=halfspace;
   numhalf++;
   }

void minigeom_polyhedron::allocate(int n)
   {
   int i;

   minigeom_halfspace *tmp;

   if (n>maxhalf)
      {
      if (n<2*maxhalf) n=2*maxhalf;

      if (half==NULL)
         {
         half=new minigeom_halfspace[n];
         maxhalf=n;
         }
      else
         {
         tmp=new minigeom_halfspace[n];

         for (i=0; i<numhalf; i++) tmp[i]=half[i];

         delete half;
         half=tmp;

         maxhalf=n;
         }
      }
   }
