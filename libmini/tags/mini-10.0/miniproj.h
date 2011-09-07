// (c) by Stefan Roettger

#ifndef MINIPROJ_H
#define MINIPROJ_H

#include <iostream>

#include "minibase.h"

#include "miniv3d.h"
#include "minidyna.h"
#include "minimesh.h"

//! coordinate base class
class miniprojcrd
   {
   public:

   //! default constructor
   miniprojcrd() {}

   //! constructor
   miniprojcrd(const miniv3d &c,const BOOLINT enabled=TRUE)
      {
      active=enabled;
      crd=c;
      }

   //! destructor
   ~miniprojcrd() {}

   BOOLINT active;
   miniv3d crd;
   };

//! add operator
inline miniprojcrd operator + (const miniprojcrd &a,const miniprojcrd &b)
   {
   if (a.active && b.active) return(a.crd+b.crd);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! sub operator
inline miniprojcrd operator - (const miniprojcrd &a,const miniprojcrd &b)
   {
   if (a.active && b.active) return(a.crd-b.crd);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! neg operator
inline miniprojcrd operator - (const miniprojcrd &v)
   {
   if (v.active) return(-v.crd);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! mul operator
inline miniprojcrd operator * (const double a,const miniprojcrd &b)
   {
   if (b.active) return(a*b.crd);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! mul operator
inline miniprojcrd operator * (const miniprojcrd &a,const double b)
   {
   if (a.active) return(a.crd*b);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! div operator
inline miniprojcrd operator / (const miniprojcrd &a,const double b)
   {
   if (a.active) return(a.crd/b);
   else return(miniprojcrd(miniv3d(0.0),FALSE));
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const miniprojcrd &v)
   {
   if (v.active) out << v.crd;
   else out << "none";

   return(out);
   }

//! dynamic coordinate array
typedef minidyna<miniprojcrd,8> dynacoord;

//! mapper base class
class miniprojmap
   {
   public:

   //! default constructor
   miniprojmap() {}

   //! constructor
   miniprojmap(const unsigned int to,const unsigned int id,const BOOLINT enabled=TRUE)
      {
      active=enabled;
      map2=to;
      brickid=id;
      }

   //! destructor
   ~miniprojmap() {}

   BOOLINT active;
   unsigned int map2;
   unsigned int brickid;
   };

//! dynamic mapper array
typedef minidyna<miniprojmap,8> miniprojmapper;

//! clip plane base class
class miniprojclip
   {
   public:

   //! default constructor
   miniprojclip()
      {
      enabled=FALSE;

      clipall=TRUE;
      slot=0;

      pos=nrm=miniv3d(0.0);
      }

   //! constructor
   miniprojclip(const miniv3d &p,const miniv3d &n)
      {
      enabled=TRUE;

      clipall=TRUE;
      slot=0;

      pos=p;
      nrm=n;
      }

   //! constructor
   miniprojclip(const miniv3d &p,const miniv3d &n,unsigned int s)
      {
      enabled=TRUE;

      clipall=FALSE;
      slot=s;

      pos=p;
      nrm=n;
      }

   //! destructor
   ~miniprojclip() {}

   BOOLINT enabled;
   BOOLINT clipall;
   unsigned int slot;
   miniv3d pos,nrm;
   };

//! dynamic clip plane array
typedef minidyna<miniprojclip,6> dynaclip;

class miniproj
   {
   public:

   //! default constructor
   miniproj();

   //! destructor
   virtual ~miniproj();

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
             const double nearp,
             const miniprojmapper *mapper=NULL);

   //! clip&project a tetrahedron (minivals version)
   void clip(const miniv3d &v1,const double c1,
             const miniv3d &v2,const double c2,
             const miniv3d &v3,const double c3,
             const miniv3d &v4,const double c4,
             const unsigned int maxslots,const minivals &vals,
             const miniv3d &col,
             const miniv3d &eye,const miniv3d &dir,
             const double nearp,
             const double clipf=0.0,
             const miniprojmapper *mapper=NULL);

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

   //! get number of active slots
   unsigned int getactive(const unsigned int maxslots,const minimesh &mesh);

   //! enable remapping of active slots
   void setactive(const unsigned int active=0);

   //! initialize static mapping
   void initmap(const unsigned int maxslots,const minivals &vals,miniprojmapper &actmap);

   //! update dynamic remapping
   void remap(const unsigned int maxslots,const minivals &vals,miniprojmapper &actmap);

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
   void clrclip();

   protected:

   float EMI,RHO;

   BOOLINT PROJMODE;
   float DELTA;

   BOOLINT PLANEMODE;
   miniv3d PLANEPNT,PLANENRM;

   unsigned int ACTIVE;
   miniprojmapper MAP;

   BOOLINT ZCLIP;
   float ZNEAR,ZFAR;
   int ZCLIPTEXID;
   int ZTEXID;

   dynaclip CLIP;

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

   virtual void dirty(const unsigned int slot,const unsigned int map2,const unsigned int brickid);

   inline BOOLINT isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e) const;
   inline double intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m) const;

   virtual void pass(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,
                     const dynacoord &a1,const dynacoord &a2,const dynacoord &a3,const dynacoord &a4);

   virtual void passSLI(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,
                        const dynacoord &a1,const dynacoord &a2,const dynacoord &a3,const dynacoord &a4);

   virtual void passPRJ(const int idx,const miniv3d &v,const dynacoord &a);

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

   inline void coords(const double c,const dynacoord &a,const double d);

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

   inline void projpri(const miniv3d &v1,const double c1,const dynacoord &a1,
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

   inline BOOLINT deact(const unsigned int slot,dynacoord &a);

   void clip1As(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                const unsigned int clipn,
                const miniv3d &col,
                const miniv3d &eye,const miniv3d &dir,
                const double nearp,
                const unsigned int slot);

   void clip1Bs(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                const unsigned int clipn,
                const miniv3d &col,
                const miniv3d &eye,const miniv3d &dir,
                const double nearp,
                const unsigned int slot);

   void clip2s(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
               const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
               const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
               const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
               const unsigned int clipn,
               const miniv3d &col,
               const miniv3d &eye,const miniv3d &dir,
               const double nearp,
               const unsigned int slot);

   void clip3s(const miniv3d &v1,const double c1,const dynacoord &a1,
               const miniv3d &v2,const double c2,const dynacoord &a2,
               const miniv3d &v3,const double c3,const dynacoord &a3,
               const miniv3d &v4,const double c4,const dynacoord &a4,
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
