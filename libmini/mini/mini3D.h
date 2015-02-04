// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINI3D_H
#define MINI3D_H

#include "miniv3f.h"
#include "miniv3d.h"
#include "minidyna.h"

//! 3D software rendering pipeline
class mini3D
   {
   public:

   struct point_struct
      {
      miniv3d pos;
      miniv3f col;
      };

   struct band_struct
      {
      miniv3d pos;
      miniv3f nrm;
      miniv3f col;
      float wdt;
      };

   struct sphere_struct
      {
      miniv3d pos;
      miniv3f col;
      double r;
      };

   struct box_struct
      {
      miniv3d pos[8];
      miniv3f col;
      };

   struct prism_struct
      {
      miniv3d pos[6];
      miniv3f col;
      };

   struct pyramid_struct
      {
      miniv3d pos[5];
      miniv3f col;
      };

   //! default constructor
   mini3D();

   //! destructor
   virtual ~mini3D();

   void line(const minidyna<point_struct> &l);
   void band(const minidyna<band_struct> &b);

   void sphere(const struct sphere_struct &s);

   void box(const struct sphere_struct &s);
   void prism(const struct prism_struct &s);
   void pyramid(const struct pyramid_struct &s);

   protected:

   struct vertex_struct
      {
      miniv3d pos;
      miniv3f col;
      };

   class primitive
      {
      public:

      primitive(miniv3d c,double r)
         : center(c),radius2(r*r)
         {}

      primitive(const minidyna<miniv3d> &p)
         {
         miniv3d c;
         double r2;

         if (p.size()>0)
            {
            c=p[0];
            for (unsigned int i=1; i<p.size(); i++)
               c+=p[i];
            c/=p.size();

            r2=0.0;
            for (unsigned int i=0; i<p.size(); i++)
               if ((p[i]-c).getlength2()>r2)
                  r2=(p[i]-c).getlength2();

            center=c;
            radius2=r2;
            }
         }

      virtual ~primitive() {}

      virtual void render(const minidyna<struct vertex_struct> &v);

      double power(miniv3d p) const
         {return((p-center).getlength2()-radius2);}

      bool order(const primitive &pr,miniv3d p) const
         {return(power(p)<pr.power(p));}

      static minidyna<miniv3d> points(miniv3d a,miniv3d b)
         {
         minidyna<miniv3d> v;
         v.append(a);
         v.append(b);
         return(v);
         }

      static minidyna<miniv3d> points(const miniv3d p[],unsigned int n)
         {
         minidyna<miniv3d> v;
         for (unsigned int i=0; i<n; i++)
            v.append(p[i]);
         return(v);
         }

      protected:

      miniv3d center;
      double radius2;
      };

   class primitive_line: public primitive
      {
      public:

      primitive_line(unsigned int idx1,unsigned int idx2,
                     const minidyna<vertex_struct> &v)
         : primitive(points(v[idx1].pos,v[idx2].pos)),
           index1(idx1),index2(idx2)
         {}

      unsigned int index1,index2;
      };

   class primitive_sphere: public primitive
      {
      public:

      primitive_sphere(unsigned int idx,double r,
                       const minidyna<vertex_struct> &v)
         : primitive(v[idx].pos,r),
           index(idx),radius(r)
         {}

      unsigned int index;
      double radius;
      };

   class primitive_box: public primitive
      {
      public:

      primitive_box(unsigned int idx,
                    const minidyna<vertex_struct> &v)
         : primitive(points(&v[idx].pos,8)),
           index(idx)
         {}

      unsigned int index;
      };

   class primitive_prism: public primitive
      {
      public:

      primitive_prism(unsigned int idx,
                      const minidyna<vertex_struct> &v)
         : primitive(points(&v[idx].pos,6)),
           index(idx)
         {}

      unsigned int index;
      };

   class primitive_pyramid: public primitive
      {
      public:

      primitive_pyramid(unsigned int idx,
                        const minidyna<vertex_struct> &v)
         : primitive(points(&v[idx].pos,5)),
           index(idx)
         {}

      unsigned int index;
      };

   minidyna<vertex_struct> vertices_;
   minidyna<primitive *> primitives_;
   };

#endif
