// (c) by Stefan Roettger

#include "miniOGL.h"

#include "miniproj.h"

// default constructor
miniproj::miniproj()
   {
   DIM=0;

   MAXE=0.0f;
   MAXL=0.0f;

   TEXID=0;
   }

// destructor
miniproj::~miniproj()
   {if (TEXID!=0) deletetexmap(TEXID);}

// calculate whether or not a triangle is front- or back-facing
inline BOOLINT miniproj::isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e)
   {
   miniv3d n;

   n=(v2-v1)/(v3-v1);

   return((n*(v1-e)>0.0)?n*(p-v1)<0.0:n*(p-v1)>0.0);
   }

// calculate the intersection of a ray with a plane
inline double miniproj::intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m)
   {
   miniv3d n;

   double lambda;

   n=d1/d2;

   lambda=n*d;
   if (lambda!=0.0) lambda=n*(o-p)/lambda;

   m=p+lambda*d;

   return(lambda);
   }

// draw a triangle fan with 3 triangles
inline void miniproj::draw3fan(const miniv3d &v1,const double c1,
                               const miniv3d &v2,const double c2,
                               const miniv3d &v3,const double c3,
                               const miniv3d &v4,const double c4,
                               const miniv3d &eye)
   {
   miniv3d d1,d2,m,n;

   double lambda,
          w1,w2,w3,
          c234;

   d1=v3-v2;
   d2=v4-v2;

   // calculate thick vertex
   lambda=intersect(v1,v1-eye,v2,d1,d2,m);

   n=d1/d2;

   n.x=FABS(n.x);
   n.y=FABS(n.y);
   n.z=FABS(n.z);

   // barycentric interpolation
   if (n.x>n.y)
      if (n.x>n.z)
         {
         w1=FABS((v3.y-m.y)*(v4.z-m.z)-(v3.z-m.z)*(v4.y-m.y));
         w2=FABS((v2.y-m.y)*(v4.z-m.z)-(v2.z-m.z)*(v4.y-m.y));
         w3=FABS((v2.y-m.y)*(v3.z-m.z)-(v2.z-m.z)*(v3.y-m.y));

         c234=(w1*c2+w2*c3+w3*c4)/n.x;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }
   else
      if (n.y>n.z)
         {
         w1=FABS((v3.z-m.z)*(v4.x-m.x)-(v3.x-m.x)*(v4.z-m.z));
         w2=FABS((v2.z-m.z)*(v4.x-m.x)-(v2.x-m.x)*(v4.z-m.z));
         w3=FABS((v2.z-m.z)*(v3.x-m.x)-(v2.x-m.x)*(v3.z-m.z));

         c234=(w1*c2+w2*c3+w3*c4)/n.y;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }

   beginfan();

   // render thick vertex
   if (lambda<0.0)
      {
      lambda=(v1-m).getlength();
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);
      }
   else
      {
      lambda=(m-v1).getlength();
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);
      }

   // render silhouette vertices
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   texcoord(c3,c3,0.0f);
   fanvertex(v3.x,v3.y,v3.z);
   texcoord(c4,c4,0.0f);
   fanvertex(v4.x,v4.y,v4.z);
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   }

// draw a triangle fan with 4 triangles
void miniproj::draw4fan(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &eye)
   {
   miniv3d m1,m2,d;

   double lambda,
          alpha,beta,
          c12,c34;

   // calculate thick vertex (first intersection)
   alpha=intersect(v1,v2-v1,eye,v3-eye,v4-eye,m1);

   // calculate thick vertex (second intersection)
   beta=intersect(v3,v4-v3,eye,v1-eye,v2-eye,m2);

   c12=(1.0f-alpha)*c1+alpha*c2;
   c34=(1.0f-beta)*c3+beta*c4;

   d=m2-m1;

   lambda=d.getlength();

   beginfan();

   // render thick vertex
   if (d*(m1-eye)<0.0f)
      {
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);
      }
   else
      {
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);
      }

   // render silhouette vertices
   texcoord(c1,c1,0.0f);
   fanvertex(v1.x,v1.y,v1.z);
   texcoord(c3,c3,0.0f);
   fanvertex(v3.x,v3.y,v3.z);
   texcoord(c2,c2,0.0f);
   fanvertex(v2.x,v2.y,v2.z);
   texcoord(c4,c4,0.0f);
   fanvertex(v4.x,v4.y,v4.z);
   texcoord(c1,c1,0.0f);
   fanvertex(v1.x,v1.y,v1.z);
   }

// project a tetrahedron
// needs to be in front of the near plane
void miniproj::proj(const miniv3d &v1,const double c1,
                    const miniv3d &v2,const double c2,
                    const miniv3d &v3,const double c3,
                    const miniv3d &v4,const double c4,
                    const miniv3d &col,
                    const miniv3d &eye)
   {
   int ff;

   if (c1==0.0 && c2==0.0 && c3==0.0 && c4==0.0) return;

   color(col.x,col.y,col.z);

   ff=0;

   if (isfront(v4,v1,v2,v3,eye)) ff|=1;
   if (isfront(v3,v1,v4,v2,eye)) ff|=2;
   if (isfront(v1,v2,v4,v3,eye)) ff|=4;
   if (isfront(v2,v3,v4,v1,eye)) ff|=8;

   switch (ff)
      {
      case 1: case 14: draw3fan(v4,c4,v1,c1,v2,c2,v3,c3,eye); break;
      case 2: case 13: draw3fan(v3,c3,v1,c1,v2,c2,v4,c4,eye); break;
      case 3: case 12: draw4fan(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
      case 4: case 11: draw3fan(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
      case 5: case 10: draw4fan(v1,c1,v4,c4,v2,c2,v3,c3,eye); break;
      case 6: case  9: draw4fan(v1,c1,v3,c3,v2,c2,v4,c4,eye); break;
      case 7: case  8: draw3fan(v2,c2,v1,c1,v3,c3,v4,c4,eye); break;
      }
   }

// build opacity lookup table
int miniproj::gentexmap(int dim,float maxe,float maxl)
   {
   int i,j;

   float e,l;

   unsigned char *image,*ptr;

   int texid;

   if (dim<2) ERRORMSG();
   if ((dim&(dim-1))!=0) ERRORMSG();

   if (maxe<=0.0f || maxl<=0.0f) ERRORMSG();

   if ((image=(unsigned char *)malloc(2*sqr(dim)))==NULL) ERRORMSG();

   ptr=image;

   for (j=0; j<dim; j++)
      for (i=0; i<dim; i++)
         {
         e=i*maxe/(dim-1);
         l=j*maxl/(dim-1);

         *ptr++=255;
         *ptr++=ftrc(255.0f*(1.0f-fexp(-e*l))+0.5f);
         }

   texid=buildLAtexmap(image,&dim,&dim);
   free(image);

   return(texid);
   }

// initialize projection state
void miniproj::initproj(int dim,float maxe,float maxl)
   {
   float mtx[16]={0.5f,0.5f,0,0,
                  0,0,1.0f/maxl,0,
                  0,0,0,0,
                  0,0,0,0};

   if (dim<2 || maxe<=0.0f || maxl<=0.0f) ERRORMSG();

   if (dim!=DIM || maxe!=MAXE || maxl!=MAXL)
      {
      DIM=dim;

      MAXE=maxe;
      MAXL=maxl;

      if (TEXID!=0) deletetexmap(TEXID);
      TEXID=gentexmap(dim,maxe,maxl);
      }

   initstate();
   disableculling();
   enableblending();
   disableZwriting();

   mtxtex();
   mtxpush();
   mtxid();
   mtxtranslate(0.5f/DIM,0.5f/DIM,0.0f);
   mtxscale((float)(DIM-1)/DIM,(float)(DIM-1)/DIM,0.0f);
   mtxmult(mtx);
   mtxmodel();

   bindtexmap(TEXID,0,0,0,0);

   beginfans();
   }

// de-initialize projection state
void miniproj::exitproj()
   {
   endfans();

   bindtexmap(0,0,0,0,0);

   mtxtex();
   mtxpop();
   mtxmodel();

   enableZwriting();
   enableBFculling();
   disableblending();
   exitstate();
   }
