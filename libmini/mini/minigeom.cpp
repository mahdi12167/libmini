// (c) by Stefan Roettger

#include "minigeom.h"

const double minigeom_base::delta=1E-10;

// intersect with half space
BOOLINT minigeom_segment::intersect(minigeom_halfspace &halfspace)
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
minigeom_segment minigeom_halfspace::intersect(minigeom_halfspace &halfspace)
   {
   miniv3d cross,dir;
   double dot,lambda;
   miniv3d orig1,orig2;
   miniv3d orig;

   minigeom_segment line;

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
      line=minigeom_segment(orig,cross);
      line.setfull();
      }

   return(line);
   }

// default constructor
minigeom_polyhedron::minigeom_polyhedron()
   {
   half=NULL;

   numhalf=0;
   maxhalf=0;

   allocate(6);

   // define a closed box
   half[0]=minigeom_halfspace(miniv3d(-MAXFLOAT/8,0,0),miniv3d(1,0,0));
   half[1]=minigeom_halfspace(miniv3d(MAXFLOAT/8,0,0),miniv3d(-1,0,0));
   half[2]=minigeom_halfspace(miniv3d(0,-MAXFLOAT/8,0),miniv3d(0,1,0));
   half[3]=minigeom_halfspace(miniv3d(0,MAXFLOAT/8,0),miniv3d(0,-1,0));
   half[4]=minigeom_halfspace(miniv3d(0,0,-MAXFLOAT/8),miniv3d(0,0,1));
   half[5]=minigeom_halfspace(miniv3d(0,0,MAXFLOAT/8),miniv3d(0,0,-1));

   numhalf=6;
   }

// copy constructor
minigeom_polyhedron::minigeom_polyhedron(const minigeom_polyhedron &poly)
   {
   int i;

   half=NULL;

   numhalf=0;
   maxhalf=0;

   allocate(poly.numhalf);

   for (i=0; i<poly.numhalf; i++) half[i]=poly.half[i];
   numhalf=poly.numhalf;
   }

// destructor
minigeom_polyhedron::~minigeom_polyhedron()
   {if (half!=NULL) delete[] half;}

// intersect with half space
void minigeom_polyhedron::intersect(minigeom_halfspace &halfspace)
   {
   int i;

   if (halfspace.ishalf())
      if (check4intersection(halfspace)) // check if half space cuts the polyhedron
         {
         allocate(numhalf+1);

         half[numhalf]=halfspace;
         numhalf++;

         for (i=numhalf-2; i>=0; i--)
            if (check4redundancy(i)) remove(i); // delete redundant half spaces
         }
   }

void minigeom_polyhedron::allocate(int n)
   {
   int i;

   minigeom_halfspace *tmp;

   // check available space
   if (n>maxhalf)
      {
      if (n<2*maxhalf) n=2*maxhalf;

      // create new
      if (half==NULL)
         {
         half=new minigeom_halfspace[n];
         maxhalf=n;
         }
      // enlarge old
      else
         {
         tmp=new minigeom_halfspace[n];

         for (i=0; i<numhalf; i++) tmp[i]=half[i];

         delete[] half;
         half=tmp;

         maxhalf=n;
         }
      }
   }

void minigeom_polyhedron::remove(int h)
   {
   int i;

   for (i=h+1; i<numhalf; i++) half[i-1]=half[i];
   numhalf--;
   }

BOOLINT minigeom_polyhedron::check4intersection(minigeom_halfspace &halfspace,int omit)
   {
   int i,j,k;

   minigeom_segment segment;

   // check for equal half space
   for (i=0; i<numhalf; i++)
      if (i!=omit)
         if (half[i].vec==halfspace.vec)
            if (FABS((half[i].pnt-halfspace.pnt)*halfspace.vec+half[i].minlambda-halfspace.minlambda)<minigeom_base::delta) return(FALSE);

   // check for intersection with all the edges of all faces
   for (i=0; i<numhalf; i++)
      for (j=i+1; j<numhalf; j++)
         if (i!=omit && j!=omit)
            {
            // calculate a line from two shared faces
            segment=half[i].intersect(half[j]);

            // the faces were parallel
            if (segment.isnull()) continue;

            // shrink the line to its corresponding edge segment
            for (k=0; k<numhalf; k++)
               if (k!=i && k!=j)
                  if (k!=omit) segment.intersect(half[k]);

            // intersect edge segement with half space
            if (segment.intersect(halfspace)) return(TRUE);
            }

   return(FALSE);
   }

BOOLINT minigeom_polyhedron::check4redundancy(int h)
   {return(!check4intersection(half[h],h));}
