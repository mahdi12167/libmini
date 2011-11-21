// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"

namespace minimath {

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

// matrix initialization to identity
void set_mtx(miniv3d mtx[3]) {mtx[0]=miniv3d(1,0,0); mtx[1]=miniv3d(0,1,0); mtx[2]=miniv3d(0,0,1);}
void set_mtx(miniv4d mtx[3]) {mtx[0]=miniv4d(1,0,0,0); mtx[1]=miniv4d(0,1,0,0); mtx[2]=miniv4d(0,0,1,0);}

// matrix copying:

void cpy_mtx(miniv3d cpy[3],miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv3d cpy[3],miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}

void cpy_mtx(miniv3d cpy[3],const miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],const miniv3d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv4d cpy[3],const miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}
void cpy_mtx(miniv3d cpy[3],const miniv4d mtx[3]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2];}

void cpy_mtx4(miniv4d cpy[4],const miniv4d mtx[4]) {cpy[0]=mtx[0]; cpy[1]=mtx[1]; cpy[2]=mtx[2]; cpy[3]=mtx[3];}

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

// multiply two 4x4 matrices
void mlt_mtx4(miniv4d mtx[4],const miniv4d mtx1[4],const miniv4d mtx2[4])
   {
   int i;

   miniv4d m[4];

   for (i=0; i<4; i++)
      {
      m[i].x=mtx1[i].x*mtx2[0].x+mtx1[i].y*mtx2[1].x+mtx1[i].z*mtx2[2].x+mtx1[i].w*mtx2[3].x;
      m[i].y=mtx1[i].x*mtx2[0].y+mtx1[i].y*mtx2[1].y+mtx1[i].z*mtx2[2].y+mtx1[i].w*mtx2[3].y;
      m[i].z=mtx1[i].x*mtx2[0].z+mtx1[i].y*mtx2[1].z+mtx1[i].z*mtx2[2].z+mtx1[i].w*mtx2[3].z;
      m[i].w=mtx1[i].x*mtx2[0].w+mtx1[i].y*mtx2[1].w+mtx1[i].z*mtx2[2].w+mtx1[i].w*mtx2[3].w;
      }

   cpy_mtx4(mtx,m);
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

// calculate determinant of 4x4 matrix
double det_mtx4(const miniv4d mtx[4])
   {
   return(mtx[0].w*mtx[1].z*mtx[2].y*mtx[3].x-
          mtx[0].z*mtx[1].w*mtx[2].y*mtx[3].x-
          mtx[0].w*mtx[1].y*mtx[2].z*mtx[3].x+
          mtx[0].y*mtx[1].w*mtx[2].z*mtx[3].x+
          mtx[0].z*mtx[1].y*mtx[2].w*mtx[3].x-
          mtx[0].y*mtx[1].z*mtx[2].w*mtx[3].x-
          mtx[0].w*mtx[1].z*mtx[2].x*mtx[3].y+
          mtx[0].z*mtx[1].w*mtx[2].x*mtx[3].y+
          mtx[0].w*mtx[1].x*mtx[2].z*mtx[3].y-
          mtx[0].x*mtx[1].w*mtx[2].z*mtx[3].y-
          mtx[0].z*mtx[1].x*mtx[2].w*mtx[3].y+
          mtx[0].x*mtx[1].z*mtx[2].w*mtx[3].y+
          mtx[0].w*mtx[1].y*mtx[2].x*mtx[3].z-
          mtx[0].y*mtx[1].w*mtx[2].x*mtx[3].z-
          mtx[0].w*mtx[1].x*mtx[2].y*mtx[3].z+
          mtx[0].x*mtx[1].w*mtx[2].y*mtx[3].z+
          mtx[0].y*mtx[1].x*mtx[2].w*mtx[3].z-
          mtx[0].x*mtx[1].y*mtx[2].w*mtx[3].z-
          mtx[0].z*mtx[1].y*mtx[2].x*mtx[3].w+
          mtx[0].y*mtx[1].z*mtx[2].x*mtx[3].w+
          mtx[0].z*mtx[1].x*mtx[2].y*mtx[3].w-
          mtx[0].x*mtx[1].z*mtx[2].y*mtx[3].w-
          mtx[0].y*mtx[1].x*mtx[2].z*mtx[3].w+
          mtx[0].x*mtx[1].y*mtx[2].z*mtx[3].w);
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

// invert a 4x4 matrix
void inv_mtx4(miniv4d inv[4],const miniv4d mtx[4])
   {
   double det;
   miniv4d m[4];

   // calculate determinant
   det=det_mtx4(mtx);

   // check determinant
   if (det==0.0) inv[0]=inv[1]=inv[2]=inv[3]=miniv4d(0.0);
   else
      {
      det=1.0/det;

      cpy_mtx4(m,mtx);

      inv[0].x=det*(m[1].z*m[2].w*m[3].y-m[1].w*m[2].z*m[3].y+m[1].w*m[2].y*m[3].z-m[1].y*m[2].w*m[3].z-m[1].z*m[2].y*m[3].w+m[1].y*m[2].z*m[3].w);
      inv[0].y=det*(m[0].w*m[2].z*m[3].y-m[0].z*m[2].w*m[3].y-m[0].w*m[2].y*m[3].z+m[0].y*m[2].w*m[3].z+m[0].z*m[2].y*m[3].w-m[0].y*m[2].z*m[3].w);
      inv[0].z=det*(m[0].z*m[1].w*m[3].y-m[0].w*m[1].z*m[3].y+m[0].w*m[1].y*m[3].z-m[0].y*m[1].w*m[3].z-m[0].z*m[1].y*m[3].w+m[0].y*m[1].z*m[3].w);
      inv[0].w=det*(m[0].w*m[1].z*m[2].y-m[0].z*m[1].w*m[2].y-m[0].w*m[1].y*m[2].z+m[0].y*m[1].w*m[2].z+m[0].z*m[1].y*m[2].w-m[0].y*m[1].z*m[2].w);
      inv[1].x=det*(m[1].w*m[2].z*m[3].x-m[1].z*m[2].w*m[3].x-m[1].w*m[2].x*m[3].z+m[1].x*m[2].w*m[3].z+m[1].z*m[2].x*m[3].w-m[1].x*m[2].z*m[3].w);
      inv[1].y=det*(m[0].z*m[2].w*m[3].x-m[0].w*m[2].z*m[3].x+m[0].w*m[2].x*m[3].z-m[0].x*m[2].w*m[3].z-m[0].z*m[2].x*m[3].w+m[0].x*m[2].z*m[3].w);
      inv[1].z=det*(m[0].w*m[1].z*m[3].x-m[0].z*m[1].w*m[3].x-m[0].w*m[1].x*m[3].z+m[0].x*m[1].w*m[3].z+m[0].z*m[1].x*m[3].w-m[0].x*m[1].z*m[3].w);
      inv[1].w=det*(m[0].z*m[1].w*m[2].x-m[0].w*m[1].z*m[2].x+m[0].w*m[1].x*m[2].z-m[0].x*m[1].w*m[2].z-m[0].z*m[1].x*m[2].w+m[0].x*m[1].z*m[2].w);
      inv[2].x=det*(m[1].y*m[2].w*m[3].x-m[1].w*m[2].y*m[3].x+m[1].w*m[2].x*m[3].y-m[1].x*m[2].w*m[3].y-m[1].y*m[2].x*m[3].w+m[1].x*m[2].y*m[3].w);
      inv[2].y=det*(m[0].w*m[2].y*m[3].x-m[0].y*m[2].w*m[3].x-m[0].w*m[2].x*m[3].y+m[0].x*m[2].w*m[3].y+m[0].y*m[2].x*m[3].w-m[0].x*m[2].y*m[3].w);
      inv[2].z=det*(m[0].y*m[1].w*m[3].x-m[0].w*m[1].y*m[3].x+m[0].w*m[1].x*m[3].y-m[0].x*m[1].w*m[3].y-m[0].y*m[1].x*m[3].w+m[0].x*m[1].y*m[3].w);
      inv[2].w=det*(m[0].w*m[1].y*m[2].x-m[0].y*m[1].w*m[2].x-m[0].w*m[1].x*m[2].y+m[0].x*m[1].w*m[2].y+m[0].y*m[1].x*m[2].w-m[0].x*m[1].y*m[2].w);
      inv[3].x=det*(m[1].z*m[2].y*m[3].x-m[1].y*m[2].z*m[3].x-m[1].z*m[2].x*m[3].y+m[1].x*m[2].z*m[3].y+m[1].y*m[2].x*m[3].z-m[1].x*m[2].y*m[3].z);
      inv[3].y=det*(m[0].y*m[2].z*m[3].x-m[0].z*m[2].y*m[3].x+m[0].z*m[2].x*m[3].y-m[0].x*m[2].z*m[3].y-m[0].y*m[2].x*m[3].z+m[0].x*m[2].y*m[3].z);
      inv[3].z=det*(m[0].z*m[1].y*m[3].x-m[0].y*m[1].z*m[3].x-m[0].z*m[1].x*m[3].y+m[0].x*m[1].z*m[3].y+m[0].y*m[1].x*m[3].z-m[0].x*m[1].y*m[3].z);
      inv[3].w=det*(m[0].y*m[1].z*m[2].x-m[0].z*m[1].y*m[2].x+m[0].z*m[1].x*m[2].y-m[0].x*m[1].z*m[2].y-m[0].y*m[1].x*m[2].z+m[0].x*m[1].y*m[2].z);
      }
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

// transpose a 4x4 matrix
void tra_mtx4(miniv4d tra[4],const miniv4d mtx[4])
   {
   miniv4d m[4];

   // compute transposition
   m[0]=miniv4d(mtx[0].x,mtx[1].x,mtx[2].x,mtx[3].x);
   m[1]=miniv4d(mtx[0].y,mtx[1].y,mtx[2].y,mtx[3].y);
   m[2]=miniv4d(mtx[0].z,mtx[1].z,mtx[2].z,mtx[3].z);
   m[3]=miniv4d(mtx[0].w,mtx[1].w,mtx[2].w,mtx[3].w);

   cpy_mtx4(tra,m);
   }

// rotate counter-clockwise in right-handed coordinate system
void rot_mtx(miniv3d rot[3],double delta,const miniv3d &axis)
   {
   double x=axis.x;
   double y=axis.y;
   double z=axis.z;

   double s=sin(delta/180.0*PI);
   double c=cos(delta/180.0*PI);
   double c1=1.0-c;

   rot[0]=miniv3d(x*x*c1+c,   x*y*c1-z*s, x*z*c1+y*s);
   rot[1]=miniv3d(y*x*c1+z*s, y*y*c1+c,   y*z*c1-x*s);
   rot[2]=miniv3d(z*x*c1-y*s, z*y*c1+x*s, z*z*c1+c);
   }

// calculate a rotation matrix
void rot_mtx(miniv3d rot[3],const miniv3d &v1,const miniv3d &v2)
   {
   miniv3d vn1,vn2;

   double dot;

   miniv3d a;
   double s,c;
   double ax2,ay2,az2;

   static const miniv3d mtx_one[3]={miniv3d(1.0,0.0,0.0),
                                    miniv3d(0.0,1.0,0.0),
                                    miniv3d(0.0,0.0,1.0)};

   static const miniv3d mtx_neg_one[3]={miniv3d(-1.0,0.0,0.0),
                                        miniv3d(0.0,-1.0,0.0),
                                        miniv3d(0.0,0.0,-1.0)};

   vn1=v1;
   vn2=v2;

   vn1.normalize();
   vn2.normalize();

   dot=vn1*vn2;

   // co-linear
   if (dot>0.999) cpy_mtx(rot,mtx_one);
   // negative colinear
   else if (dot<-0.999) cpy_mtx(rot,mtx_neg_one);
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

      rot[0]=miniv3d(ax2+(1.0-ax2)*c,       a.x*a.y*(1.0-c)-a.z*s, a.x*a.z*(1.0-c)+a.y*s);
      rot[1]=miniv3d(a.x*a.y*(1.0-c)+a.z*s, ay2+(1.0-ay2)*c,       a.y*a.z*(1.0-c)-a.x*s);
      rot[2]=miniv3d(a.x*a.z*(1.0-c)-a.y*s, a.y*a.z*(1.0-c)+a.x*s, az2+(1.0-az2)*c);
      }
   }

// merge two spheres
void merge_spheres(miniv3d &center0,double &radius0,
                   const miniv3d &center1,const double radius1)
   {
   miniv3d d=center1-center0;
   double r=d.normalize();

   if (radius1>radius0+r)
      {
      center0=center1;
      radius0=radius1;
      }
   else if (radius1+r>radius0)
      {
      miniv3d a=center1+d*radius1;
      miniv3d b=center0-d*radius0;

      center0=(a+b)/2.0;
      radius0=(a-b).getlength()/2.0;
      }
   }

// ray/unitsphere intersection
double intersect_ray_unitsphere(miniv3d p,miniv3d d)
   {
   double a,b,c;
   double s,r;

   double t1,t2;

   a=2*d*d;
   b=2*p*d;
   c=2*(p*p-1.0);

   r=b*b-a*c;

   if (r<0.0) return(MAXFLOAT);

   s=sqrt(r);

   t1=(-b+s)/a;
   t2=(-b-s)/a;

   if (t1<0.0)
      if (t2<0.0) return(MAXFLOAT);
      else return(t2);
   else
      if (t2<0.0) return(t1);
      else if (t1<t2) return(t1);
      else return(t2);
   }

// ray/ellipsoid intersection
double intersect_ray_ellipsoid(miniv3d p,miniv3d d,
                               miniv3d o,double r1,double r2,double r3)
   {
   p-=o;

   p.x/=r1;
   p.y/=r2;
   p.z/=r3;

   d.x/=r1;
   d.y/=r2;
   d.z/=r3;

   return(intersect_ray_unitsphere(p,d));
   }

// ray/plane intersection
double intersect_ray_plane(miniv3d p,miniv3d d,
                           miniv3d o,miniv3d n)
   {
   double c;

   static const double epsilon=1E-10;

   c=d*n;

   if (c>=-epsilon) return(MAXFLOAT);

   return(n*(o-p)/c);
   }

// line/plane intersection
double intersect_line_plane(miniv3d p,miniv3d d,
                            miniv3d o,miniv3d n)
   {
   double c;

   static const double epsilon=1E-10;

   c=d*n;

   if (dabs(c)<=epsilon) return(MAXFLOAT);

   return(n*(o-p)/c);
   }

// Moeller-Trumbore ray/triangle intersection
int intersect_ray_triangle(const miniv3d &o,const miniv3d &d,
                           const miniv3d &v0,const miniv3d &v1,const miniv3d &v2,
                           miniv3d *tuv)
   {
   static const double epsilon=1E-5;

   double t,u,v;
   miniv3d edge1,edge2,tvec,pvec,qvec;
   double det,inv_det;

   // find vectors for two edges sharing v0
   edge1=v1-v0;
   edge2=v2-v0;

   // begin calculating determinant - also used to calculate U parameter
   pvec=d/edge2;

   // if determinant is near zero, ray lies in plane of triangle
   det=edge1*pvec;

   // cull triangles with determinant near zero
   if (fabs(det)<epsilon) return(0);

   // calculate inverse determinant
   inv_det=1.0/det;

   // calculate distance from v0 to ray origin
   tvec=o-v0;

   // calculate U parameter and test bounds
   u=(tvec*pvec)*inv_det;
   if (u<0.0 || u>1.0) return(0);

   // prepare to test V parameter
   qvec=tvec/edge1;

   // calculate V parameter and test bounds
   v=(d*qvec)*inv_det;
   if (v<0.0 || u+v>1.0) return(0);

   // calculate t, ray intersects triangle
   t=(edge2*qvec)*inv_det;

   *tuv=miniv3d(t,u,v);

   return(1);
   }

// calculate hit distance on ray to triangle
double ray_triangle_dist(const miniv3d &o,const miniv3d &d,
                         const miniv3d &v1,const miniv3d &v2,const miniv3d &v3)
   {
   miniv3d tuv;

   if (intersect_ray_triangle(o,d,v1,v2,v3,&tuv)==0) return(MAXFLOAT);
   else return(tuv.x);
   }

// geometric ray/sphere intersection test
int itest_ray_sphere(const miniv3d &o,const miniv3d &d,
                     const miniv3d &b,const double r2)
   {
   miniv3d bmo;
   double bmo2,bmod;

   bmo=b-o;
   bmo2=bmo*bmo;
   if (bmo2<r2) return(1);

   bmod=bmo*d;
   if (bmod<0.0) return(0);
   if (r2+bmod*bmod>bmo2) return(1);

   return(0);
   }

// geometric ray/bbox intersection test
int itest_ray_bbox(const miniv3d &o,const miniv3d &d,
                   const miniv3d &b,const miniv3d &r)
   {
   double l;
   miniv3d h;

   if (d.x>0.0)
      {
      l=(b.x-r.x-o.x)/d.x;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.y-b.y)>r.y || dabs(h.z-b.z)>r.z) return(0);
         else return(1);
         }
      }
   else if (b.x-o.x>r.x) return(0);

   if (d.x<0.0)
      {
      l=(b.x+r.x-o.x)/d.x;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.y-b.y)>r.y || dabs(h.z-b.z)>r.z) return(0);
         else return(1);
         }
      }
   else if (o.x-b.x>r.x) return(0);

   if (d.y>0.0)
      {
      l=(b.y-r.y-o.y)/d.y;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.x-b.x)>r.x || dabs(h.z-b.z)>r.z) return(0);
         else return(1);
         }
      }
   else if (b.y-o.y>r.y) return(0);

   if (d.y<0.0)
      {
      l=(b.y+r.y-o.y)/d.y;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.x-b.x)>r.x || dabs(h.z-b.z)>r.z) return(0);
         else return(1);
         }
      }
   else if (o.y-b.y>r.y) return(0);

   if (d.z>0.0)
      {
      l=(b.z-r.z-o.z)/d.z;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.x-b.x)>r.x || dabs(h.y-b.y)>r.y) return(0);
         else return(1);
         }
      }
   else if (b.z-o.z>r.z) return(0);

   if (d.z<0.0)
      {
      l=(b.z+r.z-o.z)/d.z;
      if (l>0.0)
         {
         h=o+d*l;
         if (dabs(h.x-b.x)>r.x || dabs(h.y-b.y)>r.y) return(0);
         else return(1);
         }
      }
   else if (o.z-b.z>r.z) return(0);

   return(1);
   }

// geometric plane/sphere intersection test
int itest_plane_sphere(const miniv3d &o,const miniv3d &n,const double radius,
                       const miniv3d &b,const double r2)
   {
   miniv3d h;
   double l;

   h=b-o;
   l=h*n;

   if (l*l>r2) return(0); // no intersection
   if (h*h>2.0*(radius*radius+r2)) return(0); // no inclusion (approximate)

   return(1);
   }

}
