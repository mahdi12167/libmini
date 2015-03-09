// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "minibase.h"

#include "vector"
#include "glslmath.h"

//! rendering primitive (base class)
class primitive
   {
   public:

   struct vertex_struct
      {
      vec3 pos;
      vec4 pos_post;
      vec3f col;
      };

   primitive() {}

   primitive(vec3 c,double r)
      : center(c),radius2(r*r)
      {}

   primitive(vec3 a,vec3 b)
      {
      center=0.5*(a+b);
      radius2=0.25*(a-b).getlength2();
      }

   primitive(vec3 a,vec3 b,vec3 c)
      {
      center=(a+b+c)/3;
      radius2=(a-center).getlength2();
      radius2=dmax(radius2,(b-center).getlength2());
      radius2=dmax(radius2,(c-center).getlength2());
      }

   virtual ~primitive() {}

   virtual double depth(vec3 p) const = 0;

   protected:

   vec3 center;
   double radius2;
   };

//! line rendering primitive
class primitive_line: public primitive
   {
   public:

   primitive_line() {}

   primitive_line(unsigned int idx1,unsigned int idx2,
                  const std::vector<vertex_struct> *v)
      : primitive((*v)[idx1].pos,(*v)[idx2].pos),
      index1(idx1),index2(idx2)
      {}

   virtual double depth(vec3 p) const
      {return((p-center).getlength2()-radius2);}

   unsigned int index1,index2;
   };

//! triangle rendering primitive
class primitive_triangle: public primitive
   {
   public:

   primitive_triangle() {}

   primitive_triangle(unsigned int idx1,unsigned int idx2,unsigned int idx3,
                      const std::vector<vertex_struct> *v)
      : primitive((*v)[idx1].pos,(*v)[idx2].pos,(*v)[idx3].pos),
      index1(idx1),index2(idx2),index3(idx3)
      {}

   virtual double depth(vec3 p) const
      {return((p-center).getlength2());}

   unsigned int index1,index2,index3;
   };

//! 3D software rendering pipeline
class mini3D
   {
   public:

   struct point_struct
      {
      vec3 pos;
      vec3f col;
      };

   struct joint_struct
      {
      vec3 pos;
      vec3f nrm;
      vec3f col;
      float wdt;
      };

   //! default constructor
   mini3D();

   //! destructor
   virtual ~mini3D();

   //! pre-multiply vertices by 4x4 matrix
   void preMultiply(const mat4 &m);

   // post-multiply vertices by 4x4 matrix
   void postMultiply(const mat4 &m);

   //! add line to scene
   void line(const std::vector<point_struct> &l);

   //! add line to scene
   void line(const std::vector<joint_struct> &l);

   //! add band to scene
   void band(const std::vector<joint_struct> &b);

   //! add triangle strip to scene
   void strip(const std::vector<point_struct> &s);

   //! render scene
   void render();

   //! clear scene
   void clear();

   protected:

   vec3 eye_;

   mat4 preMatrix_;
   bool preMatrixOne_;

   mat4 postMatrix_;

   std::vector<primitive::vertex_struct> vertices_;
   std::vector<primitive *> primitives_;

   vec3 halfdir(vec3 dir1,vec3 dir2);
   unsigned int addvtx(vec3 v,vec3f c);

   void sort();

   bool greater(const primitive *a,const primitive *b) const;

   void merge(std::vector<primitive *> &a,
              unsigned int begin, unsigned int middle, unsigned int end,
              std::vector<primitive *> &tmp);

   void mergesort(std::vector<primitive *> &a,
                  unsigned int begin, unsigned int end,
                  std::vector<primitive *> &tmp);

   void mergesort(std::vector<primitive *> &a);

   void clip(vec4 &a,const vec4 b,vec3 &ac,const vec3 bc,vec4 P);

   void clip_line(primitive::vertex_struct *a,primitive::vertex_struct *b);
   void clip_triangle(primitive::vertex_struct *a,primitive::vertex_struct *b,primitive::vertex_struct *c);

   inline void clip1tri1(vec4 v0,double d0,vec3 c0,
                         vec4 v1,double d1,vec3 c1,
                         vec4 v2,double d2,vec3 c2);

   inline void clip2tri1(vec4 v0,double d0,vec3 c0,
                         vec4 v1,double d1,vec3 c1,
                         vec4 v2,double d2,vec3 c2);

   void cliptri1(vec4 v0,vec4 v1,vec4 v2,
                 vec3 c0,vec3 c1,vec3 c2);

   inline void clip1tri2(vec4 v0,double d0,vec3 c0,
                         vec4 v1,double d1,vec3 c1,
                         vec4 v2,double d2,vec3 c2);

   inline void clip2tri2(vec4 v0,double d0,vec3 c0,
                         vec4 v1,double d1,vec3 c1,
                         vec4 v2,double d2,vec3 c2);

   void cliptri2(vec4 v0,vec4 v1,vec4 v2,
                 vec3 c0,vec3 c1,vec3 c2);

   virtual void render_begin() {}
   virtual void render_line(vec3 a,vec3 b,vec3f ac,vec3f bc) = 0;
   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec3f ac,vec3f bc,vec3f cc) = 0;
   virtual void render_end() {}
   };

inline struct mini3D::joint_struct operator + (const struct mini3D::joint_struct &a,const struct mini3D::joint_struct &b)
   {
   struct mini3D::joint_struct j={a.pos+b.pos,vec3(a.nrm)+vec3(b.nrm),vec3(a.col)+vec3(b.col),a.wdt+b.wdt};
   return(j);
   }

inline struct mini3D::joint_struct operator * (const double a,const struct mini3D::joint_struct &b)
   {
   struct mini3D::joint_struct j={a*b.pos,a*vec3(b.nrm),a*vec3(b.col),(float)a*b.wdt};
   return(j);
   }

//! 3D pipeline primitive counter
class mini3Dcounter: public mini3D
   {
   public:

   mini3Dcounter()
      : mini3D()
      {
      lines=triangles=0;
      llength=tarea=0.0;
      }

   //! render scene
   void render()
      {
      lines=triangles=0;
      llength=tarea=0.0;

      mini3D::render();
      }

   //! get total line count
   unsigned int numLines()
      {return(lines);}

   //! get total triangle count
   unsigned int numTriangles()
      {return(triangles);}

   //! get total line length
   double lineLength()
      {return(llength);}

   //! get total triangle area
   double triangleArea()
      {return(tarea);}

   protected:

   unsigned int lines;
   unsigned int triangles;

   double llength;
   double tarea;

   virtual void render_line(vec3 a,vec3 b,vec3f ac,vec3f bc)
      {
      lines++;
      llength+=(b-a).getlength();
      }

   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec3f ac,vec3f bc,vec3f cc)
      {
      triangles++;
      tarea+=vec3::area(a,b,c);
      }

   };

#endif
