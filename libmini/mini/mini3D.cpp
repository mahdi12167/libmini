// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mini3D.h"

// default constructor
mini3D::mini3D()
   {}

// destructor
mini3D::~mini3D()
   {}

// pre-multiply vertices by 4x4 matrix
void mini3D::preMultiply(const mat4 &m)
   {preMatrix_=m;}

// post-multiply vertices by 4x4 matrix
void mini3D::postMultiply(const mat4 &m)
   {postMatrix_=m;}

// add line to scene
void mini3D::line(const std::vector<point_struct> &l)
   {
   unsigned int idx1,idx2;

   if (l.size()>0)
      {
      idx1=addvtx(l[0].pos,l[0].col);

      for (unsigned int i=1; i<l.size(); i++)
         {
         idx2=addvtx(l[i].pos,l[i].col);

         primitives_line_.push_back(primitive_line(idx1,idx2,&vertices_));
         primitives_.push_back(&primitives_line_[primitives_line_.size()-1]);

         idx1=idx2;
         }
      }
   }

// add band to scene
void mini3D::band(const std::vector<joint_struct> &b)
   {
   unsigned int idx1,idx2;

   if (b.size()>0)
      {
      idx1=addvtx(b[0].pos,b[0].col);

      for (unsigned int i=1; i<b.size(); i++)
         {
         idx2=addvtx(b[i].pos,b[i].col);

         primitives_band_.push_back(primitive_band(idx1,idx2,&vertices_));
         primitives_.push_back(&primitives_band_[primitives_band_.size()-1]);

         idx1=idx2;
         }
      }
   }

// add sphere to scene
void mini3D::sphere(const struct sphere_struct &s)
   {
   unsigned int idx=addvtx(s.pos,s.col);

   primitives_sphere_.push_back(primitive_sphere(idx,s.r,&vertices_));
   primitives_.push_back(&primitives_sphere_[primitives_sphere_.size()-1]);
   }

// add sprite to scene
void mini3D::sprite(const struct sprite_struct &s)
   {
   unsigned int idx=addvtx(s.pos,s.col);

   primitives_sprite_.push_back(primitive_sprite(idx,s.r,s.buf,&vertices_));
   primitives_.push_back(&primitives_sprite_[primitives_sprite_.size()-1]);
   }

// add pre-multiplied vertex
unsigned int mini3D::addvtx(vec3 v,vec3f c)
   {
   // multiply vertex with pre-matrix
   v=preMatrix_*v;

   // append to vertex array
   struct vertex_struct vtx={v,v,c};
   vertices_.push_back(vtx);

   return(vertices_.size()-1);
   }

// render scene with n passes
void mini3D::render(unsigned int n)
   {
   // calculate eye point and sort primitives by depth
   eye_=postMatrix_.invert()*vec4(0,0,0);
   sort();

   // multiply vertices with post-matrix
   for (unsigned int i=0; i<vertices_.size(); i++)
      vertices_[i].pos_post=postMatrix_*vertices_[i].pos;

   // render each primitive with n passes
   for (unsigned int i=0; i<n; i++)
      for (unsigned int j=0; j<primitives_.size(); j++)
         {
         primitive *p=primitives_[j];

         if (primitive_line *pl=dynamic_cast<primitive_line*>(p))
            clip_line(i,&vertices_[pl->index1],&vertices_[pl->index2]);
         else if (primitive_band *pb=dynamic_cast<primitive_band*>(p))
            clip_band(i,&vertices_[pb->index1],&vertices_[pb->index2]);
         else if (primitive_sphere *ps=dynamic_cast<primitive_sphere*>(p))
            clip_sphere(i,&vertices_[ps->index],ps->radius);
         else if (primitive_sprite *ps=dynamic_cast<primitive_sprite*>(p))
            clip_sprite(i,&vertices_[ps->index],ps->radius,&ps->buf);
         }
   }

// depth sort scene
void mini3D::sort()
   {mergesort<primitive>(primitives_);}

// clear scene
void mini3D::clear()
   {
   vertices_.clear();
   primitives_.clear();

   primitives_line_.clear();
   primitives_band_.clear();
   primitives_sphere_.clear();
   primitives_sprite_.clear();
   }

// merge two halves
template<class Item>
void mini3D::merge(std::vector<Item *> &a,
                   unsigned int begin, unsigned int middle, unsigned int end,
                   std::vector<Item *> &tmp)
   {
   unsigned int b=begin, m=middle, t=begin;

   while (b<middle && m<end)
      if (greater(*(a[b]), *(a[m]))) tmp[t++] = a[b++];
      else tmp[t++] = a[m++];

   while (b<middle) tmp[t++] = a[b++];
   while (m<end) tmp[t++] = a[m++];

   for (t=begin; t<end; t++) a[t] = tmp[t];
   }

// mergesort by recursively merging two halves
template<class Item>
void mini3D::mergesort(std::vector<Item *> &a,
                       unsigned int begin, unsigned int end,
                       std::vector<Item *> &tmp)
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
template <class Item>
void mini3D::mergesort(std::vector<Item *> &a)
   {
   std::vector<Item *> tmp;
   tmp.resize(a.size());

   mergesort(a, 0, a.size(), tmp);
   }

// clip line segment
vec4 mini3D::clip(vec4 a,vec4 b)
   {return(a+(b-a)*a.z/(a.z-b.z));}

// clip and render line
void mini3D::clip_line(unsigned int pass,vertex_struct *a,vertex_struct *b)
   {
   if (a->pos_post.z<0 && b->pos_post.z<0) render_line(pass,a->pos_post,b->pos_post);
   else if (a->pos_post.z<0) render_line(pass,a->pos_post,clip(b->pos_post,a->pos_post));
   else if (b->pos_post.z<0) render_line(pass,b->pos_post,clip(a->pos_post,b->pos_post));
   }

// clip and render band
void mini3D::clip_band(unsigned int pass,vertex_struct *a,vertex_struct *b)
   {
   if (a->pos_post.z<0 && b->pos_post.z<0) render_band(pass,a->pos_post,b->pos_post);
   else if (a->pos_post.z<0) render_band(pass,a->pos_post,clip(b->pos_post,a->pos_post));
   else if (b->pos_post.z<0) render_band(pass,b->pos_post,clip(a->pos_post,b->pos_post));
   }

// clip and render sphere
void mini3D::clip_sphere(unsigned int pass,vertex_struct *m,double r)
   {
   if (m->pos_post.z<0)
      render_sphere(pass,m->pos_post,r/-m->pos_post.w);
   }

// clip and render sphere
void mini3D::clip_sprite(unsigned int pass,vertex_struct *m,double r,databuf *b)
   {
   if (m->pos_post.z<0)
      render_sprite(pass,m->pos_post,r/-m->pos_post.w,b);
   }
