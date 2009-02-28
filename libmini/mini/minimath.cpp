// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"

namespace minimath {

const miniv3d mtx_zero3[3]={miniv3d(0.0,0.0,0.0),
                            miniv3d(0.0,0.0,0.0),
                            miniv3d(0.0,0.0,0.0)};

const miniv4d mtx_zero4[3]={miniv4d(0.0,0.0,0.0),
                            miniv4d(0.0,0.0,0.0),
                            miniv4d(0.0,0.0,0.0)};

const miniv3d mtx_one3[3]={miniv3d(1.0,0.0,0.0),
                           miniv3d(0.0,1.0,0.0),
                           miniv3d(0.0,0.0,1.0)};

const miniv4d mtx_one4[3]={miniv4d(1.0,0.0,0.0),
                           miniv4d(0.0,1.0,0.0),
                           miniv4d(0.0,0.0,1.0)};

const miniv3d mtx_neg_one3[3]={miniv3d(-1.0,0.0,0.0),
                               miniv3d(0.0,-1.0,0.0),
                               miniv3d(0.0,0.0,-1.0)};

const miniv4d mtx_neg_one4[3]={miniv4d(-1.0,0.0,0.0),
                               miniv4d(0.0,-1.0,0.0),
                               miniv4d(0.0,0.0,-1.0)};

// greatest common divisor
unsigned int gcd(unsigned int a,unsigned int b)
   {
   unsigned int c;

   while (b>0)
      {
      c=a%b;
      a=b;
      b=c;
      }

   return(a);
   }

// lowest common multiple
unsigned int lcm(unsigned int a,unsigned int b)
   {return(a*b/gcd(a,b));}

// matrix copying:

void cpy_mtx(miniv3d cpy[3],miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv3d cpy[3],miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}

void cpy_mtx(miniv3d cpy[3],const miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],const miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],const miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv3d cpy[3],const miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}

// multiply two 3x3 matrices
void mlt_mtx(miniv3d mtx[3],const miniv3d mtx1[3],const miniv3d mtx2[3])
   {
   int i;

   miniv3d m[3];

   for (i=0; i<3; i++)
      {
      m[i].x=mtx1[i].x*mtx2[0].x+mtx1[i].y*mtx2[1].x+mtx1[i].z*mtx2[2].x;
      m[i].y=mtx1[i].x*mtx2[0].y+mtx1[i].y*mtx2[1].y+mtx1[i].z*mtx2[2].y;
      m[i].z=mtx1[i].x*mtx2[0].z+mtx1[i].y*mtx2[1].z+mtx1[i].z*mtx2[2].z;
      }

   cpy_mtx(mtx,m);
   }

// multiply two 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3])
   {
   int i;

   miniv4d m[3];

   for (i=0; i<3; i++)
      {
      // fourth row is assumed to be (0,0,0,1)
      m[i].x=mtx1[i].x*mtx2[0].x+mtx1[i].y*mtx2[1].x+mtx1[i].z*mtx2[2].x;
      m[i].y=mtx1[i].x*mtx2[0].y+mtx1[i].y*mtx2[1].y+mtx1[i].z*mtx2[2].y;
      m[i].z=mtx1[i].x*mtx2[0].z+mtx1[i].y*mtx2[1].z+mtx1[i].z*mtx2[2].z;
      m[i].w=mtx1[i].x*mtx2[0].w+mtx1[i].y*mtx2[1].w+mtx1[i].z*mtx2[2].w+mtx1[i].w;
      }

   cpy_mtx(mtx,m);
   }

// multiply three 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx2,mtx3);
   mlt_mtx(mtx,mtx1,m);
   }

// multiply four 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx3,mtx4);
   mlt_mtx(mtx,mtx1,mtx2,m);
   }

// multiply five 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx3,mtx4,mtx5);
   mlt_mtx(mtx,mtx1,mtx2,m);
   }

// multiply six 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx4,mtx5,mtx6);
   mlt_mtx(mtx,mtx1,mtx2,mtx3,m);
   }

// multiply seven 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx4,mtx5,mtx6,mtx7);
   mlt_mtx(mtx,mtx1,mtx2,mtx3,m);
   }

// multiply eight 4x3 matrices
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3],const miniv4d mtx8[3])
   {
   miniv4d m[3];

   mlt_mtx(m,mtx5,mtx6,mtx7,mtx8);
   mlt_mtx(mtx,mtx1,mtx2,mtx3,mtx4,m);
   }

// calculate determinant of 3x3 matrix
double det_mtx(const miniv3d mtx[3])
   {
   return(mtx[0].x*(mtx[1].y*mtx[2].z-mtx[2].y*mtx[1].z)+
          mtx[0].y*(mtx[2].x*mtx[1].z-mtx[1].x*mtx[2].z)+
          mtx[0].z*(mtx[1].x*mtx[2].y-mtx[2].x*mtx[1].y));
   }

// calculate determinant of 4x3 matrix
double det_mtx(const miniv4d mtx[3])
   {
   return(mtx[0].x*(mtx[1].y*mtx[2].z-mtx[2].y*mtx[1].z)+
          mtx[0].y*(mtx[2].x*mtx[1].z-mtx[1].x*mtx[2].z)+
          mtx[0].z*(mtx[1].x*mtx[2].y-mtx[2].x*mtx[1].y));
   }

// invert a 3x3 matrix
void inv_mtx(miniv3d inv[3],const miniv3d mtx[3])
   {
   double det;
   miniv3d m[3];

   // calculate determinant
   det=det_mtx(mtx);

   // check determinant
   if (det==0.0) inv[0]=inv[1]=inv[2]=miniv3d(0.0);
   else
      {
      det=1.0/det;

      cpy_mtx(m,mtx);

      inv[0].x=det*(m[1].y*m[2].z-m[2].y*m[1].z);
      inv[1].x=det*(m[2].x*m[1].z-m[1].x*m[2].z);
      inv[2].x=det*(m[1].x*m[2].y-m[2].x*m[1].y);
      inv[0].y=det*(m[2].y*m[0].z-m[0].y*m[2].z);
      inv[1].y=det*(m[0].x*m[2].z-m[2].x*m[0].z);
      inv[2].y=det*(m[2].x*m[0].y-m[0].x*m[2].y);
      inv[0].z=det*(m[0].y*m[1].z-m[1].y*m[0].z);
      inv[1].z=det*(m[1].x*m[0].z-m[0].x*m[1].z);
      inv[2].z=det*(m[0].x*m[1].y-m[1].x*m[0].y);
      }
   }

// invert a 4x3 matrix
void inv_mtx(miniv3d inv[3],const miniv4d mtx[3])
   {
   miniv3d m[3];

   cpy_mtx(m,mtx);
   inv_mtx(inv,m);
   }

// invert a 4x3 matrix
void inv_mtx(miniv4d inv[3],const miniv4d mtx[3])
   {
   miniv3d m[3];
   miniv4d m1[3],m2[3];

   // extract 3x3 sub-matrix
   cpy_mtx(m,mtx);

   // it is sufficient to invert the 3x3 sub-matrix
   inv_mtx(m,m);

   // decompose 4x3 inversion into matrix #1 = inverted 3x3 sub-matrix
   cpy_mtx(m1,m);

   // decompose 4x3 inversion into matrix #2 = inverted 1x3 sub-matrix = negated vector
   m2[0]=miniv4d(1.0,0.0,0.0,-mtx[0].w);
   m2[1]=miniv4d(0.0,1.0,0.0,-mtx[1].w);
   m2[2]=miniv4d(0.0,0.0,1.0,-mtx[2].w);

   // compose the inverted matrix
   mlt_mtx(inv,m1,m2);
   }

// transpose a 3x3 matrix
void tra_mtx(miniv3d tra[3],const miniv3d mtx[3])
   {
   miniv3d m[3];

   // compute transposition
   m[0]=miniv3d(mtx[0].x,mtx[1].x,mtx[2].x);
   m[1]=miniv3d(mtx[0].y,mtx[1].y,mtx[2].y);
   m[2]=miniv3d(mtx[0].z,mtx[1].z,mtx[2].z);

   cpy_mtx(tra,m);
   }

// transpose a 4x3 matrix
void tra_mtx(miniv3d tra[3],const miniv4d mtx[3])
   {
   miniv3d m[3];

   // compute transposition
   m[0]=miniv3d(mtx[0].x,mtx[1].x,mtx[2].x);
   m[1]=miniv3d(mtx[0].y,mtx[1].y,mtx[2].y);
   m[2]=miniv3d(mtx[0].z,mtx[1].z,mtx[2].z);

   cpy_mtx(tra,m);
   }

// transpose a 4x3 matrix
void tra_mtx(miniv4d tra[3],const miniv4d mtx[3])
   {
   miniv3d m[3];

   // compute transposition
   m[0]=miniv3d(mtx[0].x,mtx[1].x,mtx[2].x);
   m[1]=miniv3d(mtx[0].y,mtx[1].y,mtx[2].y);
   m[2]=miniv3d(mtx[0].z,mtx[1].z,mtx[2].z);

   cpy_mtx(tra,m);
   }

// calculate a rotation matrix
void rot_mtx(miniv3d rot[3],const miniv3d &v1,const miniv3d &v2)
   {
   miniv3d vn1,vn2;

   double dot;

   miniv3d a;
   double s,c;
   double ax2,ay2,az2;

   vn1=v1;
   vn2=v2;

   vn1.normalize();
   vn2.normalize();

   dot=vn1*vn2;

   // co-linear
   if (dot>0.999) cpy_mtx(rot,minimath::mtx_one3);
   // negative colinear
   else if (dot<-0.999) cpy_mtx(rot,minimath::mtx_neg_one3);
   // not colinear
   else
      {
      // calculate rotation axis
      a=vn1/vn2;
      s=a.normalize();
      c=sqrt(1.0-s*s);

      // calculate squares
      ax2=a.x*a.x;
      ay2=a.y*a.y;
      az2=a.z*a.z;

      rot[0]=miniv3d(ax2+(1.0-ax2)*c,a.x*a.y*(1.0-c)-a.z*s,a.x*a.z*(1.0-c)+a.y*s);
      rot[1]=miniv3d(a.x*a.y*(1.0-c)+a.z*s,ay2+(1.0-ay2)*c,a.y*a.z*(1.0-c)-a.x*s);
      rot[2]=miniv3d(a.x*a.z*(1.0-c)-a.y*s,a.y*a.z*(1.0-c)+a.x*s,az2+(1.0-az2)*c);
      }
   }

}
