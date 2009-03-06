// (c) by Stefan Roettger

#ifndef MINIPROJ_H
#define MINIPROJ_H

#include <iostream>

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"
#include "minimesh.h"

class miniprojcrd
   {
   public:

   //! default constructor
   miniprojcrd()
      {
      ACTIVE=FALSE;
      CRD=miniv3d(0.0);
      }

   //! constructor
   miniprojcrd(const miniv3d &crd)
      {
      ACTIVE=TRUE;
      CRD=crd;
      }

   //! destructor
   ~miniprojcrd() {}

   BOOLINT ACTIVE;
   miniv3d CRD;
   };

//! add operator
inline miniprojcrd operator + (const miniprojcrd &a,const miniprojcrd &b)
   {
   if (a.ACTIVE && b.ACTIVE) return(a.CRD+b.CRD);
   else return(miniprojcrd());
   }

//! sub operator
inline miniprojcrd operator - (const miniprojcrd &a,const miniprojcrd &b)
   {
   if (a.ACTIVE && b.ACTIVE) return(a.CRD-b.CRD);
   else return(miniprojcrd());
   }

//! neg operator
inline miniprojcrd operator - (const miniprojcrd &v)
   {
   if (v.ACTIVE) return(-v.CRD);
   else return(miniprojcrd());
   }

//! mul operator
inline miniprojcrd operator * (const double a,const miniprojcrd &b)
   {
   if (b.ACTIVE) return(a*b.CRD);
   else return(miniprojcrd());
   }

//! mul operator
inline miniprojcrd operator * (const miniprojcrd &a,const double b)
   {
   if (a.ACTIVE) return(a.CRD*b);
   else return(miniprojcrd());
   }

//! div operator
inline miniprojcrd operator / (const miniprojcrd &a,const double b)
   {
   if (a.ACTIVE) return(a.CRD/b);
   else return(miniprojcrd());
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const miniprojcrd &v)
   {
   if (v.ACTIVE) out << v.CRD;
   else out << "none";

   return(out);
   }

class miniprojclip
   {
   public:

   //! default constructor
   miniprojclip()
      {
      ENABLED=FALSE;

      CLIPALL=TRUE;
      SLOT=0;

      P=N=miniv3d(0.0);
      }

   //! constructor
   miniprojclip(const miniv3d &p,const miniv3d &n)
      {
      ENABLED=TRUE;

      CLIPALL=TRUE;
      SLOT=0;

      P=p;
      N=n;
      }

   //! constructor
   miniprojclip(const miniv3d &p,const miniv3d &n,unsigned int slot)
      {
      ENABLED=TRUE;

      CLIPALL=FALSE;
      SLOT=slot;

      P=p;
      N=n;
      }

   //! destructor
   ~miniprojclip() {}

   BOOLINT ENABLED;
   BOOLINT CLIPALL;
   unsigned int SLOT;
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
   typedef minidyna<miniprojcrd,8> dynacoord;

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

   //! map slot to brickid
   BOOLINT brickid(const unsigned int slot,const minivals &vals,unsigned int *id);

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

   //! add clipping plane to specific slot
   void addclip(int num,const miniv3d &p,const miniv3d &n,unsigned int slot);

   //! delete clipping plane
   void delclip(int num);

   //! clear clipping planes
   void clearclip();

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
   miniv4d MVINVMTX[3];
   miniv4d MVINVTRA[3];

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

   void clip(const miniv3d &v1,const double c1,const dynacoord &a1,
             const miniv3d &v2,const double c2,const dynacoord &a2,
             const miniv3d &v3,const double c3,const dynacoord &a3,
             const miniv3d &v4,const double c4,const dynacoord &a4,
             const unsigned int clipn,
             const miniv3d &p,const miniv3d &n,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp,
             const unsigned int slot);

   inline void map(const unsigned int which,
                   const unsigned int maxslots,const minivals &vals,
                   dynacoord &a);

   inline BOOLINT isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e) const;
   inline double intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m) const;

   void getmodelview();

   virtual void pass(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,
                     const dynacoord &a1,const dynacoord &a2,const dynacoord &a3,const dynacoord &a4);

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

   void clip1AS(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                const unsigned int clipn,
                const miniv3d &col,
                const miniv3d &eye,const miniv3d &dir,
                const double nearp,
                const unsigned int slot);

   void clip1BS(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                const unsigned int clipn,
                const miniv3d &col,
                const miniv3d &eye,const miniv3d &dir,
                const double nearp,
                const unsigned int slot);

   void clip2S(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const unsigned int clipn,
               const miniv3d &col,
               const miniv3d &eye,const miniv3d &dir,
               const double nearp,
               const unsigned int slot);

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
