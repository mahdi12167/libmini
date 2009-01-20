// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"

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

   CULLMODE=1;
   RAYMODE=0;

   OPACITY=1.0f;
   ALPHATEST=1.0f;

   SEA_R=0.0f;
   SEA_G=0.5f;
   SEA_B=1.0f;
   SEA_A=1.0f;

   PRISM_R=PRISM_G=PRISM_B=1.0f;
   PRISM_A=0.9f;

   NONLIN=1; //!!

   PRISMEDGE_CALLBACK=NULL;
   PRISMCACHE_CALLBACK=NULL;
   PRISMRENDER_CALLBACK=NULL;
   PRISMTRIGGER_CALLBACK=NULL;
   PRISMSYNC_CALLBACK=NULL;
   CALLBACK_DATA=NULL;

   VTXPROG_STD_L=NULL;
   VTXPROG_STD_NL=NULL;
   FRGPROG_STD=NULL;
   SEAPROG_STD=NULL;

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

   FRGPROG=NULL;
   FRGDIRTY=0;

   FRGPROGID=0;
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

   PRISM_VTXPROGID=0;
   PRISM_FRGPROGID=0;

   PRESEA_CB=NULL;
   POSTSEA_CB=NULL;

   CONFIGURE_OVERLAP=0.02f;
   CONFIGURE_MINSIZE=33;
   CONFIGURE_SEATWOSIDED=1;
   CONFIGURE_SEAENABLETEX=0;
   CONFIGURE_ZSCALE_SEA=0.99f;
   CONFIGURE_ZSCALE_PATCH=0.95f;
   CONFIGURE_ZSCALE_PRISMS=0.95f;
   CONFIGURE_CALCBOUNDS=0;
   CONFIGURE_OMITSEA=0;
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

   if (VTXPROG_STD_L!=NULL) free(VTXPROG_STD_L);
   if (VTXPROG_STD_NL!=NULL) free(VTXPROG_STD_NL);
   if (FRGPROG_STD!=NULL) free(FRGPROG_STD);
   if (SEAPROG_STD!=NULL) free(SEAPROG_STD);

   if (VTXPROG!=NULL) free(VTXPROG);
   if (FRGPROG!=NULL) free(FRGPROG);
   if (SEAPROG!=NULL) free(SEAPROG);

   if (VTXPROGID!=0) deletevtxprog(VTXPROGID);
   if (FRGPROGID!=0) deletefrgprog(FRGPROGID);
   if (SEAPROGID!=0) deletefrgprog(SEAPROGID);

   if (PRISM_VTXPROGID!=0) deletevtxprog(PRISM_VTXPROGID);
   if (PRISM_FRGPROGID!=0) deletefrgprog(PRISM_FRGPROGID);

   if (PIXSHADERTEXID!=0) deletetexmap(PIXSHADERTEXID);
   if (SEASHADERTEXID!=0) deletetexmap(SEASHADERTEXID);
   }

// initialize terrain
void minicache::initterrain(TERRAIN_TYPE *t)
   {
   int i;

   CACHE_TYPE *c;

   t->cache_num=0;

   for (i=0; i<2; i++)
      {
      c=&t->cache[i];

      c->size=0;
      c->maxsize=1;

      if ((c->op=(unsigned char *)malloc(c->maxsize))==NULL) ERRORMSG();
      if ((c->arg=(float *)malloc(3*c->maxsize*sizeof(float)))==NULL) ERRORMSG();

      c->fancnt=0;
      c->vtxcnt=0;

      c->prism_size=0;
      c->prism_maxsize=1;

      if ((c->prism_buf=(float *)malloc(4*c->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
      }

   t->cache_phase=-1;

   t->ray=new miniray;
   t->first_fancnt=0;

   t->render_phase=-1;

   t->isvisible=1;
   t->ispatch=0;

   t->lx=0.0f;
   t->ly=0.0f;
   t->lz=0.0f;

   t->ls=0.0f;
   t->lo=1.0f;

   t->s1=0.0f;
   t->s2=0.0f;
   t->s3=0.0f;
   t->s4=0.0f;

   t->t1=0.0f;
   t->t2=0.0f;
   t->t3=0.0f;
   t->t4=0.0f;

   t->detail_texid=0;
   t->detail_width=t->detail_height=0;
   t->detail_mipmaps=0;
   t->detail_alpha=0.0f;
   t->detail_nofree=0;
   }

// free terrain
void minicache::freeterrain(TERRAIN_TYPE *t)
   {
   int i;

   CACHE_TYPE *c;

   for (i=0; i<2; i++)
      {
      c=&t->cache[i];

      free(c->op);
      free(c->arg);

      free(c->prism_buf);
      }

   if (t->detail_texid!=0)
      if (t->detail_nofree==0) deletetexmap(t->detail_texid);

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

void minicache::cache(const int op,const float arg1,const float arg2,const float arg3)
   {
   int cols,rows;
   float xdim,zdim;
   float centerx,centery,centerz;

   miniv3d s,o;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   float *ptr;

   t=&TERRAIN[CACHE_ID];
   c=&t->cache[t->cache_num];

   // enlarge vertex buffer
   if (c->size>=c->maxsize)
      {
      c->maxsize*=2;

      if ((c->op=(unsigned char *)realloc(c->op,c->maxsize))==NULL) ERRORMSG();
      if ((c->arg=(float *)realloc(c->arg,3*c->maxsize*sizeof(float)))==NULL) ERRORMSG();
      }

   // update state
   if (op==BEGINFAN_OP)
      {
      c->fancnt++;
      t->first_fancnt++;
      t->last_beginfan=c->size;
      }
   else if (op==FANVERTEX_OP)
      {
      c->vtxcnt++;
      c->arg[3*t->last_beginfan]++;
      }
   else
      // update ray object
      if (RAYMODE!=0)
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

            if (t->cache_phase!=3 || CONFIGURE_OMITSEA==0)
               t->ray->addtrianglefans(&c->arg,3*t->first_beginfan,t->first_fancnt,
                                       0,&s,&o,0,t->tile->getwarp(),CONFIGURE_CALCBOUNDS);
            }

         if (op==TRIGGER_OP)
            {
            if (t->cache_phase==0) t->ray->swapandclear();
            else if (t->cache_phase==1) t->first_scale=arg2;
            }
         else if (op==TEXMAP_OP)
            {
            t->first_col=ftrc(arg1+0.5f);
            t->first_row=ftrc(arg2+0.5f);
            t->first_size=ftrc(arg3+0.5f);
            }

         t->first_beginfan=c->size+1;
         t->first_fancnt=0;
         }

   // append operand
   c->op[c->size]=op;

   // append vertex cache
   ptr=&c->arg[3*c->size++];
   *ptr++=arg1;
   *ptr++=arg2;
   *ptr=arg3;
   }

void minicache::cacheprismedge(const float x,const float y,const float yf,const float z)
   {
   miniv4d v1;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   float *ptr;

   if (PRISMEDGE_CALLBACK!=NULL) PRISMEDGE_CALLBACK(x,y-yf,yf,z,CALLBACK_DATA);
   else
      {
      t=&TERRAIN[CACHE_ID];
      c=&t->cache[t->cache_num];

      // enlarge prism cache
      if (c->prism_size>=c->prism_maxsize)
         {
         c->prism_maxsize*=2;

         if ((c->prism_buf=(float *)realloc(c->prism_buf,4*c->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         }

      // append prism cache
      ptr=&c->prism_buf[4*c->prism_size++];
      *ptr++=x;
      *ptr++=y;
      *ptr++=yf;
      *ptr=z;
      }
   }

void minicache::cachetrigger(const int phase,const float scale,const float ex,const float ey,const float ez)
   {
   miniv4d v1;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   t=&TERRAIN[CACHE_ID];
   c=&t->cache[1-t->cache_num];

   t->cache_phase=phase;

   cache(TRIGGER_OP,phase,scale);

   if (t->cache_phase==0)
      {
      // shrink back vertex buffer
      if (c->size<c->maxsize/4)
         {
         c->maxsize/=2;

         if ((c->op=(unsigned char *)realloc(c->op,c->maxsize))==NULL) ERRORMSG();
         if ((c->arg=(float *)realloc(c->arg,3*c->maxsize*sizeof(float)))==NULL) ERRORMSG();
         }

      // shrink back prism buffer
      if (c->prism_size<c->prism_maxsize/4)
         {
         c->prism_maxsize/=2;

         if ((c->prism_buf=(float *)realloc(c->prism_buf,4*c->prism_maxsize*sizeof(float)))==NULL) ERRORMSG();
         }

      // reset size of back buffer
      c->size=0;
      c->prism_size=0;

      // reset counts of back buffer
      c->fancnt=0;
      c->vtxcnt=0;

      // swap vertex buffers
      t->cache_num=1-t->cache_num;
      }

   if (PRISMCACHE_CALLBACK!=NULL) PRISMCACHE_CALLBACK(phase,scale,ex,ey,ez,CALLBACK_DATA);
   }

void minicache::cachesync(const int id)
   {
   CACHE_ID=id;

   if (PRISMSYNC_CALLBACK!=NULL) PRISMSYNC_CALLBACK(id,CALLBACK_DATA);
   }

// get the modelview matrix
void minicache::getmodelview()
   {
   double mvmtx[16];

   mtxgetmodel(mvmtx);

   MVMATRIX[0].x=mvmtx[0];
   MVMATRIX[1].x=mvmtx[1];
   MVMATRIX[2].x=mvmtx[2];
   MVMATRIX[0].y=mvmtx[4];
   MVMATRIX[1].y=mvmtx[5];
   MVMATRIX[2].y=mvmtx[6];
   MVMATRIX[0].z=mvmtx[8];
   MVMATRIX[1].z=mvmtx[9];
   MVMATRIX[2].z=mvmtx[10];
   MVMATRIX[0].w=mvmtx[12];
   MVMATRIX[1].w=mvmtx[13];
   MVMATRIX[2].w=mvmtx[14];

   inv_mtx(MVINVMTX,MVMATRIX);
   tra_mtx(MVINVTRA,MVINVMTX);
   }

// render all back buffers of the cache
int minicache::rendercache()
   {
   int vtx=0;

   int id,phase;

   if (NUMTERRAIN>0)
      {
      getmodelview();

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
               if (TERRAIN[id].isvisible!=0)
                  {
                  if (TERRAIN[id].ispatch!=0)
                     {
                     if (phase==2)
                        {
                        mtxproj();
                        mtxpush();
                        mtxscale(CONFIGURE_ZSCALE_PATCH,CONFIGURE_ZSCALE_PATCH,CONFIGURE_ZSCALE_PATCH); // prevent Z-fighting
                        mtxmodel();
                        }

                     if (phase==3) enablestenciling(3);
                     }
                  else
                     if (phase==3) enablestenciling(1);

                  vtx+=rendercache(id,phase);

                  if (TERRAIN[id].ispatch!=0)
                     {
                     if (phase==2)
                        {
                        mtxproj();
                        mtxpop();
                        mtxmodel();
                        }

                     if (phase==3) disablestenciling();
                     }
                  else
                     if (phase==3) disablestenciling();
                  }
         }
      }

   return(vtx);
   }

// render back buffer of the cache
int minicache::rendercache(int id,int phase)
   {
   int vtx=0;

   int i,p;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   RENDER_ID=id;

   t=&TERRAIN[RENDER_ID];
   c=&t->cache[1-t->cache_num];

   if (PRISMSYNC_CALLBACK!=NULL) PRISMSYNC_CALLBACK(RENDER_ID,CALLBACK_DATA);

   if (phase==0) vtx+=getvtxcnt(RENDER_ID);

   i=t->render_count;

   while (i<c->size)
      {
      switch (c->op[i])
         {
         case BEGINFAN_OP:
            p=ftrc(c->arg[3*i]+0.5f);
            renderfan(i+1,p);
            i+=p;
            break;
         case FANVERTEX_OP:
            break;
         case TEXMAP_OP:
            rendertexmap(ftrc(c->arg[3*i]+0.5f),ftrc(c->arg[3*i+1]+0.5f),ftrc(c->arg[3*i+2]+0.5f));
            break;
         case TRIGGER_OP:
            t->render_count=i;
            vtx+=rendertrigger(ftrc(c->arg[3*i]+0.5f),c->arg[3*i+1]);
            if (t->render_phase!=phase) return(vtx);
            break;
         }
      i++;
      }

   t->render_count=i;

   return(vtx);
   }

// rendering functions:

void minicache::rendertexmap(int m,int n,int S)
   {
   int cols,rows;
   float xdim,zdim;
   float centerx,centery,centerz;

   miniwarpbase *warp;
   BOOLINT nonlin;

   miniv4d mtx[3];
   double oglmtx[16];

   float ox,oz;

   int texid,texw,texh,texmm;

   miniv3d light;

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

   nonlin=FALSE;

   if (NONLIN==0) warp=t->tile->getwarp();
   else
      {
      warp=t->tile->getwarp(m,n);

      if (warp==NULL) warp=t->tile->getwarp();
      else if (USEVTXSHADER!=0)
         {
         nonlin=TRUE;
         setvtxshadernonlin(S,t->scale,warp);
         warp=NULL;
         }
      }

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

   if (!nonlin)
      {
      ox=xdim*(m-(cols-1)/2.0f)+centerx;
      oz=zdim*(n-(rows-1)/2.0f)+centerz;

      mtxtranslate(ox-xdim/2.0f,centery,oz+zdim/2.0f);

      // avoid gaps between tiles (excluding the sea surface)
      if (t->render_phase==2)
         if (CONFIGURE_OVERLAP!=0.0f)
            if (S>=CONFIGURE_MINSIZE) mtxscale((S-1+CONFIGURE_OVERLAP)/(S-1),1.0f,(S-1+CONFIGURE_OVERLAP)/(S-1));

      mtxscale(xdim/(S-1),t->scale,-zdim/(S-1));
      }

   if (t->render_phase==2 || t->render_phase==3)
      {
      texid=t->tile->gettexid(m,n);
      texw=t->tile->gettexw(m,n);
      texh=t->tile->gettexh(m,n);
      texmm=t->tile->gettexmm(m,n);

      if (t->render_phase==2 || CONFIGURE_SEAENABLETEX!=0) bindtexmap(texid,texw,texh,S,texmm);
      else texid=0;

      if (USEVTXSHADER!=0)
         setvtxshadertexprm(1.0f/(S-1)*(texw-1)/texw,
                            -1.0f/(S-1)*(texh-1)/texh,
                            0.5f/texh,
                            1.0f-0.5f/texh,
                            t->scale);

      if (USEPIXSHADER!=0 || USESEASHADER!=0)
         {
         light=miniv3d(t->lx,t->ly,t->lz);
         light=miniv3d(MVINVTRA[0]*light,MVINVTRA[1]*light,MVINVTRA[2]*light);
         light.normalize();

         if (texid==0) setpixshadertexprm(0.0f,1.0f,light.x,light.y,light.z,t->ls,t->lo,t->detail_alpha); // make unspecified texture white
         else setpixshadertexprm(1.0f,0.0f,light.x,light.y,light.z,t->ls,t->lo,t->detail_alpha);
         }
      }
   }

int minicache::rendertrigger(int phase)
   {
   int vtx=0;

   if (phase==2)
      {
      initstate();
      mtxpush();

      if (CULLMODE==0) disableculling();

      if (ALPHATEST<1.0f) enableAtest(ALPHATEST);

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

      colorarray(NULL);
      normalarray(NULL);
      texcoordarray(NULL);
      }
   else if (phase==3)
      {
      if (CULLMODE==0) enableBFculling();

      if (ALPHATEST<1.0f) disableAtest();

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

         vertexarray(NULL);

         if (USEVTXSHADER!=0) disablevtxshader();

         mtxpop();
         exitstate();

         PRESEA_CB(CB_DATA);

         initstate();
         mtxpush();

         if (USEVTXSHADER!=0) enablevtxshader();
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

      vertexarray(NULL);

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

   return(vtx);
   }

int minicache::rendertrigger(int phase,float scale)
   {
   int vtx=0;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   t=&TERRAIN[RENDER_ID];
   c=&t->cache[1-t->cache_num];

   t->render_phase=phase;

   if (t->render_phase==1) t->scale=scale;
   else if (t->render_phase==2 || t->render_phase==3) vertexarray(c->arg);
   else if (t->render_phase==4) t->lambda=scale;

   if (PRISMTRIGGER_CALLBACK!=NULL) vtx+=PRISMTRIGGER_CALLBACK(phase,CALLBACK_DATA);

   return(vtx);
   }

int minicache::rendertrigger()
   {
   int vtx=0;

   int id;

   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   if (PRISMEDGE_CALLBACK==NULL)
      for (id=0; id<MAXTERRAIN; id++)
         {
         t=&TERRAIN[id];
         c=&t->cache[1-t->cache_num];

         if (t->tile!=NULL)
            {
            if (PRISMRENDER_CALLBACK!=NULL)
               vtx+=PRISMRENDER_CALLBACK(c->prism_buf,c->prism_size/3,t->lambda,t->tile->getwarp(),CALLBACK_DATA);
            else
               vtx+=renderprisms(c->prism_buf,c->prism_size/3,t->lambda,t->tile->getwarp(),
                                 PRISM_R,PRISM_G,PRISM_B,PRISM_A,
                                 t->lx,t->ly,t->lz,
                                 t->ls,t->lo);
            }
         }

   return(vtx);
   }

int minicache::renderprisms(float *cache,int cnt,float lambda,miniwarpbase *warp,
                            float pr,float pg,float pb,float pa,
                            float lx,float ly,float lz,
                            float ls,float lo)
   {
   int vtx=0;

   static const char *vtxprog="!!ARBvp1.0 \n\
      PARAM c=program.env[0]; \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM matrix[4]={state.matrix.modelview}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      TEMP vtx,col,nrm,pos,vec; \n\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position.xywz; \n\
      MOV vtx.w,c.w; \n\
      MOV col,vertex.color; \n\
      MOV nrm,vertex.normal; \n\
      ### transform vertex with combined modelview \n\
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
      ### pass normal as tex coords \n\
      MOV result.texcoord[1],vec; \n\
      ### transform vertex with modelview \n\
      DP4 pos.x,matrix[0],vtx; \n\
      DP4 pos.y,matrix[1],vtx; \n\
      DP4 pos.z,matrix[2],vtx; \n\
      DP4 pos.w,matrix[3],vtx; \n\
      ### calculate spherical fog coord \n\
      DP3 result.fogcoord.x,pos,pos; \n\
      END \n";

   static const char *frgprog="!!ARBfp1.0 \n\
      PARAM l=program.env[0]; \n\
      PARAM p=program.env[1]; \n\
      TEMP col,nrm,len; \n\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3_SAT nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n\
      ### write resulting color \n\
      MOV result.color,col; \n\
      END \n";

   miniv4d mtx[3];
   double oglmtx[16];

   miniv3d light;

   if (lambda<=0.0f || cnt==0) return(vtx);

   if (warp!=NULL)
      if (NONLIN!=0) return(vtx);

   if (PRISM_VTXPROGID==0) PRISM_VTXPROGID=buildvtxprog(vtxprog);
   if (PRISM_FRGPROGID==0) PRISM_FRGPROGID=buildfrgprog(frgprog);

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

   light=miniv3d(lx,ly,lz);
   light=miniv3d(MVINVTRA[0]*light,MVINVTRA[1]*light,MVINVTRA[2]*light);
   light.normalize();

   bindvtxprog(PRISM_VTXPROGID);
   setvtxprogpar(0,0.0f,0.0f,0.0f,1.0f); // replace w-component

   bindfrgprog(PRISM_FRGPROGID);
   setfrgprogpar(0,light.x,light.y,light.z,0.0f); // light direction
   setfrgprogpar(1,ls,lo,0.0f,0.0f); // light scale and offset

   color(pr,pg,pb,pa);

   vertexarray(cache,4);
   rendertriangles(0,3*cnt);
   vertexarray(NULL);

   vtx+=3*cnt;

   bindvtxprog(0);
   bindfrgprog(0);

   mtxpop();
   mtxproj();
   mtxpop();
   mtxmodel();

   exitstate();

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

      for (id=0; id<NUMTERRAIN; id++) TERRAIN[id].ray->clear();
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
   int id;

   if (terrain==NULL) ERRORMSG();

   id=terrain->getid();

   TERRAIN[id].tile=NULL;

   freeterrain(&TERRAIN[id]);
   }

// determine whether or not a tileset is displayed
void minicache::display(minitile *terrain,int visible)
   {
   if (terrain==NULL) ERRORMSG();

   TERRAIN[terrain->getid()].isvisible=visible;
   }

// specify whether or not a tileset is treated like a patch
void minicache::setpatch(minitile *terrain,int patch)
   {
   if (terrain==NULL) ERRORMSG();

   TERRAIN[terrain->getid()].ispatch=patch;
   }

// specify per-tileset lighting
void minicache::setlight(minitile *terrain,float lx,float ly,float lz,float ls,float lo)
   {
   int id;

   if (terrain==NULL) ERRORMSG();

   id=terrain->getid();

   TERRAIN[id].lx=lx;
   TERRAIN[id].ly=ly;
   TERRAIN[id].lz=lz;

   TERRAIN[id].ls=ls;
   TERRAIN[id].lo=lo;
   }

// make cache current
void minicache::makecurrent()
   {CACHE=this;}

// set culling mode
void minicache::setculling(int on)
   {CULLMODE=on;}

// set ray shooting mode
void minicache::setshooting(int on)
   {RAYMODE=on;}

// define triangle mesh opacity
void minicache::setopacity(float alpha)
   {OPACITY=alpha;}

// define alpha test threshold
void minicache::setalphatest(float alpha)
   {ALPHATEST=alpha;}

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

// enable non-linear warp
void minicache::usenonlinear(int on)
   {NONLIN=on;}

// check for non-linear warp
int minicache::getnonlinear()
   {return(NONLIN);}

// initialize shader programs from snippets
void minicache::initshader()
   {
   // default vertex shader (initializer snippet)
   static const char *vtxprog_i="!!ARBvp1.0 \n\
      PARAM t=program.env[0]; \n\
      PARAM e=program.env[1]; \n\
      PARAM u=program.env[2]; \n\
      PARAM v=program.env[3]; \n\
      PARAM d=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      PARAM c5=program.env[10]; \n\
      PARAM c6=program.env[11]; \n\
      PARAM c7=program.env[12]; \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM matrix[4]={state.matrix.modelview}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      TEMP vtx,col,nrm,pos,vec,gen; \n";

   // default vertex shader (main snippet #1)
   static const char *vtxprog_s1="\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position; \n\
      MOV col,vertex.color; \n\
      MOV nrm,vertex.normal; \n";

   // default vertex shader (main snippet #2, texgen)
   static const char *vtxprog_s2="\
      ### calculate tex coords \n\
      MAD result.texcoord[0].x,vtx.x,t.x,t.z; \n\
      MAD result.texcoord[0].y,vtx.z,t.y,t.w; \n\
      MUL result.texcoord[0].z,vtx.y,e.y; \n";

   // default vertex shader (main snippet #3, non-linear transformation)
   static const char *vtxprog_s3="\
      PARAM m=program.env[13]; \n\
      PARAM p1=program.env[14]; \n\
      PARAM p2=program.env[15]; \n\
      PARAM p3=program.env[16]; \n\
      PARAM p4=program.env[17]; \n\
      PARAM p5=program.env[18]; \n\
      PARAM p6=program.env[19]; \n\
      PARAM p7=program.env[20]; \n\
      PARAM p8=program.env[21]; \n\
      PARAM n1=program.env[22]; \n\
      PARAM n2=program.env[23]; \n\
      PARAM n3=program.env[24]; \n\
      PARAM n4=program.env[25]; \n\
      PARAM n5=program.env[26]; \n\
      PARAM n6=program.env[27]; \n\
      PARAM n7=program.env[28]; \n\
      PARAM n8=program.env[29]; \n\
      TEMP pos1,pos2,pos3,pos4,pos5,pos6; \n\
      TEMP vec1,vec2,vec3,vec4,vec5,vec6; \n\
      ### normalize vertex \n\
      MUL pos,m,vtx; \n\
      ADD pos.y,pos.y,0.5; \n\
      ### tri-linear vertex interpolation \n\
      SUB gen.xyz,1.0,pos; \n\
      MUL pos1,pos.x,p2; \n\
      MAD pos1,gen.x,p1,pos1; \n\
      MUL pos2,pos.x,p4; \n\
      MAD pos2,gen.x,p3,pos2; \n\
      MUL pos3,pos.x,p6; \n\
      MAD pos3,gen.x,p5,pos3; \n\
      MUL pos4,pos.x,p8; \n\
      MAD pos4,gen.x,p7,pos4; \n\
      MUL pos5,pos.z,pos2; \n\
      MAD pos5,gen.z,pos1,pos5; \n\
      MUL pos6,pos.z,pos4; \n\
      MAD pos6,gen.z,pos3,pos6; \n\
      MUL vtx,pos.y,pos6; \n\
      MAD vtx,gen.y,pos5,vtx; \n\
      ### tri-linear normal interpolation \n\
      MUL vec1,pos.x,n2; \n\
      MAD vec1,gen.x,n1,vec1; \n\
      MUL vec2,pos.x,n4; \n\
      MAD vec2,gen.x,n3,vec2; \n\
      MUL vec3,pos.x,n6; \n\
      MAD vec3,gen.x,n5,vec3; \n\
      MUL vec4,pos.x,n8; \n\
      MAD vec4,gen.x,n7,vec4; \n\
      MUL vec5,pos.z,vec2; \n\
      MAD vec5,gen.z,vec1,vec5; \n\
      MUL vec6,pos.z,vec4; \n\
      MAD vec6,gen.z,vec3,vec6; \n\
      MUL nrm,pos.y,vec6; \n\
      MAD nrm,gen.y,vec5,nrm; \n";

   // default vertex shader (main snippet #4, linear transformation)
   static const char *vtxprog_s4="\
      ### transform vertex with combined modelview \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      ### transform normal with inverse transpose \n\
      DP4 vec.x,invtra[0],nrm; \n\
      DP4 vec.y,invtra[1],nrm; \n\
      DP4 vec.z,invtra[2],nrm; \n\
      DP4 vec.w,invtra[3],nrm; \n";

   // default vertex shader (main snippet #5, write vertex)
   static const char *vtxprog_s5="\
      ### write resulting vertex \n\
      MOV result.position,pos; \n\
      MOV result.color,col; \n\
      ### pass normal as tex coords \n\
      MOV result.texcoord[1],vec; \n";

   // default vertex shader (main snippet #6, eye linear coords)
   static const char *vtxprog_s6="\
      ### calculate eye linear coordinates \n\
      DP4 pos.x,matrix[0],vtx; \n\
      DP4 pos.y,matrix[1],vtx; \n\
      DP4 pos.z,matrix[2],vtx; \n\
      DP4 pos.w,matrix[3],vtx; \n\
      DP4 gen.x,pos,u; \n\
      DP4 gen.y,pos,v; \n\
      MAD result.texcoord[2].x,gen.x,d.x,d.y; \n\
      MAD result.texcoord[2].y,gen.y,d.z,d.w; \n";

   // default vertex shader (terminator snippet)
   static const char *vtxprog_t="\
      ### calculate spherical fog coord \n\
      DP3 result.fogcoord.x,pos,pos; \n\
      END \n";

   // default pixel shader (initializer snippet)
   static const char *frgprog_i="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM l=program.env[2]; \n\
      PARAM p=program.env[3]; \n\
      PARAM o=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      PARAM c5=program.env[10]; \n\
      PARAM c6=program.env[11]; \n\
      PARAM c7=program.env[12]; \n\
      TEMP col,colt,nrm,len; \n";

   // default pixel shader (main snippet #1)
   static const char *frgprog_s1="\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n\
      ### fetch texture color \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      ### modulate with fragment color \n\
      MUL col,col,colt; \n";

   // default pixel shader (main snippet #2)
   static const char *frgprog_s2="\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3_SAT nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n";

   // default pixel shader (terminator snippet)
   static const char *frgprog_t="\
      ### write resulting color \n\
      MOV result.color,col; \n\
      END \n";

   if (VTXPROG_STD_L==NULL)
      VTXPROG_STD_L=concatprog(vtxprog_i,vtxprog_s1,vtxprog_s2,NULL,vtxprog_s4,vtxprog_s5,vtxprog_s6,vtxprog_t);

   if (VTXPROG_STD_NL==NULL)
      VTXPROG_STD_NL=concatprog(vtxprog_i,vtxprog_s1,vtxprog_s2,vtxprog_s3,vtxprog_s4,vtxprog_s5,vtxprog_s6,vtxprog_t);

   if (FRGPROG_STD==NULL)
      FRGPROG_STD=concatprog(frgprog_i,frgprog_s1,frgprog_s2,frgprog_t);

   if (SEAPROG_STD==NULL)
      SEAPROG_STD=concatprog(frgprog_i,frgprog_s1,frgprog_s2,frgprog_t);
   }

// concatenate shader program from snippets
char *minicache::concatprog(const char *s1,
                            const char *s2,
                            const char *s3,
                            const char *s4,
                            const char *s5,
                            const char *s6,
                            const char *s7,
                            const char *s8,
                            const char *s9,
                            const char *s10)
   {
   char *prog1,*prog2;

   prog1=strcct(s1,s2);
   prog2=strcct(prog1,s3);
   free(prog1);
   prog1=strcct(prog2,s4);
   free(prog2);
   prog2=strcct(prog1,s5);
   free(prog1);
   prog1=strcct(prog2,s6);
   free(prog2);
   prog2=strcct(prog1,s7);
   free(prog1);
   prog1=strcct(prog2,s8);
   free(prog2);
   prog2=strcct(prog1,s9);
   free(prog1);
   prog1=strcct(prog2,s10);
   free(prog2);

   return(prog1);
   }

// set vertex shader plugin
void minicache::setvtxshader(const char *vp)
   {
   initshader();

   if (vp==NULL)
      if (NONLIN==0) vp=VTXPROG_STD_L;
      else vp=VTXPROG_STD_NL;

   if (VTXPROG!=NULL)
      {
      if (strcmp(vp,VTXPROG)==0) return;
      free(VTXPROG);
      }

   VTXPROG=strdup(vp);
   VTXDIRTY=1;
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
   int i;

   if (VTXDIRTY!=0)
      {
      if (VTXPROGID!=0) deletevtxprog(VTXPROGID);

      VTXPROGID=buildvtxprog(VTXPROG);
      VTXDIRTY=0;
      }

   if (VTXPROGID!=0)
      {
      bindvtxprog(VTXPROGID);

      setvtxshadertexgen();
      bindvtxshaderdetailtex();

      for (i=0; i<8; i++)
         setvtxprogpar(5+i,VTXSHADERPAR1[i],VTXSHADERPAR2[i],VTXSHADERPAR3[i],VTXSHADERPAR4[i]); // external constants
      }
   }

// set vertex shader texture mapping parameters
void minicache::setvtxshadertexprm(float s1,float s2,float o1,float o2,float scale)
   {
   if (VTXPROGID!=0)
      {
      setvtxprogpar(0,s1,s2,o1,o2); // texgen scale and offset
      setvtxprogpar(1,0.0f,scale,0.0f,0.0f); // elevation scale
      }
   }

// set vertex shader texture coordinate generation parameter vector
void minicache::setvtxshadertexgen()
   {
   TERRAIN_TYPE *t;

   t=&TERRAIN[RENDER_ID];

   setvtxprogpar(2,t->s1,t->s2,t->s3,t->s4); // detail texgen s-coordinate
   setvtxprogpar(3,t->t1,t->t2,t->t3,t->t4); // detail texgen t-coordinate

   setvtxprogpar(4,1.0f,0.0f,1.0f,0.0f); // detail texgen scale and bias
   }

// bind vertex shader detail texture
void minicache::bindvtxshaderdetailtex()
   {
   TERRAIN_TYPE *t;

   t=&TERRAIN[RENDER_ID];

   if (t->detail_texid!=0 && t->detail_width>0 && t->detail_height>0)
      {
      texunit(2);
      bindtexmap(t->detail_texid,t->detail_width,t->detail_height,0,t->detail_mipmaps);
      texunit(0);

      setvtxprogpar(4,
                    0.5f/t->detail_width,(t->detail_width-1)/t->detail_width, // detail texgen scale and bias for s-coordinate
                    0.5f/t->detail_height,(t->detail_height-1)/t->detail_height); // detail texgen scale and bias for t-coordinate
      }
   }

// unbind vertex shader detail texture
void minicache::unbindvtxshaderdetailtex()
   {
   TERRAIN_TYPE *t;

   t=&TERRAIN[RENDER_ID];

   if (t->detail_texid!=0)
      {
      texunit(2);
      bindtexmap(0,0,0,0,0);
      texunit(0);
      }
   }

// set vertex shader non-linear transformation
void minicache::setvtxshadernonlin(int S,float scale,miniwarpbase *warp)
   {
   int i;

   miniv3d p[8],n[8];
   double e;

   float pars[64];

   e=warp->getcorners(p,n);

   for (i=0; i<8; i++)
      {
      pars[4*i]=p[i].x;
      pars[4*i+1]=p[i].y;
      pars[4*i+2]=p[i].z;
      pars[4*i+3]=1.0f;

      pars[32+4*i]=n[i].x;
      pars[32+4*i+1]=n[i].y;
      pars[32+4*i+2]=n[i].z;
      pars[32+4*i+3]=1.0f;
      }

   setvtxprogpar(13,1.0f/(S-1),scale/e,1.0f/(S-1),1.0);
   setvtxprogpars(14,16,pars);
   }

// disable vertex shader plugin
void minicache::disablevtxshader()
   {
   if (VTXPROGID!=0)
      {
      bindvtxprog(0);
      unbindvtxshaderdetailtex();
      }
   }

// switch vertex shader plugin on/off
void minicache::usevtxshader(int on)
   {USEVTXSHADER=on;}

// set pixel shader plugin
void minicache::setpixshader(const char *fp)
   {
   initshader();

   if (fp==NULL) fp=FRGPROG_STD;

   if (FRGPROG!=NULL)
      {
      if (strcmp(fp,FRGPROG)==0) return;
      free(FRGPROG);
      }

   FRGPROG=strdup(fp);
   FRGDIRTY=1;
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

// set pixel shader RGB texture map
void minicache::setpixshadertexRGB(unsigned char *image,int width,int height)
   {setpixshadertex(image,width,height,3);}

// set pixel shader RGBA texture map
void minicache::setpixshadertexRGBA(unsigned char *image,int width,int height)
   {setpixshadertex(image,width,height,4);}

// define parameter vector for eye linear texture coordinate generation per tileset
void minicache::setpixshadertexgen(minitile *terrain,float s1,float s2,float s3,float s4,float t1,float t2,float t3,float t4)
   {
   TERRAIN_TYPE *t;

   t=&TERRAIN[terrain->getid()];

   t->s1=s1;
   t->s2=s2;
   t->s3=s3;
   t->s4=s4;

   t->t1=t1;
   t->t2=t2;
   t->t3=t3;
   t->t4=t4;
   }

// define detail texture opacity parameter per tileset
void minicache::setpixshadertexalpha(minitile *terrain,float alpha)
   {
   TERRAIN_TYPE *t;

   t=&TERRAIN[terrain->getid()];

   t->detail_alpha=alpha;
   }

// define RGB[A] detail texture per tileset
void minicache::setpixshaderdetailtex(minitile *terrain,unsigned char *image,int width,int height,int components,int mipmaps)
   {
   TERRAIN_TYPE *t;

   if (width<2 || height<2) ERRORMSG();

   t=&TERRAIN[terrain->getid()];

   if (t->detail_texid!=0)
      {
      if (t->detail_nofree==0) deletetexmap(t->detail_texid);
      t->detail_texid=0;
      }

   if (image!=NULL)
      {
      if (components==3) t->detail_texid=buildRGBtexmap(image,&width,&height,mipmaps);
      else if (components==4) t->detail_texid=buildRGBAtexmap(image,&width,&height,mipmaps);
      else ERRORMSG();

      t->detail_width=width;
      t->detail_height=height;

      t->detail_mipmaps=mipmaps;

      t->detail_alpha=1.0f;

      t->detail_nofree=0;
      }
   }

// define RGB detail texture per tileset
void minicache::setpixshaderdetailtexRGB(minitile *terrain,unsigned char *image,int width,int height,int mipmaps)
   {setpixshaderdetailtex(terrain,image,width,height,3,mipmaps);}

// define RGBA detail texture per tileset
void minicache::setpixshaderdetailtexRGBA(minitile *terrain,unsigned char *image,int width,int height,int mipmaps)
   {setpixshaderdetailtex(terrain,image,width,height,4,mipmaps);}

// define detail texture id per tileset
void minicache::setpixshaderdetailtexid(minitile *terrain,int texid,int width,int height,int mipmaps)
   {
   TERRAIN_TYPE *t;

   if (texid!=0 && (width<2 || height<2)) ERRORMSG();

   t=&TERRAIN[terrain->getid()];

   if (t->detail_texid!=0)
      {
      if (t->detail_nofree==0) deletetexmap(t->detail_texid);
      t->detail_texid=0;
      }

   t->detail_texid=texid;

   t->detail_width=width;
   t->detail_height=height;

   t->detail_mipmaps=mipmaps;

   t->detail_alpha=1.0f;

   t->detail_nofree=1;
   }

// switch pixel shader plugin on/off
void minicache::usepixshader(int on)
   {USEPIXSHADER=on;}

// set sea shader plugin
void minicache::setseashader(const char *sp)
   {
   initshader();

   if (sp==NULL) sp=SEAPROG_STD;

   if (SEAPROG!=NULL)
      {
      if (strcmp(sp,SEAPROG)==0) return;
      free(SEAPROG);
      }

   SEAPROG=strdup(sp);
   SEADIRTY=1;
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

// set sea shader RGB texture map
void minicache::setseashadertexRGB(unsigned char *image,int width,int height)
   {setseashadertex(image,width,height,3);}

// set sea shader RGBA texture map
void minicache::setseashadertexRGBA(unsigned char *image,int width,int height)
   {setseashadertex(image,width,height,4);}

// switch sea shader plugin on/off
void minicache::useseashader(int on)
   {USESEASHADER=on;}

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
   int i;

   if (FRGDIRTY!=0)
      {
      if (FRGPROGID!=0) deletefrgprog(FRGPROGID);

      FRGPROGID=buildfrgprog(FRGPROG);
      FRGDIRTY=0;
      }

   if (FRGPROGID!=0)
      {
      bindfrgprog(FRGPROGID);
      setfrgprogpar(0,1.0f,0.0f,0.0f,0.0f); // color scale and offset

      if (PIXSHADERTEXID!=0)
         {
         texunit(1);
         bindtexmap(PIXSHADERTEXID,PIXSHADERTEXWIDTH,PIXSHADERTEXHEIGHT,0,0);
         texunit(0);

         setfrgprogpar(1,
                       (float)(PIXSHADERTEXWIDTH-1)/PIXSHADERTEXWIDTH,0.5f/PIXSHADERTEXWIDTH, // texture scale and bias for s-coordinate
                       (float)(PIXSHADERTEXHEIGHT-1)/PIXSHADERTEXHEIGHT,0.5f/PIXSHADERTEXHEIGHT); // texture scale and bias for t-coordinate
         }

      for (i=0; i<8; i++)
         setfrgprogpar(5+i,PIXSHADERPAR1[i],PIXSHADERPAR2[i],PIXSHADERPAR3[i],PIXSHADERPAR4[i]); // external constants
      }
   }

// set pixel shader texture mapping parameters
void minicache::setpixshadertexprm(float s,float o,
                                   float lx,float ly,float lz,
                                   float ls,float lo,
                                   float a)
   {
   if (FRGPROGID!=0 || SEAPROGID!=0)
      {
      setfrgprogpar(0,s,o,0.0f,0.0f); // color scale and offset

      setfrgprogpar(2,lx,ly,lz,0.0f); // light direction
      setfrgprogpar(3,ls,lo,0.0f,0.0f); // light scale and offset

      setfrgprogpar(4,0.0f,0.0f,0.0f,a); // detail texture alpha
      }
   }

// disable pixel shader plugin
void minicache::disablepixshader()
   {
   if (FRGPROGID!=0)
      {
      bindfrgprog(0);

      if (PIXSHADERTEXID!=0)
         {
         texunit(1);
         bindtexmap(0);
         texunit(0);
         }
      }
   }

// enable sea shader plugin
void minicache::enableseashader()
   {
   int i;

   if (SEADIRTY!=0)
      {
      if (SEAPROGID!=0) deletefrgprog(SEAPROGID);

      SEAPROGID=buildfrgprog(SEAPROG);
      SEADIRTY=0;
      }

   if (SEAPROGID!=0)
      {
      bindfrgprog(SEAPROGID);
      setfrgprogpar(0,1.0f,0.0f,0.0f,0.0f); // color scale and offset

      if (SEASHADERTEXID!=0)
         {
         texunit(1);
         bindtexmap(SEASHADERTEXID,SEASHADERTEXWIDTH,SEASHADERTEXHEIGHT,0,0);
         texunit(0);

         setfrgprogpar(1,
                       (float)(SEASHADERTEXWIDTH-1)/SEASHADERTEXWIDTH,0.5f/SEASHADERTEXWIDTH, // texture scale and bias for s-coordinate
                       (float)(SEASHADERTEXHEIGHT-1)/SEASHADERTEXHEIGHT,0.5f/SEASHADERTEXHEIGHT); // texture scale and bias for t-coordinate
         }

      for (i=0; i<8; i++)
         setfrgprogpar(5+i,SEASHADERPAR1[i],SEASHADERPAR2[i],SEASHADERPAR3[i],PIXSHADERPAR4[i]); // external constants
      }
   }

// disable sea shader plugin
void minicache::disableseashader()
   {
   if (SEAPROGID!=0)
      {
      bindfrgprog(0);

      if (SEASHADERTEXID!=0)
         {
         texunit(1);
         bindtexmap(0);
         texunit(0);
         }
      }
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
   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   t=&TERRAIN[id];
   c=&t->cache[1-t->cache_num];

   return(c->fancnt);
   }

// get vertex count of active buffer
int minicache::getvtxcnt(int id)
   {
   TERRAIN_TYPE *t;
   CACHE_TYPE *c;

   t=&TERRAIN[id];
   c=&t->cache[1-t->cache_num];

   return(c->vtxcnt);
   }

// get cached terrain object
minitile *minicache::getminitile(int id)
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
void minicache::configure_zfight_patch(float zscale) {CONFIGURE_ZSCALE_PATCH=zscale;}
void minicache::configure_zfight_prisms(float zscale) {CONFIGURE_ZSCALE_PRISMS=zscale;}
void minicache::configure_calcbounds(int calcbounds) {CONFIGURE_CALCBOUNDS=calcbounds;}
void minicache::configure_omitsea(int omitsea) {CONFIGURE_OMITSEA=omitsea;}
