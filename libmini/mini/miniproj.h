// (c) by Stefan Roettger

#ifndef MINIPROJ_H
#define MINIPROJ_H

#include "minibase.h"

#include "miniv3d.h"

class miniproj
   {
   public:

   //! default constructor
   miniproj();

   //! destructor
   ~miniproj();

   //! project a tetrahedron
   void proj(const miniv3d &v1,const double c1,
             const miniv3d &v2,const double c2,
             const miniv3d &v3,const double c3,
             const miniv3d &v4,const double c4,
             const miniv3d &col,const double rho,
             const miniv3d &eye);

   private:

   inline BOOLINT isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e);
   inline double intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m);

   inline void draw3fan(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &col,const double rho,
                        const miniv3d &eye);

   inline void draw4fan(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &col,const double rho,
                        const miniv3d &eye);
   };

#endif
