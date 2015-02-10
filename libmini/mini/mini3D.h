// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "minibase.h"
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

   //! render scene
   void render();

   //! clear scene
   void clear();

   protected:

   struct vertex_struct
      {
      vec3 pos;
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

      virtual void render(const std::vector<vertex_struct> &v);

      double depth(vec3 p) const
         {return((p-center).getlength2());}

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
                     const std::vector<vertex_struct> &v)
         : primitive(v[idx1].pos,v[idx2].pos),
           index1(idx1),index2(idx2)
         {}

      unsigned int index1,index2;
      };

   class primitive_sphere: public primitive
      {
      public:

      primitive_sphere()
         {}

      primitive_sphere(unsigned int idx,double r,
                       const std::vector<vertex_struct> &v)
         : primitive(v[idx].pos,r),
           index(idx),radius(r)
         {}

      unsigned int index;
      double radius;
      };

   vec3 eye_;
   mat4 preMatrix_,postMatrix_;

   std::vector<vertex_struct> vertices_;
   std::vector<primitive *> primitives_;

   std::vector<primitive_line> primitives_line_;
   std::vector<primitive_sphere> primitives_sphere_;

   void sort();

   bool greater(const primitive &a,const primitive &b)
      {return(a.depth(eye_)>b.depth(eye_));}

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
   };

#endif
