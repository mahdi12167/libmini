// (c) by Stefan Roettger

#ifndef MINIGEOM_H
#define MINIGEOM_H

#include "miniv3d.h"

class minigeom_base
   {
   public:

   //! default constructor
   minigeom_base()
      {
      point=vector=miniv3d(0.0);
      minlambda=maxlambda=0.0;
      }

   //! conversion constructor
   minigeom_base(const miniv3d &p,const miniv3d &v,double minl=0.0,double maxl=0.0)
         {
         point=p;
         vector=v;

         minlambda=minl;
         maxlambda=maxl;
         }

   //! destructor
   ~minigeom_base() {}

   miniv3d &getpoint() {return(point);}
   miniv3d &getvector() {return(vector);}

   miniv3d getpoint(const float lambda=0.0) {return(point+lambda*vector);}

   miniv3d getminpoint() {return(getpoint(minlambda));}
   miniv3d getmaxpoint() {return(getpoint(maxlambda));}

   protected:

   miniv3d point,vector;
   double minlambda,maxlambda;

   private:
   };

class minigeom_segment;
class minigeom_halfspace;

class minigeom_segment: public minigeom_base
   {
   public:

   minigeom_segment intersect(minigeom_halfspace &halfspace);

   protected:

   private:
   };

class minigeom_halfspace: public minigeom_base
   {
   public:

   minigeom_segment intersect(minigeom_segment &segment);
   minigeom_segment intersect(minigeom_halfspace &halfspace);

   protected:

   private:
   };

#endif
