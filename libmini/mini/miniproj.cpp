// (c) by Stefan Roettger

#include "miniOGL.h"

#include "minimath.h"

#include "miniproj.h"

// default constructor
miniproj::miniproj()
   {
   EMI=0.0f;
   RHO=0.0f;

   PROJMODE=TRUE;
   DELTA=0.0f;

   PLANEMODE=FALSE;
   PLANEPNT=PLANENRM=miniv3d(0.0);

   ZCLIP=FALSE;
   ZCLIPTEXID=0;

   setupprogs();
   }

// destructor
miniproj::~miniproj()
   {deleteprogs();}

// project a tetrahedron
void miniproj::proj(const miniv3d &v1,const double c1,const dynacoord &a1,
                    const miniv3d &v2,const double c2,const dynacoord &a2,
                    const miniv3d &v3,const double c3,const dynacoord &a3,
                    const miniv3d &v4,const double c4,const dynacoord &a4,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp)
   {
   if (c1==0.0 && c2==0.0 && c3==0.0 && c4==0.0) return;

   proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,0,col,eye,dir,nearp);
   }

// clip&project a tetrahedron
void miniproj::clip(const miniv3d &v1,const double c1,const dynacoord &a1,
                    const miniv3d &v2,const double c2,const dynacoord &a2,
                    const miniv3d &v3,const double c3,const dynacoord &a3,
                    const miniv3d &v4,const double c4,const dynacoord &a4,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp,
                    const double clipf)
   {
   miniv3d p;

   double factor=1.1;

   if (c1==0.0 && c2==0.0 && c3==0.0 && c4==0.0) return;

   if (clipf>1.0) factor=clipf;

   p=eye+factor*nearp*dir;

   clip(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,0,p,dir,col,eye,dir,nearp);
   }

// map slot to brickid
BOOLINT miniproj::brickid(const unsigned int slot,const minivals &vals,unsigned int *id)
   {
   unsigned int i;

   unsigned int size;

   BOOLINT active;

   size=vals.getsize();

   active=FALSE;

   // the last element mapping to the same slot takes precedence
   for (i=0; i<size; i++)
      if (vals[i].slot==slot)
         {
         active=TRUE;
         *id=vals[i].brickid;
         }

   return(active);
   }

// map minivals to dynacoord
void miniproj::map(const unsigned int which,
                   const unsigned int maxslots,const minivals &vals,
                   dynacoord &a)
   {
   unsigned int i;

   unsigned int size;
   unsigned int slot;

   size=vals.getsize();
   a.setsize(maxslots);

   // the last element mapping to the same slot takes precedence
   for (i=0; i<size; i++)
      {
      slot=vals[i].slot;
      if (slot>=maxslots) continue;

      switch (which)
         {
         case 1: a[slot]=vals[i].crd1; break;
         case 2: a[slot]=vals[i].crd2; break;
         case 3: a[slot]=vals[i].crd3; break;
         case 4: a[slot]=vals[i].crd4; break;
         }
      }
   }

// project a tetrahedron (minivals version)
void miniproj::proj(const miniv3d &v1,const double c1,
                    const miniv3d &v2,const double c2,
                    const miniv3d &v3,const double c3,
                    const miniv3d &v4,const double c4,
                    const unsigned int maxslots,const minivals &vals,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp)
   {
   dynacoord a1,a2,a3,a4;

   map(1,maxslots,vals,a1);
   map(2,maxslots,vals,a2);
   map(3,maxslots,vals,a3);
   map(4,maxslots,vals,a4);

   proj(v1,c1,a1,
        v2,c2,a2,
        v3,c3,a3,
        v4,c4,a4,
        col,
        eye,dir,
        nearp);
   }

// clip&project a tetrahedron (minivals version)
void miniproj::clip(const miniv3d &v1,const double c1,
                    const miniv3d &v2,const double c2,
                    const miniv3d &v3,const double c3,
                    const miniv3d &v4,const double c4,
                    const unsigned int maxslots,const minivals &vals,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp,
                    const double clipf)
   {
   dynacoord a1,a2,a3,a4;

   map(1,maxslots,vals,a1);
   map(2,maxslots,vals,a2);
   map(3,maxslots,vals,a3);
   map(4,maxslots,vals,a4);

   clip(v1,c1,a1,
        v2,c2,a2,
        v3,c3,a3,
        v4,c4,a4,
        col,
        eye,dir,
        nearp,
        clipf);
   }

// calculate whether or not a triangle is front- or back-facing
inline BOOLINT miniproj::isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e) const
   {
   miniv3d n;

   n=(v2-v1)/(v3-v1);

   return((n*(v1-e)>0.0)?n*(p-v1)<0.0:n*(p-v1)>0.0);
   }

// calculate the intersection of a ray with a plane
inline double miniproj::intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m) const
   {
   miniv3d n;

   double lambda;

   n=d1/d2;

   lambda=n*d;
   if (lambda!=0.0) lambda=n*(o-p)/lambda;

   m=p+lambda*d;

   return(lambda);
   }

// pass parameters down to the shader when in slicing mode
void miniproj::pass(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &v4,
                    const dynacoord &a1,const dynacoord &a2,const dynacoord &a3,const dynacoord &a4) {}

// pass parameters down to the shader when in projection mode
void miniproj::pass(const int,const miniv3d &,const dynacoord &) {}

// project 3 triangles
inline void miniproj::proj3tri(const miniv3d &v1,const double c1,
                               const miniv3d &v2,const double c2,
                               const miniv3d &v3,const double c3,
                               const miniv3d &v4,const double c4,
                               const miniv3d &eye)
   {
   miniv3d d1,d2,m,n;

   double lambda,
          w1,w2,w3,
          c234;

   // calculate span vectors
   d1=v3-v2;
   d2=v4-v2;

   // calculate thick vertex
   lambda=intersect(v1,v1-eye,v2,d1,d2,m);

   // calculate thick normal
   n=d1/d2;

   // use absolute normal components
   n.x=FABS(n.x);
   n.y=FABS(n.y);
   n.z=FABS(n.z);

   // barycentric interpolation at thick vertex
   if (n.x>n.y)
      if (n.x>n.z)
         {
         w1=FABS((v3.y-m.y)*(v4.z-m.z)-(v3.z-m.z)*(v4.y-m.y));
         w2=FABS((v2.y-m.y)*(v4.z-m.z)-(v2.z-m.z)*(v4.y-m.y));
         w3=FABS((v2.y-m.y)*(v3.z-m.z)-(v2.z-m.z)*(v3.y-m.y));

         c234=(w1*c2+w2*c3+w3*c4)/n.x;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }
   else
      if (n.y>n.z)
         {
         w1=FABS((v3.z-m.z)*(v4.x-m.x)-(v3.x-m.x)*(v4.z-m.z));
         w2=FABS((v2.z-m.z)*(v4.x-m.x)-(v2.x-m.x)*(v4.z-m.z));
         w3=FABS((v2.z-m.z)*(v3.x-m.x)-(v2.x-m.x)*(v3.z-m.z));

         c234=(w1*c2+w2*c3+w3*c4)/n.y;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }

   // check orientation
   if (lambda<0.0)
      {
      // calculate thickness
      lambda=(v1-m).getlength();

      // calculate back-facing normal
      n=(v2-v1)/(v3-v1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      endfans();

      // calculate back-facing normal
      n=(v3-v1)/(v4-v1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      endfans();

      // calculate back-facing normal
      n=(v4-v1)/(v2-v1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      endfans();
      }
   else
      {
      // calculate thickness
      lambda=(m-v1).getlength();

      // calculate back-facing normal
      n=(v2-m)/(v3-m);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      endfans();

      // calculate back-facing normal
      n=(v3-m)/(v4-m);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      endfans();

      // calculate back-facing normal
      n=(v4-m)/(v2-m);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      endfans();
      }
   }

// project 4 triangles
void miniproj::proj4tri(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &eye)
   {
   miniv3d m1,m2,d,n;

   double lambda,
          alpha,beta,
          c12,c34;

   // calculate thick vertex (first intersection)
   alpha=intersect(v1,v2-v1,eye,v3-eye,v4-eye,m1);

   // calculate thick vertex (second intersection)
   beta=intersect(v3,v4-v3,eye,v1-eye,v2-eye,m2);

   // linear interpolation at thick vertex
   c12=(1.0f-alpha)*c1+alpha*c2;
   c34=(1.0f-beta)*c3+beta*c4;

   // calculate thick vector
   d=m2-m1;

   // calculate thickness
   lambda=d.getlength();

   // check orientation
   if (d*(m1-eye)<0.0f)
      {
      // calculate back-facing normal
      n=(v1-m1)/(v3-m1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      endfans();

      // calculate back-facing normal
      n=(v3-m1)/(v2-m1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      endfans();

      // calculate back-facing normal
      n=(v2-m1)/(v4-m1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      endfans();

      // calculate back-facing normal
      n=(v4-m1)/(v1-m1);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      endfans();
      }
   else
      {
      // calculate back-facing normal
      n=(v1-m2)/(v3-m2);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      endfans();

      // calculate back-facing normal
      n=(v3-m2)/(v2-m2);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      endfans();

      // calculate back-facing normal
      n=(v2-m2)/(v4-m2);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      endfans();

      // calculate back-facing normal
      n=(v4-m2)/(v1-m2);
      n.normalize();

      // render front-facing thick vertex
      beginfans();
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      endfans();
      }
   }

// project a tetrahedron
// needs to be in front of the near plane
void miniproj::projtri(const miniv3d &v1,const double c1,const dynacoord &a1,
                       const miniv3d &v2,const double c2,const dynacoord &a2,
                       const miniv3d &v3,const double c3,const dynacoord &a3,
                       const miniv3d &v4,const double c4,const dynacoord &a4,
                       const miniv3d &col,
                       const miniv3d &eye,const miniv3d &dir,
                       const double nearp)
   {
   int ff;

   color(col.x,col.y,col.z);

   if (PROJMODE)
      {
      ff=0;

      // calculate front faces
      if (isfront(v4,v1,v2,v3,eye)) ff|=1;
      if (isfront(v3,v1,v4,v2,eye)) ff|=2;
      if (isfront(v1,v2,v4,v3,eye)) ff|=4;
      if (isfront(v2,v3,v4,v1,eye)) ff|=8;

      // pass down shader parameters
      pass(0,v1,a1);
      pass(1,v2,a2);
      pass(2,v3,a3);
      pass(3,v4,a4);

      // determine projection type with either 3 or 4 triangles
      switch (ff)
         {
         case 1: case 14: proj3tri(v4,c4,v1,c1,v2,c2,v3,c3,eye); break;
         case 2: case 13: proj3tri(v3,c3,v1,c1,v2,c2,v4,c4,eye); break;
         case 3: case 12: proj4tri(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
         case 4: case 11: proj3tri(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
         case 5: case 10: proj4tri(v1,c1,v4,c4,v2,c2,v3,c3,eye); break;
         case 6: case  9: proj4tri(v1,c1,v3,c3,v2,c2,v4,c4,eye); break;
         case 7: case  8: proj3tri(v2,c2,v1,c1,v3,c3,v4,c4,eye); break;
         }
      }
   else
      {
      // pass down shader parameters
      pass(v1,v2,v3,v4,a1,a2,a3,a4);

      // calculate slices
      slicetet(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,eye,dir,nearp,DELTA);
      }
   }

// project a tetrahedron
void miniproj::proj(const miniv3d &v1,const double c1,const dynacoord &a1,
                    const miniv3d &v2,const double c2,const dynacoord &a2,
                    const miniv3d &v3,const double c3,const dynacoord &a3,
                    const miniv3d &v4,const double c4,const dynacoord &a4,
                    const unsigned int clipn,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp)
   {
   unsigned int n;

   for (n=clipn; n<CLIP.getsize(); n++)
      if (CLIP[n].ENABLED) break;

   if (n<CLIP.getsize())
      if (CLIP[n].CLIPALL)
         clip(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,n+1,CLIP[n].P,CLIP[n].N,col,eye,dir,nearp);
      else
         clip(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,n+1,CLIP[n].P,CLIP[n].N,col,eye,dir,nearp,CLIP[n].SLOT);
   else
      projtri(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,col,eye,dir,nearp);
   }

// pass coords down to the render pipeline
void miniproj::coords(const double c,const dynacoord &a,const double d)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   texcoord(c,c,d);

   for (i=0; i<size; i++)
      if (a[i].ACTIVE) multitexcoord(i+1,a[i].CRD.x,a[i].CRD.y,a[i].CRD.z);
   }

// extract 1 triangle from tetrahedron
void miniproj::slice1tri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                         const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                         const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                         const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                         const double delta)
   {
   miniv3d p1,p2,p3;
   double pc1,pc2,pc3;
   dynacoord pa1,pa2,pa3;

   p1=(d2*v1+d1*v2)/(d1+d2);
   pc1=(d2*c1+d1*c2)/(d1+d2);
   pa1=(d2*a1+d1*a2)/(d1+d2);
   p2=(d3*v1+d1*v3)/(d1+d3);
   pc2=(d3*c1+d1*c3)/(d1+d3);
   pa2=(d3*a1+d1*a3)/(d1+d3);
   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   beginfans();
   beginfan();
   coords(pc1,pa1,delta);
   fanvertex(p1.x,p1.y,p1.z);
   coords(pc2,pa2,delta);
   fanvertex(p2.x,p2.y,p2.z);
   coords(pc3,pa3,delta);
   fanvertex(p3.x,p3.y,p3.z);
   endfans();
   }

// extract 2 triangles from tetrahedron
void miniproj::slice2tri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                         const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                         const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                         const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                         const double delta)
   {
   miniv3d p1,p2,p3,p4;
   double pc1,pc2,pc3,pc4;
   dynacoord pa1,pa2,pa3,pa4;

   p1=(d3*v1+d1*v3)/(d1+d3);
   pc1=(d3*c1+d1*c3)/(d1+d3);
   pa1=(d3*a1+d1*a3)/(d1+d3);
   p2=(d3*v2+d2*v3)/(d2+d3);
   pc2=(d3*c2+d2*c3)/(d2+d3);
   pa2=(d3*a2+d2*a3)/(d2+d3);
   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);
   p4=(d4*v2+d2*v4)/(d2+d4);
   pc4=(d4*c2+d2*c4)/(d2+d4);
   pa4=(d4*a2+d2*a4)/(d2+d4);

   beginfans();
   beginfan();
   coords(pc1,pa1,delta);
   fanvertex(p1.x,p1.y,p1.z);
   coords(pc2,pa2,delta);
   fanvertex(p2.x,p2.y,p2.z);
   coords(pc4,pa4,delta);
   fanvertex(p4.x,p4.y,p4.z);
   coords(pc3,pa3,delta);
   fanvertex(p3.x,p3.y,p3.z);
   endfans();
   }

// extract slice from tetrahedron
void miniproj::slicetri(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                        const double delta)
   {
   int ff;

   ff=0;

   if (d1<0.0) ff|=1;
   if (d2<0.0) ff|=2;
   if (d3<0.0) ff|=4;
   if (d4<0.0) ff|=8;

   switch (ff)
      {
      // 1 triangle
      case 1: case 14: slice1tri(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),delta); break;
      case 2: case 13: slice1tri(v2,c2,a2,FABS(d2),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),delta); break;
      case 4: case 11: slice1tri(v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),delta); break;
      case 8: case 7: slice1tri(v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),delta); break;

      // 2 triangles
      case 3: slice2tri(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),delta); break;
      case 5: slice2tri(v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),delta); break;
      case 6: slice2tri(v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),delta); break;
      case 9: slice2tri(v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),delta); break;
      case 10: slice2tri(v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),delta); break;
      case 12: slice2tri(v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),delta); break;
      }
   }

// slice tetrahedron from back to front
void miniproj::slicetet(const miniv3d &v1,const double c1,const dynacoord &a1,
                        const miniv3d &v2,const double c2,const dynacoord &a2,
                        const miniv3d &v3,const double c3,const dynacoord &a3,
                        const miniv3d &v4,const double c4,const dynacoord &a4,
                        const miniv3d &eye,const miniv3d &dir,
                        const double nearp,
                        const double delta)
   {
   double d1,d2,d3,d4;
   double dmin,dmax;
   double dsnap;

   if (PLANEMODE)
      {
      d1=(v1-PLANEPNT)*PLANENRM;
      d2=(v2-PLANEPNT)*PLANENRM;
      d3=(v3-PLANEPNT)*PLANENRM;
      d4=(v4-PLANEPNT)*PLANENRM;

      dmin=FMIN(FMIN(d1,d2),FMIN(d3,d4));
      dmax=FMAX(FMAX(d1,d2),FMAX(d3,d4));

      if (dmin<=0.0 && dmax>=0.0)
         slicetri(v1,c1,a1,d1,v2,c2,a2,d2,v3,c3,a3,d3,v4,c4,a4,d4,delta);
      }
   else
      {
      d1=(v1-eye)*dir;
      d2=(v2-eye)*dir;
      d3=(v3-eye)*dir;
      d4=(v4-eye)*dir;

      dmin=FMIN(FMIN(d1,d2),FMIN(d3,d4));
      dmax=FMAX(FMAX(d1,d2),FMAX(d3,d4));

      dsnap=(floor((dmax-nearp)/delta)+0.5)*delta+nearp;

      d1-=dsnap;
      d2-=dsnap;
      d3-=dsnap;
      d4-=dsnap;

      while (dsnap>=dmin && dsnap>nearp)
         {
         slicetri(v1,c1,a1,d1,v2,c2,a2,d2,v3,c3,a3,d3,v4,c4,a4,d4,delta);

         d1+=delta;
         d2+=delta;
         d3+=delta;
         d4+=delta;

         dsnap-=delta;
         }
      }
   }

// break a prism into 3 tetrahedra
// needs to be in front of the near plane
void miniproj::projpri(const miniv3d &v1,const double c1,const dynacoord &a1,
                       const miniv3d &v2,const double c2,const dynacoord &a2,
                       const miniv3d &v3,const double c3,const dynacoord &a3,
                       const miniv3d &v4,const double c4,const dynacoord &a4,
                       const miniv3d &v5,const double c5,const dynacoord &a5,
                       const miniv3d &v6,const double c6,const dynacoord &a6,
                       const unsigned int clipn,
                       const miniv3d &col,
                       const miniv3d &eye,const miniv3d &dir,
                       const double nearp)
   {
   int ff;

   ff=0;

   // calculate front faces
   if (isfront(v1,v2,v3,v4,eye)) ff|=1;
   if (isfront(v6,v3,v4,v5,eye)) ff|=2;

   // determine projection order
   switch (ff)
      {
      case 0:
         proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp);
         proj(v3,c3,a3,v4,c4,a4,v5,c5,a5,v6,c6,a6,clipn,col,eye,dir,nearp);
         proj(v2,c2,a2,v3,c3,a3,v4,c4,a4,v5,c5,a5,clipn,col,eye,dir,nearp);
         break;
      case 1:
         proj(v3,c3,a3,v4,c4,a4,v5,c5,a5,v6,c6,a6,clipn,col,eye,dir,nearp);
         proj(v2,c2,a2,v3,c3,a3,v4,c4,a4,v5,c5,a5,clipn,col,eye,dir,nearp);
         proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp);
         break;
      case 2:
         proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp);
         proj(v2,c2,a2,v3,c3,a3,v4,c4,a4,v5,c5,a5,clipn,col,eye,dir,nearp);
         proj(v3,c3,a3,v4,c4,a4,v5,c5,a5,v6,c6,a6,clipn,col,eye,dir,nearp);
         break;
      case 3:
         proj(v2,c2,a2,v3,c3,a3,v4,c4,a4,v5,c5,a5,clipn,col,eye,dir,nearp);
         proj(v3,c3,a3,v4,c4,a4,v5,c5,a5,v6,c6,a6,clipn,col,eye,dir,nearp);
         proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp);
         break;
      }
   }

// clipping subcase #1A
void miniproj::clip1A(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                      const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                      const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                      const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                      const unsigned int clipn,
                      const miniv3d &col,
                      const miniv3d &eye,const miniv3d &dir,
                      const double nearp)
   {
   miniv3d p1,p2,p3;
   float pc1,pc2,pc3;
   dynacoord pa1,pa2,pa3;

   p1=(d2*v1+d1*v2)/(d1+d2);
   pc1=(d2*c1+d1*c2)/(d1+d2);
   pa1=(d2*a1+d1*a2)/(d1+d2);

   p2=(d3*v1+d1*v3)/(d1+d3);
   pc2=(d3*c1+d1*c3)/(d1+d3);
   pa2=(d3*a1+d1*a3)/(d1+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   projpri(v2,c2,a2,v3,c3,a3,v4,c4,a4,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);
   }

// clipping subcase #1B
void miniproj::clip1B(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                      const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                      const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                      const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                      const unsigned int clipn,
                      const miniv3d &col,
                      const miniv3d &eye,const miniv3d &dir,
                      const double nearp)
   {
   miniv3d p1,p2,p3;
   float pc1,pc2,pc3;
   dynacoord pa1,pa2,pa3;

   p1=(d2*v1+d1*v2)/(d1+d2);
   pc1=(d2*c1+d1*c2)/(d1+d2);
   pa1=(d2*a1+d1*a2)/(d1+d2);

   p2=(d3*v1+d1*v3)/(d1+d3);
   pc2=(d3*c1+d1*c3)/(d1+d3);
   pa2=(d3*a1+d1*a3)/(d1+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   proj(v1,c1,a1,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);
   }

// clipping subcase #2
void miniproj::clip2(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                     const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                     const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                     const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                     const unsigned int clipn,
                     const miniv3d &col,
                     const miniv3d &eye,const miniv3d &dir,
                     const double nearp)
   {
   miniv3d p1,p2,p3,p4;
   float pc1,pc2,pc3,pc4;
   dynacoord pa1,pa2,pa3,pa4;

   p1=(d3*v1+d1*v3)/(d1+d3);
   pc1=(d3*c1+d1*c3)/(d1+d3);
   pa1=(d3*a1+d1*a3)/(d1+d3);

   p2=(d3*v2+d2*v3)/(d2+d3);
   pc2=(d3*c2+d2*c3)/(d2+d3);
   pa2=(d3*a2+d2*a3)/(d2+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   p4=(d4*v2+d2*v4)/(d2+d4);
   pc4=(d4*c2+d2*c4)/(d2+d4);
   pa4=(d4*a2+d2*a4)/(d2+d4);

   projpri(v3,c3,a3,p1,pc1,pa1,p2,pc2,pa2,v4,c4,a4,p3,pc3,pa3,p4,pc4,pa4,clipn,col,eye,dir,nearp);
   }

// clip a tetrahedron
void miniproj::clip(const miniv3d &v1,const double c1,const dynacoord &a1,
                    const miniv3d &v2,const double c2,const dynacoord &a2,
                    const miniv3d &v3,const double c3,const dynacoord &a3,
                    const miniv3d &v4,const double c4,const dynacoord &a4,
                    const unsigned int clipn,
                    const miniv3d &p,const miniv3d &n,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp)
   {
   int ff;

   double d1,d2,d3,d4;

   d1=n*(v1-p);
   d2=n*(v2-p);
   d3=n*(v3-p);
   d4=n*(v4-p);

   ff=0;

   // calculate invisible corners
   if (d1<0.0) ff|=1;
   if (d2<0.0) ff|=2;
   if (d3<0.0) ff|=4;
   if (d4<0.0) ff|=8;

   // determine clipping type
   switch (ff)
      {
      // no clipping
      case 0: proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp); break;

      // one corner needs to be clipped (leaving a prism)
      case 1: clip1A(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 2: clip1A(v2,c2,a2,FABS(d2),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 4: clip1A(v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 8: clip1A(v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp); break;

      // three corners need to be clipped (leaving a tetrahedron)
      case 14: clip1B(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 13: clip1B(v2,c2,a2,FABS(d2),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 11: clip1B(v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 7: clip1B(v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp); break;

      // two corners need to be clipped (leaving a prism)
      case 3: clip2(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 5: clip2(v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 6: clip2(v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp); break;
      case 9: clip2(v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp); break;
      case 10: clip2(v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp); break;
      case 12: clip2(v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),clipn,col,eye,dir,nearp); break;
      }
   }

// deactivate coordinate slot
BOOLINT miniproj::deact(const unsigned int slot,dynacoord &a)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   if (slot<size) a[slot].ACTIVE=FALSE;

   for (i=0; i<size; i++)
      if (a[i].ACTIVE) return(TRUE);

   return(FALSE);
   }

// clipping subcase #1As
void miniproj::clip1As(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                       const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                       const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                       const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                       const unsigned int clipn,
                       const miniv3d &col,
                       const miniv3d &eye,const miniv3d &dir,
                       const double nearp,
                       const unsigned int slot)
   {
   miniv3d p1,p2,p3;
   float pc1,pc2,pc3;
   dynacoord pa1,pa2,pa3;

   BOOLINT active;

   p1=(d2*v1+d1*v2)/(d1+d2);
   pc1=(d2*c1+d1*c2)/(d1+d2);
   pa1=(d2*a1+d1*a2)/(d1+d2);

   p2=(d3*v1+d1*v3)/(d1+d3);
   pc2=(d3*c1+d1*c3)/(d1+d3);
   pa2=(d3*a1+d1*a3)/(d1+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   projpri(v2,c2,a2,v3,c3,a3,v4,c4,a4,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);

   active=deact(slot,pa1);
   active&=deact(slot,pa2);
   active&=deact(slot,pa3);

   if (active) proj(v1,c1,a1,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);
   }

// clipping subcase #1Bs
void miniproj::clip1Bs(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                       const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                       const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                       const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                       const unsigned int clipn,
                       const miniv3d &col,
                       const miniv3d &eye,const miniv3d &dir,
                       const double nearp,
                       const unsigned int slot)
   {
   miniv3d p1,p2,p3;
   float pc1,pc2,pc3;
   dynacoord pa1,pa2,pa3;

   BOOLINT active;

   p1=(d2*v1+d1*v2)/(d1+d2);
   pc1=(d2*c1+d1*c2)/(d1+d2);
   pa1=(d2*a1+d1*a2)/(d1+d2);

   p2=(d3*v1+d1*v3)/(d1+d3);
   pc2=(d3*c1+d1*c3)/(d1+d3);
   pa2=(d3*a1+d1*a3)/(d1+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   proj(v1,c1,a1,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);

   active=deact(slot,pa1);
   active&=deact(slot,pa2);
   active&=deact(slot,pa3);

   if (active) projpri(v2,c2,a2,v3,c3,a3,v4,c4,a4,p1,pc1,pa1,p2,pc2,pa2,p3,pc3,pa3,clipn,col,eye,dir,nearp);
   }

// clipping subcase #2s
void miniproj::clip2s(const miniv3d &v1,const double c1,const dynacoord &a1,const double d1,
                      const miniv3d &v2,const double c2,const dynacoord &a2,const double d2,
                      const miniv3d &v3,const double c3,const dynacoord &a3,const double d3,
                      const miniv3d &v4,const double c4,const dynacoord &a4,const double d4,
                      const unsigned int clipn,
                      const miniv3d &col,
                      const miniv3d &eye,const miniv3d &dir,
                      const double nearp,
                      const unsigned int slot)
   {
   miniv3d p1,p2,p3,p4;
   float pc1,pc2,pc3,pc4;
   dynacoord pa1,pa2,pa3,pa4;

   BOOLINT active;

   p1=(d3*v1+d1*v3)/(d1+d3);
   pc1=(d3*c1+d1*c3)/(d1+d3);
   pa1=(d3*a1+d1*a3)/(d1+d3);

   p2=(d3*v2+d2*v3)/(d2+d3);
   pc2=(d3*c2+d2*c3)/(d2+d3);
   pa2=(d3*a2+d2*a3)/(d2+d3);

   p3=(d4*v1+d1*v4)/(d1+d4);
   pc3=(d4*c1+d1*c4)/(d1+d4);
   pa3=(d4*a1+d1*a4)/(d1+d4);

   p4=(d4*v2+d2*v4)/(d2+d4);
   pc4=(d4*c2+d2*c4)/(d2+d4);
   pa4=(d4*a2+d2*a4)/(d2+d4);

   projpri(v3,c3,a3,p1,pc1,pa1,p2,pc2,pa2,v4,c4,a4,p3,pc3,pa3,p4,pc4,pa4,clipn,col,eye,dir,nearp);

   active=deact(slot,pa1);
   active&=deact(slot,pa2);
   active&=deact(slot,pa3);
   active&=deact(slot,pa4);

   if (active) projpri(v2,c2,a2,p2,pc2,pa2,p4,pc4,pa4,v1,c1,a1,p1,pc1,pa1,p3,pc3,pa3,clipn,col,eye,dir,nearp);
   }

// clip a particular slot of a tetrahedron
void miniproj::clip(const miniv3d &v1,const double c1,const dynacoord &a1,
                    const miniv3d &v2,const double c2,const dynacoord &a2,
                    const miniv3d &v3,const double c3,const dynacoord &a3,
                    const miniv3d &v4,const double c4,const dynacoord &a4,
                    const unsigned int clipn,
                    const miniv3d &p,const miniv3d &n,
                    const miniv3d &col,
                    const miniv3d &eye,const miniv3d &dir,
                    const double nearp,
                    const unsigned int slot)
   {
   int ff;

   double d1,d2,d3,d4;

   d1=n*(v1-p);
   d2=n*(v2-p);
   d3=n*(v3-p);
   d4=n*(v4-p);

   ff=0;

   // calculate invisible corners
   if (d1<0.0) ff|=1;
   if (d2<0.0) ff|=2;
   if (d3<0.0) ff|=4;
   if (d4<0.0) ff|=8;

   // determine clipping type
   switch (ff)
      {
      // no clipping
      case 0: proj(v1,c1,a1,v2,c2,a2,v3,c3,a3,v4,c4,a4,clipn,col,eye,dir,nearp); break;

      // one corner needs to be clipped (leaving a prism)
      case 1: clip1As(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 2: clip1As(v2,c2,a2,FABS(d2),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 4: clip1As(v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 8: clip1As(v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp,slot); break;

      // three corners need to be clipped (leaving a tetrahedron)
      case 14: clip1Bs(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 13: clip1Bs(v2,c2,a2,FABS(d2),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 11: clip1Bs(v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 7: clip1Bs(v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp,slot); break;

      // two corners need to be clipped (leaving a prism)
      case 3: clip2s(v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 5: clip2s(v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 6: clip2s(v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),clipn,col,eye,dir,nearp,slot); break;
      case 9: clip2s(v1,c1,a1,FABS(d1),v4,c4,a4,FABS(d4),v2,c2,a2,FABS(d2),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp,slot); break;
      case 10: clip2s(v2,c2,a2,FABS(d2),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v3,c3,a3,FABS(d3),clipn,col,eye,dir,nearp,slot); break;
      case 12: clip2s(v3,c3,a3,FABS(d3),v4,c4,a4,FABS(d4),v1,c1,a1,FABS(d1),v2,c2,a2,FABS(d2),clipn,col,eye,dir,nearp,slot); break;
      }
   }

// get modelview matrix
void miniproj::getmodelview()
   {
   double mvmtx[16];

   mtxgetmodel(mvmtx);

   MVMATRIX[0].x=mvmtx[0];
   MVMATRIX[1].x=mvmtx[1];
   MVMATRIX[2].x=mvmtx[2];
   MVMATRIX[0].y=mvmtx[4];
   MVMATRIX[1].y=mvmtx[5];
   MVMATRIX[2].y=mvmtx[6];
   MVMATRIX[0].z=mvmtx[8];
   MVMATRIX[1].z=mvmtx[9];
   MVMATRIX[2].z=mvmtx[10];
   MVMATRIX[0].w=mvmtx[12];
   MVMATRIX[1].w=mvmtx[13];
   MVMATRIX[2].w=mvmtx[14];

   inv_mtx(MVINVMTX,MVMATRIX);
   tra_mtx(MVINVTRA,MVINVMTX);
   }

// initialize projection state
void miniproj::initproj(float emi,float rho)
   {
   if (emi<0.0f || rho<0.0f) ERRORMSG();

   // save global emission and opacity
   EMI=emi;
   RHO=rho;

   // set OpenGL state
   initstate();
   disableculling();
   enableRKEblending();
   disableZwriting();

   // get z-clip texture if necessary
   if (ZCLIP && PROJMODE) initzclip();

   // check for projection or slicing mode
   if (PROJMODE)
      {
      // enable projective shaders
      enablevtxshader();
      enablepixshader();

      // pass shader parameters
      setfrgprogpar(0,EMI,RHO,0.0f,0.0f);
      setfrgprogpar(1,0.5f,fexp(1.0f),1.0f,0.0f);
      }
   else
      {
      // get modelview matrix if necessary
      getmodelview();
      }
   }

// de-initialize projection state
void miniproj::exitproj()
   {
   // check for projection or slicing mode
   if (PROJMODE)
      {
      // disable projective shaders
      disablevtxshader();
      disablepixshader();
      }

   // release z-clip texture if necessary
   if (ZCLIP && PROJMODE) exitzclip();

   // reset OpenGL state
   enableZwriting();
   enableBFculling();
   disableblending();
   exitstate();
   }

// enable projection mode
void miniproj::setproj(float delta)
   {
   if (delta>0.0f) PROJMODE=FALSE;
   else PROJMODE=TRUE;

   DELTA=delta;
   }

// get projection mode
BOOLINT miniproj::getproj()
   {return(PROJMODE);}

// enable cutting plane mode
void miniproj::setplane(BOOLINT plane,
                        const miniv3d &p,const miniv3d &n)
   {
   PLANEMODE=plane;

   PLANEPNT=p;
   PLANENRM=n;
   }

// enable z-clipping
void miniproj::setzclip(float nearp,float farp,int zcliptexid)
   {
   if (nearp>0.0f && farp>0.0f && nearp<farp) ZCLIP=TRUE;
   else ZCLIP=FALSE;

   ZNEAR=nearp;
   ZFAR=farp;

   ZCLIPTEXID=zcliptexid;
   }

// get z-clipping
BOOLINT miniproj::getzclip()
   {return(ZCLIP);}

// initialize z-clipping
void miniproj::initzclip()
   {
   static const int zbits=24;
   static const float zscale=1.0f; // use 0.9 for sloppy Intel GMA 950
   static const float zbias=2.0f/((1<<zbits)-1);

   float zfactor;

   texunit(4);

   if (ZCLIPTEXID!=0) ZTEXID=ZCLIPTEXID;
   else ZTEXID=copytexrect(1);

   bindtexrect(ZTEXID);

   texunit(0);

   // calculate factors for z-value reconstruction
   zfactor=(ZNEAR-ZFAR)/(ZNEAR*ZFAR);
   setfrgprogpar(2,zfactor/zscale,(1.0f/ZNEAR-zbias*zfactor)/zscale,0.0f,0.0f);
   }

// de-initialize z-clipping
void miniproj::exitzclip()
   {
   texunit(4);

   bindtexrect(0);

   if (ZCLIPTEXID==0) deletetexrect(ZTEXID);

   texunit(0);
   }

// add clipping plane
void miniproj::addclip(int num,const miniv3d &p,const miniv3d &n)
   {
   CLIP.growsize(num+1);
   CLIP[num]=miniprojclip(p,n);
   }

// add clipping plane to specific slot
void miniproj::addclip(int num,const miniv3d &p,const miniv3d &n,unsigned int slot)
   {
   CLIP.growsize(num+1);
   CLIP[num]=miniprojclip(p,n,slot);
   }

// delete clipping plane
void miniproj::delclip(int num)
   {
   CLIP.growsize(num+1);
   CLIP[num]=miniprojclip();
   }

//! clear clipping planes
void miniproj::clearclip()
   {CLIP.setnull();}

// vertex and fragment programs:

// enable vertex shader
void miniproj::enablevtxshader()
   {
   if (VTXPROG!=NULL)
      {
      if (VTXPROGID==0) VTXPROGID=buildvtxprog(VTXPROG);

      bindvtxprog(VTXPROGID);
      }
   }

// disable vertex shader
void miniproj::disablevtxshader()
   {if (VTXPROG!=NULL) bindvtxprog(0);}

// enable pixel shader
void miniproj::enablepixshader()
   {
   if (FRGPROG!=NULL && FRGPROGZ!=NULL)
      if (!ZCLIP)
         {
         if (FRGPROGID==0) FRGPROGID=buildfrgprog(FRGPROG);

         bindfrgprog(FRGPROGID);
         }
      else
         {
         if (FRGPROGZID==0) FRGPROGZID=buildfrgprog(FRGPROGZ);

         bindfrgprog(FRGPROGZID);
         }
   }

// disable pixel shader
void miniproj::disablepixshader()
   {if (FRGPROG!=NULL && FRGPROGZ!=NULL) bindfrgprog(0);}

// initialize vertex and fragment program setup
void miniproj::setupprogs()
   {
   // vertex shader
   static const char *vtxprog="!!ARBvp1.0 \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM matrix[4]={state.matrix.modelview}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      TEMP vtx,col,nrm,tex,pos,vec,dir,len; \n\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position; \n\
      MOV col,vertex.color; \n\
      MOV nrm,vertex.normal; \n\
      MOV tex,vertex.texcoord[0]; \n\
      ### transform vertex with combined modelview \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      ### write resulting vertex \n\
      MOV result.position,pos; \n\
      ### transform vertex with modelview \n\
      DP4 pos.x,matrix[0],vtx; \n\
      DP4 pos.y,matrix[1],vtx; \n\
      DP4 pos.z,matrix[2],vtx; \n\
      DP4 pos.w,matrix[3],vtx; \n\
      ### transform normal with inverse transpose \n\
      DP4 vec.x,invtra[0],nrm; \n\
      DP4 vec.y,invtra[1],nrm; \n\
      DP4 vec.z,invtra[2],nrm; \n\
      DP4 vec.w,invtra[3],nrm; \n\
      ### normalize view vector \n\
      DP3 len.x,pos,pos; \n\
      RSQ len.x,len.x; \n\
      MUL dir,pos,len.x; \n\
      ### write resulting vertex attributes \n\
      MOV result.color,col; \n\
      MOV result.texcoord[0],vec; \n\
      MOV result.texcoord[1],tex; \n\
      MOV result.texcoord[2],pos; \n\
      MAD result.texcoord[3],dir,tex.z,pos; \n\
      END \n";

   // pixel shader
   static const char *frgprog="!!ARBfp1.0 \n\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      TEMP col,nrm,tex,pos1,pos2,dir,len; \n\
      ### fetch actual fragment \n\
      MOV col,fragment.color; \n\
      MOV nrm,fragment.texcoord[0]; \n\
      MOV tex,fragment.texcoord[1]; \n\
      MOV pos1,fragment.texcoord[2]; \n\
      MOV pos2,fragment.texcoord[3]; \n\
      ### normalize view vector \n\
      DP3 len.x,pos1,pos1; \n\
      RSQ len.x,len.x; \n\
      MUL dir,pos1,len.x; \n\
      ### perspective reprojection \n\
      SUB pos2,pos2,pos1; \n\
      DP3 len.x,nrm,pos2; \n\
      DP3 len.y,nrm,dir; \n\
      RCP len.y,len.y; \n\
      MUL len.x,len.x,len.y; \n\
      MAD pos2,dir,len.x,pos1; \n\
      ### calculate optical depth \n\
      ADD len.y,tex.x,tex.y; \n\
      MUL len.y,len.y,c1.x; \n\
      MUL len.x,len.x,len.y; \n\
      MUL len.x,len.x,c0.y; \n\
      ### calculate absorption \n\
      POW len.x,c1.y,-len.x; \n\
      SUB len.x,c1.z,len.x; \n\
      ### pre-multiply alpha \n\
      MUL col.xyz,col,len.x; \n\
      ### write resulting fragment \n\
      MUL result.color.xyz,col,c0.x; \n\
      MOV result.color.w,len.x; \n\
      END \n";

   // pixel shader with z-clipping
   static const char *frgprogz="!!ARBfp1.0 \n\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      PARAM c2=program.env[2]; \n\
      TEMP col,nrm,tex,pos1,pos2,pos3,zval,dir,len; \n\
      ### fetch actual fragment \n\
      MOV col,fragment.color; \n\
      MOV nrm,fragment.texcoord[0]; \n\
      MOV tex,fragment.texcoord[1]; \n\
      MOV pos1,fragment.texcoord[2]; \n\
      MOV pos2,fragment.texcoord[3]; \n\
      TEX zval,fragment.position,texture[4],RECT; \n\
      ### reconstruct z-value \n\
      MAD zval.z,zval.z,c2.x,c2.y; \n\
      RCP zval.z,zval.z; \n\
      ### normalize view vector \n\
      DP3 len.x,pos1,pos1; \n\
      RSQ len.x,len.x; \n\
      MUL dir,pos1,len.x; \n\
      ### perspective reprojection \n\
      SUB pos2,pos2,pos1; \n\
      DP3 len.x,nrm,pos2; \n\
      DP3 len.y,nrm,dir; \n\
      RCP len.y,len.y; \n\
      MUL len.x,len.x,len.y; \n\
      MAD pos2,dir,len.x,pos1; \n\
      ### calculate hit point \n\
      RCP len.z,-dir.z; \n\
      MUL zval.z,zval.z,len.z; \n\
      MUL pos3,dir,zval.z; \n\
      ### clip against hit point \n\
      DP3 len.x,dir,pos2; \n\
      SUB len.x,len.x,zval.z; \n\
      CMP pos2,len.x,pos2,pos3; \n\
      DP3 len.x,dir,pos1; \n\
      SUB len.x,len.x,zval.z; \n\
      CMP pos1,len.x,pos1,pos3; \n\
      ### calculate thickness \n\
      SUB pos2,pos2,pos1; \n\
      DP3 len.x,dir,pos2; \n\
      ### calculate optical depth \n\
      ADD len.y,tex.x,tex.y; \n\
      MUL len.y,len.y,c1.x; \n\
      MUL len.x,len.x,len.y; \n\
      MUL len.x,len.x,c0.y; \n\
      ### calculate absorption \n\
      POW len.x,c1.y,-len.x; \n\
      SUB len.x,c1.z,len.x; \n\
      ### pre-multiply alpha \n\
      MUL col.xyz,col,len.x; \n\
      ### write resulting fragment \n\
      MUL result.color.xyz,col,c0.x; \n\
      MOV result.color.w,len.x; \n\
      END \n";

   VTXPROG=strdup(vtxprog);
   VTXPROGID=0;

   FRGPROG=strdup(frgprog);
   FRGPROGID=0;

   FRGPROGZ=strdup(frgprogz);
   FRGPROGZID=0;
   }

// delete vertex and fragment program
void miniproj::deleteprogs()
   {
   free(VTXPROG);
   free(FRGPROG);
   free(FRGPROGZ);

   if (VTXPROGID!=0) deletevtxprog(VTXPROGID);
   if (FRGPROGID!=0) deletefrgprog(FRGPROGID);
   if (FRGPROGZID!=0) deletefrgprog(FRGPROGZID);
   }
