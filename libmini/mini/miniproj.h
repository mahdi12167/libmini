// (c) by Stefan Roettger

#ifndef MINIPROJ_H
#define MINIPROJ_H

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"
#include "minimesh.h"

class miniprojclip
   {
   public:

   //! default constructor
   miniprojclip() {ENABLED=FALSE;}

   //! constructor
   miniprojclip(const miniv3d &p,const miniv3d &n) {ENABLED=TRUE; P=p; N=n;}

   //! destructor
   ~miniprojclip() {}

   BOOLINT ENABLED;
   miniv3d P,N;
   };

class miniproj
   {
   public:

   //! default constructor
   miniproj();

   //! destructor
   virtual ~miniproj();

   // dynamic coordinate array
   typedef minidyna<miniv3d,8> dynacoord;

   // dynamic clip plane array
   typedef minidyna<miniprojclip,6> dynaclip;

   //! initialize projection state
   virtual void initproj(float emi,float rho);

   //! project a tetrahedron
   void proj(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp);

   //! clip&project a tetrahedron
   void clip(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp,
             const double clipf=0.0);

   //! project a tetrahedron (minivals version)
   void proj(const miniv3d &v1,const double c1,
             const miniv3d &v2,const double c2,
             const miniv3d &v3,const double c3,
             const miniv3d &v4,const double c4,
             const unsigned int maxslots,const minivals &vals,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp);

   //! clip&project a tetrahedron (minivals version)
   void clip(const miniv3d &v1,const double c1,
             const miniv3d &v2,const double c2,
             const miniv3d &v3,const double c3,
             const miniv3d &v4,const double c4,
             const unsigned int maxslots,const minivals &vals,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp,
             const double clipf=0.0);

   //! de-initialize projection state
   virtual void exitproj();

   //! enable projection mode
   void setproj(float delta=0.0f);

   //! get projection mode
   BOOLINT getproj();

   //! enable cutting plane mode
   void setplane(BOOLINT plane,
                 const miniv3d &p=miniv3d(0.0),const miniv3d &n=miniv3d(0.0));

   //! enable z-clipping
   void setzclip(float nearp=0.0f,float farp=0.0f,int zcliptexid=0);

   //! get z-clipping
   BOOLINT getzclip();

   //! add clipping plane
   void addclip(int num,const miniv3d &p,const miniv3d &n);

   //! delete clipping plane
   void delclip(int num);

   protected:

   float EMI,RHO;

   BOOLINT PROJMODE;
   float DELTA;

   BOOLINT PLANEMODE;
   miniv3d PLANEPNT,PLANENRM;

   BOOLINT ZCLIP;
   float ZNEAR,ZFAR;
   int ZCLIPTEXID;
   int ZTEXID;

   dynaclip CLIP;

   miniv4d MVMATRIX[3];

   void proj(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const unsigned int clipn,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp);

   void clip(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const unsigned int clipn,
             const miniv3d &p,const miniv3d &n,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp);

   inline void map(const unsigned int which,
                   const unsigned int maxslots,const minivals vals,
                   dynacoord &a);

   inline BOOLINT isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e);
   inline double intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m);

   void getmodelview();

   inline void passmtx(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,
                       const dynacoord &a1,const dynacoord &a2,const dynacoord &a3,const dynacoord &a4);

   virtual void pass(const dynacoord &m);
   virtual void pass(const int idx,const miniv3d &v,const dynacoord &a);

   inline void proj3tri(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &eye);

   inline void proj4tri(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &eye);

   void coords(const double c,const dynacoord &a,const double d);

   inline void slice1tri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                         const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                         const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                         const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                         const double delta);

   inline void slice2tri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                         const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                         const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                         const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                         const double delta);

   inline void slicetri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                        const double delta);

   inline void slicetet(const miniv3d &v1,const double c1,const dynacoord &a1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,
                        const miniv3d &eye,const miniv3d &dir,
                        const double nearp,
                        const double delta);

   inline void projtri(const miniv3d &v1,const double c1,const dynacoord &a1,
                       const miniv3d &v2,const double c2,const dynacoord &a2,
                       const miniv3d &v3,const double c3,const dynacoord &a3,
                       const miniv3d &v4,const double c4,const dynacoord &a4,
                       const miniv3d &col,
                       const miniv3d &eye,const miniv3d &dir,
                       const double nearp);

   void projpri(const miniv3d &v1,const double c1,const dynacoord &a1,
                const miniv3d &v2,const double c2,const dynacoord &a2,
                const miniv3d &v3,const double c3,const dynacoord &a3,
                const miniv3d &v4,const double c4,const dynacoord &a4,
                const miniv3d &v5,const double c5,const dynacoord &a5,
                const miniv3d &v6,const double c6,const dynacoord &a6,
                const unsigned int clipn,
                const miniv3d &col,
                const miniv3d &eye,const miniv3d &dir,
                const double nearp);

   void clip1A(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const unsigned int clipn,
               const miniv3d &col,
               const miniv3d &eye,const miniv3d &dir,
               const double nearp);

   void clip1B(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const unsigned int clipn,
               const miniv3d &col,
               const miniv3d &eye,const miniv3d &dir,
               const double nearp);

   void clip2(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
              const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
              const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
              const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
              const unsigned int clipn,
              const miniv3d &col,
              const miniv3d &eye,const miniv3d &dir,
              const double nearp);

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
