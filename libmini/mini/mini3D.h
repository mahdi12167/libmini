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

   struct box_struct
      {
      vec3 pos[8];
      vec3f col;
      };

   struct prism_struct
      {
      vec3 pos[6];
      vec3f col;
      };

   struct pyramid_struct
      {
      vec3 pos[5];
      vec3f col;
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

   //! add box to scene
   void box(const struct sphere_struct &s);

   //! add prism to scene
   void prism(const struct prism_struct &s);

   //! add pyramid to scene
   void pyramid(const struct pyramid_struct &s);

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

      primitive(const std::vector<vec3> &p)
         {
         vec3 c;
         double r2;

         if (p.size()>0)
            {
            c=p[0];
            for (unsigned int i=1; i<p.size(); i++)
               c=c+p[i];
            c=c/p.size();

            r2=0.0;
            for (unsigned int i=0; i<p.size(); i++)
               if ((p[i]-c).getlength2()>r2)
                  r2=(p[i]-c).getlength2();

            center=c;
            radius2=r2;
            }
         }

      virtual ~primitive() {}

      virtual void render(const std::vector<vertex_struct> &v);

      double power(vec3 p) const
         {return((p-center).getlength2()-radius2);}

      static std::vector<vec3> points(vec3 a,vec3 b)
         {
         std::vector<vec3> v;
         v.push_back(a);
         v.push_back(b);
         return(v);
         }

      static std::vector<vec3> points(const vec3 p[],unsigned int n)
         {
         std::vector<vec3> v;
         for (unsigned int i=0; i<n; i++)
            v.push_back(p[i]);
         return(v);
         }

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
         : primitive(points(v[idx1].pos,v[idx2].pos)),
           index1(idx1),index2(idx2)
         {}

      unsigned int index1,index2;
      };

   class primitive_quad: public primitive
      {
      public:

      primitive_quad()
         {}

      primitive_quad(unsigned int idx,
                     const std::vector<vertex_struct> &v)
         : primitive(points(&v[idx].pos,4)),
           index(idx)
         {}

      unsigned int index;
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

   class primitive_box: public primitive
      {
      public:

      primitive_box()
         {}

      primitive_box(unsigned int idx,
                    const std::vector<vertex_struct> &v)
         : primitive(points(&v[idx].pos,8)),
           index(idx)
         {}

      unsigned int index;
      };

   class primitive_prism: public primitive
      {
      public:

      primitive_prism()
         {}

      primitive_prism(unsigned int idx,
                      const std::vector<vertex_struct> &v)
         : primitive(points(&v[idx].pos,6)),
           index(idx)
         {}

      unsigned int index;
      };

   class primitive_pyramid: public primitive
      {
      public:

      primitive_pyramid()
         {}

      primitive_pyramid(unsigned int idx,
                        const std::vector<vertex_struct> &v)
         : primitive(points(&v[idx].pos,5)),
           index(idx)
         {}

      unsigned int index;
      };

   vec3 eye_;
   mat4 preMatrix_,postMatrix_;

   std::vector<vertex_struct> vertices_;
   std::vector<primitive *> primitives_;

   std::vector<primitive_line> primitives_line_;
   std::vector<primitive_quad> primitives_quad_;
   std::vector<primitive_sphere> primitives_sphere_;
   std::vector<primitive_box> primitives_box_;
   std::vector<primitive_prism> primitives_prism_;
   std::vector<primitive_pyramid> primitives_pyramid_;

   void sort();

   bool less(const primitive &a,const primitive &b)
      {return(a.power(eye_)<b.power(eye_));}

   template<class Item>
   bool less(const Item &a,const Item &b)
      {return(less(a,b));}

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
