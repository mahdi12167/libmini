// (c) by Stefan Roettger

#include "miniOGL.h"

#include "miniproj.h"

// default constructor
miniproj::miniproj() {}

// destructor
miniproj::~miniproj() {}

// calculate whether or not a triangle is front- or back-facing
inline BOOLINT miniproj::isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e)
   {
   miniv3d n;

   n=(v2-v1)/(v3-v1);

   return((n*(v1-e)>0.0)?n*(p-v1)<0.0:n*(p-v1)>0.0);
   }

// calculate the intersection of a ray with a plane
inline double miniproj::intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m)
   {
   miniv3d n;

   double lambda;

   n=d1/d2;

   lambda=n*d;
   if (lambda!=0.0) lambda=n*(o-p)/lambda;

   m=p+lambda*d;

   return(lambda);
   }

// draw a triangle fan with 3 triangles
inline void miniproj::draw3fan(const miniv3d &v1,const double c1,
                               const miniv3d &v2,const double c2,
                               const miniv3d &v3,const double c3,
                               const miniv3d &v4,const double c4,
                               const miniv3d &col,const double rho,
                               const miniv3d &eye)
   {
   miniv3d d1,d2,m,n;

   double lambda,
          w1,w2,w3,
          c234;

   d1=v3-v2;
   d2=v4-v2;

   // calculate thick vertex
   lambda=intersect(v1,v1-eye,v2,d1,d2,m);

   n=d1/d2;

   n.x=FABS(n.x);
   n.y=FABS(n.y);
   n.z=FABS(n.z);

   // barycentric interpolation
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

   beginfan();
   color(col.x,col.y,col.z);

   // render thick vertex
   if (lambda<0.0)
      {
      lambda=(v1-m).getlength();
      texcoord(c1,c234,lambda*rho);
      fanvertex(0.5*(v1.x+m.x),0.5*(v1.y+m.y),0.5*(v1.z+m.z));
      }
   else
      {
      lambda=(v1-m).getlength();
      texcoord(c234,c1,lambda*rho);
      fanvertex(0.5*(v1.x+m.x),0.5*(v1.y+m.y),0.5*(v1.z+m.z));
      }

   // render silhouette vertices
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   texcoord(c3,c3,0.0f);
   fanvertex(v3.x,v3.y,v3.z);
   texcoord(c4,c4,0.0f);
   fanvertex(v4.x,v4.y,v4.z);
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   }

// draw a triangle fan with 4 triangles
void miniproj::draw4fan(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &col,const double rho,
                        const miniv3d &eye)
   {
   miniv3d m1,m2,d;

   double lambda,
          alpha,beta,
          c12,c34;

   // calculate thick vertex (first intersection)
   alpha=intersect(v1,v2-v1,eye,v3-eye,v4-eye,m1);

   // calculate thick vertex (second intersection)
   beta=intersect(v3,v4-v3,eye,v1-eye,v2-eye,m2);

   c12=(1.0f-alpha)*c1+alpha*c2;
   c34=(1.0f-beta)*c3+beta*c4;

   d=m2-m1;

   lambda=d.getlength();

   beginfan();
   color(col.x,col.y,col.z);

   // render thick vertex
   if (d*(m1-eye)<0.0f)
      {
      texcoord(c12,c34,lambda*rho);
      fanvertex(0.5*(m1.x+m2.x),0.5*(m1.y+m2.y),0.5*(m1.z+m2.z));
      }
   else
      {
      texcoord(c34,c12,lambda*rho);
      fanvertex(0.5*(m1.x+m2.x),0.5*(m1.y+m2.y),0.5*(m1.z+m2.z));
      }

   // render silhouette vertices
   texcoord(c1,c1,0.0f);
   fanvertex(v1.x,v1.y,v1.z);
   texcoord(c3,c3,0.0f);
   fanvertex(v3.x,v3.y,v3.z);
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   texcoord(c4,c4,0.0f);
   fanvertex(v4.x,v4.y,v4.z);
   texcoord(c1,c1,0.0f);
   fanvertex(v1.x,v1.y,v1.z);
   }

// project a tetrahedron
// needs to be in front of the near plane
void miniproj::proj(const miniv3d &v1,const double c1,
                    const miniv3d &v2,const double c2,
                    const miniv3d &v3,const double c3,
                    const miniv3d &v4,const double c4,
                    const miniv3d &col,const double rho,
                    const miniv3d &eye)
   {
   int ff;

   if (c1==0.0 && c2==0.0 && c3==0.0 && c4==0.0) return;

   ff=0;

   if (isfront(v4,v1,v2,v3,eye)) ff|=1;
   if (isfront(v3,v1,v4,v2,eye)) ff|=2;
   if (isfront(v1,v2,v4,v3,eye)) ff|=4;
   if (isfront(v2,v3,v4,v1,eye)) ff|=8;

   switch (ff)
      {
      case 1: case 14: draw3fan(v4,c4,v1,c1,v2,c2,v3,c3,col,rho,eye); break;
      case 2: case 13: draw3fan(v3,c3,v1,c1,v2,c2,v4,c4,col,rho,eye); break;
      case 3: case 12: draw4fan(v1,c1,v2,c2,v3,c3,v4,c4,col,rho,eye); break;
      case 4: case 11: draw3fan(v1,c1,v2,c2,v3,c3,v4,c4,col,rho,eye); break;
      case 5: case 10: draw4fan(v1,c1,v4,c4,v2,c2,v3,c3,col,rho,eye); break;
      case 6: case  9: draw4fan(v1,c1,v3,c3,v2,c2,v4,c4,col,rho,eye); break;
      case 7: case  8: draw3fan(v2,c2,v1,c1,v3,c3,v4,c4,col,rho,eye); break;
      }
   }
