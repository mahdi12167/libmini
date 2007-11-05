// (c) by Stefan Roettger

#include "minibase.h"

#include "miniOGL.h"

#include "miniray.h"

// default constructor
miniray::miniray()
   {FRONT=BACK=NULL;}

// destructor
miniray::~miniray()
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
                           miniv3f *scaling,miniv3f *offset,
                           int swapyz)
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
   else ref->scaling=miniv3f(1.0f,1.0f,1.0f);

   if (offset!=NULL) ref->offset=*offset;
   else ref->offset=miniv3f(0.0f,0.0f,0.0f);

   ref->swapyz=swapyz;

   calcbound(ref);

   ref->next=BACK;
   BACK=ref;
   }

// add reference to triangle fans to the back buffer
void miniray::addtrianglefans(float **array,int index,int num,int stride,
                              miniv3f *scaling,miniv3f *offset,
                              int swapyz)
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
   else ref->scaling=miniv3f(1.0f,1.0f,1.0f);

   if (offset!=NULL) ref->offset=*offset;
   else ref->offset=miniv3f(0.0f,0.0f,0.0f);

   ref->swapyz=swapyz;

   calcbound(ref);

   ref->next=BACK;
   BACK=ref;
   }

// swap front and back triangle reference buffer
void miniray::swapbuffer()
   {
   TRIANGLEREF *ref;

   ref=FRONT;
   FRONT=BACK;
   BACK=ref;
   }

// shoot a ray and return the distance to the closest triangle
float miniray::shoot(const miniv3f &o,const miniv3f &d)
   {
   float result;

   miniv3f dn;

   TRIANGLEREF *ref;

   dn=d;
   dn.normalize();

   result=MAXFLOAT;

   ref=FRONT;

   while (ref!=NULL)
      {
      if (checkbound(o,dn,ref->b,ref->r2)!=0) result=calcdist(ref,o,d,result);
      ref=ref->next;
      }

   return(result);
   }

void miniray::calcbound(TRIANGLEREF *ref)
   {
   int i,j,k;

   float *array;
   int num,stride;

   miniv3f v;
   miniv3f vmin,vmax;

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

   ref->b=(vmin+vmax)/2.0f;

   ref->r2=0.75f*(fsqr(vmax.x-vmin.x)+
                  fsqr(vmax.y-vmin.y)+
                  fsqr(vmax.z-vmin.z));
   }

float miniray::calcdist(TRIANGLEREF *ref,
                        const miniv3f &o,const miniv3f &d,
                        float dist)
   {
   int i,j,k;

   float result;

   float *array;
   int num,stride;

   miniv3f v1,v2,v3;

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

void miniray::renderwire()
   {
   TRIANGLEREF *ref;

   initstate();

   ref=FRONT;

   while (ref!=NULL)
      {
      renderwire(ref);
      ref=ref->next;
      }

   exitstate();
   }

void miniray::renderwire(TRIANGLEREF *ref)
   {
   int i,j,k;

   float *array;
   int num,stride;

   miniv3f v1,v2,v3;

   array=*(ref->array)+ref->index;
   num=ref->num;
   stride=ref->stride;

   mtxpush();
   mtxtranslate(ref->offset.x,ref->offset.y,ref->offset.z);
   mtxscale(ref->scaling.x,ref->scaling.y,ref->scaling.z);

   color(0.0f,0.0f,0.0f);

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

            drawline(v1.x,v1.y,v1.z,v2.x,v2.y,v2.z);
            drawline(v2.x,v2.y,v2.z,v3.x,v3.y,v3.z);
            drawline(v3.x,v3.y,v3.z,v1.x,v1.y,v1.z);
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

            drawline(v1.x,v1.y,v1.z,v2.x,v2.y,v2.z);
            drawline(v2.x,v2.y,v2.z,v3.x,v3.y,v3.z);
            drawline(v3.x,v3.y,v3.z,v1.x,v1.y,v1.z);
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

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.y=*array++;
               v3.z=*array++;

               array+=stride;

               drawline(v1.x,v1.y,v1.z,v2.x,v2.y,v2.z);
               drawline(v2.x,v2.y,v2.z,v3.x,v3.y,v3.z);
               drawline(v3.x,v3.y,v3.z,v1.x,v1.y,v1.z);

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

            for (j=2; j<k; j++)
               {
               v3.x=*array++;
               v3.z=*array++;
               v3.y=*array++;

               array+=stride;

               drawline(v1.x,v1.y,v1.z,v2.x,v2.y,v2.z);
               drawline(v2.x,v2.y,v2.z,v3.x,v3.y,v3.z);
               drawline(v3.x,v3.y,v3.z,v1.x,v1.y,v1.z);

               v2=v3;
               }
            }

   mtxpop();
   }

// geometric ray/sphere intersection test
int miniray::checkbound(const miniv3f &o,const miniv3f &d,
                        const miniv3f &b,const float r2)
   {
   miniv3f bmo;
   float bmo2,bmod;

   bmo=b-o;
   bmo2=bmo*bmo;
   if (bmo2<r2) return(1);

   bmod=bmo*d;
   if (bmod<0.0f) return(0);
   if (r2+bmod*bmod>bmo2) return(1);

   return(0);
   }

float miniray::checkdist(const miniv3f &o,const miniv3f &d,
                         const miniv3f &v1,const miniv3f &v2,const miniv3f &v3)
   {
   miniv3f tuv;

   if (intersect(o,d,v1,v2,v3,&tuv)==0) return(MAXFLOAT);
   else return(tuv.x);
   }

// Moeller-Trumbore ray/triangle intersection
int miniray::intersect(const miniv3f &o,const miniv3f &d,
                       const miniv3f &v0,const miniv3f &v1,const miniv3f &v2,
                       miniv3f *tuv)
   {
   static const float EPSILON=0.000001f;

   float t,u,v;
   miniv3f edge1,edge2,tvec,pvec,qvec;
   float det,inv_det;

   /* find vectors for two edges sharing v0 */
   edge1=v1-v0;
   edge2=v2-v0;

   /* begin calculating determinant - also used to calculate U parameter */
   pvec=d/edge2;

   /* if determinant is near zero, ray lies in plane of triangle */
   det=edge1*pvec;

   /* cull triangles with determinant near zero */
   if (fabs(det)<EPSILON) return(0);

   /* calculate inverse determinant */
   inv_det=1.0f/det;

   /* calculate distance from v0 to ray origin */
   tvec=o-v0;

   /* calculate U parameter and test bounds */
   u=(tvec*pvec)*inv_det;
   if (u<0.0f || u>1.0f) return(0);

   /* prepare to test V parameter */
   qvec=tvec/edge1;

   /* calculate V parameter and test bounds */
   v=(d*qvec)*inv_det;
   if (v<0.0f || u+v>1.0f) return(0);

   /* calculate t, ray intersects triangle */
   t=(edge2*qvec)*inv_det;

   *tuv=miniv3f(t,u,v);

   return(1);
   }
