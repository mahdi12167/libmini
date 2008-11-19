// (c) by Stefan Roettger

#include "minibase.h"

#include "miniOGL.h"

#include "miniv3f.h"
#include "miniv4d.h"

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
                           int swapyz,miniwarp *warp,
                           int calcbounds)
   {
   int n;

   if (num<=CONFIGURE_MAXCHUNKSIZE_TRIANGLES) addtriangles_chunked(array,index,num,stride,scaling,offset,swapyz,warp,calcbounds);
   else
      {
      for (n=0; n<num-CONFIGURE_MAXCHUNKSIZE_TRIANGLES; n+=CONFIGURE_MAXCHUNKSIZE_TRIANGLES)
         {
         addtriangles_chunked(array,index,CONFIGURE_MAXCHUNKSIZE_TRIANGLES,stride,scaling,offset,swapyz,warp,calcbounds);
         index+=(3+stride)*CONFIGURE_MAXCHUNKSIZE_TRIANGLES;
         }

      addtriangles_chunked(array,index,num-n,stride,scaling,offset,swapyz,warp,calcbounds);
      }
   }

// add reference to triangles to the back buffer
void miniray::addtriangles_chunked(float **array,int index,int num,int stride,
                                   miniv3d *scaling,miniv3d *offset,
                                   int swapyz,miniwarp *warp,
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

   if (calcbounds!=0) calcbound(ref);
   else ref->hasbound=0;

   ref->next=BACK;
   BACK=ref;
   }

// add reference to triangle fans to the back buffer
void miniray::addtrianglefans(float **array,int index,int num,int stride,
                              miniv3d *scaling,miniv3d *offset,
                              int swapyz,miniwarp *warp,
                              int calcbounds)
   {
   int n;

   int i,k;

   if (num<=CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS) addtrianglefans_chunked(array,index,num,stride,scaling,offset,swapyz,warp,calcbounds);
   else
      {
      for (n=0; n<num-CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS; n+=CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS)
         {
         addtrianglefans_chunked(array,index,CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS,stride,scaling,offset,swapyz,warp,calcbounds);

         for (i=0; i<CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS; i++)
            {
            k=ftrc((*array)[index]+0.5f);
            index+=3+k*(3+stride);
            }
         }

      addtrianglefans_chunked(array,index,num-n,stride,scaling,offset,swapyz,warp,calcbounds);
      }
   }

// add reference to triangle fans to the back buffer
void miniray::addtrianglefans_chunked(float **array,int index,int num,int stride,
                                      miniv3d *scaling,miniv3d *offset,
                                      int swapyz,miniwarp *warp,
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

// shoot a ray and return the distance to the closest triangle
double miniray::shoot(const miniv3d &o,const miniv3d &d,double hitdist)
   {
   double result;

   miniv3d dn;

   TRIANGLEREF *ref;

   miniwarp *lastwarp;

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
         {
         ref->warp->getinv(inv);
         o1=miniv4d(o.x,o.y,o.z,1.0);
         oi=miniv3d(inv[0]*o1,inv[1]*o1,inv[2]*o1);

         ref->warp->gettra(tra);
         di=miniv3d(tra[0]*dn,tra[1]*dn,tra[2]*dn); // warp matrix is assumed to be ortho-normal

         lastwarp=ref->warp;
         }

      if (checkbound(oi,di,ref->b,ref->r2)!=0)
         if (checkbbox(oi,di,ref->b,ref->r)!=0)
            {
            result=calcdist(ref,oi,di,result);
            if (result<hitdist) break;
            }

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

   miniwarp *lastwarp;

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
         {
         ref->warp->getinv(inv);
         o1=miniv4d(o.x,o.y,o.z,1.0);
         oi=miniv3d(inv[0]*o1,inv[1]*o1,inv[2]*o1);

         ref->warp->gettra(tra);
         di=miniv3d(tra[0]*dn,tra[1]*dn,tra[2]*dn); // warp matrix is assumed to be ortho-normal

         lastwarp=ref->warp;
         }

      if (checkplane(oi,di,radius,ref->b,ref->r2)!=0) result.append(calcmesh(ref));

      ref=ref->next;
      }

   unlock();

   return(result);
   }

// get triangle bounds
void miniray::getbounds(miniv3d &bmin,miniv3d &bmax)
   {
   TRIANGLEREF *ref;

   miniwarp *lastwarp;

   miniv3d p;
   double r;

   miniv3d b1,b2;

   lock();

   b1=miniv3d(MAXFLOAT);
   b2=miniv3d(-MAXFLOAT);

   lastwarp=NULL;

   ref=FRONT;

   while (ref!=NULL)
      {
      if (ref->hasbound==0) calcbound(ref);

      p=calcpoint(ref,&lastwarp,ref->b);
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

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

   vmin.x=vmin.y=vmin.z=MAXFLOAT;
   vmax.x=vmax.y=vmax.z=-MAXFLOAT;

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

   ref->b=0.5*(vmin+vmax);

   ref->r=miniv3d(0.5*FABS(vmax.x-vmin.x),
                  0.5*FABS(vmax.y-vmin.y),
                  0.5*FABS(vmax.z-vmin.z));

   ref->r2=0.75*(FSQR(vmax.x-vmin.x)+
                 FSQR(vmax.y-vmin.y)+
                 FSQR(vmax.z-vmin.z));

   ref->hasbound=1;
   }

// calculate smallest hit distance
double miniray::calcdist(TRIANGLEREF *ref,
                         const miniv3d &o,const miniv3d &d,
                         double dist)
   {
   int i,j,k;

   double result;

   float *array;
   int num,stride;

   miniv3d v1,v2,v3;

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v3.x=v3.x*ref->scaling.x+ref->offset.x;
            v3.y=v3.y*ref->scaling.y+ref->offset.y;
            v3.z=v3.z*ref->scaling.z+ref->offset.z;

            dist=checkdist(o,d,v1,v2,v3);

            if (dist>0.0f) result=fmin(result,dist);
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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v3.x=v3.x*ref->scaling.x+ref->offset.x;
            v3.y=v3.y*ref->scaling.y+ref->offset.y;
            v3.z=v3.z*ref->scaling.z+ref->offset.z;

            dist=checkdist(o,d,v1,v2,v3);

            if (dist>0.0f) result=fmin(result,dist);
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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.y=*array++;
               v3.z=*array++;

               array+=stride;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;

               dist=checkdist(o,d,v1,v2,v3);

               if (dist>0.0f) result=fmin(result,dist);

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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.z=*array++;
               v3.y=*array++;

               array+=stride;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;

               dist=checkdist(o,d,v1,v2,v3);

               if (dist>0.0f) result=fmin(result,dist);

               v2=v3;
               }
            }

   return(result);
   }

// calculate triangle mesh point
miniv3d miniray::calcpoint(TRIANGLEREF *ref,miniwarp **lastwarp,miniv3d p)
   {
   static miniv4d mtx[3];
   static BOOLINT one;

   miniv4d p1;

   if (ref->warp!=*lastwarp)
      {
      if (ref->warp!=NULL)
         {
         ref->warp->getwarp(mtx);
         one=FALSE;
         }
      else one=TRUE;

      *lastwarp=ref->warp;
      }

   if (one) return(p);

   p1=miniv4d(p,1.0);
   return(miniv3d(mtx[0]*p1,mtx[1]*p1,mtx[2]*p1));
   }

// calculate triangle mesh
minidyna<miniv3d> miniray::calcmesh(TRIANGLEREF *ref)
   {
   int i,j,k;

   miniwarp *lastwarp;

   minidyna<miniv3d> result;

   float *array;
   int num,stride;

   miniv3d v1,v2,v3;

   lastwarp=NULL;

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v3.x=v3.x*ref->scaling.x+ref->offset.x;
            v3.y=v3.y*ref->scaling.y+ref->offset.y;
            v3.z=v3.z*ref->scaling.z+ref->offset.z;

            result.append(calcpoint(ref,&lastwarp,v1));
            result.append(calcpoint(ref,&lastwarp,v2));
            result.append(calcpoint(ref,&lastwarp,v3));
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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v3.x=v3.x*ref->scaling.x+ref->offset.x;
            v3.y=v3.y*ref->scaling.y+ref->offset.y;
            v3.z=v3.z*ref->scaling.z+ref->offset.z;

            result.append(calcpoint(ref,&lastwarp,v1));
            result.append(calcpoint(ref,&lastwarp,v2));
            result.append(calcpoint(ref,&lastwarp,v3));
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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v1=calcpoint(ref,&lastwarp,v1);
            v2=calcpoint(ref,&lastwarp,v2);

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.y=*array++;
               v3.z=*array++;

               array+=stride;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;

               v3=calcpoint(ref,&lastwarp,v3);

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

            v1.x=v1.x*ref->scaling.x+ref->offset.x;
            v1.y=v1.y*ref->scaling.y+ref->offset.y;
            v1.z=v1.z*ref->scaling.z+ref->offset.z;

            v2.x=v2.x*ref->scaling.x+ref->offset.x;
            v2.y=v2.y*ref->scaling.y+ref->offset.y;
            v2.z=v2.z*ref->scaling.z+ref->offset.z;

            v1=calcpoint(ref,&lastwarp,v1);
            v2=calcpoint(ref,&lastwarp,v2);

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.z=*array++;
               v3.y=*array++;

               array+=stride;

               v3.x=v3.x*ref->scaling.x+ref->offset.x;
               v3.y=v3.y*ref->scaling.y+ref->offset.y;
               v3.z=v3.z*ref->scaling.z+ref->offset.z;

               v3=calcpoint(ref,&lastwarp,v3);

               result.append(v1);
               result.append(v2);
               result.append(v3);

               v2=v3;
               }
            }

   return(result);
   }

// geometric ray/sphere intersection test
int miniray::checkbound(const miniv3d &o,const miniv3d &d,
                        const miniv3d &b,const double r2)
   {
   miniv3d bmo;
   double bmo2,bmod;

   bmo=b-o;
   bmo2=bmo*bmo;
   if (bmo2<r2) return(1);

   bmod=bmo*d;
   if (bmod<0.0) return(0);
   if (r2+bmod*bmod>bmo2) return(1);

   return(0);
   }

// geometric ray/bbox intersection test
int miniray::checkbbox(const miniv3d &o,const miniv3d &d,
                       const miniv3d &b,const miniv3d r)
   {
   double l;
   miniv3d h;

   if (d.x!=0.0)
      {
      l=(b.x+r.x-o.x)/d.x;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.y-b.y)<=r.y && FABS(h.z-b.z)<=r.z) return(1);
         }
      else if (d.x>0.0) return(0);

      l=(b.x-r.x-o.x)/d.x;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.y-b.y)<=r.y && FABS(h.z-b.z)<=r.z) return(1);
         }
      else if (d.x<0.0) return(0);
      }

   if (d.y!=0.0)
      {
      l=(b.y+r.y-o.y)/d.y;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.x-b.x)<=r.x && FABS(h.z-b.z)<=r.z) return(1);
         }
      else if (d.y>0.0) return(0);

      l=(b.y-r.y-o.y)/d.y;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.x-b.x)<=r.x && FABS(h.z-b.z)<=r.z) return(1);
         }
      else if (d.y<0.0) return(0);
      }

   if (d.z!=0.0)
      {
      l=(b.z+r.z-o.z)/d.z;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.x-b.x)<=r.x && FABS(h.y-b.y)<=r.y) return(1);
         }
      else if (d.z>0.0) return(0);

      l=(b.z-r.z-o.z)/d.z;
      if (l>0.0)
         {
         h=o+d*l;
         if (FABS(h.x-b.x)<=r.x && FABS(h.y-b.y)<=r.y) return(1);
         }
      else if (d.z<0.0) return(0);
      }

   return(0);
   }

// geometric plane/sphere intersection test
int miniray::checkplane(const miniv3d &o,const miniv3d &n,const double radius,
                        const miniv3d &b,const double r2)
   {
   miniv3d h;
   double l;

   h=b-o;
   l=h*n;

   if (l*l>r2) return(0); // no intersection
   if (h*h>2.0*(radius*radius+r2)) return(0); // no inclusion (approximate)

   return(1);
   }

// calculate hit distance
double miniray::checkdist(const miniv3d &o,const miniv3d &d,
                         const miniv3d &v1,const miniv3d &v2,const miniv3d &v3)
   {
   miniv3d tuv;

   if (intersect(o,d,v1,v2,v3,&tuv)==0) return(MAXFLOAT);
   else return(tuv.x);
   }

// Moeller-Trumbore ray/triangle intersection
int miniray::intersect(const miniv3d &o,const miniv3d &d,
                       const miniv3d &v0,const miniv3d &v1,const miniv3d &v2,
                       miniv3d *tuv)
   {
   static const double epsilon=1E-5;

   double t,u,v;
   miniv3d edge1,edge2,tvec,pvec,qvec;
   double det,inv_det;

   // find vectors for two edges sharing v0
   edge1=v1-v0;
   edge2=v2-v0;

   // begin calculating determinant - also used to calculate U parameter
   pvec=d/edge2;

   // if determinant is near zero, ray lies in plane of triangle
   det=edge1*pvec;

   // cull triangles with determinant near zero
   if (fabs(det)<epsilon) return(0);

   // calculate inverse determinant
   inv_det=1.0/det;

   // calculate distance from v0 to ray origin
   tvec=o-v0;

   // calculate U parameter and test bounds
   u=(tvec*pvec)*inv_det;
   if (u<0.0 || u>1.0) return(0);

   // prepare to test V parameter
   qvec=tvec/edge1;

   // calculate V parameter and test bounds
   v=(d*qvec)*inv_det;
   if (v<0.0 || u+v>1.0) return(0);

   // calculate t, ray intersects triangle
   t=(edge2*qvec)*inv_det;

   *tuv=miniv3f(t,u,v);

   return(1);
   }

// configuring:

void miniray::configure_maxchunksize_triangles(int maxchunksize)
   {CONFIGURE_MAXCHUNKSIZE_TRIANGLES=maxchunksize;}

void miniray::configure_maxchunksize_trianglefans(int maxchunksize)
   {CONFIGURE_MAXCHUNKSIZE_TRIANGLEFANS=maxchunksize;}
