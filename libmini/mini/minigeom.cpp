// (c) by Stefan Roettger

#include "minigeom.h"

minigeom_segment minigeom_segment::intersect(minigeom_halfspace &halfspace)
   {
   return(*this);
   }

minigeom_segment minigeom_halfspace::intersect(minigeom_segment &segment)
   {
   return(segment.intersect(*this));
   }

minigeom_segment minigeom_halfspace::intersect(minigeom_halfspace &halfspace)
   {
   return(minigeom_segment());
   }
