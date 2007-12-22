// (c) by Stefan Roettger

#include "minibase.h"

#include "miniOGL.h"

#include "minicache.h"

// there is only one cache
minicache *minicache::CACHE;

// default constructor
minicache::minicache()
   {
   int i;

   CACHE=NULL;

   TERRAIN=NULL;
   NUMTERRAIN=MAXTERRAIN=0;

   CACHE_ID=0;
   RENDER_ID=0;

   OPACITY=1.0f;

   SEA_R=0.0f;
   SEA_G=0.5f;
   SEA_B=1.0f;
   SEA_A=1.0f;

   PRISM_R=PRISM_G=PRISM_B=1.0f;
   PRISM_A=0.9f;

   PRISMEDGE_CALLBACK=NULL;
   PRISMCACHE_CALLBACK=NULL;
   PRISMRENDER_CALLBACK=NULL;
   PRISMTRIGGER_CALLBACK=NULL;
   PRISMSYNC_CALLBACK=NULL;
   CALLBACK_DATA=NULL;

   VTXPROG=NULL;
   VTXDIRTY=0;

   VTXPROGID=0;
   USEVTXSHADER=0;

   for (i=0; i<8; i++)
      {
      VTXSHADERPAR1[i]=0.0f;
      VTXSHADERPAR2[i]=0.0f;
      VTXSHADERPAR3[i]=0.0f;
      VTXSHADERPAR4[i]=0.0f;
      }

   FRAGPROG=NULL;
   FRAGDIRTY=0;

   FRAGPROGID=0;
   USEPIXSHADER=0;

   for (i=0; i<8; i++)
      {
      PIXSHADERPAR1[i]=0.0f;
      PIXSHADERPAR2[i]=0.0f;
      PIXSHADERPAR3[i]=0.0f;
      PIXSHADERPAR4[i]=0.0f;
      }

   PIXSHADERTEXID=0;

   SEAPROG=NULL;
   SEADIRTY=0;

   SEAPROGID=0;
   USESEASHADER=0;

   for (i=0; i<8; i++)
      {
      SEASHADERPAR1[i]=0.0f;
      SEASHADERPAR2[i]=0.0f;
      SEASHADERPAR3[i]=0.0f;
      SEASHADERPAR4[i]=0.0f;
      }

   SEASHADERTEXID=0;

   PRISMCACHE_VTXPROGID=0;

   PRESEA_CB=NULL;
   POSTSEA_CB=NULL;

   GLSETUP=0;
   WGLSETUP=0;

   GLEXT_MT=0;
   GLEXT_VP=0;
   GLEXT_FP=0;

   CONFIGURE_OVERLAP=0.02f;
   CONFIGURE_MINSIZE=33;
   CONFIGURE_SEATWOSIDED=1;
   CONFIGURE_SEAENABLETEX=0;
   CONFIGURE_ZSCALE_SEA=0.99f;
   CONFIGURE_ZSCALE_PRISMS=0.95f;
   CONFIGURE_ENABLERAY=0;
   }

// destructor
minicache::~minicache()
   {
   int id;

   if (TERRAIN!=NULL)
      {
      for (id=0; id<MAXTERRAIN; id++)
         if (TERRAIN[id].tile!=NULL) freeterrain(&TERRAIN[id]);

      free(TERRAIN);
      }

#ifndef NOOGL

   GLuint progid;

   if (VTXPROG!=NULL) free(VTXPROG);
   if (FRAGPROG!=NULL) free(FRAGPROG);
   if (SEAPROG!=NULL) free(SEAPROG);

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      {
      if (VTXPROGID!=0)
         {
         progid=VTXPROGID;
         glDeleteProgramsARB(1,&progid);
         }

      if (FRAGPROGID!=0)
         {
         progid=FRAGPROGID;
         glDeleteProgramsARB(1,&progid);
         }

      if (SEAPROGID!=0)
         {
         progid=SEAPROGID;
         glDeleteProgramsARB(1,&progid);
         }

      if (PRISMCACHE_VTXPROGID!=0)
         {
         progid=PRISMCACHE_VTXPROGID;
         glDeleteProgramsARB(1,&progid);
         }
      }

#endif

   if (PIXSHADERTEXID!=0) deletetexmap(PIXSHADERTEXID);
   if (SEASHADERTEXID!=0) deletetexmap(SEASHADERTEXID);

#endif
   }

// initialize terrain
void minicache::initterrain(TERRAIN_TYPE *t)
   {
   t->cache_num=1;

   t->cache_size1=0;
   t->cache_size2=0;

   t->cache_maxsize=1;

   if ((t->cache1_op=(unsigned char *)malloc(t->cache_maxsize))==NULL) ERRORMSG();
   if ((t->cache2_op=(unsigned char *)malloc(t->cache_maxsize))==NULL) ERRORMSG();

   if ((t->cache1_arg=(float *)malloc(3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();
   if ((t->cache2_arg=(float *)malloc(3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();

   t->cache_phase=-1;

   t->prism_size1=0;
   t->prism_size2=0;

   t->prism_maxsize=1;

   if ((t->prism_cache1=(float *)malloc(4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
   if ((t->prism_cache2=(float *)malloc(4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();

   t->ray=new miniray;
   t->first_fancnt=0;

   t->fancnt1=0;
   t->fancnt2=0;

   t->vtxcnt1=0;
   t->vtxcnt2=0;

   t->render_phase=-1;

   t->isvisible=1;
   }

// free terrain
void minicache::freeterrain(TERRAIN_TYPE *t)
   {
   free(t->cache1_op);
   free(t->cache2_op);

   free(t->cache1_arg);
   free(t->cache2_arg);

   free(t->prism_cache1);
   free(t->prism_cache2);

   delete t->ray;
   }

// static callback functions:

void minicache::cache_beginfan()
   {CACHE->cache(BEGINFAN_OP);}

void minicache::cache_fanvertex(float i,float y,float j)
   {CACHE->cache(FANVERTEX_OP,i,y,j);}

void minicache::cache_texmap(int m,int n,int S)
   {CACHE->cache(TEXMAP_OP,m,n,S);}

void minicache::cache_prismedge(float x,float y,float yf,float z)
   {CACHE->cacheprismedge(x,y,yf,z);}

void minicache::cache_trigger(int phase,float scale,float ex,float ey,float ez)
   {CACHE->cachetrigger(phase,scale,ex,ey,ez);}

void minicache::cache_sync(int id)
   {CACHE->cachesync(id);}

// caching functions:

void minicache::cache(int op,float a,float b,float c)
   {
   int cols,rows;
   float xdim,zdim;
   float centerx,centery,centerz;

   miniv3d s,o;

   TERRAIN_TYPE *t;

   float *ptr;

   t=&TERRAIN[CACHE_ID];

   // enlarge vertex cache
   if (t->cache_size1>=t->cache_maxsize || t->cache_size2>=t->cache_maxsize)
      {
      t->cache_maxsize*=2;

      if ((t->cache1_op=(unsigned char *)realloc(t->cache1_op,t->cache_maxsize))==NULL) ERRORMSG();
      if ((t->cache2_op=(unsigned char *)realloc(t->cache2_op,t->cache_maxsize))==NULL) ERRORMSG();

      if ((t->cache1_arg=(float *)realloc(t->cache1_arg,3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();
      if ((t->cache2_arg=(float *)realloc(t->cache2_arg,3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();
      }

   if (t->cache_num==1)
      // update vertex buffer #1
      {
      if (op==BEGINFAN_OP)
         {
         t->fancnt1++;
         t->first_fancnt++;
         t->last_beginfan=t->cache_size1;
         }
      else if (op==FANVERTEX_OP)
         {
         t->vtxcnt1++;
         t->cache1_arg[3*t->last_beginfan]++;
         }
      else
         // update ray object
         if (CONFIGURE_ENABLERAY!=0)
            {
            if (t->first_fancnt>0)
               {
               cols=t->tile->getcols();
               rows=t->tile->getrows();

               xdim=t->tile->getcoldim();
               zdim=t->tile->getrowdim();

               centerx=t->tile->getcenterx();
               centery=t->tile->getcentery();
               centerz=t->tile->getcenterz();

               s.x=xdim/(t->first_size-1);
               s.y=t->first_scale;
               s.z=-zdim/(t->first_size-1);

               o.x=xdim*(t->first_col-(cols-1)/2.0f)+centerx-xdim/2.0f;
               o.y=centery;
               o.z=zdim*(t->first_row-(rows-1)/2.0f)+centerz+zdim/2.0f;

               t->ray->addtrianglefans(&t->cache1_arg,3*t->first_beginfan,t->first_fancnt,0,&s,&o,0,t->tile->getwarp());
               }

            if (op==TRIGGER_OP)
               {
               if (t->cache_phase==0) t->ray->clearbuffer();
               else if (t->cache_phase==1) t->first_scale=b;
               else if (t->cache_phase==4) t->ray->swapbuffer();
               }
            else if (op==TEXMAP_OP)
               {
               t->first_col=ftrc(a+0.5f);
               t->first_row=ftrc(b+0.5f);
               t->first_size=ftrc(c+0.5f);
               }

            t->first_beginfan=t->cache_size1+1;
            t->first_fancnt=0;
            }

      t->cache1_op[t->cache_size1]=op;

      ptr=&t->cache1_arg[3*t->cache_size1++];
      }
   else
      // update vertex buffer #2
      {
      if (op==BEGINFAN_OP)
         {
         t->fancnt2++;
         t->first_fancnt++;
         t->last_beginfan=t->cache_size2;
         }
      else if (op==FANVERTEX_OP)
         {
         t->vtxcnt2++;
         t->cache2_arg[3*t->last_beginfan]++;
         }
      else
         // update ray object
         if (CONFIGURE_ENABLERAY!=0)
            {
            if (t->first_fancnt>0)
               {
               cols=t->tile->getcols();
               rows=t->tile->getrows();

               xdim=t->tile->getcoldim();
               zdim=t->tile->getrowdim();

               centerx=t->tile->getcenterx();
               centery=t->tile->getcentery();
               centerz=t->tile->getcenterz();

               s.x=xdim/(t->first_size-1);
               s.y=t->first_scale;
               s.z=-zdim/(t->first_size-1);

               o.x=xdim*(t->first_col-(cols-1)/2.0f)+centerx-xdim/2.0f;
               o.y=centery;
               o.z=zdim*(t->first_row-(rows-1)/2.0f)+centerz+zdim/2.0f;

               t->ray->addtrianglefans(&t->cache2_arg,3*t->first_beginfan,t->first_fancnt,0,&s,&o,0,t->tile->getwarp());
               }

            if (op==TRIGGER_OP)
               {
               if (t->cache_phase==0) t->ray->clearbuffer();
               else if (t->cache_phase==1) t->first_scale=b;
               else if (t->cache_phase==4) t->ray->swapbuffer();
               }
            else if (op==TEXMAP_OP)
               {
               t->first_col=ftrc(a+0.5f);
               t->first_row=ftrc(b+0.5f);
               t->first_size=ftrc(c+0.5f);
               }

            t->first_beginfan=t->cache_size2+1;
            t->first_fancnt=0;
            }

      t->cache2_op[t->cache_size2]=op;

      ptr=&t->cache2_arg[3*t->cache_size2++];
      }

   // update vertex cache
   *ptr++=a;
   *ptr++=b;
   *ptr=c;
   }

void minicache::cacheprismedge(float x,float y,float yf,float z)
   {
   miniv4d v1;

   TERRAIN_TYPE *t;

   float *ptr;

   if (PRISMEDGE_CALLBACK!=NULL) PRISMEDGE_CALLBACK(x,y-yf,yf,z,CALLBACK_DATA);
   else
      {
      t=&TERRAIN[CACHE_ID];

      // enlarge prism cache
      if (t->prism_size1>=t->prism_maxsize || t->prism_size2>=t->prism_maxsize)
         {
         t->prism_maxsize*=2;

         if ((t->prism_cache1=(float *)realloc(t->prism_cache1,4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         if ((t->prism_cache2=(float *)realloc(t->prism_cache2,4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         }

      if (t->cache_num==1) ptr=&t->prism_cache1[4*t->prism_size1++];
      else ptr=&t->prism_cache2[4*t->prism_size2++];

      // update prism cache
      *ptr++=x;
      *ptr++=y;
      *ptr++=yf;
      *ptr=z;
      }
   }

void minicache::cachetrigger(int phase,float scale,float ex,float ey,float ez)
   {
   miniv4d v1;

   TERRAIN_TYPE *t;

   t=&TERRAIN[CACHE_ID];

   t->cache_phase=phase;

   cache(TRIGGER_OP,phase,scale);

   if (t->cache_phase==0)
      {
      // swap vertex buffers
      if (t->cache_num==1)
         {
         t->cache_size2=0;
         t->prism_size2=0;

         t->fancnt2=0;
         t->vtxcnt2=0;

         t->cache_num=2;
         }
      else
         {
         t->cache_size1=0;
         t->prism_size1=0;

         t->fancnt1=0;
         t->vtxcnt1=0;

         t->cache_num=1;
         }

      // shrink vertex buffer
      if (t->cache_size1<t->cache_maxsize/4 && t->cache_size2<t->cache_maxsize/4)
         {
         t->cache_maxsize/=2;

         if ((t->cache1_op=(unsigned char *)realloc(t->cache1_op,t->cache_maxsize))==NULL) ERRORMSG();
         if ((t->cache2_op=(unsigned char *)realloc(t->cache2_op,t->cache_maxsize))==NULL) ERRORMSG();

         if ((t->cache1_arg=(float *)realloc(t->cache1_arg,3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();
         if ((t->cache2_arg=(float *)realloc(t->cache2_arg,3*t->cache_maxsize*sizeof(float)))==NULL) ERRORMSG();
         }

      // shrink prism buffer
      if (t->prism_size1<t->prism_maxsize/4 && t->prism_size2<t->prism_maxsize/4)
         {
         t->prism_maxsize/=2;

         if ((t->prism_cache1=(float *)realloc(t->prism_cache1,4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         if ((t->prism_cache2=(float *)realloc(t->prism_cache2,4*t->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         }
      }

   if (PRISMCACHE_CALLBACK!=NULL) PRISMCACHE_CALLBACK(phase,scale,ex,ey,ez,CALLBACK_DATA);
   }

void minicache::cachesync(int id)
   {
   CACHE_ID=id;

   if (PRISMSYNC_CALLBACK!=NULL) PRISMSYNC_CALLBACK(id,CALLBACK_DATA);
   }

// render all back buffers of the cache
int minicache::rendercache()
   {
   int vtx=0;

   int id,phase;

   for (id=0; id<MAXTERRAIN; id++)
      if (TERRAIN[id].tile!=NULL)
         {
         TERRAIN[id].render_phase=0;
         TERRAIN[id].render_count=0;
         }

   for (phase=0; phase<=4; phase++)
      {
      rendertrigger(phase);

      for (id=0; id<MAXTERRAIN; id++)
         if (TERRAIN[id].tile!=NULL)
            if (TERRAIN[id].isvisible!=0) vtx+=rendercache(id,phase);
      }

   return(vtx);
   }

// render back buffer of the cache
int minicache::rendercache(int id,int phase)
   {
   int vtx=0;

#ifndef NOOGL

   int i,p;

   TERRAIN_TYPE *t;

   RENDER_ID=id;

   t=&TERRAIN[RENDER_ID];

   if (PRISMSYNC_CALLBACK!=NULL) PRISMSYNC_CALLBACK(RENDER_ID,CALLBACK_DATA);

   if (phase==0) vtx+=getvtxcnt(RENDER_ID);

   i=t->render_count;

   if (t->cache_num==1)
      while (i<t->cache_size2)
         {
         switch (t->cache2_op[i])
            {
            case BEGINFAN_OP:
               p=ftrc(t->cache2_arg[3*i]+0.5f);
               glDrawArrays(GL_TRIANGLE_FAN,i+1,p);
               i+=p;
               break;
            case FANVERTEX_OP:
               break;
            case TEXMAP_OP:
               rendertexmap(ftrc(t->cache2_arg[3*i]+0.5f),ftrc(t->cache2_arg[3*i+1]+0.5f),ftrc(t->cache2_arg[3*i+2]+0.5f));
               break;
            case TRIGGER_OP:
               t->render_count=i;
               vtx+=rendertrigger(ftrc(t->cache2_arg[3*i]+0.5f),t->cache2_arg[3*i+1]);
               if (t->render_phase!=phase) return(vtx);
               break;
            }
         i++;
         }
   else
      while (i<t->cache_size1)
         {
         switch (t->cache1_op[i])
            {
            case BEGINFAN_OP:
               p=ftrc(t->cache1_arg[3*i]+0.5f);
               glDrawArrays(GL_TRIANGLE_FAN,i+1,p);
               i+=p;
               break;
            case FANVERTEX_OP:
               break;
            case TEXMAP_OP:
               rendertexmap(ftrc(t->cache1_arg[3*i]+0.5f),ftrc(t->cache1_arg[3*i+1]+0.5f),ftrc(t->cache1_arg[3*i+2]+0.5f));
               break;
            case TRIGGER_OP:
               t->render_count=i;
               vtx+=rendertrigger(ftrc(t->cache1_arg[3*i]+0.5f),t->cache1_arg[3*i+1]);
               if (t->render_phase!=phase) return(vtx);
               break;
            }
         i++;
         }

   t->render_count=i;

#endif

   return(vtx);
   }

// rendering functions:

void minicache::rendertexmap(int m,int n,int S)
   {
#ifndef NOOGL

   int cols,rows;
   float xdim,zdim;
   float centerx,centery,centerz;
   miniwarp *warp;

   miniv4d mtx[3];
   double oglmtx[16];

   float ox,oz;

   int texid,texw,texh,texmm;

   TERRAIN_TYPE *t;

   t=&TERRAIN[RENDER_ID];

   mtxpop();

   cols=t->tile->getcols();
   rows=t->tile->getrows();

   xdim=t->tile->getcoldim();
   zdim=t->tile->getrowdim();

   centerx=t->tile->getcenterx();
   centery=t->tile->getcentery();
   centerz=t->tile->getcenterz();

   mtxpush();

   warp=t->tile->getwarp();

   if (warp!=NULL)
      {
      warp->getwarp(mtx);

      oglmtx[0]=mtx[0].x;
      oglmtx[4]=mtx[0].y;
      oglmtx[8]=mtx[0].z;
      oglmtx[12]=mtx[0].w;

      oglmtx[1]=mtx[1].x;
      oglmtx[5]=mtx[1].y;
      oglmtx[9]=mtx[1].z;
      oglmtx[13]=mtx[1].w;

      oglmtx[2]=mtx[2].x;
      oglmtx[6]=mtx[2].y;
      oglmtx[10]=mtx[2].z;
      oglmtx[14]=mtx[2].w;

      oglmtx[3]=0.0;
      oglmtx[7]=0.0;
      oglmtx[11]=0.0;
      oglmtx[15]=1.0;

      mtxmult(oglmtx);
      }

   ox=xdim*(m-(cols-1)/2.0f)+centerx;
   oz=zdim*(n-(rows-1)/2.0f)+centerz;

   mtxtranslate(ox-xdim/2.0f,centery,oz+zdim/2.0f);

   // avoid gaps between tiles (excluding the sea surface)
   if (t->render_phase==2)
      if (CONFIGURE_OVERLAP!=0.0f)
         if (S>=CONFIGURE_MINSIZE) mtxscale((S-1+CONFIGURE_OVERLAP)/(S-1),1.0f,(S-1+CONFIGURE_OVERLAP)/(S-1));

   mtxscale(xdim/(S-1),t->scale,-zdim/(S-1));

   if (t->render_phase==2 || CONFIGURE_SEAENABLETEX!=0)
      {
      texid=t->tile->gettexid(m,n);
      texw=t->tile->gettexw(m,n);
      texh=t->tile->gettexh(m,n);
      texmm=t->tile->gettexmm(m,n);

      bindtexmap(texid,texw,texh,S,texmm);

      if (USEVTXSHADER!=0)
         setvtxshadertexprm(1.0f/(S-1)*(texw-1)/texw,
                            -1.0f/(S-1)*(texh-1)/texh,
                            0.5f/texh,
                            1.0f-0.5f/texh,
                            t->scale);

      if (USEPIXSHADER!=0 || USESEASHADER!=0)
         if (texid==0) setpixshadertexprm(0.0f,1.0f);
         else setpixshadertexprm(1.0f,0.0f);
      }

#endif
   }

int minicache::rendertrigger(int phase)
   {
   int vtx=0;

#ifndef NOOGL

   if (phase==2)
      {
      initstate();
      mtxpush();

      if (OPACITY<1.0f)
         {
         if (OPACITY<=0.0f) disableRGBAwriting();

         disableZwriting();
         enableblending();

         color(1.0f,1.0f,1.0f,OPACITY);
         }
      else color(1.0f,1.0f,1.0f);

      normal(0.0f,1.0f,0.0f);

      if (USEVTXSHADER!=0) enablevtxshader();
      if (USEPIXSHADER!=0) enablepixshader();

      glEnableClientState(GL_VERTEX_ARRAY);
      }
   else if (phase==3)
      {
      if (OPACITY<1.0f)
         {
         if (OPACITY<=0.0f) enableRGBAwriting();

         enableZwriting();
         disableblending();
         }

      if (USEPIXSHADER!=0) disablepixshader();

      if (PRESEA_CB!=NULL)
         {
         bindtexmap(0,0,0,0);

         glDisableClientState(GL_VERTEX_ARRAY);

         if (USEVTXSHADER!=0) disablevtxshader();

         mtxpop();
         exitstate();

         PRESEA_CB(CB_DATA);

         initstate();
         mtxpush();

         if (USEVTXSHADER!=0) enablevtxshader();

         glEnableClientState(GL_VERTEX_ARRAY);
         }

      if (CONFIGURE_SEATWOSIDED!=0) disableculling();

      if (SEA_A!=1.0f) enableblending();

      color(SEA_R,SEA_G,SEA_B,SEA_A);
      normal(0.0f,1.0f,0.0f);

      if (USESEASHADER!=0) enableseashader();

      if (CONFIGURE_ZSCALE_SEA!=1.0f)
         {
         mtxproj();
         mtxpush();
         mtxscale(CONFIGURE_ZSCALE_SEA,CONFIGURE_ZSCALE_SEA,CONFIGURE_ZSCALE_SEA); // prevent Z-fighting
         mtxmodel();
         }
      }
   else if (phase==4)
      {
      if (CONFIGURE_SEATWOSIDED!=0) enableBFculling();

      if (SEA_A!=1.0f) disableblending();

      if (USESEASHADER!=0) disableseashader();
      if (USEVTXSHADER!=0) disablevtxshader();

      bindtexmap(0,0,0,0);

      glDisableClientState(GL_VERTEX_ARRAY);

      if (CONFIGURE_ZSCALE_SEA!=1.0f)
         {
         mtxproj();
         mtxpop();
         mtxmodel();
         }

      mtxpop();
      exitstate();

      if (POSTSEA_CB!=NULL) POSTSEA_CB(CB_DATA);
      }

   if (phase==4) vtx+=rendertrigger();

#endif

   return(vtx);
   }

int minicache::rendertrigger(int phase,float scale)
   {
   int vtx=0;

#ifndef NOOGL

   TERRAIN_TYPE *t;

   t=&TERRAIN[RENDER_ID];

   t->render_phase=phase;

   if (t->render_phase==1) t->scale=scale;
   else if (t->render_phase==2 || t->render_phase==3)
      if (t->cache_num==1) glVertexPointer(3,GL_FLOAT,0,t->cache2_arg);
      else glVertexPointer(3,GL_FLOAT,0,t->cache1_arg);
   else if (t->render_phase==4) t->lambda=scale;

   if (PRISMTRIGGER_CALLBACK!=NULL) vtx+=PRISMTRIGGER_CALLBACK(phase,CALLBACK_DATA);

#endif

   return(vtx);
   }

int minicache::rendertrigger()
   {
   int vtx=0;

#ifndef NOOGL

   int id;

   if (PRISMEDGE_CALLBACK==NULL)
      for (id=0; id<MAXTERRAIN; id++)
         if (TERRAIN[id].tile!=NULL)
            if (PRISMRENDER_CALLBACK!=NULL)
               if (TERRAIN[id].cache_num==1)
                  vtx+=PRISMRENDER_CALLBACK(TERRAIN[id].prism_cache2,TERRAIN[id].prism_size2/3,TERRAIN[id].lambda,TERRAIN[id].tile->getwarp(),CALLBACK_DATA);
               else
                  vtx+=PRISMRENDER_CALLBACK(TERRAIN[id].prism_cache1,TERRAIN[id].prism_size1/3,TERRAIN[id].lambda,TERRAIN[id].tile->getwarp(),CALLBACK_DATA);
            else
               if (TERRAIN[id].cache_num==1)
                  vtx+=renderprisms(TERRAIN[id].prism_cache2,TERRAIN[id].prism_size2/3,TERRAIN[id].lambda,TERRAIN[id].tile->getwarp(),
                                    PRISM_R,PRISM_G,PRISM_B,PRISM_A);
               else
                  vtx+=renderprisms(TERRAIN[id].prism_cache1,TERRAIN[id].prism_size1/3,TERRAIN[id].lambda,TERRAIN[id].tile->getwarp(),
                                    PRISM_R,PRISM_G,PRISM_B,PRISM_A);

#endif

   return(vtx);
   }

int minicache::renderprisms(float *cache,int cnt,float lambda,miniwarp *warp,
                            float pr,float pg,float pb,float pa)
   {
   int vtx=0;

#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   static char *vtxprog="!!ARBvp1.0 \n\
      PARAM c=program.env[0]; \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      TEMP vtx,pos; \n\
      MOV vtx,vertex.position.xywz; \n\
      MOV result.color,vertex.color; \n\
      MOV vtx.w,c.w; \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      MOV result.fogcoord.x,pos.z; \n\
      MOV result.position,pos; \n\
      END";

   GLuint vtxprogid;

   miniv4d mtx[3];
   double oglmtx[16];

   if (lambda<=0.0f) return(vtx);

   initglexts();
   initwglprocs();

   if (PRISMCACHE_VTXPROGID==0)
      {
      glGenProgramsARB(1,&vtxprogid);
      glBindProgramARB(GL_VERTEX_PROGRAM_ARB,vtxprogid);
      glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(vtxprog),vtxprog);
      PRISMCACHE_VTXPROGID=vtxprogid;
      }

   initstate();

   enableblending();

   mtxpush();
   mtxproj();
   mtxpush();
   mtxscale(CONFIGURE_ZSCALE_PRISMS,CONFIGURE_ZSCALE_PRISMS,CONFIGURE_ZSCALE_PRISMS); // prevent Z-fighting
   mtxmodel();

   if (warp!=NULL)
      {
      warp->getwarp(mtx);

      oglmtx[0]=mtx[0].x;
      oglmtx[4]=mtx[0].y;
      oglmtx[8]=mtx[0].z;
      oglmtx[12]=mtx[0].w;

      oglmtx[1]=mtx[1].x;
      oglmtx[5]=mtx[1].y;
      oglmtx[9]=mtx[1].z;
      oglmtx[13]=mtx[1].w;

      oglmtx[2]=mtx[2].x;
      oglmtx[6]=mtx[2].y;
      oglmtx[10]=mtx[2].z;
      oglmtx[14]=mtx[2].w;

      oglmtx[3]=0.0;
      oglmtx[7]=0.0;
      oglmtx[11]=0.0;
      oglmtx[15]=1.0;

      mtxmult(oglmtx);
      }

   glBindProgramARB(GL_VERTEX_PROGRAM_ARB,PRISMCACHE_VTXPROGID);
   glEnable(GL_VERTEX_PROGRAM_ARB);

   glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,0,0.0f,0.0f,0.0f,1.0f);

   color(pr,pg,pb,pa);

   glVertexPointer(4,GL_FLOAT,0,cache);
   glEnableClientState(GL_VERTEX_ARRAY);
   glDrawArrays(GL_TRIANGLES,0,3*cnt);
   glDisableClientState(GL_VERTEX_ARRAY);

   vtx+=3*cnt;

   glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
   glDisable(GL_VERTEX_PROGRAM_ARB);

   mtxpop();
   mtxproj();
   mtxpop();
   mtxmodel();

   exitstate();

#endif

#endif

   return(vtx);
   }

// attach a tileset for scene double buffering
void minicache::attach(minitile *terrain,
                       void (*prismedge)(float x,float y,float yf,float z,void *data),
                       void (*prismcache)(int phase,float scale,float ex,float ey,float ez,void *data),
                       int (*prismrender)(float *cache,int cnt,float lambda,miniwarp *warp,void *data),
                       int (*prismtrigger)(int phase,void *data),
                       void (*prismsync)(int id,void *data),
                       void *data)
   {
   int id;

   makecurrent();

   if (TERRAIN==NULL)
      {
      MAXTERRAIN=1;
      if ((TERRAIN=(TERRAIN_TYPE *)malloc(MAXTERRAIN*sizeof(TERRAIN_TYPE)))==NULL) ERRORMSG();
      TERRAIN[0].tile=NULL;
      }

   if (NUMTERRAIN>=MAXTERRAIN)
      {
      if ((TERRAIN=(TERRAIN_TYPE *)realloc(TERRAIN,2*MAXTERRAIN*sizeof(TERRAIN_TYPE)))==NULL) ERRORMSG();
      for (id=NUMTERRAIN; id<2*MAXTERRAIN; id++) TERRAIN[id].tile=NULL;
      MAXTERRAIN*=2;
      }

   if (terrain!=NULL)
      for (id=0; id<MAXTERRAIN; id++)
         if (TERRAIN[id].tile==NULL)
            {
            TERRAIN[id].tile=terrain;

            terrain->setcallbacks(cache_beginfan,
                                  cache_fanvertex,
                                  NULL,cache_texmap,
                                  cache_prismedge,
                                  cache_trigger,
                                  cache_sync,
                                  id);

            initterrain(&TERRAIN[id]);

            NUMTERRAIN++;

            break;
            }

   PRISMEDGE_CALLBACK=prismedge;
   PRISMCACHE_CALLBACK=prismcache;
   PRISMRENDER_CALLBACK=prismrender;
   PRISMTRIGGER_CALLBACK=prismtrigger;
   PRISMSYNC_CALLBACK=prismsync;
   CALLBACK_DATA=data;
   }

// detach a tileset
void minicache::detach(minitile *terrain)
   {
   if (terrain==NULL) ERRORMSG();

   TERRAIN[terrain->getid()].tile=NULL;

   freeterrain(&TERRAIN[terrain->getid()]);
   }

// determine whether or not a tileset is displayed
void minicache::display(minitile *terrain,int yes)
   {
   if (terrain==NULL) ERRORMSG();

   TERRAIN[terrain->getid()].isvisible=yes;
   }

// make cache current
void minicache::makecurrent()
   {CACHE=this;}

// define triangle mesh opacity
void minicache::setopacity(float alpha)
   {OPACITY=alpha;}

// define color of semi-transparent sea
void minicache::setseacolor(float r,float g,float b,float a)
   {
   SEA_R=r;
   SEA_G=g;
   SEA_B=b;
   SEA_A=a;
   }

// define rendering color of prism boundary
void minicache::setprismcolor(float prismR,float prismG,float prismB,float prismA)
   {
   PRISM_R=prismR;
   PRISM_G=prismG;
   PRISM_B=prismB;
   PRISM_A=prismA;
   }

// set vertex shader plugin
void minicache::setvtxshader(char *vp)
   {
#ifndef NOOGL

   // default vertex shader
   static char *vtxprog="!!ARBvp1.0 \n\
      PARAM t=program.env[0]; \n\
      PARAM e=program.env[1]; \n\
      PARAM c0=program.env[2]; \n\
      PARAM c1=program.env[3]; \n\
      PARAM c2=program.env[4]; \n\
      PARAM c3=program.env[5]; \n\
      PARAM c4=program.env[6]; \n\
      PARAM c5=program.env[7]; \n\
      PARAM c6=program.env[8]; \n\
      PARAM c7=program.env[9]; \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      TEMP vtx,col,nrm,pos,vec; \n\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position; \n\
      MOV col,vertex.color; \n\
      MOV nrm,vertex.normal; \n\
      ### transform vertex with modelview \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      ### transform normal with inverse transpose \n\
      DP4 vec.x,invtra[0],nrm; \n\
      DP4 vec.y,invtra[1],nrm; \n\
      DP4 vec.z,invtra[2],nrm; \n\
      DP4 vec.w,invtra[3],nrm; \n\
      ### write resulting vertex \n\
      MOV result.position,pos; \n\
      MOV result.color,col; \n\
      ### calculate tex coords \n\
      MAD result.texcoord[0].x,vtx.x,t.x,t.z; \n\
      MAD result.texcoord[0].y,vtx.z,t.y,t.w; \n\
      MUL result.texcoord[0].z,vtx.y,e.y; \n\
      ### pass normal as tex coords \n\
      MOV result.texcoord[1],vec; \n\
      ### calculate spherical fog coord \n\
      DP3 result.fogcoord.x,pos,pos; \n\
      END \n";

   if (vp==NULL) vp=vtxprog;

   if (VTXPROG!=NULL)
      {
      if (strcmp(vp,VTXPROG)==0) return;
      free(VTXPROG);
      }

   VTXPROG=strdup(vp);
   VTXDIRTY=1;

#endif
   }

// set vertex shader parameter vector
void minicache::setvtxshaderparams(float p1,float p2,float p3,float p4,int n)
   {
   if (n<0 || n>=8) ERRORMSG();

   VTXSHADERPAR1[n]=p1;
   VTXSHADERPAR2[n]=p2;
   VTXSHADERPAR3[n]=p3;
   VTXSHADERPAR4[n]=p4;
   }

// enable vertex shader plugin
void minicache::enablevtxshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   int i;

   GLuint vtxprogid;

   initglexts();
   initwglprocs();

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      {
      if (VTXDIRTY!=0)
         {
         if (VTXPROGID!=0)
            {
            vtxprogid=VTXPROGID;
            glDeleteProgramsARB(1,&vtxprogid);
            }

         glGenProgramsARB(1,&vtxprogid);
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,vtxprogid);
         glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(VTXPROG),VTXPROG);
         VTXPROGID=vtxprogid;

         VTXDIRTY=0;
         }

      if (VTXPROGID!=0)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,VTXPROGID);
         glEnable(GL_VERTEX_PROGRAM_ARB);

         for (i=0; i<8; i++)
            glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,2+i,VTXSHADERPAR1[i],VTXSHADERPAR2[i],VTXSHADERPAR3[i],VTXSHADERPAR4[i]);
         }
      }

#endif

#endif
   }

// set vertex shader texture mapping parameters
void minicache::setvtxshadertexprm(float s1,float s2,float o1,float o2,float scale)
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      if (VTXPROGID!=0)
         {
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,0,s1,s2,o1,o2);
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,1,0.0f,scale,0.0f,0.0f);
         }

#endif

#endif
   }

// disable vertex shader plugin
void minicache::disablevtxshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      if (VTXPROGID!=0)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
         glDisable(GL_VERTEX_PROGRAM_ARB);
         }

#endif

#endif
   }

// set pixel shader plugin
void minicache::setpixshader(char *fp)
   {
#ifndef NOOGL

   //!!
   // default pixel shader
   static char *fragprog="!!ARBfp1.0 \n\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      PARAM c2=program.env[2]; \n\
      PARAM c3=program.env[3]; \n\
      PARAM c4=program.env[4]; \n\
      PARAM c5=program.env[5]; \n\
      PARAM c6=program.env[6]; \n\
      PARAM c7=program.env[7]; \n\
      PARAM a=program.env[8]; \n\
      PARAM t=program.env[9]; \n\
      PARAM l=program.env[10]; \n\
      PARAM p=program.env[11]; \n\
      TEMP col,nrm,len; \n\
      ### fetch texture color \n\
      TEX col,fragment.texcoord[0],texture[0],2D; \n\
      MAD col,col,a.x,a.b; \n\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n\
      ### modulate with fragment color \n\
      MUL result.color,col,fragment.color; \n\
      END \n";

   if (fp==NULL) fp=fragprog;

   if (FRAGPROG!=NULL)
      {
      if (strcmp(fp,FRAGPROG)==0) return;
      free(FRAGPROG);
      }

   FRAGPROG=strdup(fp);
   FRAGDIRTY=1;

#endif
   }

// set pixel shader parameter vector
void minicache::setpixshaderparams(float p1,float p2,float p3,float p4,int n)
   {
   if (n<0 || n>=8) ERRORMSG();

   PIXSHADERPAR1[n]=p1;
   PIXSHADERPAR2[n]=p2;
   PIXSHADERPAR3[n]=p3;
   PIXSHADERPAR4[n]=p4;
   }

// set pixel shader RGB texture map
void minicache::setpixshadertexRGB(unsigned char *image,int width,int height)
   {setpixshadertex(image,width,height,3);}

// set pixel shader RGBA texture map
void minicache::setpixshadertexRGBA(unsigned char *image,int width,int height)
   {setpixshadertex(image,width,height,4);}

// set pixel shader RGB[A] texture map
void minicache::setpixshadertex(unsigned char *image,int width,int height,int components)
   {
   if (width<2 || height<2) ERRORMSG();

   if (PIXSHADERTEXID!=0)
      {
      deletetexmap(PIXSHADERTEXID);
      PIXSHADERTEXID=0;
      }

   if (image!=NULL)
      {
      if (components==3) PIXSHADERTEXID=buildRGBtexmap(image,&width,&height,0);
      else if (components==4) PIXSHADERTEXID=buildRGBAtexmap(image,&width,&height,0);
      else ERRORMSG();

      PIXSHADERTEXWIDTH=width;
      PIXSHADERTEXHEIGHT=height;
      }
   }

// set sea shader plugin
void minicache::setseashader(char *sp)
   {
#ifndef NOOGL

   //!!
   // default sea shader
   static char *seaprog="!!ARBfp1.0 \n\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      PARAM c2=program.env[2]; \n\
      PARAM c3=program.env[3]; \n\
      PARAM c4=program.env[4]; \n\
      PARAM c5=program.env[5]; \n\
      PARAM c6=program.env[6]; \n\
      PARAM c7=program.env[7]; \n\
      PARAM a=program.env[8]; \n\
      PARAM t=program.env[9]; \n\
      PARAM l=program.env[10]; \n\
      PARAM p=program.env[11]; \n\
      TEMP col,nrm,len; \n\
      ### fetch texture color \n\
      TEX col,fragment.texcoord[0],texture[0],2D; \n\
      MAD col,col,a.x,a.b; \n\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n\
      ### modulate with fragment color \n\
      MUL result.color,col,fragment.color; \n\ ###!!
      END \n";

   if (sp==NULL) sp=seaprog;

   if (SEAPROG!=NULL)
      {
      if (strcmp(sp,SEAPROG)==0) return;
      free(SEAPROG);
      }

   SEAPROG=strdup(sp);
   SEADIRTY=1;

#endif
   }

// set sea shader parameter vector
void minicache::setseashaderparams(float p1,float p2,float p3,float p4,int n)
   {
   if (n<0 || n>=8) ERRORMSG();

   SEASHADERPAR1[n]=p1;
   SEASHADERPAR2[n]=p2;
   SEASHADERPAR3[n]=p3;
   SEASHADERPAR4[n]=p4;
   }

// set sea shader RGB texture map
void minicache::setseashadertexRGB(unsigned char *image,int width,int height)
   {setseashadertex(image,width,height,3);}

// set sea shader RGBA texture map
void minicache::setseashadertexRGBA(unsigned char *image,int width,int height)
   {setseashadertex(image,width,height,4);}

// set sea shader RGB[A] texture map
void minicache::setseashadertex(unsigned char *image,int width,int height,int components)
   {
   if (width<2 || height<2) ERRORMSG();

   if (SEASHADERTEXID!=0)
      {
      deletetexmap(SEASHADERTEXID);
      SEASHADERTEXID=0;
      }

   if (image!=NULL)
      {
      if (components==3) SEASHADERTEXID=buildRGBtexmap(image,&width,&height,0);
      else if (components==4) SEASHADERTEXID=buildRGBAtexmap(image,&width,&height,0);
      else ERRORMSG();

      SEASHADERTEXWIDTH=width;
      SEASHADERTEXHEIGHT=height;
      }
   }

// define optional sea callbacks
void minicache::setseacb(void (*preseacb)(void *data),
                         void (*postseacb)(void *data),
                         void *data)
   {
   PRESEA_CB=preseacb;
   POSTSEA_CB=postseacb;
   CB_DATA=data;
   }

// enable pixel shader plugin
void minicache::enablepixshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   int i;

   GLuint fragprogid;

   initglexts();
   initwglprocs();

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      {
      if (FRAGDIRTY!=0)
         {
         if (FRAGPROGID!=0)
            {
            fragprogid=FRAGPROGID;
            glDeleteProgramsARB(1,&fragprogid);
            }

         glGenProgramsARB(1,&fragprogid);
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,fragprogid);
         glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(FRAGPROG),FRAGPROG);
         FRAGPROGID=fragprogid;

         FRAGDIRTY=0;
         }

      if (FRAGPROGID!=0)
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,FRAGPROGID);
         glEnable(GL_FRAGMENT_PROGRAM_ARB);

         for (i=0; i<8; i++)
            glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,i,PIXSHADERPAR1[i],PIXSHADERPAR2[i],PIXSHADERPAR3[i],PIXSHADERPAR4[i]);

         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,8,1.0f,0.0f,0.0f,0.0f);

         if (GLEXT_MT!=0)
            if (PIXSHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(PIXSHADERTEXID,PIXSHADERTEXWIDTH,PIXSHADERTEXHEIGHT,0,0);
               glActiveTextureARB(GL_TEXTURE0_ARB);

               glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,9,
                                          (float)(PIXSHADERTEXWIDTH-1)/PIXSHADERTEXWIDTH,0.5f/PIXSHADERTEXWIDTH,
                                          (float)(PIXSHADERTEXHEIGHT-1)/PIXSHADERTEXHEIGHT,0.5f/PIXSHADERTEXHEIGHT);
#endif
               }
         }
      }

#endif

#endif
   }

// set pixel shader texture mapping parameters
void minicache::setpixshadertexprm(float s,float o)
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      if (FRAGPROGID!=0 || SEAPROGID!=0)
         {
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,8,s,o,0.0f,0.0f);

         //!!
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,10,0.0f,0.0f,0.0f,0.0f);
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,11,0.5f,0.5f,0.0f,0.0f);
         }

#endif

#endif
   }

// disable pixel shader plugin
void minicache::disablepixshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      if (FRAGPROGID!=0)
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
         glDisable(GL_FRAGMENT_PROGRAM_ARB);

         if (GLEXT_MT!=0)
            if (PIXSHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(0);
               glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
               }
         }

#endif

#endif
   }

// enable sea shader plugin
void minicache::enableseashader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   int i;

   GLuint seaprogid;

   initglexts();
   initwglprocs();

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      {
      if (SEADIRTY!=0)
         {
         if (SEAPROGID!=0)
            {
            seaprogid=SEAPROGID;
            glDeleteProgramsARB(1,&seaprogid);
            }

         glGenProgramsARB(1,&seaprogid);
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,seaprogid);
         glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(SEAPROG),SEAPROG);
         SEAPROGID=seaprogid;

         SEADIRTY=0;
         }

      if (SEAPROGID!=0)
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,SEAPROGID);
         glEnable(GL_FRAGMENT_PROGRAM_ARB);

         for (i=0; i<8; i++)
            glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,i,SEASHADERPAR1[i],SEASHADERPAR2[i],SEASHADERPAR3[i],PIXSHADERPAR4[i]);

         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,8,1.0f,0.0f,0.0f,0.0f);

         if (GLEXT_MT!=0)
            if (SEASHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(SEASHADERTEXID,SEASHADERTEXWIDTH,SEASHADERTEXHEIGHT,0,0);
               glActiveTextureARB(GL_TEXTURE0_ARB);

               glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,9,
                                          (float)(SEASHADERTEXWIDTH-1)/SEASHADERTEXWIDTH,0.5f/SEASHADERTEXWIDTH,
                                          (float)(SEASHADERTEXHEIGHT-1)/SEASHADERTEXHEIGHT,0.5f/SEASHADERTEXHEIGHT);
#endif
               }
         }
      }

#endif

#endif
   }

// disable sea shader plugin
void minicache::disableseashader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_VP!=0 && GLEXT_FP!=0)
      if (SEAPROGID!=0)
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
         glDisable(GL_FRAGMENT_PROGRAM_ARB);

         if (GLEXT_MT!=0)
            if (SEASHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(0);
               glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
               }
         }

#endif

#endif
   }

// check for OpenGL extensions
void minicache::initglexts()
   {
#ifndef NOOGL

   char *gl_exts;

   if (GLSETUP==0)
      {
      GLEXT_MT=0;
      GLEXT_VP=0;
      GLEXT_FP=0;

      if ((gl_exts=(char *)glGetString(GL_EXTENSIONS))==NULL) ERRORMSG();

      if (strstr(gl_exts,"GL_ARB_multitexture")!=NULL) GLEXT_MT=1;
      if (strstr(gl_exts,"GL_ARB_vertex_program")!=NULL) GLEXT_VP=1;
      if (strstr(gl_exts,"GL_ARB_fragment_program")!=NULL) GLEXT_FP=1;

      GLSETUP=1;
      }

#endif
   }

// Windows OpenGL extension setup
void minicache::initwglprocs()
   {
#ifndef NOOGL

#ifdef _WIN32

   if (WGLSETUP==0)
      {
#ifdef GL_ARB_multitexture
      glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
      glGenProgramsARB=(PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB");
      glBindProgramARB=(PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
      glProgramStringARB=(PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB");
      glProgramEnvParameter4fARB=(PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB");
      glDeleteProgramsARB=(PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB");
#endif

      WGLSETUP=1;
      }

#endif

#endif
   }

// get triangle fan count of active cache
int minicache::getfancnt()
   {
   int id,fancnt;

   fancnt=0;

   for (id=0; id<MAXTERRAIN; id++)
      if (TERRAIN[id].tile!=NULL) fancnt+=getfancnt(id);

   return(fancnt);
   }

// get vertex count of active cache
int minicache::getvtxcnt()
   {
   int id,vtxcnt;

   vtxcnt=0;

   for (id=0; id<MAXTERRAIN; id++)
      if (TERRAIN[id].tile!=NULL) vtxcnt+=getvtxcnt(id);

   return(vtxcnt);
   }

// get triangle fan count of active buffer
int minicache::getfancnt(int id)
   {
   if (TERRAIN[id].cache_num==1) return(TERRAIN[id].fancnt2);
   else return(TERRAIN[id].fancnt1);
   }

// get vertex count of active buffer
int minicache::getvtxcnt(int id)
   {
   if (TERRAIN[id].cache_num==1) return(TERRAIN[id].vtxcnt2);
   else return(TERRAIN[id].vtxcnt1);
   }

// get cached terrain object
minitile *minicache::gettile(int id)
   {return(TERRAIN[id].tile);}

// get ray intersection test object
miniray *minicache::getray(int id)
   {return(TERRAIN[id].ray);}

// configuring
void minicache::configure_overlap(float overlap) {CONFIGURE_OVERLAP=overlap;}
void minicache::configure_minsize(int minsize) {CONFIGURE_MINSIZE=minsize;}
void minicache::configure_seatwosided(int seatwosided) {CONFIGURE_SEATWOSIDED=seatwosided;}
void minicache::configure_seaenabletex(int seaenabletex) {CONFIGURE_SEAENABLETEX=seaenabletex;}
void minicache::configure_zfight_sea(float zscale) {CONFIGURE_ZSCALE_SEA=zscale;}
void minicache::configure_zfight_prisms(float zscale) {CONFIGURE_ZSCALE_PRISMS=zscale;}
void minicache::configure_enableray(int enableray) {CONFIGURE_ENABLERAY=enableray;}
