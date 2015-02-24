// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "minibase.h"
#include "database.h"

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
      {return((p-center).getlength2()+radius2);}

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

//! sphere rendering primitive
class primitive_sphere: public primitive
   {
   public:

   primitive_sphere() {}

   primitive_sphere(unsigned int idx,double r,
                    const std::vector<vertex_struct> *v)
      : primitive((*v)[idx].pos,r),
      index(idx),radius(r)
      {}

   virtual double depth(vec3 p) const
      {return((p-center).getlength2());}

   unsigned int index;
   double radius;
   };

//! sprite rendering primitive
class primitive_sprite: public primitive_sphere
   {
   public:

   primitive_sprite() {}

   primitive_sprite(unsigned int idx,double r,const databuf &b,
                    const std::vector<vertex_struct> *v)
      : primitive_sphere(idx,r,v),
      buf(b)
      {}

   databuf buf;
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

   struct sphere_struct
      {
      vec3 pos;
      vec3f col;
      double r;
      };

   struct sprite_struct
      {
      vec3 pos;
      vec3f col;
      double r;
      databuf buf;
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

   //! add band to scene
   void band(const std::vector<joint_struct> &b);

   //! add triangle strip to scene
   void strip(const std::vector<point_struct> &s);

   //! add sphere to scene
   void sphere(const struct sphere_struct &s);

   //! add sprite to scene
   void sprite(const struct sprite_struct &s);

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

   void clip(vec4 &a,const vec4 b,vec3 &ac,const vec3 bc,double z);

   void clip_line(primitive::vertex_struct *a,primitive::vertex_struct *b);
   void clip_triangle(primitive::vertex_struct *a,primitive::vertex_struct *b,primitive::vertex_struct *c);
   void clip_sphere(primitive::vertex_struct *m,double r);
   void clip_sprite(primitive::vertex_struct *m,double r,databuf *b);

   inline void clip1tri(vec4 v0,double d0,vec3 c0,
                        vec4 v1,double d1,vec3 c1,
                        vec4 v2,double d2,vec3 c2);

   inline void clip2tri(vec4 v0,double d0,vec3 c0,
                        vec4 v1,double d1,vec3 c1,
                        vec4 v2,double d2,vec3 c2);

   void cliptri(vec4 v0,vec4 v1,vec4 v2,
                vec3 c0,vec3 c1,vec3 c2);

   virtual void render_line(vec3 a,vec3 b,vec3f ac,vec3f bc) = 0;
   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec3f ac,vec3f bc,vec3f cc) = 0;
   virtual void render_sphere(vec3 m,double r,vec3f c) = 0;
   virtual void render_sprite(vec3 m,double r,vec3f c,databuf *b) = 0;
   };

//! 3D test pipeline
class mini3Dtest: public mini3D
   {
   public:

   mini3Dtest()
      : mini3D()
      {}

   protected:

   virtual void render_line(vec3 a,vec3 b,vec3f ac,vec3f bc)
      {std::cout << "line from " << a << " to " << b << std::endl;}

   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec3f ac,vec3f bc,vec3f cc)
      {std::cout << "triangle with corners " << a << ", " << b << ", " << c << std::endl;}

   virtual void render_sphere(vec3 m,double r,vec3f c)
      {std::cout << "sphere at " << m << " with radius " << r << std::endl;}

   virtual void render_sprite(vec3 m,double r,vec3f c,databuf *b)
      {std::cout << "sprite at " << m << " with radius " << r << std::endl;}

   };

#endif
