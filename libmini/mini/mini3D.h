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
      vec4f col;
      };

   typedef std::vector<vertex_struct> vertex_array;

   primitive() {}

   primitive(vec3 c,double r,vertex_array *v)
      : center(c),radius2(r*r),array(v)
      {}

   primitive(vec3 a,vec3 b,vertex_array *v)
      : array(v)
      {
      center=0.5*(a+b);
      radius2=0.25*(a-b).getlength2();
      }

   primitive(vec3 a,vec3 b,vec3 c,vertex_array *v)
      : array(v)
      {
      center=(a+b+c)/3;
      radius2=(a-center).getlength2();
      radius2=dmax(radius2,(b-center).getlength2());
      radius2=dmax(radius2,(c-center).getlength2());
      }

   virtual ~primitive() {}

   virtual double depth(vec3 p) const = 0;

   vec3 center;
   double radius2;

   vertex_array *array;
   };

//! line rendering primitive
class primitive_line: public primitive
   {
   public:

   primitive_line() : primitive() {}

   primitive_line(unsigned int idx1,unsigned int idx2,
                  primitive::vertex_array *vertices)
      : primitive((*vertices)[idx1].pos,(*vertices)[idx2].pos,vertices),
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

   primitive_triangle() : primitive() {}

   primitive_triangle(unsigned int idx1,unsigned int idx2,unsigned int idx3,
                      primitive::vertex_array *vertices)
      : primitive((*vertices)[idx1].pos,(*vertices)[idx2].pos,(*vertices)[idx3].pos,vertices),
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
      vec4f col;
      };

   struct joint_struct
      {
      vec3 pos;
      vec3f nrm;
      vec4f col;
      float wdt;
      };

   //! default constructor
   mini3D();

   //! destructor
   virtual ~mini3D();

   //! pre-multiply vertices by 4x4 matrix
   void preMultiply(const mat4 &m);

   //! post-multiply vertices by 4x4 matrix
   void postMultiply(const mat4 &m);

   //! add triangle to scene
   void triangle(point_struct p1,point_struct p2,point_struct p3);

   //! add quadrilateral to scene
   void quad(point_struct p1,point_struct p2,point_struct p3,point_struct p4);

   //! add line to scene
   void line(const std::vector<point_struct> &l);

   //! add line to scene
   void line(const std::vector<joint_struct> &l);

   //! add band to scene
   void band(const std::vector<joint_struct> &b);

   //! add triangle strip to scene
   void strip(const std::vector<point_struct> &s);

   //! add triangle fan to scene
   void fan(const std::vector<point_struct> &f);

   //! add disc to scene
   void disc(double radius,vec4f col,int n=20);

   //! get number of vertices in the scene
   unsigned int vertices() const;

   //! get number of primitives in the scene
   unsigned int primitives() const;

   //! render scene
   void render();

   //! clear scene
   void clear();

   //! attach another renderer
   mini3D *attach();

   protected:

   vec3 eye_;

   mat4 preMatrix_;
   bool preMatrixOne_;

   mat4 postMatrix_;

   primitive::vertex_array vertices_;
   std::vector<primitive *> primitives_;

   std::vector<mini3D *> renderers_;

   vec3 halfdir(vec3 dir1,vec3 dir2);

   unsigned int addvtx(vec3 v,vec4f c,
                       primitive::vertex_array *vertices);

   void sort(std::vector<primitive *> &primitives);

   bool greater(const primitive *a,const primitive *b) const;

   void merge(std::vector<primitive *> &a,
              unsigned int begin, unsigned int middle, unsigned int end,
              std::vector<primitive *> &tmp);

   void mergesort(std::vector<primitive *> &a,
                  unsigned int begin, unsigned int end,
                  std::vector<primitive *> &tmp);

   void mergesort(std::vector<primitive *> &a);

   void clip(vec4 &a,const vec4 b,vec4 &ac,const vec4 bc,vec4 P);

   void clip_line(primitive::vertex_struct *a,primitive::vertex_struct *b);
   void clip_triangle(primitive::vertex_struct *a,primitive::vertex_struct *b,primitive::vertex_struct *c);

   inline void clip1tri1(vec4 v0,double d0,vec4 c0,
                         vec4 v1,double d1,vec4 c1,
                         vec4 v2,double d2,vec4 c2);

   inline void clip2tri1(vec4 v0,double d0,vec4 c0,
                         vec4 v1,double d1,vec4 c1,
                         vec4 v2,double d2,vec4 c2);

   void cliptri1(vec4 v0,vec4 v1,vec4 v2,
                 vec4 c0,vec4 c1,vec4 c2);

   inline void clip1tri2(vec4 v0,double d0,vec4 c0,
                         vec4 v1,double d1,vec4 c1,
                         vec4 v2,double d2,vec4 c2);

   inline void clip2tri2(vec4 v0,double d0,vec4 c0,
                         vec4 v1,double d1,vec4 c1,
                         vec4 v2,double d2,vec4 c2);

   void cliptri2(vec4 v0,vec4 v1,vec4 v2,
                 vec4 c0,vec4 c1,vec4 c2);

   void culltri(vec4 v0,vec4 v1,vec4 v2,
                vec4 c0,vec4 c1,vec4 c2);

   virtual void render_setup(const mat4 &m);
   virtual void render_begin() {}
   virtual void render_line(vec3 a,vec3 b,vec4f ac,vec4f bc) {}
   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec4f ac,vec4f bc,vec4f cc) {}
   virtual void render_yield() {}
   virtual void render_end() {}
   };

inline struct mini3D::point_struct operator + (const struct mini3D::point_struct &a,const struct mini3D::point_struct &b)
   {
   struct mini3D::point_struct p={a.pos+b.pos,vec4(a.col)+vec4(b.col)};
   return(p);
   }

inline struct mini3D::point_struct operator * (const double a,const struct mini3D::point_struct &b)
   {
   struct mini3D::point_struct p={a*b.pos,a*vec4(b.col)};
   return(p);
   }

inline struct mini3D::joint_struct operator + (const struct mini3D::joint_struct &a,const struct mini3D::joint_struct &b)
   {
   struct mini3D::joint_struct j={a.pos+b.pos,vec3(a.nrm)+vec3(b.nrm),vec4(a.col)+vec4(b.col),a.wdt+b.wdt};
   return(j);
   }

inline struct mini3D::joint_struct operator * (const double a,const struct mini3D::joint_struct &b)
   {
   struct mini3D::joint_struct j={a*b.pos,a*vec3(b.nrm),a*vec4(b.col),(float)(a*b.wdt)};
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

   virtual void render_line(vec3 a,vec3 b,vec4f ac,vec4f bc)
      {
      lines++;
      llength+=(b-a).getlength();
      }

   virtual void render_triangle(vec3 a,vec3 b,vec3 c,vec4f ac,vec4f bc,vec4f cc)
      {
      triangles++;
      tarea+=vec3::area(a,b,c);
      }

   };

#endif
