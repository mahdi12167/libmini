// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <math.h>
#include <assert.h>

#include <iostream>
#include <vector>

// 3D double vector
//  definition of components via constructor v3d(x,y,z)
//  access to components x/y/z via . component selector
//  supplies vector operators + - * dot and cross product
//  supplies getters for length and normalization
class v3d
   {
   public:

   // default constructor
   v3d() {}

   // copy constructor
   v3d(const v3d &v) {x=v.x; y=v.y; z=v.z;}

   // component-wise constructor
   v3d(const double vx,const double vy,const double vz) {x=vx; y=vy; z=vz;}

   // destructor
   ~v3d() {}

   double getlength() const {return(sqrt(x*x+y*y+z*z));}

   double normalize();

   double x,y,z;
   };

// addition of two vectors
inline v3d operator + (const v3d &a,const v3d &b)
   {return(v3d(a.x+b.x,a.y+b.y,a.z+b.z));}

// subtraction of two vectors
inline v3d operator - (const v3d &a,const v3d &b)
   {return(v3d(a.x-b.x,a.y-b.y,a.z-b.z));}

// negation of a vector
inline v3d operator - (const v3d &v)
   {return(v3d(-v.x,-v.y,-v.z));}

// inner product
inline v3d operator * (const double a,const v3d &b)
   {return(v3d(a*b.x,a*b.y,a*b.z));}

// inner product
inline v3d operator * (const v3d &a,const double b)
   {return(v3d(a.x*b,a.y*b,a.z*b));}

// inner division
inline v3d operator / (const v3d &a,const double b)
   {return(v3d(a.x/b,a.y/b,a.z/b));}

// dot product
inline double operator * (const v3d &a,const v3d &b)
   {return(a.x*b.x+a.y*b.y+a.z*b.z);}

// cross product (0,0,-1)/(-1,0,0)=(0,1,0)
inline v3d operator / (const v3d &a,const v3d &b)
   {return(v3d(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x));}

// comparison
inline int operator == (const v3d &a,const v3d &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z);}

// negated comparison
inline int operator != (const v3d &a,const v3d &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z);}

// normalization to unit length
inline double v3d::normalize()
   {
   double length=getlength();
   if (length>0.0) *this=*this/length;
   return(length);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const v3d &v)
   {return(out << '(' << v.x << ',' << v.y << ',' << v.z << ')');}

// 4D double vector
//  definition of components via constructor v4d(x,y,z,w)
//  access to components x/y/z/w via . component selector
//  supplies vector operators + - * and dot product
//  supplies getter for length
class v4d
   {
   public:

   // default constructor
   v4d() {}

   // copy constructor
   v4d(const v4d &v) {x=v.x; y=v.y; z=v.z; w=v.w;}

   // copy constructor
   v4d(const v3d &v) {x=v.x; y=v.y; z=v.z; w=1.0;}

   // component-wise constructor
   v4d(const double vx,const double vy,const double vz, const double vw=1.0) {x=vx; y=vy; z=vz; w=vw;}

   //! homogenization cast operator
   operator v3d() const
      {
      double c;

      assert(w!=0.0);

      if (w!=1.0)
         {
         c=1.0/w;
         return(v3d(x*c,y*c,z*c));
         }

      return(v3d(x,y,z));
      }

   // destructor
   ~v4d() {}

   double getlength() const {return(sqrt(x*x+y*y+z*z+w*w));}

   double normalize();

   double x,y,z,w;
   };

// addition of two vectors
inline v4d operator + (const v4d &a,const v4d &b)
   {return(v4d(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w));}

// subtraction of two vectors
inline v4d operator - (const v4d &a,const v4d &b)
   {return(v4d(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w));}

// negation of a vector
inline v4d operator - (const v4d &v)
   {return(v4d(-v.x,-v.y,-v.z,-v.w));}

// inner product
inline v4d operator * (const double a,const v4d &b)
   {return(v4d(a*b.x,a*b.y,a*b.z,a*b.w));}

// inner product
inline v4d operator * (const v4d &a,const double b)
   {return(v4d(a.x*b,a.y*b,a.z*b,a.w*b));}

// inner division
inline v4d operator / (const v4d &a,const double b)
   {return(v4d(a.x/b,a.y/b,a.z/b,a.w/b));}

// dot product
inline double operator * (const v4d &a,const v4d &b)
   {return(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);}

// comparison
inline int operator == (const v4d &a,const v4d &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w);}

// negated comparison
inline int operator != (const v4d &a,const v4d &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w);}

// normalization to unit length
inline double v4d::normalize()
   {
   double length=getlength();
   if (length>0.0) *this=*this/length;
   return(length);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const v4d &v)
   {return(out << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')');}

// 4x4 double matrix
//  definition of matrix via constructor taking four row vectors
//  supplies matrix operators + and *
class M4x4
   {
   public:

   M4x4(const v4d &r1=v4d(1,0,0,0),
        const v4d &r2=v4d(0,1,0,0),
        const v4d &r3=v4d(0,0,1,0),
        const v4d &r4=v4d(0,0,0,1))
      {
      mtx[0]=r1;
      mtx[1]=r2;
      mtx[2]=r3;
      mtx[3]=r4;
      }

   const v4d &operator[] (const int i) const
      {
      assert(i>=0 && i<4);
      return(mtx[i]);
      }

   M4x4 transpose() const
      {
      return(M4x4(v4d(mtx[0].x,mtx[1].x,mtx[2].x,mtx[3].x),
                  v4d(mtx[0].y,mtx[1].y,mtx[2].y,mtx[3].y),
                  v4d(mtx[0].z,mtx[1].z,mtx[2].z,mtx[3].z),
                  v4d(mtx[0].w,mtx[1].w,mtx[2].w,mtx[3].w)));
      }

   static M4x4 translate(double x,double y,double z)
      {
      return(M4x4(v4d(1,0,0,x),
                  v4d(0,1,0,y),
                  v4d(0,0,1,z),
                  v4d(0,0,0,1)));
      }

   static M4x4 scale(double s,double t,double r)
      {
      return(M4x4(v4d(s,0,0,0),
                  v4d(0,t,0,0),
                  v4d(0,0,r,0),
                  v4d(0,0,0,1)));
      }

   protected:

   v4d mtx[4];
   };

// addition of two matrices
inline M4x4 operator + (const M4x4 &m1,const M4x4 &m2)
   {
   return(M4x4(m1[0]+m2[0],
               m1[1]+m2[1],
               m1[2]+m2[2],
               m1[3]+m2[3]));
   }

// multiplication of two matrices
inline M4x4 operator * (const M4x4 &m1,const M4x4 &m2)
   {
   M4x4 t=m2.transpose();

   return(M4x4(v4d(m1[0]*t[0],m1[0]*t[1],m1[0]*t[2],m1[0]*t[3]),
               v4d(m1[1]*t[0],m1[1]*t[1],m1[1]*t[2],m1[1]*t[3]),
               v4d(m1[2]*t[0],m1[2]*t[1],m1[2]*t[2],m1[2]*t[3]),
               v4d(m1[3]*t[0],m1[3]*t[1],m1[3]*t[2],m1[3]*t[3])));
   }

// right-hand vector multiplication
inline v4d operator * (const M4x4 &m,const v4d &v)
   {return(v4d(m[0]*v,m[1]*v,m[2]*v,m[3]*v));}

// output operator
inline std::ostream& operator << (std::ostream &out,const M4x4 &v)
   {return(out << '(' << v[0] << ',' << v[1] << ',' << v[2] << ',' << v[3] << ')');}

// matrix stack
class scoped_push
   {
   public:

   scoped_push(const M4x4 &m)
      {
      if (stack_.begin()==stack_.end())
         stack_.push_back(m);
      else
         stack_.push_back(top()*m);
      }

   ~scoped_push()
      {stack_.pop_back();}

   static const M4x4 top()
      {
      assert(stack_.begin()!=stack_.end());
      return(*(--stack_.end()));
      }

   protected:

   static std::vector<M4x4> stack_;
   };

#define mult_matrix(m) scoped_push p(m)
#define top_matrix() scoped_push::top()

#endif
