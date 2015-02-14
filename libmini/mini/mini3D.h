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

   //! add sphere to scene
   void sphere(const struct sphere_struct &s);

   //! add sprite to scene
   void sprite(const struct sprite_struct &s);

   //! render scene with n passes
   void render(unsigned int n);

   //! clear scene
   void clear();

   protected:

   struct vertex_struct
      {
      vec3 pos;
      vec3 pos_post;
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

      virtual ~primitive() {}

      virtual double depth() const = 0;

      int operator > (const primitive &p)
         {return(depth()>p.depth());}

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

      virtual double depth() const
         {return(center.getlength2()+radius2);}

      protected:

      unsigned int index1,index2;
      };

   class primitive_band: public primitive_line
      {
      public:

      primitive_band()
         {}

      primitive_band(unsigned int idx1,unsigned int idx2,
                     const std::vector<vertex_struct> *v)
         : primitive_line(idx1,idx2,v)
         {}

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

      virtual double depth() const
         {return(center.getlength2());}

      protected:

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

      protected:

      databuf buf;
      };

   mat4 preMatrix_,postMatrix_;

   std::vector<vertex_struct> vertices_;
   std::vector<primitive *> primitives_;

   std::vector<primitive_line> primitives_line_;
   std::vector<primitive_band> primitives_band_;
   std::vector<primitive_sphere> primitives_sphere_;
   std::vector<primitive_sphere> primitives_sprite_;

   unsigned int addvtx(vec3 v,vec3f c);

   void sort();

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

   virtual void render(unsigned int pass,
                       const primitive *p,
                       const std::vector<vertex_struct> *v) = 0;

   };

#endif
