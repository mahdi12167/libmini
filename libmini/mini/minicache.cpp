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

   CACHE_NUM=1;

   CACHE_SIZE1=CACHE_SIZE2=0;
   CACHE_MAXSIZE=1;

   if ((CACHE1_OP=(unsigned char *)malloc(CACHE_MAXSIZE))==NULL) ERRORMSG();
   if ((CACHE2_OP=(unsigned char *)malloc(CACHE_MAXSIZE))==NULL) ERRORMSG();

   if ((CACHE1_ARG=(float *)malloc(3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
   if ((CACHE2_ARG=(float *)malloc(3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();

   PRISM_SIZE1=PRISM_SIZE2=0;
   PRISM_MAXSIZE=1;

   if ((PRISM_CACHE1=(float *)malloc(4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
   if ((PRISM_CACHE2=(float *)malloc(4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();

   CACHE_ID=0;
   CACHE_PHASE=-1;

   CACHE_WARP=NULL;

   RAY=new miniray;

   FIRST_FANCNT=0;

   RENDER_ID=0;
   RENDER_PHASE=-1;

   OPACITY=1.0f;

   SEA_R=0.0f;
   SEA_G=0.5f;
   SEA_B=1.0f;
   SEA_A=1.0f;

   PRISM_R=PRISM_G=PRISM_B=1.0f;
   PRISM_A=0.9f;

   FANCNT1=FANCNT2=0;
   VTXCNT1=VTXCNT2=0;

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
   if (TERRAIN!=NULL) free(TERRAIN);

   free(CACHE1_OP);
   free(CACHE2_OP);

   free(CACHE1_ARG);
   free(CACHE2_ARG);

   free(PRISM_CACHE1);
   free(PRISM_CACHE2);

   delete RAY;

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

   if (CACHE_SIZE1>=CACHE_MAXSIZE || CACHE_SIZE2>=CACHE_MAXSIZE)
      {
      CACHE_MAXSIZE*=2;

      if ((CACHE1_OP=(unsigned char *)realloc(CACHE1_OP,CACHE_MAXSIZE))==NULL) ERRORMSG();
      if ((CACHE2_OP=(unsigned char *)realloc(CACHE2_OP,CACHE_MAXSIZE))==NULL) ERRORMSG();

      if ((CACHE1_ARG=(float *)realloc(CACHE1_ARG,3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
      if ((CACHE2_ARG=(float *)realloc(CACHE2_ARG,3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
      }

   if (CACHE_NUM==1)
      {
      if (op==BEGINFAN_OP)
         {
         FANCNT1++;
         FIRST_FANCNT++;
         LAST_BEGINFAN=CACHE_SIZE1;
         }
      else if (op==FANVERTEX_OP)
         {
         VTXCNT1++;
         CACHE1_ARG[3*LAST_BEGINFAN]++;
         }
      else
         if (CONFIGURE_ENABLERAY!=0)
            {
            if (FIRST_FANCNT>0)
               {
               cols=TERRAIN[CACHE_ID].tile->getcols();
               rows=TERRAIN[CACHE_ID].tile->getrows();

               xdim=TERRAIN[CACHE_ID].tile->getcoldim();
               zdim=TERRAIN[CACHE_ID].tile->getrowdim();

               centerx=TERRAIN[CACHE_ID].tile->getcenterx();
               centery=TERRAIN[CACHE_ID].tile->getcentery();
               centerz=TERRAIN[CACHE_ID].tile->getcenterz();

               s.x=xdim/(FIRST_SIZE-1);
               s.y=FIRST_SCALE;
               s.z=-zdim/(FIRST_SIZE-1);

               o.x=xdim*(FIRST_COL-(cols-1)/2.0f)+centerx-xdim/2.0f;
               o.y=centery;
               o.z=zdim*(FIRST_ROW-(rows-1)/2.0f)+centerz+zdim/2.0f;

               RAY->addtrianglefans(&CACHE1_ARG,3*FIRST_BEGINFAN,FIRST_FANCNT,0,&s,&o,0,CACHE_WARP);
               }

            if (op==TRIGGER_OP)
               {
               if (CACHE_PHASE==0) RAY->clearbuffer();
               else if (CACHE_PHASE==1) FIRST_SCALE=b;
               else if (CACHE_PHASE==4) RAY->swapbuffer();
               }
            else if (op==TEXMAP_OP)
               {
               FIRST_COL=ftrc(a+0.5f);
               FIRST_ROW=ftrc(b+0.5f);
               FIRST_SIZE=ftrc(c+0.5f);
               }

            FIRST_BEGINFAN=CACHE_SIZE1+1;
            FIRST_FANCNT=0;
            }

      CACHE1_OP[CACHE_SIZE1]=op;

      CACHE1_ARG[3*CACHE_SIZE1]=a;
      CACHE1_ARG[3*CACHE_SIZE1+1]=b;
      CACHE1_ARG[3*CACHE_SIZE1+2]=c;

      CACHE_SIZE1++;
      }
   else
      {
      if (op==BEGINFAN_OP)
         {
         FANCNT2++;
         FIRST_FANCNT++;
         LAST_BEGINFAN=CACHE_SIZE2;
         }
      else if (op==FANVERTEX_OP)
         {
         VTXCNT2++;
         CACHE2_ARG[3*LAST_BEGINFAN]++;
         }
      else
         if (CONFIGURE_ENABLERAY!=0)
            {
            if (FIRST_FANCNT>0)
               {
               cols=TERRAIN[CACHE_ID].tile->getcols();
               rows=TERRAIN[CACHE_ID].tile->getrows();

               xdim=TERRAIN[CACHE_ID].tile->getcoldim();
               zdim=TERRAIN[CACHE_ID].tile->getrowdim();

               centerx=TERRAIN[CACHE_ID].tile->getcenterx();
               centery=TERRAIN[CACHE_ID].tile->getcentery();
               centerz=TERRAIN[CACHE_ID].tile->getcenterz();

               s.x=xdim/(FIRST_SIZE-1);
               s.y=FIRST_SCALE;
               s.z=-zdim/(FIRST_SIZE-1);

               o.x=xdim*(FIRST_COL-(cols-1)/2.0f)+centerx-xdim/2.0f;
               o.y=centery;
               o.z=zdim*(FIRST_ROW-(rows-1)/2.0f)+centerz+zdim/2.0f;

               RAY->addtrianglefans(&CACHE2_ARG,3*FIRST_BEGINFAN,FIRST_FANCNT,0,&s,&o,0,CACHE_WARP);
               }

            if (op==TRIGGER_OP)
               {
               if (CACHE_PHASE==0) RAY->clearbuffer();
               else if (CACHE_PHASE==1) FIRST_SCALE=b;
               else if (CACHE_PHASE==4) RAY->swapbuffer();
               }
            else if (op==TEXMAP_OP)
               {
               FIRST_COL=ftrc(a+0.5f);
               FIRST_ROW=ftrc(b+0.5f);
               FIRST_SIZE=ftrc(c+0.5f);
               }

            FIRST_BEGINFAN=CACHE_SIZE2+1;
            FIRST_FANCNT=0;
            }

      CACHE2_OP[CACHE_SIZE2]=op;

      CACHE2_ARG[3*CACHE_SIZE2]=a;
      CACHE2_ARG[3*CACHE_SIZE2+1]=b;
      CACHE2_ARG[3*CACHE_SIZE2+2]=c;

      CACHE_SIZE2++;
      }
   }

void minicache::cacheprismedge(float x,float y,float yf,float z)
   {
   miniv4d v1;

   if (CACHE_WARP!=NULL)
      if (CACHE_WARP->getglb()==minicoord::MINICOORD_LINEAR)
         {
         x+=CACHE_WARP_MTX[0].w;
         y+=CACHE_WARP_MTX[1].w;
         z+=CACHE_WARP_MTX[2].w;
         }
      else
         {
         v1=miniv4d(x,y,z,1.0);

         x=CACHE_WARP_MTX[0]*v1;
         y=CACHE_WARP_MTX[1]*v1;
         z=CACHE_WARP_MTX[2]*v1;
         }

   if (PRISMEDGE_CALLBACK!=NULL) PRISMEDGE_CALLBACK(x,y,yf,z,CALLBACK_DATA);
   else
      {
      if (PRISM_SIZE1>=PRISM_MAXSIZE || PRISM_SIZE2>=PRISM_MAXSIZE)
         {
         PRISM_MAXSIZE*=2;

         if ((PRISM_CACHE1=(float *)realloc(PRISM_CACHE1,4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         if ((PRISM_CACHE2=(float *)realloc(PRISM_CACHE2,4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         }

      if (CACHE_NUM==1)
         {
         PRISM_CACHE1[4*PRISM_SIZE1]=x;
         PRISM_CACHE1[4*PRISM_SIZE1+1]=y;
         PRISM_CACHE1[4*PRISM_SIZE1+2]=yf;
         PRISM_CACHE1[4*PRISM_SIZE1+3]=z;

         PRISM_SIZE1++;
         }
      else
         {
         PRISM_CACHE2[4*PRISM_SIZE2]=x;
         PRISM_CACHE2[4*PRISM_SIZE2+1]=y;
         PRISM_CACHE2[4*PRISM_SIZE2+2]=yf;
         PRISM_CACHE2[4*PRISM_SIZE2+3]=z;

         PRISM_SIZE2++;
         }
      }
   }

void minicache::cachetrigger(int phase,float scale,float ex,float ey,float ez)
   {
   miniv4d v1;

   CACHE_PHASE=phase;

   if (CACHE_PHASE==0)
      {
      CACHE_WARP=TERRAIN[CACHE_ID].tile->getwarp();
      if (CACHE_WARP!=NULL) CACHE_WARP->getwarp(CACHE_WARP_MTX);
      }

   if (CACHE_PHASE==3)
      if (PRISMWARP_CALLBACK!=NULL) PRISMWARP_CALLBACK(CACHE_WARP,CALLBACK_DATA);

   cache(TRIGGER_OP,phase,scale);

   if (CACHE_PHASE==0)
      {
      // swap vertex buffers
      if (CACHE_NUM==1)
         {
         CACHE_SIZE2=PRISM_SIZE2=0;
         FANCNT2=VTXCNT2=0;
         CACHE_NUM=2;
         }
      else
         {
         CACHE_SIZE1=PRISM_SIZE1=0;
         FANCNT1=VTXCNT1=0;
         CACHE_NUM=1;
         }

      // shrink vertex buffer
      if (CACHE_SIZE1<CACHE_MAXSIZE/4 && CACHE_SIZE2<CACHE_MAXSIZE/4)
         {
         CACHE_MAXSIZE/=2;

         if ((CACHE1_OP=(unsigned char *)realloc(CACHE1_OP,CACHE_MAXSIZE))==NULL) ERRORMSG();
         if ((CACHE2_OP=(unsigned char *)realloc(CACHE2_OP,CACHE_MAXSIZE))==NULL) ERRORMSG();

         if ((CACHE1_ARG=(float *)realloc(CACHE1_ARG,3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         if ((CACHE2_ARG=(float *)realloc(CACHE2_ARG,3*CACHE_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         }

      // shrink prism buffer
      if (PRISM_SIZE1<PRISM_MAXSIZE/4 && PRISM_SIZE2<PRISM_MAXSIZE/4)
         {
         PRISM_MAXSIZE/=2;

         if ((PRISM_CACHE1=(float *)realloc(PRISM_CACHE1,4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         if ((PRISM_CACHE2=(float *)realloc(PRISM_CACHE2,4*PRISM_MAXSIZE*sizeof(float)))==NULL) ERRORMSG();
         }
      }

   if (PRISMCACHE_CALLBACK!=NULL)
      if (CACHE_WARP==NULL)
         PRISMCACHE_CALLBACK(phase,scale,ex,ey,ez,CALLBACK_DATA);
      else
         {
         v1=miniv4d(ex,ey,ez,1.0);

         PRISMCACHE_CALLBACK(phase,scale,
                             CACHE_WARP_MTX[0]*v1,CACHE_WARP_MTX[1]*v1,CACHE_WARP_MTX[2]*v1,
                             CALLBACK_DATA);
         }
   }

void minicache::cachesync(int id)
   {CACHE_ID=id;}

// render back buffer of the cache
int minicache::rendercache()
   {
   int vtx=0;

#ifndef NOOGL

   int i,p;

   vtx+=getvtxcnt();

   if (CACHE_NUM==1)
      for (i=0; i<CACHE_SIZE2; i++)
         switch (CACHE2_OP[i])
            {
            case BEGINFAN_OP:
               p=ftrc(CACHE2_ARG[3*i]+0.5f);
               glDrawArrays(GL_TRIANGLE_FAN,i+1,p);
               i+=p;
               break;
            case FANVERTEX_OP:
               break;
            case TEXMAP_OP:
               rendertexmap(ftrc(CACHE2_ARG[3*i]+0.5f),ftrc(CACHE2_ARG[3*i+1]+0.5f),ftrc(CACHE2_ARG[3*i+2]+0.5f));
               break;
            case TRIGGER_OP:
               vtx+=rendertrigger(ftrc(CACHE2_ARG[3*i]+0.5f),CACHE2_ARG[3*i+1]);
               break;
            }
   else
      for (i=0; i<CACHE_SIZE1; i++)
         switch (CACHE1_OP[i])
            {
            case BEGINFAN_OP:
               p=ftrc(CACHE1_ARG[3*i]+0.5f);
               glDrawArrays(GL_TRIANGLE_FAN,i+1,p);
               i+=p;
               break;
            case FANVERTEX_OP:
               break;
            case TEXMAP_OP:
               rendertexmap(ftrc(CACHE1_ARG[3*i]+0.5f),ftrc(CACHE1_ARG[3*i+1]+0.5f),ftrc(CACHE1_ARG[3*i+2]+0.5f));
               break;
            case TRIGGER_OP:
               vtx+=rendertrigger(ftrc(CACHE1_ARG[3*i]+0.5f),CACHE1_ARG[3*i+1]);
               break;
            }

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

   mtxpop();

   cols=TERRAIN[RENDER_ID].tile->getcols();
   rows=TERRAIN[RENDER_ID].tile->getrows();

   xdim=TERRAIN[RENDER_ID].tile->getcoldim();
   zdim=TERRAIN[RENDER_ID].tile->getrowdim();

   centerx=TERRAIN[RENDER_ID].tile->getcenterx();
   centery=TERRAIN[RENDER_ID].tile->getcentery();
   centerz=TERRAIN[RENDER_ID].tile->getcenterz();

   mtxpush();

   warp=TERRAIN[RENDER_ID].tile->getwarp();

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
   if (RENDER_PHASE==2)
      if (CONFIGURE_OVERLAP!=0.0f)
         if (S>=CONFIGURE_MINSIZE) mtxscale((S-1+CONFIGURE_OVERLAP)/(S-1),1.0f,(S-1+CONFIGURE_OVERLAP)/(S-1));

   mtxscale(xdim/(S-1),TERRAIN[RENDER_ID].scale,-zdim/(S-1));

   if (RENDER_PHASE==2 || CONFIGURE_SEAENABLETEX!=0)
      {
      texid=TERRAIN[RENDER_ID].tile->gettexid(m,n);
      texw=TERRAIN[RENDER_ID].tile->gettexw(m,n);
      texh=TERRAIN[RENDER_ID].tile->gettexh(m,n);
      texmm=TERRAIN[RENDER_ID].tile->gettexmm(m,n);

      bindtexmap(texid,texw,texh,S,texmm);

      if (USEVTXSHADER!=0)
         setvtxshadertexprm(1.0f/(S-1)*(texw-1)/texw,
                            -1.0f/(S-1)*(texh-1)/texh,
                            0.5f/texh,
                            1.0f-0.5f/texh,
                            TERRAIN[RENDER_ID].scale);
      }

#endif
   }

int minicache::rendertrigger(int phase,float scale)
   {
   int vtx=0;

#ifndef NOOGL

   RENDER_PHASE=phase;

   if (RENDER_PHASE==1) TERRAIN[RENDER_ID].scale=scale;
   else if (RENDER_PHASE==2)
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

      if (CACHE_NUM==1) glVertexPointer(3,GL_FLOAT,0,CACHE2_ARG);
      else glVertexPointer(3,GL_FLOAT,0,CACHE1_ARG);

      glEnableClientState(GL_VERTEX_ARRAY);
      }
   else if (RENDER_PHASE==3)
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

         if (CACHE_NUM==1) glVertexPointer(3,GL_FLOAT,0,CACHE2_ARG);
         else glVertexPointer(3,GL_FLOAT,0,CACHE1_ARG);

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
   else if (RENDER_PHASE==4)
      {
      TERRAIN[RENDER_ID].lambda=scale;

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

   if (PRISMTRIGGER_CALLBACK!=NULL)
      vtx+=PRISMTRIGGER_CALLBACK(phase,CALLBACK_DATA);

   if (RENDER_PHASE==4)
      if (PRISMEDGE_CALLBACK==NULL)
         if (PRISMRENDER_CALLBACK!=NULL)
            if (CACHE_NUM==1)
               vtx+=PRISMRENDER_CALLBACK(PRISM_CACHE2,PRISM_SIZE2/3,TERRAIN[RENDER_ID].lambda,CALLBACK_DATA);
            else
               vtx+=PRISMRENDER_CALLBACK(PRISM_CACHE1,PRISM_SIZE1/3,TERRAIN[RENDER_ID].lambda,CALLBACK_DATA);
         else
            if (CACHE_NUM==1)
               vtx+=renderprisms(PRISM_CACHE2,PRISM_SIZE2/3,TERRAIN[RENDER_ID].lambda,
                                 PRISM_R,PRISM_G,PRISM_B,PRISM_A);
            else
               vtx+=renderprisms(PRISM_CACHE1,PRISM_SIZE1/3,TERRAIN[RENDER_ID].lambda,
                                 PRISM_R,PRISM_G,PRISM_B,PRISM_A);

#endif

   return(vtx);
   }

int minicache::renderprisms(float *cache,int cnt,float lambda,
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
      MOV result.position,pos; \n\
      END";

   GLuint vtxprogid;

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
                       void (*prismwarp)(miniwarp *warp,void *data),
                       void (*prismcache)(int phase,float scale,float ex,float ey,float ez,void *data),
                       int (*prismrender)(float *cache,int cnt,float lambda,void *data),
                       int (*prismtrigger)(int phase,void *data),
                       void *data)
   {
   int id;

   CACHE=this;

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

            NUMTERRAIN++;

            break;
            }

   PRISMEDGE_CALLBACK=prismedge;
   PRISMWARP_CALLBACK=prismwarp;
   PRISMCACHE_CALLBACK=prismcache;
   PRISMRENDER_CALLBACK=prismrender;
   PRISMTRIGGER_CALLBACK=prismtrigger;
   CALLBACK_DATA=data;
   }

// detach a tileset
void minicache::detach(minitile *terrain)
   {
   if (terrain==NULL) ERRORMSG();

   TERRAIN[terrain->getid()].tile=NULL;
   }

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
      PARAM t=program.env[8]; \n\
      TEMP col; \n\
      ### fetch texture color \n\
      TEX col,fragment.texcoord[0],texture[0],2D; \n\
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
      PARAM t=program.env[8]; \n\
      TEMP col; \n\
      ### fetch texture color \n\
      TEX col,fragment.texcoord[0],texture[0],2D; \n\
      ### modulate with fragment color \n\
      MUL result.color,col,fragment.color; \n\
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

         if (GLEXT_MT!=0)
            if (PIXSHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(PIXSHADERTEXID,PIXSHADERTEXWIDTH,PIXSHADERTEXHEIGHT,0,0);
               glActiveTextureARB(GL_TEXTURE0_ARB);

               glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,10,
                                          (float)(PIXSHADERTEXWIDTH-1)/PIXSHADERTEXWIDTH,0.5f/PIXSHADERTEXWIDTH,
                                          (float)(PIXSHADERTEXHEIGHT-1)/PIXSHADERTEXHEIGHT,0.5f/PIXSHADERTEXHEIGHT);
#endif
               }
         }
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

         if (GLEXT_MT!=0)
            if (SEASHADERTEXID!=0)
               {
#ifdef GL_ARB_multitexture
               glActiveTextureARB(GL_TEXTURE1_ARB);
               bindtexmap(SEASHADERTEXID,SEASHADERTEXWIDTH,SEASHADERTEXHEIGHT,0,0);
               glActiveTextureARB(GL_TEXTURE0_ARB);

               glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,10,
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
   if (CACHE_NUM==1) return(FANCNT2);
   else return(FANCNT1);
   }

// get vertex count of active cache
int minicache::getvtxcnt()
   {
   if (CACHE_NUM==1) return(VTXCNT2);
   else return(VTXCNT1);
   }

// configuring
void minicache::configure_overlap(float overlap) {CONFIGURE_OVERLAP=overlap;}
void minicache::configure_minsize(int minsize) {CONFIGURE_MINSIZE=minsize;}
void minicache::configure_seatwosided(int seatwosided) {CONFIGURE_SEATWOSIDED=seatwosided;}
void minicache::configure_seaenabletex(int seaenabletex) {CONFIGURE_SEAENABLETEX=seaenabletex;}
void minicache::configure_zfight_sea(float zscale) {CONFIGURE_ZSCALE_SEA=zscale;}
void minicache::configure_zfight_prisms(float zscale) {CONFIGURE_ZSCALE_PRISMS=zscale;}
void minicache::configure_enableray(int enableray) {CONFIGURE_ENABLERAY=enableray;}
