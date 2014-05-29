// (c) by Stefan Roettger, licensed under MIT license

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

   // get squared vector length
   double getlength2() const {return(x*x+y*y);}

   // normalize vector to unit length
   vec2 normalize() const;

   // inner product
   double dot(const vec2 &v) const
      {return(x*v.x+y*v.y);}

   // test for approximate equality
   int approx(const vec2 &v,const double e=1E-10) const
      {return((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)<e*e);}

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

// left-hand side scalar multiplication
inline vec2 operator * (const double a,const vec2 &b)
   {return(vec2(a*b.x,a*b.y));}

// right-hand side scalar multiplication
inline vec2 operator * (const vec2 &a,const double b)
   {return(vec2(a.x*b,a.y*b));}

// right-hand side scalar division
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
inline vec2 vec2::normalize() const
   {
   double length2=getlength2();
   if (length2>0.0 && length2!=1.0) return(*this/sqrt(length2));
   return(*this);
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

   // get squared vector length
   double getlength2() const {return(x*x+y*y+z*z);}

   // normalize vector to unit length
   vec3 normalize() const;

   // inner product
   double dot(const vec3 &v) const
      {return(x*v.x+y*v.y+z*v.z);}

   // cross product (0,0,-1)/(-1,0,0)=(0,1,0)
   vec3 cross(const vec3 &v) const
      {return(vec3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x));}

   // reflect incident vector at normalized surface normal
   vec3 reflect(const vec3 &n) const;

   // test for approximate equality
   int approx(const vec3 &v,const double e=1E-10) const
      {return((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z)<e*e);}

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

// left-hand side scalar multiplication
inline vec3 operator * (const double a,const vec3 &b)
   {return(vec3(a*b.x,a*b.y,a*b.z));}

// right-hand side scalar multiplication
inline vec3 operator * (const vec3 &a,const double b)
   {return(vec3(a.x*b,a.y*b,a.z*b));}

// right-hand side scalar division
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
inline vec3 vec3::normalize() const
   {
   double length2=getlength2();
   if (length2>0.0 && length2!=1.0) return(*this/sqrt(length2));
   return(*this);
   }

// reflect incident vector at normalized surface normal
vec3 vec3::reflect(const vec3 &n) const
   {return((*this)-2*n*dot(n));}

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

      if (w!=0.0)
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

   // get squared vector length
   double getlength2() const {return(x*x+y*y+z*z+w*w);}

   // normalize vector to unit length
   vec4 normalize() const;

   // inner product
   double dot(const vec4 &v) const
      {return(x*v.x+y*v.y+z*v.z+w*v.w);}

   // cross product (0,0,-1,0)/(-1,0,0,0)=(0,1,0,0)
   vec4 cross(const vec4 &v) const
      {
      assert(w==0.0 && v.w==0.0);
      return(vec4(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x,0.0));
      }

   // reflect incident vector at normalized surface normal
   vec4 reflect(const vec4 &n) const;

   // test for approximate equality
   int approx(const vec4 &v,const double e=1E-10) const
      {return((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z)+(v.w-w)*(v.w-w)<e*e);}

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

// left-hand side scalar multiplication
inline vec4 operator * (const double a,const vec4 &b)
   {return(vec4(a*b.x,a*b.y,a*b.z,a*b.w));}

// right-hand side scalar multiplication
inline vec4 operator * (const vec4 &a,const double b)
   {return(vec4(a.x*b,a.y*b,a.z*b,a.w*b));}

// right-hand side scalar division
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
inline vec4 vec4::normalize() const
   {
   double length2=getlength2();
   if (length2>0.0 && length2!=1.0) return(*this/sqrt(length2));
   return(*this);
   }

// reflect incident vector at normalized surface normal
inline vec4 vec4::reflect(const vec4 &n) const
   {
   assert(w==0.0 && n.w==0.0);
   return((*this)-2*n*dot(n));
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const vec4 &v)
   {return(out << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')');}

// 2x2 double matrix
//  definition of matrix via constructor taking two row vectors
//  supplies matrix operators + and *
class mat2
   {
   public:

   // default constructor
   mat2(const vec2 &r1=vec2(1,0),
        const vec2 &r2=vec2(0,1))
      {
      mtx[0][0]=r1.x;
      mtx[0][1]=r2.x;

      mtx[1][0]=r1.y;
      mtx[1][1]=r2.y;
      }

   // subscript operator (column getter)
   vec2 operator[] (const int i) const
      {
      assert(i>=0 && i<2);
      return(vec2(mtx[i][0],mtx[i][1]));
      }

   // row getter
   vec2 row(const int i) const
      {
      assert(i>=0 && i<2);
      return(vec2(mtx[0][i],mtx[1][i]));
      }

   // cast operator to column-first array
   operator const double *() const
      {return(pointer);}

   // calculate determinant of 2x2 matrix
   double det() const
      {return(mtx[0][0]*mtx[1][1]-mtx[0][1]*mtx[1][0]);}

   // transpose 2x2 matrix
   mat2 transpose() const
      {return(mat2(row(0),row(1)));}

   // invert 2x2 matrix
   mat2 invert() const
      {
      mat2 m;
      double d;

      // calculate determinant
      d=det();

      // check determinant
      assert(d!=0.0);

      // calculate inverse
      d=1.0/d;
      m.mtx[0][0]=d*mtx[1][1];
      m.mtx[1][0]=-d*mtx[1][0];
      m.mtx[0][1]=-d*mtx[0][1];
      m.mtx[1][1]=d*mtx[0][0];

      return(m);
      }

   // create scaling matrix
   static mat2 scale(double s,double t)
      {
      return(mat2(vec2(s,0),
                  vec2(0,t)));
      }

   // create scaling matrix
   static mat2 scale(const vec2 &c)
      {
      return(mat2(vec2(c.x,0),
                  vec2(0,c.y)));
      }

   protected:

   // matrix
   union {
      double mtx[2][2];
      double pointer[4];
      };
   };

// addition of two matrices
inline mat2 operator + (const mat2 &m1,const mat2 &m2)
   {
   return(mat2(m1[0]+m2[0],
               m1[1]+m2[1]));
   }

// multiplication of two matrices
inline mat2 operator * (const mat2 &m1,const mat2 &m2)
   {
   return(mat2(vec2(m1.row(0).dot(m2[0]),m1.row(0).dot(m2[1])),
               vec2(m1.row(1).dot(m2[0]),m1.row(1).dot(m2[1]))));
   }

// comparison
inline int operator == (const mat2 &a,const mat2 &b)
   {
   unsigned int i;

   for (i=0; i<4; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(false);

   return(true);
   }

// negated comparison
inline int operator != (const mat2 &a,const mat2 &b)
   {
   unsigned int i;

   for (i=0; i<4; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(true);

   return(false);
   }

// right-hand side vector multiplication
inline vec2 operator * (const mat2 &m,const vec2 &v)
   {return(vec2(m.row(0).dot(v),m.row(1).dot(v)));}

// output operator
inline std::ostream& operator << (std::ostream &out,const mat2 &m)
   {return(out << '(' << m.row(0) << ',' << m.row(1) << ')');}

// 3x3 double matrix
//  definition of matrix via constructor taking three row vectors
//  supplies matrix operators + and *
class mat3
   {
   public:

   // default constructor
   mat3(const vec3 &r1=vec3(1,0,0),
        const vec3 &r2=vec3(0,1,0),
        const vec3 &r3=vec3(0,0,1))
      {
      mtx[0][0]=r1.x;
      mtx[0][1]=r2.x;
      mtx[0][2]=r3.x;

      mtx[1][0]=r1.y;
      mtx[1][1]=r2.y;
      mtx[1][2]=r3.y;

      mtx[2][0]=r1.z;
      mtx[2][1]=r2.z;
      mtx[2][2]=r3.z;
      }

   // copy constructor
   mat3(const mat2 &m)
      {
      mtx[0][0]=m[0].x;
      mtx[0][1]=m[1].x;
      mtx[0][2]=0.0;

      mtx[1][0]=m[0].y;
      mtx[1][1]=m[1].y;
      mtx[1][2]=0.0;

      mtx[2][0]=0.0;
      mtx[2][1]=0.0;
      mtx[2][2]=1.0;
      }

   // subscript operator (column getter)
   vec3 operator[] (const int i) const
      {
      assert(i>=0 && i<3);
      return(vec3(mtx[i][0],mtx[i][1],mtx[i][2]));
      }

   // row getter
   vec3 row(const int i) const
      {
      assert(i>=0 && i<3);
      return(vec3(mtx[0][i],mtx[1][i],mtx[2][i]));
      }

   // cast operator to column-first array
   operator const double *() const
      {return(pointer);}

   // calculate determinant of 3x3 matrix
   double det() const
      {
      return(mtx[0][0]*(mtx[1][1]*mtx[2][2]-mtx[2][1]*mtx[1][2])+
             mtx[0][1]*(mtx[2][0]*mtx[1][2]-mtx[1][0]*mtx[2][2])+
             mtx[0][2]*(mtx[1][0]*mtx[2][1]-mtx[2][0]*mtx[1][1]));
      }

   // transpose 3x3 matrix
   mat3 transpose() const
      {return(mat3(row(0),row(1),row(2)));}

   // invert 3x3 matrix
   mat3 invert() const
      {
      mat3 m;
      double d;

      // calculate determinant
      d=det();

      // check determinant
      assert(d!=0.0);

      // calculate inverse
      d=1.0/d;
      m.mtx[0][0]=d*(mtx[1][1]*mtx[2][2]-mtx[2][1]*mtx[1][2]);
      m.mtx[1][0]=d*(mtx[2][0]*mtx[1][2]-mtx[1][0]*mtx[2][2]);
      m.mtx[2][0]=d*(mtx[1][0]*mtx[2][1]-mtx[2][0]*mtx[1][1]);
      m.mtx[0][1]=d*(mtx[2][1]*mtx[0][2]-mtx[0][1]*mtx[2][2]);
      m.mtx[1][1]=d*(mtx[0][0]*mtx[2][2]-mtx[2][0]*mtx[0][2]);
      m.mtx[2][1]=d*(mtx[2][0]*mtx[0][1]-mtx[0][0]*mtx[2][1]);
      m.mtx[0][2]=d*(mtx[0][1]*mtx[1][2]-mtx[1][1]*mtx[0][2]);
      m.mtx[1][2]=d*(mtx[1][0]*mtx[0][2]-mtx[0][0]*mtx[1][2]);
      m.mtx[2][2]=d*(mtx[0][0]*mtx[1][1]-mtx[1][0]*mtx[0][1]);

      return(m);
      }

   // create scaling matrix
   static mat3 scale(double s,double t,double r)
      {
      return(mat3(vec3(s,0,0),
                  vec3(0,t,0),
                  vec3(0,0,r)));
      }

   // create scaling matrix
   static mat3 scale(const vec3 &c)
      {
      return(mat3(vec3(c.x,0,0),
                  vec3(0,c.y,0),
                  vec3(0,0,c.z)));
      }

   // create rotation matrix
   static mat3 rotate(double angle,const vec3 &v)
      {
      vec3 axis;
      double c,s;
      double c1;

      double x,y,z;

      angle*=M_PI/180;

      axis=v.normalize();

      c=cos(angle);
      s=sin(angle);

      x=axis.x;
      y=axis.y;
      z=axis.z;

      c1=1.0-c;

      return(mat3(vec3(x*x*c1+c,x*y*c1-z*s,x*z*c1+y*s),
                  vec3(y*x*c1+z*s,y*y*c1+c,y*z*c1-x*s),
                  vec3(x*z*c1-y*s,y*z*c1+x*s,z*z*c1+c)));
      }

   protected:

   // matrix
   union {
      double mtx[3][3];
      double pointer[9];
      };
   };

// addition of two matrices
inline mat3 operator + (const mat3 &m1,const mat3 &m2)
   {
   return(mat3(m1[0]+m2[0],
               m1[1]+m2[1],
               m1[2]+m2[2]));
   }

// multiplication of two matrices
inline mat3 operator * (const mat3 &m1,const mat3 &m2)
   {
   return(mat3(vec3(m1.row(0).dot(m2[0]),m1.row(0).dot(m2[1]),m1.row(0).dot(m2[2])),
               vec3(m1.row(1).dot(m2[0]),m1.row(1).dot(m2[1]),m1.row(1).dot(m2[2])),
               vec3(m1.row(2).dot(m2[0]),m1.row(2).dot(m2[1]),m1.row(2).dot(m2[2]))));
   }

// comparison
inline int operator == (const mat3 &a,const mat3 &b)
   {
   unsigned int i;

   for (i=0; i<9; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(false);

   return(true);
   }

// negated comparison
inline int operator != (const mat3 &a,const mat3 &b)
   {
   unsigned int i;

   for (i=0; i<9; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(true);

   return(false);
   }

// right-hand side vector multiplication
inline vec3 operator * (const mat3 &m,const vec3 &v)
   {return(vec3(m.row(0).dot(v),m.row(1).dot(v),m.row(2).dot(v)));}

// output operator
inline std::ostream& operator << (std::ostream &out,const mat3 &m)
   {return(out << '(' << m.row(0) << ',' << m.row(1) << ',' << m.row(2) << ')');}

// 3x3 float matrix
//  this matrix type is not designed for calculations
//  it is just designed for storage and transfer purposes
//   use mat3 for linear math calulations, then copy to mat3f and
//   cast to const float * to transfer via glUniform for example:
//    glUniformMatrix3fv(location, 1, GL_FALSE, (const float *)mat3f(M));
class mat3f
   {
   public:

   // default constructor
   mat3f()
      {
      mtx[0][0]=1.0f;
      mtx[0][1]=0.0f;
      mtx[0][2]=0.0f;

      mtx[1][0]=0.0f;
      mtx[1][1]=1.0f;
      mtx[1][2]=0.0f;

      mtx[2][0]=0.0f;
      mtx[2][1]=0.0f;
      mtx[2][2]=1.0f;
      }

   // copy constructor
   mat3f(const mat3 &m)
      {
      mtx[0][0]=m[0].x;
      mtx[0][1]=m[1].x;
      mtx[0][2]=m[2].x;

      mtx[1][0]=m[0].y;
      mtx[1][1]=m[1].y;
      mtx[1][2]=m[2].y;

      mtx[2][0]=m[0].z;
      mtx[2][1]=m[1].z;
      mtx[2][2]=m[2].z;
      }

   // subscript operator (column getter)
   vec4 operator[] (const int i) const
      {
      assert(i>=0 && i<3);
      return(vec3(mtx[i][0],mtx[i][1],mtx[i][2]));
      }

   // row getter
   vec4 row(const int i) const
      {
      assert(i>=0 && i<3);
      return(vec4(mtx[0][i],mtx[1][i],mtx[2][i]));
      }

   // cast operator to column-first array
   operator const float *() const
      {return(pointer);}

   protected:

   // matrix
   union {
      float mtx[3][3];
      float pointer[9];
      };
   };

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

   // copy constructor
   mat4(const mat3 &m)
      {
      mtx[0][0]=m[0].x;
      mtx[0][1]=m[1].x;
      mtx[0][2]=m[2].x;
      mtx[0][3]=0.0;

      mtx[1][0]=m[0].y;
      mtx[1][1]=m[1].y;
      mtx[1][2]=m[2].y;
      mtx[1][3]=0.0;

      mtx[2][0]=m[0].z;
      mtx[2][1]=m[1].z;
      mtx[2][2]=m[2].z;
      mtx[2][3]=0.0;

      mtx[3][0]=0.0;
      mtx[3][1]=0.0;
      mtx[3][2]=0.0;
      mtx[3][3]=1.0;
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
      {return(pointer);}

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

   // create scaling matrix
   static mat4 scale(double s,double t,double r,double w=1.0)
      {
      return(mat4(vec4(s,0,0,0),
                  vec4(0,t,0,0),
                  vec4(0,0,r,0),
                  vec4(0,0,0,w)));
      }

   // create scaling matrix
   static mat4 scale(const vec4 &c)
      {
      return(mat4(vec4(c.x,0,0,0),
                  vec4(0,c.y,0,0),
                  vec4(0,0,c.z,0),
                  vec4(0,0,0,c.w)));
      }

   // create translation matrix
   static mat4 translate(double x,double y,double z)
      {
      return(mat4(vec4(1,0,0,x),
                  vec4(0,1,0,y),
                  vec4(0,0,1,z),
                  vec4(0,0,0,1)));
      }

   // create translation matrix
   static mat4 translate(const vec4 &v)
      {
      return(mat4(vec4(1,0,0,v.x),
                  vec4(0,1,0,v.y),
                  vec4(0,0,1,v.z),
                  vec4(0,0,0,v.w)));
      }

   // create rotation matrix
   static mat4 rotate(double angle,const vec3 &v)
      {return(mat3::rotate(angle,v));}

   // create orthographic matrix
   static mat4 ortho(double l,double r,double b,double t,double n,double f);

   // create perspective matrix
   static mat4 perspective(double fovy,double aspect,double znear,double zfar);

   // create lookat matrix
   static mat4 lookat(const vec3 &eye,const vec3 &center,const vec3 &up);

   protected:

   // matrix
   union {
      double mtx[4][4];
      double pointer[16];
      };
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

// comparison
inline int operator == (const mat4 &a,const mat4 &b)
   {
   unsigned int i;

   for (i=0; i<16; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(false);

   return(true);
   }

// negated comparison
inline int operator != (const mat4 &a,const mat4 &b)
   {
   unsigned int i;

   for (i=0; i<16; i++)
      if (((const double *)a)[i]!=((const double *)b)[i]) return(true);

   return(false);
   }

// right-hand side vector multiplication
inline vec4 operator * (const mat4 &m,const vec4 &v)
   {return(vec4(m.row(0).dot(v),m.row(1).dot(v),m.row(2).dot(v),m.row(3).dot(v)));}

// output operator
inline std::ostream& operator << (std::ostream &out,const mat4 &m)
   {return(out << '(' << m.row(0) << ',' << m.row(1) << ',' << m.row(2) << ',' << m.row(3) << ')');}

// create orthographic matrix
inline mat4 mat4::ortho(double l,double r,double b,double t,double n,double f)
   {
   assert (r>l && t>b && f>n);

   return(mat4(vec4(2.0/(r-l),0,0,-(r+l)/(r-l)),
               vec4(0,2.0/(t-b),0,-(t+b)/(t-b)),
               vec4(0,0,-2.0/(f-n),-(f+n)/(f-n))));
   }

// create perspective matrix
inline mat4 mat4::perspective(double fovy,double aspect,double znear,double zfar)
   {
   double f;

   fovy*=M_PI/180;

   assert(fovy>0.0 && fovy<180.0);
   assert(znear>0.0 && zfar>znear);
   assert(aspect>0.0);

   f=1.0/tan(0.5*fovy);

   return(mat4(vec4(f/aspect,0,0,0),
               vec4(0,f,0,0),
               vec4(0,0,(zfar+znear)/(znear-zfar),(2*zfar*znear)/(znear-zfar)),
               vec4(0,0,-1,0)));
   }

// create lookat matrix
inline mat4 mat4::lookat(const vec3 &eye,const vec3 &center,const vec3 &up)
   {
   vec3 dir,right,top;

   dir=(center-eye).normalize();
   top=up.normalize();

   right=dir.cross(top);
   top=right.cross(dir);

   return(mat4(vec4(right,0),vec4(top,0),vec4(-dir,0))*translate(-eye));
   }

// 4x4 float matrix
//  this matrix type is not designed for calculations
//  it is just designed for storage and transfer purposes
//   use mat4 for linear math calulations, then copy to mat4f and
//   cast to const float * to transfer via glUniform for example:
//    glUniformMatrix4fv(location, 1, GL_FALSE, (const float *)mat4f(M));
class mat4f
   {
   public:

   // default constructor
   mat4f()
      {
      mtx[0][0]=1.0f;
      mtx[0][1]=0.0f;
      mtx[0][2]=0.0f;
      mtx[0][3]=0.0f;

      mtx[1][0]=0.0f;
      mtx[1][1]=1.0f;
      mtx[1][2]=0.0f;
      mtx[1][3]=0.0f;

      mtx[2][0]=0.0f;
      mtx[2][1]=0.0f;
      mtx[2][2]=1.0f;
      mtx[2][3]=0.0f;

      mtx[3][0]=0.0f;
      mtx[3][1]=0.0f;
      mtx[3][2]=0.0f;
      mtx[3][3]=1.0f;
      }

   // copy constructor
   mat4f(const mat4 &m)
      {
      mtx[0][0]=m[0].x;
      mtx[0][1]=m[1].x;
      mtx[0][2]=m[2].x;
      mtx[0][3]=m[3].x;

      mtx[1][0]=m[0].y;
      mtx[1][1]=m[1].y;
      mtx[1][2]=m[2].y;
      mtx[1][3]=m[3].y;

      mtx[2][0]=m[0].z;
      mtx[2][1]=m[1].z;
      mtx[2][2]=m[2].z;
      mtx[2][3]=m[3].z;

      mtx[3][0]=m[0].w;
      mtx[3][1]=m[1].w;
      mtx[3][2]=m[2].w;
      mtx[3][3]=m[3].w;
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
   operator const float *() const
      {return(pointer);}

   protected:

   // matrix
   union {
      float mtx[4][4];
      float pointer[16];
      };
   };

// quaternion
class quat
   {
   public:

   // default constructor
   quat(const vec4 &v=vec4(0,0,0,1))
      : q(v)
      {}

   // inner product
   double dot(const quat &r) const
      {return(q.dot(r.q));}

   // get quaternion length
   double getlength() const {return(q.getlength());}

   // get quaternion norm
   double getnorm() const {return(q.getlength2());}

   // normalization
   quat normalize() const
      {return(q.normalize());}

   // conjugation
   quat conjugate() const
      {return(vec4(-vec3(q.x,q.y,q.z),q.w));}

   // invert normalized quaternion
   quat invert() const
      {return(normalize().conjugate());}

   // cast operator
   operator vec4() const
      {return(q);}

   // cast operator
   operator mat3() const
      {
      double a=q.w;
      double b=q.x;
      double c=q.y;
      double d=q.z;
      double a2=a*a;
      double b2=b*b;
      double c2=c*c;
      double d2=d*d;

      return(mat3(vec3(a2+b2-c2-d2, 2*(b*c-a*d), 2*(b*d+a*c)),
                  vec3(2*(b*c+a*d), a2-b2+c2-d2, 2*(c*d-a*b)),
                  vec3(2*(b*d-a*c), 2*(c*d+a*b), a2-b2-c2+d2)));
      }

   // create rotating quaternion
   static quat rotate(double angle,const vec3 &v)
      {
      angle*=M_PI/360;
      return(vec4(sin(angle)*v,cos(angle)));
      }

   friend quat operator + (const quat &a,const quat &b);
   friend quat operator - (const quat &a,const quat &b);
   friend quat operator * (const quat &a,const quat &b);
   friend quat operator * (double a,const quat &b);
   friend quat operator * (const quat &a,double b);
   friend quat operator - (const quat &r);
   friend vec3 operator * (const quat &r,const vec3 &v);

   protected:

   vec4 q;
   };

// addition of two quaternions
inline quat operator + (const quat &a,const quat &b)
   {return(a.q+b.q);}

// subtraction of two quaternions
inline quat operator - (const quat &a,const quat &b)
   {return(a.q-b.q);}

// left-hand side scalar multiplication
inline quat operator * (const double a,const quat &b)
   {return(a*b.q);}

// right-hand side scalar multiplication
inline quat operator * (const quat &a,const double b)
   {return(a.q*b);}

// conjugation of a quaternion
inline quat operator - (const quat &r)
   {return(vec4(-vec3(r.q.x,r.q.y,r.q.z),r.q.w));}

// multiplication of two quaternions
inline quat operator * (const quat &a,const quat &b)
   {
   double v=a.q.w,w=b.q.w;
   vec3 p(a.q.x,a.q.y,a.q.z),q(b.q.x,b.q.y,b.q.z);

   return(vec4(p*w+q*v+p.cross(q),v*w-p.dot(q)));
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const quat &r)
   {return(out << "quat" << (vec4)r);}

// right-hand side vector multiplication with normalized quaternion
inline vec3 operator * (const quat &r,const vec3 &v)
   {
   quat w=r.normalize();
   w=-w*quat(vec4(v,0.0))*w;

   return(vec3(w.q.x,w.q.y,w.q.z));
   }

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

// matrix stack convenience macros
#define mult_matrix(m) scoped_push<mat4> p(m)
#define top_matrix() scoped_push<mat4>::top()

template <class T>
std::vector<T> scoped_push<T>::stack_;

// scoped opengl matrix stack
class glScopedMatrixStack
   {
   public:

   glScopedMatrixStack()
      {mtxpush();}

   ~glScopedMatrixStack()
      {mtxpop();}
   };

// scoped replacement for glPushMatrix/glPopMatrix pairs
#define glPushMatrixScoped() glScopedMatrixStack p

// test function
int test_glslmath()
   {
   int errors=0;

   // test vectors
   {
      vec3 v(0,3,4);
      if (!(v.getlength()==5)) errors++;
      vec3 a(-10,0,10),b(10,10,10);
      if (!(0.5*(a+b)==vec3(0,5,10))) errors++;
      vec4 r=vec4(1,-1,0,0).reflect(vec4(0,1,0,0));
      if (!(r==vec4(1,1,0,0))) errors++;
   }

   // test matrices
   {
      mat4 M(vec4(0,1,0,0),
             vec4(-1,0,0,0),
             vec4(0,0,1,0));

      if (!(M*M.invert()==mat4())) errors++;
   }

   // test transformations
   {
      mat4 translate=mat4::translate(0,0,0);
      mat4 rotate=mat4::rotate(0,vec3(1,0,0));
      mat4 lookat=mat4::lookat(vec3(0,0,0),vec3(0,0,-1),vec3(0,1,0));
      if (!(translate==mat4() && rotate==mat4() && lookat==mat4())) errors++;
      mat4 perspective=mat4::perspective(90,1,1,2);
      if (!(fabs(1-perspective[0].x)<1E-10 && fabs(-3-perspective[2].z)<1E-10)) errors++;
   }

   // test matrix stack
   {
      double x1,x2;
      {
         mult_matrix(mat4::scale(0.5,1,1));
         {
            mult_matrix(mat4::translate(1,1,1));
            x1=top_matrix()[3].x;
         }
         x2=top_matrix()[3].x;
      }
      if (!(x1==0.5 && x2==0.0)) errors++;
   }

   // test quaternions
   {
      quat q;
      vec3 v(0,1,0);
      vec3 w=q*v;
      quat a=quat::rotate(10,vec3(0,1,0));
      quat b=quat::rotate(80,vec3(0,1,0));
      quat r=a*b;
      vec3 p=r*vec3(1,0,0);
      if (!(v.approx(w) && p.approx(vec3(0,0,1)))) errors++;
   }

   return(errors);
   }

#endif
