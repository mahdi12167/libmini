// (c) by Stefan Roettger

#ifndef MINICACHE_H
#define MINICACHE_H

#include "miniv3d.h"
#include "miniv4d.h"

#include "miniOGL.h"

#include "minitile.h"
#include "miniwarp.h"

#include "miniray.h"

class minicache
   {
   public:

   //! default constructor
   minicache();

   //! destructor
   ~minicache();

   //! attach a tileset for scene double buffering
   void attach(minitile *terrain,
               void (*prismedge)(float x,float y,float yf,float z,void *data)=0,
               void (*prismcache)(int phase,float scale,float ex,float ey,float ez,void *data)=0,
               int (*prismrender)(float *cache,int cnt,float lambda,miniwarp *warp,void *data)=0,
               int (*prismtrigger)(int phase,void *data)=0,
               void (*prismsync)(int id,void *data)=0,
               void *data=0);

   //! detach a tileset
   void detach(minitile *terrain);

   //! make cache current
   //! required for multiple rendering windows/contexts
   //! this should be called before updating the vertex buffers of the corresponding window
   void makecurrent();

   //! render back buffer of the cache
   int rendercache();

   //! define triangle mesh opacity
   void setopacity(float alpha=1.0f);

   //! define color of semi-transparent sea
   void setseacolor(float r=0.0f,float g=0.5f,float b=1.0f,float a=1.0f);

   //! define color of prism boundary
   void setprismcolor(float prismR=1.0f,float prismG=1.0f,float prismB=1.0f,float prismA=0.9f);

   //! define optional vertex shader
   void setvtxshader(char *vtxprog=0);
   void setvtxshaderparams(float p1=0.0f,float p2=0.0f,float p3=0.0f,float p4=0.0f,int n=0);
   void usevtxshader(int on=1) {USEVTXSHADER=on;}

   //! define optional pixel shader
   void setpixshader(char *fragprog=0);
   void setpixshaderparams(float p1=0.0f,float p2=0.0f,float p3=0.0f,float p4=0.0f,int n=0);
   void setpixshadertex(unsigned char *image,int width,int height,int components=3);
   void setpixshadertexRGB(unsigned char *image,int width,int height);
   void setpixshadertexRGBA(unsigned char *image,int width,int height);
   void usepixshader(int on=1) {USEPIXSHADER=on;}

   //! define optional sea shader
   void setseashader(char *seaprog=0);
   void setseashaderparams(float p1=0.0f,float p2=0.0f,float p3=0.0f,float p4=0.0f,int n=0);
   void setseashadertex(unsigned char *image,int width,int height,int components=3);
   void setseashadertexRGB(unsigned char *image,int width,int height);
   void setseashadertexRGBA(unsigned char *image,int width,int height);
   void useseashader(int on=0) {USESEASHADER=on;}

   //! define optional sea callbacks
   void setseacb(void (*preseacb)(void *data)=0,
                 void (*postseacb)(void *data)=0,
                 void *data=0);

   //! get number of rendered primitives
   int getfancnt(); // rendered triangle fans
   int getvtxcnt(); // rendered vertices

   //! get cached terrain object
   minitile *gettile(int id);

   //! get ray intersection test object
   miniray *getray(int id);

   //! configuring
   void configure_overlap(float overlap=0.02f); // overlap of tile borders (prevents pixel flickering)
   void configure_minsize(int minsize=33); // disable overlap for mesh size less than minsize
   void configure_seatwosided(int seatwosided=1); // disable back face culling for the sea surface
   void configure_seaenabletex(int seaenabletex=0); // enable texture mappping for the sea surface
   void configure_zfight_sea(float zscale=0.99f); // shift sea towards the viewer to avoid Z-fighting
   void configure_zfight_prisms(float zscale=0.95f); // shift prisms towards the viewer to avoid Z-fighting
   void configure_enableray(int enableray=0); // enable ray object

   protected:

   enum
      {
      BEGINFAN_OP=1,
      FANVERTEX_OP=2,
      TEXMAP_OP=3,
      TRIGGER_OP=4
      };

   struct TERRAIN_STRUCT
      {
      minitile *tile;
      float scale,lambda;

      int cache_num;

      unsigned char *cache1_op,*cache2_op;
      float *cache1_arg,*cache2_arg;

      int cache_size1,cache_size2;
      int cache_maxsize;

      int cache_phase;

      int last_beginfan;

      int fancnt1,fancnt2;
      int vtxcnt1,vtxcnt2;

      float *prism_cache1,*prism_cache2;

      int prism_size1,prism_size2;
      int prism_maxsize;

      miniray *ray;

      int first_fancnt;
      int first_beginfan;
      float first_scale;
      int first_col;
      int first_row;
      int first_size;

      int render_phase;
      int render_count;
      };

   int CACHE_ID;
   int RENDER_ID;

   typedef TERRAIN_STRUCT TERRAIN_TYPE;

   static minicache *CACHE;

   TERRAIN_TYPE *TERRAIN;
   int NUMTERRAIN,MAXTERRAIN;

   static void cache_beginfan();
   static void cache_fanvertex(float i,float y,float j);
   static void cache_texmap(int m,int n,int S);
   static void cache_prismedge(float x,float y,float yf,float z);
   static void cache_trigger(int phase,float scale,float ex,float ey,float ez);
   static void cache_sync(int id);

   void cache(int op,float a=0.0f,float b=0.0f,float c=0.0f);
   void cacheprismedge(float x,float y,float yf,float z);
   void cachetrigger(int phase,float scale,float ex,float ey,float ez);
   void cachesync(int id);

   inline int rendercache(int id,int phase);

   inline void rendertexmap(int m,int n,int S);

   inline int rendertrigger(int phase);
   inline int rendertrigger(int phase,float scale);
   inline int rendertrigger();

   float OPACITY;
   float SEA_R,SEA_G,SEA_B,SEA_A;

   float PRISM_R,PRISM_G,PRISM_B,PRISM_A;

   char *VTXPROG;
   int VTXDIRTY;

   int VTXPROGID;
   int USEVTXSHADER;

   float VTXSHADERPAR1[8];
   float VTXSHADERPAR2[8];
   float VTXSHADERPAR3[8];
   float VTXSHADERPAR4[8];

   char *FRAGPROG;
   int FRAGDIRTY;

   int FRAGPROGID;
   int USEPIXSHADER;

   float PIXSHADERPAR1[8];
   float PIXSHADERPAR2[8];
   float PIXSHADERPAR3[8];
   float PIXSHADERPAR4[8];

   int PIXSHADERTEXID;
   int PIXSHADERTEXWIDTH;
   int PIXSHADERTEXHEIGHT;

   char *SEAPROG;
   int SEADIRTY;

   int SEAPROGID;
   int USESEASHADER;

   float SEASHADERPAR1[8];
   float SEASHADERPAR2[8];
   float SEASHADERPAR3[8];
   float SEASHADERPAR4[8];

   int SEASHADERTEXID;
   int SEASHADERTEXWIDTH;
   int SEASHADERTEXHEIGHT;

   int PRISMCACHE_VTXPROGID;

   void (*PRISMEDGE_CALLBACK)(float x,float y,float yf,float z,void *data);
   void (*PRISMCACHE_CALLBACK)(int phase,float scale,float ex,float ey,float ez,void *data);
   int (*PRISMRENDER_CALLBACK)(float *cache,int cnt,float lambda,miniwarp *warp,void *data);
   int (*PRISMTRIGGER_CALLBACK)(int phase,void *data);
   void (*PRISMSYNC_CALLBACK)(int id,void *data);
   void *CALLBACK_DATA;

   void (*PRESEA_CB)(void *data);
   void (*POSTSEA_CB)(void *data);
   void *CB_DATA;

   private:

   float CONFIGURE_OVERLAP;
   int CONFIGURE_MINSIZE;
   int CONFIGURE_SEATWOSIDED;
   int CONFIGURE_SEAENABLETEX;
   float CONFIGURE_ZSCALE_SEA;
   float CONFIGURE_ZSCALE_PRISMS;
   float CONFIGURE_ENABLERAY;

   void initterrain(TERRAIN_TYPE *terrain);
   void freeterrain(TERRAIN_TYPE *terrain);

   int renderprisms(float *cache,int cnt,float lambda,miniwarp *warp,
                    float pr=1.0f,float pg=1.0f,float pb=1.0f,float pa=0.9f);

   void enablevtxshader();
   void setvtxshadertexprm(float s1,float s2,float o1,float o2,float scale);
   void disablevtxshader();

   void enablepixshader();
   void disablepixshader();

   void enableseashader();
   void disableseashader();

   int getfancnt(int id);
   int getvtxcnt(int id);

   // OpenGL extensions:

   int GLSETUP;

   int GLEXT_MT;
   int GLEXT_VP;
   int GLEXT_FP;

   void initglexts();

   // Windows OpenGL extensions:

   int WGLSETUP;

#ifndef NOOGL

#ifdef _WIN32

#ifdef GL_ARB_multitexture
   PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
   PFNGLBINDPROGRAMARBPROC glBindProgramARB;
   PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
   PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB;
   PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
#endif

#endif

#endif

   void initwglprocs();
   };

#endif
