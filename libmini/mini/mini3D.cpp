// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mini3D.h"

// default constructor
mini3D::mini3D()
   {}

// destructor
mini3D::~mini3D()
   {clear();}

// pre-multiply vertices by 4x4 matrix
void mini3D::preMultiply(const mat4 &m)
   {
   preMatrix_=m;
   preMatrixOne_=(preMatrix_==mat4());
   }

// post-multiply vertices by 4x4 matrix
void mini3D::postMultiply(const mat4 &m)
   {postMatrix_=m;}

// add line to scene
void mini3D::line(const std::vector<point_struct> &l)
   {
   unsigned int idx1,idx2;

   if (l.size()>1)
      {
      idx1=addvtx(l[0].pos,l[0].col);

      for (unsigned int i=1; i<l.size(); i++)
         {
         idx2=addvtx(l[i].pos,l[i].col);

         primitives_.push_back(new primitive_line(idx1,idx2,&vertices_));

         idx1=idx2;
         }
      }
   }

// add band to scene
void mini3D::band(const std::vector<joint_struct> &b)
   {
   std::vector<point_struct> s;

   for (unsigned int i=0; i<b.size(); i++)
      {
      vec3 dir;

      if (i==0) dir=b[i+1].pos-b[i].pos;
      else if (i==b.size()-1) dir=b[i].pos-b[i-1].pos;
      else dir=halfdir(b[i].pos-b[i-1].pos,b[i+1].pos-b[i].pos);
      dir=dir.normalize();

      vec3 right=dir.cross(b[i].nrm);
      right=right.normalize();

      point_struct p1={b[i].pos-right*b[i].wdt/2,b[i].col};
      s.push_back(p1);

      point_struct p2={b[i].pos+right*b[i].wdt/2,b[i].col};
      s.push_back(p2);
      }

   strip(s);
   }

// add triangle strip to scene
void mini3D::strip(const std::vector<point_struct> &s)
   {
   unsigned int idx1,idx2,idx3;

   if (s.size()>2)
      {
      idx1=addvtx(s[0].pos,s[0].col);
      idx2=addvtx(s[1].pos,s[1].col);

      for (unsigned int i=2; i<s.size(); i++)
         {
         idx3=addvtx(s[i].pos,s[i].col);

         primitives_.push_back(new primitive_triangle(idx1,idx2,idx3,&vertices_));

         idx1=idx2;
         idx2=idx3;
         }
      }
   }

// add sphere to scene
void mini3D::sphere(const struct sphere_struct &s)
   {
   unsigned int idx=addvtx(s.pos,s.col);

   primitives_.push_back(new primitive_sphere(idx,s.r,&vertices_));
   }

// add sprite to scene
void mini3D::sprite(const struct sprite_struct &s)
   {
   unsigned int idx=addvtx(s.pos,s.col);

   primitives_.push_back(new primitive_sprite(idx,s.r,s.buf,&vertices_));
   }

// compute half direction between two vectors
vec3 mini3D::halfdir(vec3 dir1,vec3 dir2)
   {
   dir1=dir1.normalize();
   dir2=dir2.normalize();

   return(dir1+dir2);
   }

// add pre-multiplied vertex
unsigned int mini3D::addvtx(vec3 v,vec3f c)
   {
   // multiply vertex with pre-matrix
   if (!preMatrixOne_) v=preMatrix_*v;

   // append to vertex array
   struct primitive::vertex_struct vtx={v,v,c};
   vertices_.push_back(vtx);

   return(vertices_.size()-1);
   }

// render scene
void mini3D::render()
   {
   // calculate eye point and sort primitives by depth
   eye_=postMatrix_.invert()*vec4(0,0,0);
   sort();

   // multiply vertices with post-matrix
   for (unsigned int i=0; i<vertices_.size(); i++)
      vertices_[i].pos_post=postMatrix_*vertices_[i].pos;

   // render each primitive
   for (unsigned int i=0; i<primitives_.size(); i++)
      {
      primitive *p=primitives_[i];

      if (primitive_line *pl=dynamic_cast<primitive_line*>(p))
         clip_line(&vertices_[pl->index1],&vertices_[pl->index2]);
      else if (primitive_triangle *pt=dynamic_cast<primitive_triangle*>(p))
         clip_triangle(&vertices_[pt->index1],&vertices_[pt->index2],&vertices_[pt->index3]);
      else if (primitive_sphere *ps=dynamic_cast<primitive_sphere*>(p))
         clip_sphere(&vertices_[ps->index],ps->radius);
      else if (primitive_sprite *ps=dynamic_cast<primitive_sprite*>(p))
         clip_sprite(&vertices_[ps->index],ps->radius,&ps->buf);
      }
   }

// depth sort scene
void mini3D::sort()
   {mergesort(primitives_);}

// clear scene
void mini3D::clear()
   {
   vertices_.clear();

   for (unsigned int i=0; i<primitives_.size(); i++)
      delete primitives_[i];

   primitives_.clear();
   }

// compare object depth
bool mini3D::greater(const primitive *a,const primitive *b) const
   {return(a->depth(eye_) > b->depth(eye_));}

// merge two halves
void mini3D::merge(std::vector<primitive *> &a,
                   unsigned int begin, unsigned int middle, unsigned int end,
                   std::vector<primitive *> &tmp)
   {
   unsigned int b=begin, m=middle, t=begin;

   while (b<middle && m<end)
      if (greater(a[b], a[m])) tmp[t++] = a[b++];
      else tmp[t++] = a[m++];

   while (b<middle) tmp[t++] = a[b++];
   while (m<end) tmp[t++] = a[m++];

   for (t=begin; t<end; t++) a[t] = tmp[t];
   }

// mergesort by recursively merging two halves
void mini3D::mergesort(std::vector<primitive *> &a,
                       unsigned int begin, unsigned int end,
                       std::vector<primitive *> &tmp)
   {
   unsigned int s = end-begin;

   if (s>1)
      {
      unsigned int middle = begin+s/2;

      mergesort(a, begin, middle, tmp);
      mergesort(a, middle, end, tmp);

      merge(a, begin, middle, end, tmp);
      }
   }

// mergesort
void mini3D::mergesort(std::vector<primitive *> &a)
   {
   std::vector<primitive *> tmp;
   tmp.resize(a.size());

   mergesort(a, 0, a.size(), tmp);
   }

// clip line segment
void mini3D::clip(vec4 &a,const vec4 b,vec3 &ac,const vec3 bc,double z)
   {
   double f=(a.z-z)/(a.z-b.z);

   a=a+(b-a)*f;
   ac=ac+(bc-ac)*f;
   }

// clip and render line
void mini3D::clip_line(primitive::vertex_struct *a,primitive::vertex_struct *b)
   {
   bool af=a->pos_post.z>-a->pos_post.w;
   bool bf=b->pos_post.z>-b->pos_post.w;

   if (af && bf)
      render_line(a->pos_post,b->pos_post,a->col,b->col);
   else if (af)
      {
      double z=-b->pos_post.w;
      vec4 clipa=a->pos_post;
      vec4 clipb=b->pos_post;
      vec3 clipac=vec3(a->col);
      vec3 clipbc=vec3(b->col);

      clip(clipb,clipa,clipbc,clipac,z);
      render_line(clipa,clipb,clipac,clipbc);
      }
   else if (bf)
      {
      double z=-a->pos_post.w;
      vec4 clipa=a->pos_post;
      vec4 clipb=b->pos_post;
      vec3 clipac=vec3(a->col);
      vec3 clipbc=vec3(b->col);

      clip(clipa,clipb,clipac,clipbc,z);
      render_line(clipa,clipb,clipac,clipbc);
      }
   }

// clip and render triangle
void mini3D::clip_triangle(primitive::vertex_struct *a,primitive::vertex_struct *b,primitive::vertex_struct *c)
   {
   cliptri(a->pos_post,b->pos_post,c->pos_post,
           a->col,b->col,c->col);
   }

// clip and render sphere
void mini3D::clip_sphere(primitive::vertex_struct *m,double r)
   {
   if (m->pos_post.z>-m->pos_post.w)
      render_sphere(m->pos_post,r/m->pos_post.w,m->col);
   }

// clip and render sprite
void mini3D::clip_sprite(primitive::vertex_struct *m,double r,databuf *b)
   {
   if (m->pos_post.z>-m->pos_post.w)
      render_sprite(m->pos_post,r/m->pos_post.w,m->col,b);
   }

// clip a triangle (resulting in one remaining triangle)
//  v0 is the contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip1tri(vec4 v0,double d0,vec3 c0,
                             vec4 v1,double d1,vec3 c1,
                             vec4 v2,double d2,vec3 c2)
   {
   vec4 p1,p2;
   vec3 pc1,pc2;

   p1=(d1*v0+d0*v1)/(d0+d1);
   p2=(d2*v0+d0*v2)/(d0+d2);

   pc1=(d1*c0+d0*c1)/(d0+d1);
   pc2=(d2*c0+d0*c2)/(d0+d2);

   render_triangle(v0,p1,p2,c0,pc1,pc2);
   }

// clip a triangle (resulting in two remaining triangles)
//  v0 is the non-contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip2tri(vec4 v0,double d0,vec3 c0,
                             vec4 v1,double d1,vec3 c1,
                             vec4 v2,double d2,vec3 c2)
   {
   vec4 p1,p2;
   vec3 pc1,pc2;

   p1=(d1*v0+d0*v1)/(d0+d1);
   p2=(d2*v0+d0*v2)/(d0+d2);

   pc1=(d1*c0+d0*c1)/(d0+d1);
   pc2=(d2*c0+d0*c2)/(d0+d2);

   render_triangle(v1,v2,p2,c1,c2,pc2);
   render_triangle(p2,p1,v1,pc2,pc1,c1);
   }

// clip a triangle in homogeneous clip coordinates so that -w<=z
//  2 cases: clipped triangle geometry consists of either 1 or 2 triangles
void mini3D::cliptri(vec4 v0, // vertex v0
                     vec4 v1, // vertex v1
                     vec4 v2, // vertex v2
                     vec3 c0, // color c0
                     vec3 c1, // color c1
                     vec3 c2) // color c2
   {
   double d0,d1,d2;

   int ff;

   d0=v0.z+v0.w;
   d1=v1.z+v1.w;
   d2=v2.z+v2.w;

   ff=0;

   if (d0>0.0) ff|=1;
   if (d1>0.0) ff|=2;
   if (d2>0.0) ff|=4;

   switch (ff)
      {
      // 1 clipped triangle
      case 1: clip1tri(v0,fabs(d0),c0,
                       v1,fabs(d1),c1,
                       v2,fabs(d2),c2); break;
      case 2: clip1tri(v1,fabs(d1),c1,
                       v0,fabs(d0),c0,
                       v2,fabs(d2),c2); break;
      case 4: clip1tri(v2,fabs(d2),c2,
                       v0,fabs(d0),c0,
                       v1,fabs(d1),c1); break;

      // 2 clipped triangles
      case 6: clip2tri(v0,fabs(d0),c0,
                       v1,fabs(d1),c1,
                       v2,fabs(d2),c2); break;
      case 5: clip2tri(v1,fabs(d1),c1,
                       v0,fabs(d0),c0,
                       v2,fabs(d2),c2); break;
      case 3: clip2tri(v2,fabs(d2),c2,
                       v0,fabs(d0),c0,
                       v1,fabs(d1),c1); break;

      // entire triangle
      case 7: render_triangle(v0,v1,v2,c0,c1,c2); break;
      }
   }
