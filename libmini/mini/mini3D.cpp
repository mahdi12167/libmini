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

// add line to scene
void mini3D::line(const std::vector<joint_struct> &l)
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
   if (b.size()<2) return;

   std::vector<mini3D::joint_struct> b1;

   // average points that are closer than half band width
   b1.push_back(b.front());
   if (b.size()>2) b1.push_back(b[1]);
   for (unsigned int i=2; i+1<b.size(); i++)
      if (b[i].wdt!=0.0 && b1.back().wdt!=0.0)
         {
         double d,w;

         d=(b[i].pos-b1.back().pos).getlength2();
         w=0.5*(b[i].wdt+b1.back().wdt);

         if (d<dsqr(0.5*w)) b1.back()=0.5*(b[i]+b1.back());
         else b1.push_back(b[i]);
         }
      else b1.push_back(b[i]);
   b1.push_back(b.back());

   std::vector<mini3D::joint_struct> b2;

   // create helper points for band turns greater than 90 degrees
   b2.push_back(b1.front());
   for (unsigned int i=1; i+1<b1.size(); i++)
      if (b1[i].wdt!=0.0 && b2.back().wdt!=0.0 && b1[i+1].wdt!=0.0)
         {
         vec3 d1,d2;

         d1=b1[i].pos-b1[i-1].pos;
         d2=b1[i+1].pos-b1[i].pos;

         if (d1.dot(d2)<0.0)
            {
            vec3 dir=halfdir(d1,d2);
            vec3 d=0.5*dir.normalize()*(double)b1[i].wdt;

            b2.push_back(b1[i]);
            b2.back().pos=b2.back().pos-d;

            b2.push_back(b1[i]);
            b2.back().pos=b2.back().pos+d;
            }
         else b2.push_back(b1[i]);
         }
      else b2.push_back(b1[i]);
   b2.push_back(b1.back());

   std::vector<point_struct> s;

   // convert band to triangle strip
   for (unsigned int i=0; i<b2.size(); i++)
      {
      vec3 dir;

      if (i==0) dir=b2[i+1].pos-b2[i].pos;
      else if (i+1==b2.size()) dir=b2[i].pos-b2[i-1].pos;
      else dir=halfdir(b2[i].pos-b2[i-1].pos,b2[i+1].pos-b2[i].pos);
      dir=dir.normalize();

      vec3 right=dir.cross(b2[i].nrm);
      right=right.normalize();

      point_struct p1={b2[i].pos-0.5*right*(double)b2[i].wdt,b2[i].col};
      s.push_back(p1);

      point_struct p2={b2[i].pos+0.5*right*(double)b2[i].wdt,b2[i].col};
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

         vec3 p1=s[idx1].pos;
         vec3 p2=s[idx2].pos;
         vec3 p3=s[idx3].pos;

         if (p1!=p2 && p2!=p3 && p3!=p1)
            primitives_.push_back(new primitive_triangle(idx1,idx2,idx3,&vertices_));

         idx1=idx2;
         idx2=idx3;
         }
      }
   }

// add triangle fan to scene
void mini3D::fan(const std::vector<point_struct> &f)
   {
   unsigned int idx1,idx2,idx3;

   if (f.size()>2)
      {
      idx1=addvtx(f[0].pos,f[0].col);
      idx2=addvtx(f[1].pos,f[1].col);

      for (unsigned int i=2; i<f.size(); i++)
         {
         idx3=addvtx(f[i].pos,f[i].col);

         vec3 p1=f[idx1].pos;
         vec3 p2=f[idx2].pos;
         vec3 p3=f[idx3].pos;

         if (p1!=p2 && p2!=p3 && p3!=p1)
            primitives_.push_back(new primitive_triangle(idx1,idx2,idx3,&vertices_));

         idx2=idx3;
         }
      }
   }

// compute half direction between two vectors
vec3 mini3D::halfdir(vec3 dir1,vec3 dir2)
   {
   dir1=dir1.normalize();
   dir2=dir2.normalize();

   return(dir1+dir2);
   }

// add pre-multiplied vertex
unsigned int mini3D::addvtx(vec3 v,vec4f c)
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
   render_begin();
   for (unsigned int i=0; i<primitives_.size(); i++)
      {
      primitive *p=primitives_[i];

      if (primitive_line *pl=dynamic_cast<primitive_line*>(p))
         clip_line(&vertices_[pl->index1],&vertices_[pl->index2]);
      else if (primitive_triangle *pt=dynamic_cast<primitive_triangle*>(p))
         clip_triangle(&vertices_[pt->index1],&vertices_[pt->index2],&vertices_[pt->index3]);

      render_yield();
      }
   render_end();
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

// clip line segment in homogeneous clip space
void mini3D::clip(vec4 &a,const vec4 b,vec4 &ac,const vec4 bc,vec4 P)
   {
   double t;

   t=-P.dot(a)/(P.dot(b)-P.dot(a));

   a=a+(b-a)*t;
   ac=ac+(bc-ac)*t;
   }

// clip and render line using homogeneous clip coordinates
void mini3D::clip_line(primitive::vertex_struct *a,primitive::vertex_struct *b)
   {
   bool af=a->pos_post.z>-a->pos_post.w;
   bool bf=b->pos_post.z>-b->pos_post.w;

   bool ab=a->pos_post.z<a->pos_post.w;
   bool bb=b->pos_post.z<b->pos_post.w;

   if (af && bf && ab && bb)
      {
      vec4 pointa=a->pos_post;
      vec4 pointb=b->pos_post;
      vec4 pointac=a->col;
      vec4 pointbc=b->col;

      if (pointa.x<-pointa.w && pointb.x<-pointb.w) return;
      if (pointa.x>pointa.w && pointb.x>pointb.w) return;
      if (pointa.y<-pointa.w && pointb.y<-pointb.w) return;
      if (pointa.y>pointa.w && pointb.y>pointb.w) return;

      render_line(pointa,pointb,pointac,pointbc);
      }
   else if ((af || bf) && (ab || bb))
      {
      vec4 clipa=a->pos_post;
      vec4 clipb=b->pos_post;
      vec4 clipac=a->col;
      vec4 clipbc=b->col;

      if (!af) clip(clipa,clipb,clipac,clipbc,vec4(0,0,1,1));
      if (!bf) clip(clipb,clipa,clipbc,clipac,vec4(0,0,1,1));
      if (!ab) clip(clipa,clipb,clipac,clipbc,vec4(0,0,-1,1));
      if (!bb) clip(clipb,clipa,clipbc,clipac,vec4(0,0,-1,1));

      if (clipa.x<-clipa.w && clipb.x<-clipb.w) return;
      if (clipa.x>clipa.w && clipb.x>clipb.w) return;
      if (clipa.y<-clipa.w && clipb.y<-clipb.w) return;
      if (clipa.y>clipa.w && clipb.y>clipb.w) return;

      render_line(clipa,clipb,clipac,clipbc);
      }
   }

// clip and render triangle using homogeneous clip coordinates
void mini3D::clip_triangle(primitive::vertex_struct *a,primitive::vertex_struct *b,primitive::vertex_struct *c)
   {
   cliptri1(a->pos_post,b->pos_post,c->pos_post,
            a->col,b->col,c->col);
   }

// clip a triangle (resulting in one remaining triangle)
//  v0 is the contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip1tri1(vec4 v0,double d0,vec4 c0,
                              vec4 v1,double d1,vec4 c1,
                              vec4 v2,double d2,vec4 c2)
   {
   vec4 p1,p2;
   vec4 pc1,pc2;

   double t1,t2;

   t1=d0/(d0-d1);
   t2=d0/(d0-d2);

   p1=v0*(1.0-t1)+v1*t1;
   p2=v0*(1.0-t2)+v2*t2;

   pc1=c0*(1.0-t1)+c1*t1;
   pc2=c0*(1.0-t2)+c2*t2;

   cliptri2(v0,p1,p2,c0,pc1,pc2);
   }

// clip a triangle (resulting in two remaining triangles)
//  v0 is the non-contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip2tri1(vec4 v0,double d0,vec4 c0,
                              vec4 v1,double d1,vec4 c1,
                              vec4 v2,double d2,vec4 c2)
   {
   vec4 p1,p2;
   vec4 pc1,pc2;

   double t1,t2;

   t1=d0/(d0-d1);
   t2=d0/(d0-d2);

   p1=v0*(1.0-t1)+v1*t1;
   p2=v0*(1.0-t2)+v2*t2;

   pc1=c0*(1.0-t1)+c1*t1;
   pc2=c0*(1.0-t2)+c2*t2;

   cliptri2(v1,v2,p2,c1,c2,pc2);
   cliptri2(p2,p1,v1,pc2,pc1,c1);
   }

// clip a triangle in homogeneous clip coordinates so that -w<=z
//  2 cases: clipped triangle geometry consists of either 1 or 2 triangles
void mini3D::cliptri1(vec4 v0, // vertex v0
                      vec4 v1, // vertex v1
                      vec4 v2, // vertex v2
                      vec4 c0, // color c0
                      vec4 c1, // color c1
                      vec4 c2) // color c2
   {
   double d0,d1,d2;

   int ff;

   d0=-v0.z-v0.w;
   d1=-v1.z-v1.w;
   d2=-v2.z-v2.w;

   ff=0;

   if (d0<0.0) ff|=1;
   if (d1<0.0) ff|=2;
   if (d2<0.0) ff|=4;

   switch (ff)
      {
      // 1 clipped triangle
      case 1: clip1tri1(v0,d0,c0,
                        v1,d1,c1,
                        v2,d2,c2); break;
      case 2: clip1tri1(v1,d1,c1,
                        v0,d0,c0,
                        v2,d2,c2); break;
      case 4: clip1tri1(v2,d2,c2,
                        v0,d0,c0,
                        v1,d1,c1); break;

      // 2 clipped triangles
      case 6: clip2tri1(v0,d0,c0,
                        v1,d1,c1,
                        v2,d2,c2); break;
      case 5: clip2tri1(v1,d1,c1,
                        v0,d0,c0,
                        v2,d2,c2); break;
      case 3: clip2tri1(v2,d2,c2,
                        v0,d0,c0,
                        v1,d1,c1); break;

      // entire triangle
      case 7: cliptri2(v0,v1,v2,c0,c1,c2); break;
      }
   }

// clip a triangle (resulting in one remaining triangle)
//  v0 is the contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip1tri2(vec4 v0,double d0,vec4 c0,
                              vec4 v1,double d1,vec4 c1,
                              vec4 v2,double d2,vec4 c2)
   {
   vec4 p1,p2;
   vec4 pc1,pc2;

   double t1,t2;

   t1=d0/(d0-d1);
   t2=d0/(d0-d2);

   p1=v0*(1.0-t1)+v1*t1;
   p2=v0*(1.0-t2)+v2*t2;

   pc1=c0*(1.0-t1)+c1*t1;
   pc2=c0*(1.0-t2)+c2*t2;

   culltri(v0,p1,p2,c0,pc1,pc2);
   }

// clip a triangle (resulting in two remaining triangles)
//  v0 is the non-contained vertex
//  d is distance of the respective point to the clipping plane
inline void mini3D::clip2tri2(vec4 v0,double d0,vec4 c0,
                              vec4 v1,double d1,vec4 c1,
                              vec4 v2,double d2,vec4 c2)
   {
   vec4 p1,p2;
   vec4 pc1,pc2;

   double t1,t2;

   t1=d0/(d0-d1);
   t2=d0/(d0-d2);

   p1=v0*(1.0-t1)+v1*t1;
   p2=v0*(1.0-t2)+v2*t2;

   pc1=c0*(1.0-t1)+c1*t1;
   pc2=c0*(1.0-t2)+c2*t2;

   culltri(v1,v2,p2,c1,c2,pc2);
   culltri(p2,p1,v1,pc2,pc1,c1);
   }

// clip a triangle in homogeneous clip coordinates so that z<=w
//  2 cases: clipped triangle geometry consists of either 1 or 2 triangles
void mini3D::cliptri2(vec4 v0, // vertex v0
                      vec4 v1, // vertex v1
                      vec4 v2, // vertex v2
                      vec4 c0, // color c0
                      vec4 c1, // color c1
                      vec4 c2) // color c2
   {
   double d0,d1,d2;

   int ff;

   d0=v0.z-v0.w;
   d1=v1.z-v1.w;
   d2=v2.z-v2.w;

   ff=0;

   if (d0<0.0) ff|=1;
   if (d1<0.0) ff|=2;
   if (d2<0.0) ff|=4;

   switch (ff)
      {
      // 1 clipped triangle
      case 1: clip1tri2(v0,d0,c0,
                        v1,d1,c1,
                        v2,d2,c2); break;
      case 2: clip1tri2(v1,d1,c1,
                        v0,d0,c0,
                        v2,d2,c2); break;
      case 4: clip1tri2(v2,d2,c2,
                        v0,d0,c0,
                        v1,d1,c1); break;

      // 2 clipped triangles
      case 6: clip2tri2(v0,d0,c0,
                        v1,d1,c1,
                        v2,d2,c2); break;
      case 5: clip2tri2(v1,d1,c1,
                        v0,d0,c0,
                        v2,d2,c2); break;
      case 3: clip2tri2(v2,d2,c2,
                        v0,d0,c0,
                        v1,d1,c1); break;

      // entire triangle
      case 7: culltri(v0,v1,v2,c0,c1,c2); break;
      }
   }

// cull a triangle in homogeneous clip coordinates
void mini3D::culltri(vec4 v0, // vertex v0
                     vec4 v1, // vertex v1
                     vec4 v2, // vertex v2
                     vec4 c0, // color c0
                     vec4 c1, // color c1
                     vec4 c2) // color c2
   {
   if (v0.x<-v0.w && v1.x<-v1.w && v2.x<-v2.w) return;
   if (v0.x>v0.w && v1.x>v1.w && v2.x>v2.w) return;
   if (v0.y<-v0.w && v1.y<-v1.w && v2.y<-v2.w) return;
   if (v0.y>v0.w && v1.y>v1.w && v2.y>v2.w) return;

   render_triangle(v0,v1,v2,c0,c1,c2);
   }
