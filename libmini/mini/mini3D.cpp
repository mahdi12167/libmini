// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "mini3D.h"

// default constructor
mini3D::mini3D()
   {}

// destructor
mini3D::~mini3D()
   {}

void mini3D::preMultiply(const mat4 &m)
   {preMatrix_=m;}

void mini3D::postMultiply(const mat4 &m)
   {postMatrix_=m;}

void mini3D::line(const minidyna<point_struct> &l)
   {
   }

void mini3D::band(const minidyna<joint_struct> &b)
   {
   }

void mini3D::sphere(const struct sphere_struct &s)
   {
   }

void mini3D::box(const struct sphere_struct &s)
   {
   }

void mini3D::prism(const struct prism_struct &s)
   {
   }

void mini3D::pyramid(const struct pyramid_struct &s)
   {
   }

void mini3D::render()
   {
   eye_=postMatrix_.invert()*vec4(0,0,0);
   sort();

   for (unsigned int i=0; i<primitives_.size(); i++)
      primitives_[i]->render(vertices_);
   }

void mini3D::sort()
   {
   }

void mini3D::clear()
   {
   vertices_.clear();
   primitives_.clear();

   primitives_line_.clear();
   primitives_quad_.clear();
   primitives_sphere_.clear();
   primitives_box_.clear();
   primitives_prism_.clear();
   primitives_pyramid_.clear();
   }
