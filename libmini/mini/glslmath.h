// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef GLSLMATH_H
#define GLSLMATH_H

#include <math.h>
#include <assert.h>

#include <iostream>
#include <vector>

// 2D double vector
//  definition of components via constructor vec2(x,y)
//  access to components x/y via . component selector
//  supplies vector operators + - * and dot product
//  supplies getters for length and normalization
class vec2
   {
   public:

   // default constructor
   vec2() {}

   // copy constructor
   vec2(const vec2 &v) {x=v.x; y=v.y;}

   // component-wise constructor
   vec2(const double vx,const double vy) {x=vx; y=vy;}

   // destructor
   ~vec2() {}

   // get vector length
   double getlength() const {return(sqrt(x*x+y*y));}

   // normalize vector to unit length
   double normalize();

   // dot product
   double dot(const vec2 &v)
      {return(x*v.x+y*v.y);}

   // vector components
   double x,y;
   };

// addition of two vectors
inline vec2 operator + (const vec2 &a,const vec2 &b)
   {return(vec2(a.x+b.x,a.y+b.y));}

// subtraction of two vectors
inline vec2 operator - (const vec2 &a,const vec2 &b)
   {return(vec2(a.x-b.x,a.y-b.y));}

// negation of a vector
inline vec2 operator - (const vec2 &v)
   {return(vec2(-v.x,-v.y));}

// inner product
inline vec2 operator * (const double a,const vec2 &b)
   {return(vec2(a*b.x,a*b.y));}

// inner product
inline vec2 operator * (const vec2 &a,const double b)
   {return(vec2(a.x*b,a.y*b));}

// inner division
inline vec2 operator / (const vec2 &a,const double b)
   {return(vec2(a.x/b,a.y/b));}

// component-wise multiplication
inline vec2 operator * (const vec2 &a,const vec2 &b)
   {return(vec2(a.x*b.x,a.y*b.y));}

// comparison
inline int operator == (const vec2 &a,const vec2 &b)
   {return(a.x==b.x && a.y==b.y);}

// negated comparison
inline int operator != (const vec2 &a,const vec2 &b)
   {return(a.x!=b.x || a.y!=b.y);}

// normalization to unit length
inline double vec2::normalize()
   {
   double length=getlength();
   if (length>0.0) *this=*this/length;
   return(length);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const vec2 &v)
   {return(out << '(' << v.x << ',' << v.y << ')');}

// 3D double vector
//  definition of components via constructor vec3(x,y,z)
//  access to components x/y/z via . component selector
//  supplies vector operators + - * dot and cross product
//  supplies getters for length and normalization
class vec3
   {
   public:

   // default constructor
   vec3() {}

   // copy constructor
   vec3(const vec3 &v) {x=v.x; y=v.y; z=v.z;}

   // copy constructor
   vec3(const vec2 &v,double vz=0.0) {x=v.x; y=v.y; z=vz;}

   // component-wise constructor
   vec3(const double vx,const double vy,const double vz) {x=vx; y=vy; z=vz;}

   // cast operator
   operator vec2() const
      {return(vec2(x,y));}

   // destructor
   ~vec3() {}

   // get vector length
   double getlength() const {return(sqrt(x*x+y*y+z*z));}

   // normalize vector to unit length
   double normalize();

   // dot product
   double dot(const vec3 &v)
      {return(x*v.x+y*v.y+z*v.z);}

   // cross product (0,0,-1)/(-1,0,0)=(0,1,0)
   vec3 cross(const vec3 &v)
      {return(vec3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x));}

   // vector components
   double x,y,z;
   };

// addition of two vectors
inline vec3 operator + (const vec3 &a,const vec3 &b)
   {return(vec3(a.x+b.x,a.y+b.y,a.z+b.z));}

// subtraction of two vectors
inline vec3 operator - (const vec3 &a,const vec3 &b)
   {return(vec3(a.x-b.x,a.y-b.y,a.z-b.z));}

// negation of a vector
inline vec3 operator - (const vec3 &v)
   {return(vec3(-v.x,-v.y,-v.z));}

// inner product
inline vec3 operator * (const double a,const vec3 &b)
   {return(vec3(a*b.x,a*b.y,a*b.z));}

// inner product
inline vec3 operator * (const vec3 &a,const double b)
   {return(vec3(a.x*b,a.y*b,a.z*b));}

// inner division
inline vec3 operator / (const vec3 &a,const double b)
   {return(vec3(a.x/b,a.y/b,a.z/b));}

// component-wise multiplication
inline vec3 operator * (const vec3 &a,const vec3 &b)
   {return(vec3(a.x*b.x,a.y*b.y,a.z*b.z));}

// comparison
inline int operator == (const vec3 &a,const vec3 &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z);}

// negated comparison
inline int operator != (const vec3 &a,const vec3 &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z);}

// normalization to unit length
inline double vec3::normalize()
   {
   double length=getlength();
   if (length>0.0) *this=*this/length;
   return(length);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const vec3 &v)
   {return(out << '(' << v.x << ',' << v.y << ',' << v.z << ')');}

// 4D double vector
//  definition of components via constructor vec4(x,y,z,w)
//  access to components x/y/z/w via . component selector
//  supplies vector operators + - * and dot product
//  supplies getter for length
class vec4
   {
   public:

   // default constructor
   vec4() {}

   // copy constructor
   vec4(const vec4 &v) {x=v.x; y=v.y; z=v.z; w=v.w;}

   // copy constructor
   vec4(const vec3 &v,double vw=1.0) {x=v.x; y=v.y; z=v.z; w=vw;}

   // copy constructor
   vec4(const vec2 &v,double vz=0.0,double vw=1.0) {x=v.x; y=v.y; z=vz; w=vw;}

   // component-wise constructor
   vec4(const double vx,const double vy,const double vz, const double vw=1.0) {x=vx; y=vy; z=vz; w=vw;}

   // cast operator with homogenization
   operator vec3() const
      {
      double c;

      assert(w!=0.0);

      if (w!=1.0)
         {
         c=1.0/w;
         return(vec3(x*c,y*c,z*c));
         }

      return(vec3(x,y,z));
      }

   // destructor
   ~vec4() {}

   // get vector length
   double getlength() const {return(sqrt(x*x+y*y+z*z+w*w));}

   // normalize vector to unit length
   double normalize();

   // dot product
   double dot(const vec4 &v)
      {return(x*v.x+y*v.y+z*v.z+w*v.w);}

   // vector components
   double x,y,z,w;
   };

// addition of two vectors
inline vec4 operator + (const vec4 &a,const vec4 &b)
   {return(vec4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w));}

// subtraction of two vectors
inline vec4 operator - (const vec4 &a,const vec4 &b)
   {return(vec4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w));}

// negation of a vector
inline vec4 operator - (const vec4 &v)
   {return(vec4(-v.x,-v.y,-v.z,-v.w));}

// inner product
inline vec4 operator * (const double a,const vec4 &b)
   {return(vec4(a*b.x,a*b.y,a*b.z,a*b.w));}

// inner product
inline vec4 operator * (const vec4 &a,const double b)
   {return(vec4(a.x*b,a.y*b,a.z*b,a.w*b));}

// inner division
inline vec4 operator / (const vec4 &a,const double b)
   {return(vec4(a.x/b,a.y/b,a.z/b,a.w/b));}

// component-wise multiplication
inline vec4 operator * (const vec4 &a,const vec4 &b)
   {return(vec4(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w));}

// comparison
inline int operator == (const vec4 &a,const vec4 &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w);}

// negated comparison
inline int operator != (const vec4 &a,const vec4 &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w);}

// normalization to unit length
inline double vec4::normalize()
   {
   double length=getlength();
   if (length>0.0) *this=*this/length;
   return(length);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const vec4 &v)
   {return(out << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')');}

// 4x4 double matrix
//  definition of matrix via constructor taking four row vectors
//  supplies matrix operators + and *
class mat4
   {
   public:

   // default constructor
   mat4(const vec4 &r1=vec4(1,0,0,0),
        const vec4 &r2=vec4(0,1,0,0),
        const vec4 &r3=vec4(0,0,1,0),
        const vec4 &r4=vec4(0,0,0,1))
      {
      mtx[0][0]=r1.x;
      mtx[0][1]=r2.x;
      mtx[0][2]=r3.x;
      mtx[0][3]=r4.x;

      mtx[1][0]=r1.y;
      mtx[1][1]=r2.y;
      mtx[1][2]=r3.y;
      mtx[1][3]=r4.y;

      mtx[2][0]=r1.z;
      mtx[2][1]=r2.z;
      mtx[2][2]=r3.z;
      mtx[2][3]=r4.z;

      mtx[3][0]=r1.w;
      mtx[3][1]=r2.w;
      mtx[3][2]=r3.w;
      mtx[3][3]=r4.w;
      }

   // subscript operator (column getter)
   vec4 operator[] (const int i) const
      {
      assert(i>=0 && i<4);
      return(vec4(mtx[i][0],mtx[i][1],mtx[i][2],mtx[i][3]));
      }

   // row getter
   vec4 row(const int i) const
      {
      assert(i>=0 && i<4);
      return(vec4(mtx[0][i],mtx[1][i],mtx[2][i],mtx[3][i]));
      }

   // cast operator to column-first array
   operator const double *() const
      {return(mtx[0]);}

   // calculate determinant of 4x4 matrix
   double det() const
      {
      return(mtx[0][3]*mtx[1][2]*mtx[2][1]*mtx[3][0]-
             mtx[0][2]*mtx[1][3]*mtx[2][1]*mtx[3][0]-
             mtx[0][3]*mtx[1][1]*mtx[2][2]*mtx[3][0]+
             mtx[0][1]*mtx[1][3]*mtx[2][2]*mtx[3][0]+
             mtx[0][2]*mtx[1][1]*mtx[2][3]*mtx[3][0]-
             mtx[0][1]*mtx[1][2]*mtx[2][3]*mtx[3][0]-
             mtx[0][3]*mtx[1][2]*mtx[2][0]*mtx[3][1]+
             mtx[0][2]*mtx[1][3]*mtx[2][0]*mtx[3][1]+
             mtx[0][3]*mtx[1][0]*mtx[2][2]*mtx[3][1]-
             mtx[0][0]*mtx[1][3]*mtx[2][2]*mtx[3][1]-
             mtx[0][2]*mtx[1][0]*mtx[2][3]*mtx[3][1]+
             mtx[0][0]*mtx[1][2]*mtx[2][3]*mtx[3][1]+
             mtx[0][3]*mtx[1][1]*mtx[2][0]*mtx[3][2]-
             mtx[0][1]*mtx[1][3]*mtx[2][0]*mtx[3][2]-
             mtx[0][3]*mtx[1][0]*mtx[2][1]*mtx[3][2]+
             mtx[0][0]*mtx[1][3]*mtx[2][1]*mtx[3][2]+
             mtx[0][1]*mtx[1][0]*mtx[2][3]*mtx[3][2]-
             mtx[0][0]*mtx[1][1]*mtx[2][3]*mtx[3][2]-
             mtx[0][2]*mtx[1][1]*mtx[2][0]*mtx[3][3]+
             mtx[0][1]*mtx[1][2]*mtx[2][0]*mtx[3][3]+
             mtx[0][2]*mtx[1][0]*mtx[2][1]*mtx[3][3]-
             mtx[0][0]*mtx[1][2]*mtx[2][1]*mtx[3][3]-
             mtx[0][1]*mtx[1][0]*mtx[2][2]*mtx[3][3]+
             mtx[0][0]*mtx[1][1]*mtx[2][2]*mtx[3][3]);
      }

   // transpose 4x4 matrix
   mat4 transpose() const
      {return(mat4(row(0),row(1),row(2),row(3)));}

   // invert 4x4 matrix
   mat4 invert() const
      {
      mat4 m;
      double d;

      // calculate determinant
      d=det();

      // check determinant
      assert(d!=0.0);

      // calculate inverse
      d=1.0/d;
      m.mtx[0][0]=d*(mtx[1][2]*mtx[2][3]*mtx[3][1]-mtx[1][3]*mtx[2][2]*mtx[3][1]+mtx[1][3]*mtx[2][1]*mtx[3][2]-mtx[1][1]*mtx[2][3]*mtx[3][2]-mtx[1][2]*mtx[2][1]*mtx[3][3]+mtx[1][1]*mtx[2][2]*mtx[3][3]);
      m.mtx[0][1]=d*(mtx[0][3]*mtx[2][2]*mtx[3][1]-mtx[0][2]*mtx[2][3]*mtx[3][1]-mtx[0][3]*mtx[2][1]*mtx[3][2]+mtx[0][1]*mtx[2][3]*mtx[3][2]+mtx[0][2]*mtx[2][1]*mtx[3][3]-mtx[0][1]*mtx[2][2]*mtx[3][3]);
      m.mtx[0][2]=d*(mtx[0][2]*mtx[1][3]*mtx[3][1]-mtx[0][3]*mtx[1][2]*mtx[3][1]+mtx[0][3]*mtx[1][1]*mtx[3][2]-mtx[0][1]*mtx[1][3]*mtx[3][2]-mtx[0][2]*mtx[1][1]*mtx[3][3]+mtx[0][1]*mtx[1][2]*mtx[3][3]);
      m.mtx[0][3]=d*(mtx[0][3]*mtx[1][2]*mtx[2][1]-mtx[0][2]*mtx[1][3]*mtx[2][1]-mtx[0][3]*mtx[1][1]*mtx[2][2]+mtx[0][1]*mtx[1][3]*mtx[2][2]+mtx[0][2]*mtx[1][1]*mtx[2][3]-mtx[0][1]*mtx[1][2]*mtx[2][3]);
      m.mtx[1][0]=d*(mtx[1][3]*mtx[2][2]*mtx[3][0]-mtx[1][2]*mtx[2][3]*mtx[3][0]-mtx[1][3]*mtx[2][0]*mtx[3][2]+mtx[1][0]*mtx[2][3]*mtx[3][2]+mtx[1][2]*mtx[2][0]*mtx[3][3]-mtx[1][0]*mtx[2][2]*mtx[3][3]);
      m.mtx[1][1]=d*(mtx[0][2]*mtx[2][3]*mtx[3][0]-mtx[0][3]*mtx[2][2]*mtx[3][0]+mtx[0][3]*mtx[2][0]*mtx[3][2]-mtx[0][0]*mtx[2][3]*mtx[3][2]-mtx[0][2]*mtx[2][0]*mtx[3][3]+mtx[0][0]*mtx[2][2]*mtx[3][3]);
      m.mtx[1][2]=d*(mtx[0][3]*mtx[1][2]*mtx[3][0]-mtx[0][2]*mtx[1][3]*mtx[3][0]-mtx[0][3]*mtx[1][0]*mtx[3][2]+mtx[0][0]*mtx[1][3]*mtx[3][2]+mtx[0][2]*mtx[1][0]*mtx[3][3]-mtx[0][0]*mtx[1][2]*mtx[3][3]);
      m.mtx[1][3]=d*(mtx[0][2]*mtx[1][3]*mtx[2][0]-mtx[0][3]*mtx[1][2]*mtx[2][0]+mtx[0][3]*mtx[1][0]*mtx[2][2]-mtx[0][0]*mtx[1][3]*mtx[2][2]-mtx[0][2]*mtx[1][0]*mtx[2][3]+mtx[0][0]*mtx[1][2]*mtx[2][3]);
      m.mtx[2][0]=d*(mtx[1][1]*mtx[2][3]*mtx[3][0]-mtx[1][3]*mtx[2][1]*mtx[3][0]+mtx[1][3]*mtx[2][0]*mtx[3][1]-mtx[1][0]*mtx[2][3]*mtx[3][1]-mtx[1][1]*mtx[2][0]*mtx[3][3]+mtx[1][0]*mtx[2][1]*mtx[3][3]);
      m.mtx[2][1]=d*(mtx[0][3]*mtx[2][1]*mtx[3][0]-mtx[0][1]*mtx[2][3]*mtx[3][0]-mtx[0][3]*mtx[2][0]*mtx[3][1]+mtx[0][0]*mtx[2][3]*mtx[3][1]+mtx[0][1]*mtx[2][0]*mtx[3][3]-mtx[0][0]*mtx[2][1]*mtx[3][3]);
      m.mtx[2][2]=d*(mtx[0][1]*mtx[1][3]*mtx[3][0]-mtx[0][3]*mtx[1][1]*mtx[3][0]+mtx[0][3]*mtx[1][0]*mtx[3][1]-mtx[0][0]*mtx[1][3]*mtx[3][1]-mtx[0][1]*mtx[1][0]*mtx[3][3]+mtx[0][0]*mtx[1][1]*mtx[3][3]);
      m.mtx[2][3]=d*(mtx[0][3]*mtx[1][1]*mtx[2][0]-mtx[0][1]*mtx[1][3]*mtx[2][0]-mtx[0][3]*mtx[1][0]*mtx[2][1]+mtx[0][0]*mtx[1][3]*mtx[2][1]+mtx[0][1]*mtx[1][0]*mtx[2][3]-mtx[0][0]*mtx[1][1]*mtx[2][3]);
      m.mtx[3][0]=d*(mtx[1][2]*mtx[2][1]*mtx[3][0]-mtx[1][1]*mtx[2][2]*mtx[3][0]-mtx[1][2]*mtx[2][0]*mtx[3][1]+mtx[1][0]*mtx[2][2]*mtx[3][1]+mtx[1][1]*mtx[2][0]*mtx[3][2]-mtx[1][0]*mtx[2][1]*mtx[3][2]);
      m.mtx[3][1]=d*(mtx[0][1]*mtx[2][2]*mtx[3][0]-mtx[0][2]*mtx[2][1]*mtx[3][0]+mtx[0][2]*mtx[2][0]*mtx[3][1]-mtx[0][0]*mtx[2][2]*mtx[3][1]-mtx[0][1]*mtx[2][0]*mtx[3][2]+mtx[0][0]*mtx[2][1]*mtx[3][2]);
      m.mtx[3][2]=d*(mtx[0][2]*mtx[1][1]*mtx[3][0]-mtx[0][1]*mtx[1][2]*mtx[3][0]-mtx[0][2]*mtx[1][0]*mtx[3][1]+mtx[0][0]*mtx[1][2]*mtx[3][1]+mtx[0][1]*mtx[1][0]*mtx[3][2]-mtx[0][0]*mtx[1][1]*mtx[3][2]);
      m.mtx[3][3]=d*(mtx[0][1]*mtx[1][2]*mtx[2][0]-mtx[0][2]*mtx[1][1]*mtx[2][0]+mtx[0][2]*mtx[1][0]*mtx[2][1]-mtx[0][0]*mtx[1][2]*mtx[2][1]-mtx[0][1]*mtx[1][0]*mtx[2][2]+mtx[0][0]*mtx[1][1]*mtx[2][2]);

      return(m);
      }

   // create translation matrix
   static mat4 translate(double x,double y,double z)
      {
      return(mat4(vec4(1,0,0,x),
                  vec4(0,1,0,y),
                  vec4(0,0,1,z),
                  vec4(0,0,0,1)));
      }

   // create scaling matrix
   static mat4 scale(double s,double t,double r)
      {
      return(mat4(vec4(s,0,0,0),
                  vec4(0,t,0,0),
                  vec4(0,0,r,0),
                  vec4(0,0,0,1)));
      }

   protected:

   // matrix
   double mtx[4][4];
   };

// addition of two matrices
inline mat4 operator + (const mat4 &m1,const mat4 &m2)
   {
   return(mat4(m1[0]+m2[0],
               m1[1]+m2[1],
               m1[2]+m2[2],
               m1[3]+m2[3]));
   }

// multiplication of two matrices
inline mat4 operator * (const mat4 &m1,const mat4 &m2)
   {
   return(mat4(vec4(m1.row(0).dot(m2[0]),m1.row(0).dot(m2[1]),m1.row(0).dot(m2[2]),m1.row(0).dot(m2[3])),
               vec4(m1.row(1).dot(m2[0]),m1.row(1).dot(m2[1]),m1.row(1).dot(m2[2]),m1.row(1).dot(m2[3])),
               vec4(m1.row(2).dot(m2[0]),m1.row(2).dot(m2[1]),m1.row(2).dot(m2[2]),m1.row(2).dot(m2[3])),
               vec4(m1.row(3).dot(m2[0]),m1.row(3).dot(m2[1]),m1.row(3).dot(m2[2]),m1.row(3).dot(m2[3]))));
   }

// right-hand vector multiplication
inline vec4 operator * (const mat4 &m,const vec4 &v)
   {return(vec4(m.row(0).dot(v),m.row(1).dot(v),m.row(2).dot(v),m.row(3).dot(v)));}

// output operator
inline std::ostream& operator << (std::ostream &out,const mat4 &m)
   {return(out << '(' << m.row(0) << ',' << m.row(1) << ',' << m.row(2) << ',' << m.row(3) << ')');}

// matrix stack
template <class T>
class scoped_push
   {
   public:

   // default constructor
   scoped_push(const T &m)
      {
      if (stack_.begin()==stack_.end())
         stack_.push_back(m);
      else
         stack_.push_back(top()*m);
      }

   // destructor
   ~scoped_push()
      {stack_.pop_back();}

   // top stack element
   static const T top()
      {
      assert(stack_.begin()!=stack_.end());
      return(*(--stack_.end()));
      }

   protected:

   static std::vector<T> stack_;
   };

#define mult_matrix(m) scoped_push<mat4> p(m)
#define top_matrix() scoped_push<mat4>::top()

template <class T>
std::vector<T> scoped_push<T>::stack_;

#endif
