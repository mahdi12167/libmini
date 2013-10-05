// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "miniOGL.h"

#include "miniv3f.h"
#include "miniv4d.h"

#include "minimath.h"

#include "miniray.h"

void (*miniray::LOCK_CALLBACK)(void *data)=NULL;
void (*miniray::UNLOCK_CALLBACK)(void *data)=NULL;
void *miniray::LOCK_DATA=NULL;

// default constructor
miniray::miniray()
   {
   FRONT=BACK=NULL;

   CONFIGURE_MAXCHUNKSIZE_TRIANGLES=100;
   CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS=20;
   }

// destructor
miniray::~miniray()
   {clear();}

// clear triangle reference buffers
void miniray::clear()
   {
   clearbuffer();
   swapbuffer();
   clearbuffer();
   }

// clear back triangle reference buffer
void miniray::clearbuffer()
   {
   TRIANGLEREF *ref,*next;

   ref=BACK;

   while (ref!=NULL)
      {
      next=ref->next;
      delete ref;

      ref=next;
      }

   BACK=NULL;
   }

// add reference to triangles to the back buffer
void miniray::addtriangles(float **array,int index,int num,int stride,
                           miniv3d *scaling,miniv3d *offset,
                           int swapyz,miniwarpbase *warp,
                           int nonlin,miniv3d *crdgen,
                           int calcbounds)
   {
   int n;

   if (num<=CONFIGURE_MAXCHUNKSIZE_TRIANGLES) addtriangles_chunked(array,index,num,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);
   else
      {
      for (n=0; n<num-CONFIGURE_MAXCHUNKSIZE_TRIANGLES; n+=CONFIGURE_MAXCHUNKSIZE_TRIANGLES)
         {
         addtriangles_chunked(array,index,CONFIGURE_MAXCHUNKSIZE_TRIANGLES,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);
         index+=(3+stride)*CONFIGURE_MAXCHUNKSIZE_TRIANGLES;
         }

      addtriangles_chunked(array,index,num-n,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);
      }
   }

// add reference to triangles to the back buffer
void miniray::addtriangles_chunked(float **array,int index,int num,int stride,
                                   miniv3d *scaling,miniv3d *offset,
                                   int swapyz,miniwarpbase *warp,
                                   int nonlin,miniv3d *crdgen,
                                   int calcbounds)
   {
   TRIANGLEREF *ref;

   if (num==0) return;

   ref=new TRIANGLEREF;

   ref->array=array;
   ref->index=index;

   ref->num=num;
   ref->stride=stride;

   ref->isfan=0;

   if (scaling!=NULL) ref->scaling=*scaling;
   else ref->scaling=miniv3d(1.0,1.0,1.0);

   if (offset!=NULL) ref->offset=*offset;
   else ref->offset=miniv3d(0.0,0.0,0.0);

   ref->swapyz=swapyz;

   ref->warp=warp;

   ref->nonlin=nonlin;

   if (crdgen!=NULL) ref->crdgen=*crdgen;
   else ref->crdgen=miniv3d(0.0,0.0,0.0);

   if (calcbounds!=0) calcbound(ref);
   else ref->hasbound=0;

   ref->next=BACK;
   BACK=ref;
   }

// add reference to triangle fans to the back buffer
void miniray::addtrianglefans(float **array,int index,int num,int stride,
                              miniv3d *scaling,miniv3d *offset,
                              int swapyz,miniwarpbase *warp,
                              int nonlin,miniv3d *crdgen,
                              int calcbounds)
   {
   int n;

   int i,k;

   if (num<=CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS) addtrianglefans_chunked(array,index,num,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);
   else
      {
      for (n=0; n<num-CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS; n+=CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS)
         {
         addtrianglefans_chunked(array,index,CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);

         for (i=0; i<CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS; i++)
            {
            k=ftrc((*array)[index]+0.5f);
            index+=3+k*(3+stride);
            }
         }

      addtrianglefans_chunked(array,index,num-n,stride,scaling,offset,swapyz,warp,nonlin,crdgen,calcbounds);
      }
   }

// add reference to triangle fans to the back buffer
void miniray::addtrianglefans_chunked(float **array,int index,int num,int stride,
                                      miniv3d *scaling,miniv3d *offset,
                                      int swapyz,miniwarpbase *warp,
                                      int nonlin,miniv3d *crdgen,
                                      int calcbounds)
   {
   TRIANGLEREF *ref;

   if (num==0) return;

   ref=new TRIANGLEREF;

   ref->array=array;
   ref->index=index;

   ref->num=num;
   ref->stride=stride;

   ref->isfan=1;

   if (scaling!=NULL) ref->scaling=*scaling;
   else ref->scaling=miniv3d(1.0,1.0,1.0);

   if (offset!=NULL) ref->offset=*offset;
   else ref->offset=miniv3d(0.0,0.0,0.0);

   ref->swapyz=swapyz;

   ref->warp=warp;

   ref->nonlin=nonlin;

   if (crdgen!=NULL) ref->crdgen=*crdgen;
   else ref->crdgen=miniv3d(0.0,0.0,0.0);

   if (calcbounds!=0) calcbound(ref);
   else ref->hasbound=0;

   ref->next=BACK;
   BACK=ref;
   }

// swap front and back triangle reference buffer
void miniray::swapbuffer()
   {
   TRIANGLEREF *ref;

   lock();

   ref=FRONT;
   FRONT=BACK;
   BACK=ref;

   unlock();
   }

// swap and clear back triangle reference buffer
void miniray::swapandclear()
   {
   swapbuffer();
   clearbuffer();
   }

// shoot a ray and return the distance to the closest triangle
double miniray::shoot(const miniv3d &o,const miniv3d &d,double mindist)
   {
   double result;

   miniv3d dn;

   TRIANGLEREF *ref;

   miniwarpbase *lastwarp;

   miniv4d inv[3];
   miniv3d tra[3];

   miniv4d o1;
   miniv3d oi,di;

   lock();

   dn=d;
   dn.normalize();

   result=MAXFLOAT;

   oi=di=miniv3d(0.0);

   lastwarp=NULL;

   ref=FRONT;

   while (ref!=NULL)
      {
      if (ref->hasbound==0) calcbound(ref);

      if (ref->warp==NULL)
         {
         oi=o;
         di=dn;

         lastwarp=NULL;
         }
      else if (ref->warp!=lastwarp)
         if (ref->nonlin!=0)
            {
            oi=o;
            di=dn;

            lastwarp=ref->warp;
            }
         else
            {
            ref->warp->getinv(inv);
            o1=miniv4d(o.x,o.y,o.z,1.0);
            oi=miniv3d(inv[0]*o1,inv[1]*o1,inv[2]*o1);

            ref->warp->gettra(tra);
            di=miniv3d(tra[0]*dn,tra[1]*dn,tra[2]*dn); // warp matrix is assumed to be ortho-normal

            lastwarp=ref->warp;
            }

      if (itest_ray_sphere(oi,di,ref->b,ref->r2)!=0)
         if (itest_ray_bbox(oi,di,ref->b,ref->r)!=0)
            result=calcdist(ref,oi,di,result,mindist);

      ref=ref->next;
      }

   unlock();

   return(result);
   }

// extract triangles that [possibly] intersect a plane
minidyna<miniv3d> miniray::extract(const miniv3d &o,const miniv3d &n,double radius)
   {
   minidyna<miniv3d> result;

   miniv3d dn;

   TRIANGLEREF *ref;

   miniwarpbase *lastwarp;

   miniv4d inv[3];
   miniv3d tra[3];

   miniv4d o1;
   miniv3d oi,di;

   lock();

   dn=n;
   dn.normalize();

   oi=di=miniv3d(0.0);

   lastwarp=NULL;

   ref=FRONT;

   while (ref!=NULL)
      {
      if (ref->hasbound==0) calcbound(ref);

      if (ref->warp==NULL)
         {
         oi=o;
         di=dn;

         lastwarp=NULL;
         }
      else if (ref->warp!=lastwarp)
         if (ref->nonlin!=0)
            {
            oi=o;
            di=dn;

            lastwarp=ref->warp;
            }
         else
            {
            ref->warp->getinv(inv);
            o1=miniv4d(o.x,o.y,o.z,1.0);
            oi=miniv3d(inv[0]*o1,inv[1]*o1,inv[2]*o1);

            ref->warp->gettra(tra);
            di=miniv3d(tra[0]*dn,tra[1]*dn,tra[2]*dn); // warp matrix is assumed to be ortho-normal

            lastwarp=ref->warp;
            }

      if (itest_plane_sphere(oi,di,radius,ref->b,ref->r2)!=0) result.append(calcmesh(ref));

      ref=ref->next;
      }

   unlock();

   return(result);
   }

// get triangle bounds
void miniray::getbounds(miniv3d &bmin,miniv3d &bmax)
   {
   TRIANGLEREF *ref;

   miniv3d p;
   double r;

   miniv3d b1,b2;

   lock();

   b1=miniv3d(MAXFLOAT);
   b2=miniv3d(-MAXFLOAT);

   ref=FRONT;

   while (ref!=NULL)
      {
      if (ref->hasbound==0) calcbound(ref);

      p=calcpoint(ref,ref->b);
      r=sqrt(ref->r2);

      if (p.x-r<b1.x) b1.x=p.x-r;
      if (p.x+r>b2.x) b2.x=p.x+r;

      if (p.y-r<b1.y) b1.y=p.y-r;
      if (p.y+r>b2.y) b2.y=p.y+r;

      if (p.z-r<b1.z) b1.z=p.z-r;
      if (p.z+r>b2.z) b2.z=p.z+r;

      ref=ref->next;
      }

   unlock();

   bmin=b1;
   bmax=b2;
   }

// set locking callbacks
void miniray::setcallbacks(void (*lock)(void *data),void *data,
                           void (*unlock)(void *data))
   {
   LOCK_CALLBACK=lock;
   UNLOCK_CALLBACK=unlock;
   LOCK_DATA=data;
   }

// lock ray shooting
void miniray::lock()
   {if (LOCK_CALLBACK!=NULL) LOCK_CALLBACK(LOCK_DATA);}

// unlock ray shooting
void miniray::unlock()
   {if (LOCK_CALLBACK!=NULL) UNLOCK_CALLBACK(LOCK_DATA);}

// calculate bounding box and sphere
void miniray::calcbound(TRIANGLEREF *ref)
   {
   int i,j,k;

   float *array;
   int num,stride;

   miniv3d v;
   miniv3d vmin,vmax;

   miniv3d corner[8];

   vmin.x=vmin.y=vmin.z=MAXFLOAT;
   vmax.x=vmax.y=vmax.z=-MAXFLOAT;

   if (ref->warp!=NULL && ref->nonlin!=0)
      {
      ref->warp->getcorners(corner);

      for (i=0; i<8; i++)
         {
         if (corner[i].x<vmin.x) vmin.x=corner[i].x;
         if (corner[i].y<vmin.y) vmin.y=corner[i].y;
         if (corner[i].z<vmin.z) vmin.z=corner[i].z;

         if (corner[i].x>vmax.x) vmax.x=corner[i].x;
         if (corner[i].y>vmax.y) vmax.y=corner[i].y;
         if (corner[i].z>vmax.z) vmax.z=corner[i].z;
         }
      }
   else
      {
      array=*(ref->array)+ref->index;
      num=ref->num;
      stride=ref->stride;

      if (ref->isfan==0)
         if (ref->swapyz==0)
            for (i=0; i<3*num; i++)
               {
               v.x=*array++;
               v.y=*array++;
               v.z=*array++;

               array+=stride;

               if (v.x<vmin.x) vmin.x=v.x;
               if (v.y<vmin.y) vmin.y=v.y;
               if (v.z<vmin.z) vmin.z=v.z;

               if (v.x>vmax.x) vmax.x=v.x;
               if (v.y>vmax.y) vmax.y=v.y;
               if (v.z>vmax.z) vmax.z=v.z;
               }
         else
            for (i=0; i<3*num; i++)
               {
               v.x=*array++;
               v.z=*array++;
               v.y=*array++;

               array+=stride;

               if (v.x<vmin.x) vmin.x=v.x;
               if (v.y<vmin.y) vmin.y=v.y;
               if (v.z<vmin.z) vmin.z=v.z;

               if (v.x>vmax.x) vmax.x=v.x;
               if (v.y>vmax.y) vmax.y=v.y;
               if (v.z>vmax.z) vmax.z=v.z;
               }
      else
         if (ref->swapyz==0)
            for (i=0; i<num; i++)
               {
               k=ftrc(*array+0.5f);

               array+=3;

               for (j=0; j<k; j++)
                  {
                  v.x=*array++;
                  v.y=*array++;
                  v.z=*array++;

                  array+=stride;

                  if (v.x<vmin.x) vmin.x=v.x;
                  if (v.y<vmin.y) vmin.y=v.y;
                  if (v.z<vmin.z) vmin.z=v.z;

                  if (v.x>vmax.x) vmax.x=v.x;
                  if (v.y>vmax.y) vmax.y=v.y;
                  if (v.z>vmax.z) vmax.z=v.z;
                  }
               }
         else
            for (i=0; i<num; i++)
               {
               k=ftrc(*array+0.5f);

               array+=3;

               for (j=0; j<k; j++)
                  {
                  v.x=*array++;
                  v.z=*array++;
                  v.y=*array++;

                  array+=stride;

                  if (v.x<vmin.x) vmin.x=v.x;
                  if (v.y<vmin.y) vmin.y=v.y;
                  if (v.z<vmin.z) vmin.z=v.z;

                  if (v.x>vmax.x) vmax.x=v.x;
                  if (v.y>vmax.y) vmax.y=v.y;
                  if (v.z>vmax.z) vmax.z=v.z;
                  }
               }

      vmin.x=vmin.x*ref->scaling.x+ref->offset.x;
      vmin.y=vmin.y*ref->scaling.y+ref->offset.y;
      vmin.z=vmin.z*ref->scaling.z+ref->offset.z;

      vmax.x=vmax.x*ref->scaling.x+ref->offset.x;
      vmax.y=vmax.y*ref->scaling.y+ref->offset.y;
      vmax.z=vmax.z*ref->scaling.z+ref->offset.z;
      }

   ref->b=0.5*(vmin+vmax);

   ref->r=miniv3d(0.5*dabs(vmax.x-vmin.x),
                  0.5*dabs(vmax.y-vmin.y),
                  0.5*dabs(vmax.z-vmin.z));

   ref->r2=0.25*(dsqr(vmax.x-vmin.x)+
                 dsqr(vmax.y-vmin.y)+
                 dsqr(vmax.z-vmin.z));

   ref->hasbound=1;
   }

// calculate smallest hit distance
double miniray::calcdist(const TRIANGLEREF *ref,
                         const miniv3d &o,const miniv3d &d,
                         double dist,double mindist)
   {
   int i,j,k;

   double result;

   float *array;
   int num,stride;

   miniv3d v1,v2,v3;

   miniwarpbase *warp;

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

   if (ref->warp!=NULL && ref->nonlin!=0) warp=ref->warp;
   else warp=NULL;

   result=dist;

   if (ref->isfan==0)
      if (ref->swapyz==0)
         for (i=0; i<num; i++)
            {
            v1.x=*array++;
            v1.y=*array++;
            v1.z=*array++;

            array+=stride;

            v2.x=*array++;
            v2.y=*array++;
            v2.z=*array++;

            array+=stride;

            v3.x=*array++;
            v3.y=*array++;
            v3.z=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               v3=warp->triwarp(v3,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;
               }

            dist=ray_triangle_dist(o,d,v1,v2,v3);

            if (dist>=mindist) result=dmin(result,dist);
            }
      else
         for (i=0; i<num; i++)
            {
            v1.x=*array++;
            v1.z=*array++;
            v1.y=*array++;

            array+=stride;

            v2.x=*array++;
            v2.z=*array++;
            v2.y=*array++;

            array+=stride;

            v3.x=*array++;
            v3.z=*array++;
            v3.y=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               v3=warp->triwarp(v3,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;
               }

            dist=ray_triangle_dist(o,d,v1,v2,v3);

            if (dist>=mindist) result=dmin(result,dist);
            }
   else
      if (ref->swapyz==0)
         for (i=0; i<num; i++)
            {
            k=ftrc(*array+0.5f);

            array+=3;

            v1.x=*array++;
            v1.y=*array++;
            v1.z=*array++;

            array+=stride;

            v2.x=*array++;
            v2.y=*array++;
            v2.z=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;
               }

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.y=*array++;
               v3.z=*array++;

               array+=stride;

               if (warp!=NULL) v3=warp->triwarp(v3,ref->crdgen);
               else
                  {
                  v3.x=v3.x*ref->scaling.x+ref->offset.x;
                  v3.y=v3.y*ref->scaling.y+ref->offset.y;
                  v3.z=v3.z*ref->scaling.z+ref->offset.z;
                  }

               dist=ray_triangle_dist(o,d,v1,v2,v3);

               if (dist>=mindist) result=dmin(result,dist);

               v2=v3;
               }
            }
      else
         for (i=0; i<num; i++)
            {
            k=ftrc(*array+0.5f);

            array+=3;

            v1.x=*array++;
            v1.z=*array++;
            v1.y=*array++;

            array+=stride;

            v2.x=*array++;
            v2.z=*array++;
            v2.y=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;
               }

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.z=*array++;
               v3.y=*array++;

               array+=stride;

               if (warp!=NULL) v3=warp->triwarp(v3,ref->crdgen);
               else
                  {
                  v3.x=v3.x*ref->scaling.x+ref->offset.x;
                  v3.y=v3.y*ref->scaling.y+ref->offset.y;
                  v3.z=v3.z*ref->scaling.z+ref->offset.z;
                  }

               dist=ray_triangle_dist(o,d,v1,v2,v3);

               if (dist>=mindist) result=dmin(result,dist);

               v2=v3;
               }
            }

   return(result);
   }

// calculate triangle mesh
minidyna<miniv3d> miniray::calcmesh(const TRIANGLEREF *ref)
   {
   int i,j,k;

   minidyna<miniv3d> result;

   float *array;
   int num,stride;

   miniv3d v1,v2,v3;

   miniwarpbase *warp;

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

   if (ref->warp!=NULL && ref->nonlin!=0) warp=ref->warp;
   else warp=NULL;

   if (ref->isfan==0)
      if (ref->swapyz==0)
         for (i=0; i<num; i++)
            {
            v1.x=*array++;
            v1.y=*array++;
            v1.z=*array++;

            array+=stride;

            v2.x=*array++;
            v2.y=*array++;
            v2.z=*array++;

            array+=stride;

            v3.x=*array++;
            v3.y=*array++;
            v3.z=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               v3=warp->triwarp(v3,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;
               }

            result.append(calcpoint(ref,v1));
            result.append(calcpoint(ref,v2));
            result.append(calcpoint(ref,v3));
            }
      else
         for (i=0; i<num; i++)
            {
            v1.x=*array++;
            v1.z=*array++;
            v1.y=*array++;

            array+=stride;

            v2.x=*array++;
            v2.z=*array++;
            v2.y=*array++;

            array+=stride;

            v3.x=*array++;
            v3.z=*array++;
            v3.y=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               v3=warp->triwarp(v3,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;
               }

            result.append(calcpoint(ref,v1));
            result.append(calcpoint(ref,v2));
            result.append(calcpoint(ref,v3));
            }
   else
      if (ref->swapyz==0)
         for (i=0; i<num; i++)
            {
            k=ftrc(*array+0.5f);

            array+=3;

            v1.x=*array++;
            v1.y=*array++;
            v1.z=*array++;

            array+=stride;

            v2.x=*array++;
            v2.y=*array++;
            v2.z=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;
               }

            v1=calcpoint(ref,v1);
            v2=calcpoint(ref,v2);

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.y=*array++;
               v3.z=*array++;

               array+=stride;

               if (warp!=NULL) v3=warp->triwarp(v3,ref->crdgen);
               else
                  {
                  v3.x=v3.x*ref->scaling.x+ref->offset.x;
                  v3.y=v3.y*ref->scaling.y+ref->offset.y;
                  v3.z=v3.z*ref->scaling.z+ref->offset.z;
                  }

               v3=calcpoint(ref,v3);

               result.append(v1);
               result.append(v2);
               result.append(v3);

               v2=v3;
               }
            }
      else
         for (i=0; i<num; i++)
            {
            k=ftrc(*array+0.5f);

            array+=3;

            v1.x=*array++;
            v1.z=*array++;
            v1.y=*array++;

            array+=stride;

            v2.x=*array++;
            v2.z=*array++;
            v2.y=*array++;

            array+=stride;

            if (warp!=NULL)
               {
               v1=warp->triwarp(v1,ref->crdgen);
               v2=warp->triwarp(v2,ref->crdgen);
               }
            else
               {
               v1.x=v1.x*ref->scaling.x+ref->offset.x;
               v1.y=v1.y*ref->scaling.y+ref->offset.y;
               v1.z=v1.z*ref->scaling.z+ref->offset.z;

               v2.x=v2.x*ref->scaling.x+ref->offset.x;
               v2.y=v2.y*ref->scaling.y+ref->offset.y;
               v2.z=v2.z*ref->scaling.z+ref->offset.z;
               }

            v1=calcpoint(ref,v1);
            v2=calcpoint(ref,v2);

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.z=*array++;
               v3.y=*array++;

               array+=stride;

               if (warp!=NULL) v3=warp->triwarp(v3,ref->crdgen);
               else
                  {
                  v3.x=v3.x*ref->scaling.x+ref->offset.x;
                  v3.y=v3.y*ref->scaling.y+ref->offset.y;
                  v3.z=v3.z*ref->scaling.z+ref->offset.z;
                  }

               v3=calcpoint(ref,v3);

               result.append(v1);
               result.append(v2);
               result.append(v3);

               v2=v3;
               }
            }

   return(result);
   }

// calculate triangle mesh point
miniv3d miniray::calcpoint(const TRIANGLEREF *ref,const miniv3d &p)
   {
   miniwarpbase *warp;

   warp=ref->warp;

   if (warp==NULL) return(p);
   if (ref->nonlin!=0) return(warp->triwarp(p,ref->crdgen));
   return(warp->linwarp(p));
   }

// configuring:

void miniray::configure_maxchunksize_triangles(int maxchunksize)
   {CONFIGURE_MAXCHUNKSIZE_TRIANGLES=maxchunksize;}

void miniray::configure_maxchunksize_trianglefans(int maxchunksize)
   {CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS=maxchunksize;}
