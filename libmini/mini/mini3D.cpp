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

// add sphere to scene
void mini3D::sphere(const struct sphere_struct &s)
   {
   //!! todo
   }

// add line to scene
void mini3D::line(const std::vector<point_struct> &l)
   {
   //!! todo
   }

// add band to scene
void mini3D::band(const std::vector<joint_struct> &b)
   {
   //!! todo
   }

// render scene
void mini3D::render()
   {
   eye_=postMatrix_.invert()*vec4(0,0,0);
   sort();

   for (unsigned int i=0; i<primitives_.size(); i++)
      primitives_[i]->render(vertices_);
   }

// depth sort scene
void mini3D::sort()
   {mergesort<primitive>(primitives_);}

// clear scene
void mini3D::clear()
   {
   vertices_.clear();
   primitives_.clear();

   primitives_sphere_.clear();
   primitives_line_.clear();
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
