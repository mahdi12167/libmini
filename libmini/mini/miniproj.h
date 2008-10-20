// (c) by Stefan Roettger

#ifndef MINIPROJ_H
#define MINIPROJ_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"

class miniproj
   {
   public:

   //! default constructor
   miniproj();

   //! destructor
   virtual ~miniproj();

   // dynamic coordinate array
   typedef minidyna<miniv3d,8> dynacoord;

   //! initialize projection state
   virtual void initproj(float emi,float rho);

   //! project a tetrahedron
   void proj(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const miniv3d &col,
             const miniv3d &eye);

   //! clip&project a tetrahedron
   void clip(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp);

   //! de-initialize projection state
   virtual void exitproj();

   //! enable z-clipping
   void setzclip(float nearp=0.0f,float farp=0.0f,int zcliptexid=0);

   protected:

   float EMI,RHO;

   BOOLINT ZCLIP;
   float ZNEAR,ZFAR;
   int ZCLIPTEXID;
   int ZTEXID;

   inline BOOLINT isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e);
   inline double intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m);

   virtual void pass(const int idx,const miniv3d &v,const dynacoord &a);

   inline void proj3tri(const miniv3d &v1,const double c1,const dynacoord &a1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,
                        const miniv3d &eye);

   inline void proj4tri(const miniv3d &v1,const double c1,const dynacoord &a1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,
                        const miniv3d &eye);

   void projpri(const miniv3d &v1,const double c1,const dynacoord &a1,
                const miniv3d &v2,const double c2,const dynacoord &a2,
                const miniv3d &v3,const double c3,const dynacoord &a3,
                const miniv3d &v4,const double c4,const dynacoord &a4,
                const miniv3d &v5,const double c5,const dynacoord &a5,
                const miniv3d &v6,const double c6,const dynacoord &a6,
                const miniv3d &col,
                const miniv3d &eye);

   void clip1A(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const miniv3d &col,
               const miniv3d &eye);

   void clip1B(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const miniv3d &col,
               const miniv3d &eye);

   void clip2(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
              const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
              const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
              const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
              const miniv3d &col,
              const miniv3d &eye);

   virtual void initzclip();
   virtual void exitzclip();

   // vertex and fragment programs:

   char *VTXPROG,*FRGPROG,*FRGPROGZ;
   int VTXPROGID,FRGPROGID,FRGPROGZID;

   void enablevtxshader();
   void disablevtxshader();

   void enablepixshader();
   void disablepixshader();

   void setupprogs();
   void deleteprogs();
   };

#endif
