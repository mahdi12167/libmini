// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "minibase.h"
#include "database.h"
#include "vector"

#include "glslmath.h"

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

   struct vertex_struct
      {
      vec3 pos;
      vec4 pos_post;
      vec3f col;
      };

   class primitive
      {
      public:

      primitive()
         {}

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

   class primitive_line: public primitive
      {
      public:

      primitive_line()
         {}

      primitive_line(unsigned int idx1,unsigned int idx2,
                     const std::vector<vertex_struct> *v)
         : primitive((*v)[idx1].pos,(*v)[idx2].pos),
           index1(idx1),index2(idx2)
         {}

      virtual double depth(vec3 p) const
         {return((p-center).getlength2()+radius2);}

      unsigned int index1,index2;
      };

   class primitive_triangle: public primitive
      {
      public:

      primitive_triangle()
         {}

      primitive_triangle(unsigned int idx1,unsigned int idx2,unsigned int idx3,
                         const std::vector<vertex_struct> *v)
         : primitive((*v)[idx1].pos,(*v)[idx2].pos,(*v)[idx3].pos),
           index1(idx1),index2(idx2),index3(idx3)
         {}

      virtual double depth(vec3 p) const
         {return((p-center).getlength2());}

      unsigned int index1,index2,index3;
      };

   class primitive_sphere: public primitive
      {
      public:

      primitive_sphere()
         {}

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

   class primitive_sprite: public primitive_sphere
      {
      public:

      primitive_sprite()
         {}

      primitive_sprite(unsigned int idx,double r,const databuf &b,
                       const std::vector<vertex_struct> *v)
         : primitive_sphere(idx,r,v),
           buf(b)
         {}

      databuf buf;
      };

   vec3 eye_;
   double near_;

   mat4 preMatrix_,postMatrix_;

   std::vector<vertex_struct> vertices_;
   std::vector<primitive *> primitives_;

   std::vector<primitive_line> primitives_line_;
   std::vector<primitive_triangle> primitives_triangle_;
   std::vector<primitive_sphere> primitives_sphere_;
   std::vector<primitive_sphere> primitives_sprite_;

   vec3 halfdir(vec3 dir1,vec3 dir2);
   unsigned int addvtx(vec3 v,vec3f c);

   void sort();

   bool greater(const primitive &a,const primitive &b)
      {return(a.depth(eye_) > b.depth(eye_));}

   template<class Item>
   bool greater(const Item &a,const Item &b)
      {return(greater(a,b));}

   template<class Item>
   void merge(std::vector<Item *> &a,
              unsigned int begin, unsigned int middle, unsigned int end,
              std::vector<Item *> &tmp);

   template<class Item>
   void mergesort(std::vector<Item *> &a,
                  unsigned int begin, unsigned int end,
                  std::vector<Item *> &tmp);

   template <class Item>
   void mergesort(std::vector<Item *> &a);

   void clip(vec4 &a,const vec4 b,vec3 &ac,const vec3 bc,double z);

   void clip_line(vertex_struct *a,vertex_struct *b);
   void clip_triangle(vertex_struct *a,vertex_struct *b,vertex_struct *c);
   void clip_sphere(vertex_struct *m,double r);
   void clip_sprite(vertex_struct *m,double r,databuf *b);

   inline void clip1tri(vec3 v0,double d0,vec3 c0,
                        vec3 v1,double d1,vec3 c1,
                        vec3 v2,double d2,vec3 c2);

   inline void clip2tri(vec3 v0,double d0,vec3 c0,
                        vec3 v1,double d1,vec3 c1,
                        vec3 v2,double d2,vec3 c2);

   void cliptri(vec3 v0, // vertex v0
                vec3 v1, // vertex v1
                vec3 v2, // vertex v2
                vec3 c0, // color c0
                vec3 c1, // color c1
                vec3 c2, // color c2
                vec3 o,  // origin of clip plane
                vec3 n); // normal of clip plane

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
