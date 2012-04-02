// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISTRIP_H
#define MINISTRIP_H

//! pre-defined snippet names
#define MINI_SNIPPET_VTX_BEGIN "snippet_vtx_begin"
#define MINI_SNIPPET_VTX_HEADER "snippet_vtx_header"
#define MINI_SNIPPET_VTX_BASIC "snippet_vtx_basic"
#define MINI_SNIPPET_VTX_VIEWPOS "snippet_vtx_viewpos"
#define MINI_SNIPPET_VTX_NORMAL "snippet_vtx_normal"
#define MINI_SNIPPET_VTX_NORMAL_DIRECT "snippet_vtx_normal_direct"
#define MINI_SNIPPET_VTX_TEX "snippet_vtx_tex"
#define MINI_SNIPPET_VTX_TEXGEN "snippet_vtx_texgen"
#define MINI_SNIPPET_VTX_FOG "snippet_vtx_fog"
#define MINI_SNIPPET_VTX_FOOTER "snippet_vtx_footer"
#define MINI_SNIPPET_VTX_END "snippet_vtx_end"
#define MINI_SNIPPET_FRG_BEGIN "snippet_frg_begin"
#define MINI_SNIPPET_FRG_HEADER "snippet_frg_header"
#define MINI_SNIPPET_FRG_BASIC "snippet_frg_basic"
#define MINI_SNIPPET_FRG_TEX "snippet_frg_tex"
#define MINI_SNIPPET_FRG_TEX3 "snippet_frg_tex3"
#define MINI_SNIPPET_FRG_TEX_MASK "snippet_frg_tex_mask"
#define MINI_SNIPPET_FRG_TEX2_DIRECT "snippet_frg_tex2_direct"
#define MINI_SNIPPET_FRG_SHADE "snippet_frg_shade"
#define MINI_SNIPPET_FRG_SHADE_ALPHA "snippet_frg_shade_alpha"
#define MINI_SNIPPET_FRG_SHADE_ALPHA2 "snippet_frg_shade_alpha2"
#define MINI_SNIPPET_FRG_SHADE_DIRECT "snippet_frg_shade_direct"
#define MINI_SNIPPET_FRG_FOG "snippet_frg_fog"
#define MINI_SNIPPET_FRG_FOOTER "snippet_frg_footer"
#define MINI_SNIPPET_FRG_END "snippet_frg_end"

#include "miniOGL.h"
#include "miniv3f.h"
#include "miniv3d.h"
#include "miniv4d.h"

//! container class for triangle strips
class ministrip
   {
   public:

   //! maximum number of shader resources
   static const int SHADERMAX=100;
   static const int SNIPPETMAX=1000;
   static const int SHADERVTXPRMMAX=8;
   static const int SHADERFRGPRMMAX=8;
   static const int SHADERFRGTEXMAX=6;

   //! default constructor
   ministrip(int colcomps=0,int nrmcomps=0,int texcomps=0);

   //! copy constructor
   ministrip(const ministrip &strip);

   //! destructor
   ~ministrip();

   //! clear strip
   void clear();

   //! begin next triangle strip
   void beginstrip();

   //! set color of next vertex
   void setcol(const float r,const float g,const float b,float a=1.0f);
   void setcol(const miniv3d &rgb);
   void setcol(const miniv4d &rgba);

   //! set normal of next vertex
   void setnrm(const float nx,const float ny,const float nz);
   void setnrm(const miniv3d &n);

   //! set normal from three points
   void setnrm(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3);

   //! set tex coords of next vertex
   void settex(const float tx,const float ty=0.0f,const float tz=0.0f,float tw=1.0f);
   void settex(const miniv3d &t);
   void settex(const miniv4d &t);

   //! add one vertex
   void addvtx(const float x,const float y,const float z);
   void addvtx(const miniv3d &v);

   //! check color array
   BOOLINT hascolor() const {return(COLCOMPS>0);}

   //! check normal array
   BOOLINT hasnormal() const {return(NRMCOMPS>0);}

   //! check tex coord array
   BOOLINT hastex() const {return(TEXCOMPS>0);}

   //! set scale
   void setscale(float scale);

   //! set matrix
   void setmatrix(double mtx[16]);

   //! set texture matrix
   void settexmatrix(double texmtx[16]);

   //! set Z-scaling
   void setZscale(float zscale);

   //! define shader snippet
   static void addsnippet(const char *snippetname,const char *snippet);

   //! get free shader slot
   static int getfreeslot();

   //! define vertex shader
   static void setvtxshader(int num,char *vtxprog);
   static void concatvtxshader(int num,const char *snippetname);
   static void setvtxshaderparams(int num,float p1=0.0f,float p2=0.0f,float p3=0.0f,float p4=0.0f,int n=0);

   //! define pixel shader
   static void setpixshader(int num,char *frgprog);
   static void concatpixshader(int num,const char *snippetname);
   static void setpixshaderparams(int num,float p1=0.0f,float p2=0.0f,float p3=0.0f,float p4=0.0f,int n=0);
   static void setpixshadertex(int num,unsigned char *image,int width,int height,int components=3,int clamp=1,int mipmaps=1,int n=0);
   static void setpixshadertexRGB(int num,unsigned char *image,int width,int height,int clamp=1,int mipmaps=1,int n=0);
   static void setpixshadertexRGBA(int num,unsigned char *image,int width,int height,int clamp=1,int mipmaps=1,int n=0);
   static void setpixshadertexid(int num,int id,int width,int height,int clamp=1,int mipmaps=1,int n=0);
   static void setpixshadertexbuf(int num,databuf *buf,int clamp=1,int mipmaps=1,int n=0);

   //! set direct shading parameters
   static void setshadedirectparams(int num,
                                    float lightdir[3],
                                    float lightbias=0.5f,float lightoffset=0.5f);

   //! set global direct shading parameters
   static void setglobalshadedirectparams(float lightdir[3],
                                          float lightbias=0.5f,float lightoffset=0.5f);

   //! set direct texturing parameters
   static void settexturedirectparams(int num,
                                      float lightdir[3],
                                      float transbias=4.0f,float transoffset=0.0f);

   //! set global direct texturing parameters
   static void setglobaltexturedirectparams(float lightdir[3],
                                            float transbias=4.0f,float transoffset=0.0f);

   //! set fog parameters
   static void setfogparams(int num,
                            float fogstart,float fogend,
                            float fogdensity,
                            float fogcolor[3]);

   //! set global fog parameters
   static void setglobalfogparams(float fogstart,float fogend,
                                  float fogdensity,
                                  float fogcolor[3]);

   //! get default shader
   static int getdefaultshader() {return(0);}

   //! set global shader
   static void useglobalshader(int num=0);

   //! enable/disable global texture coordinate generation
   static void setglobal_texgen(BOOLINT on)
      {global_texgen=on; enableglobalshader();}

   //! get global texture coordinate generation state
   static BOOLINT getglobal_texgen() {return(global_texgen);}

   //! enable/disable global shading
   static void setglobal_shade(BOOLINT on)
      {global_shade=on; if (on) global_shade_direct=FALSE; enableglobalshader();}

   //! get global shading state
   static BOOLINT getglobal_shade() {return(global_shade);}

   //! enable/disable global direct shading
   static void setglobal_shade_direct(BOOLINT on)
      {global_shade_direct=on; if (on) global_shade=FALSE; enableglobalshader();}

   //! get global direct shading state
   static BOOLINT getglobal_shade_direct() {return(global_shade_direct);}

   //! enable/disable global texturing (2D)
   static void setglobal_tex(BOOLINT on)
      {global_tex=on; if (on) global_tex3=FALSE; enableglobalshader();}

   //! get global texturing state (2D)
   static BOOLINT getglobal_tex() {return(global_tex);}

   //! enable/disable global texturing (3D)
   static void setglobal_tex3(BOOLINT on)
      {global_tex3=on; if (on) global_tex=FALSE; enableglobalshader();}

   //! get global texturing state (3D)
   static BOOLINT getglobal_tex3() {return(global_tex3);}

   //! enable/disable global fogging
   static void setglobal_fog(BOOLINT on)
      {global_fog=on; enableglobalshader();}

   //! get global fogging
   static BOOLINT getglobal_fog() {return(global_fog);}

   //! get global shader
   static int getglobalshader();

   //! set actual shader
   void useshader(int num=0);

   //! get actual shader
   int getshader();

   //! check whether or not the geometry is fully opaque
   BOOLINT isopaque();

   //! render triangle strips
   void render(int wocolor=0,int wonrm=0,int wotex=0);

   //! render triangle strips with multi-pass method for unordered semi-transparent geometry
   void rendermultipass(int pass,
                        int passes=4,
                        float ambient=0.1f,
                        float bordercontrol=1.0f,float centercontrol=1.0f,float colorcontrol=1.0f,
                        float bordercontrol2=1.0f,float centercontrol2=1.0f,float colorcontrol2=1.0f,
                        float stripewidth=1.0f,float stripeoffset=0.0f,
                        float stripedx=0.0f,float stripedy=0.0f,float stripedz=1.0f,
                        int correctz=0);

   //! render triangle strip with multi-pass method
   void rendermulti(int passes=4,
                    float ambient=0.1f,
                    float bordercontrol=1.0f,float centercontrol=1.0f,float colorcontrol=1.0f,
                    float bordercontrol2=1.0f,float centercontrol2=1.0f,float colorcontrol2=1.0f,
                    float stripewidth=1.0f,float stripeoffset=0.0f,
                    float stripedx=0.0f,float stripedy=0.0f,float stripedz=1.0f,
                    int correctz=0);

   //! render bbox
   void renderbbox();

   //! get array size (number of vertices)
   int getsize() {return(SIZE);}

   //! get vertex array
   float *getvtx() {return(VTXARRAY);}

   //! get reference to vertex array
   float **getvtxref() {return(&VTXARRAY);}

   //! get vertex shader
   static char *getvtxshader(int num);

   //! get pixel shader
   static char *getpixshader(int num);

   //! get bounding box
   void getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const;

   //! get bounding sphere
   void getbsphere(miniv3d &center,double &radius2) const;

   //! shoot a ray and return the distance to the closest triangle
   //! only the triangles with a positive distance are considered
   //! o is the origin of the ray, d is the ray direction
   //! a valid hit distance to be returned has to be no less than mindist
   //! a return value of MAXFLOAT indicates that there was no hit
   double shoot(const miniv3d &o,const miniv3d &d,double mindist=0.0) const;

   protected:

   int SIZE;
   int MAXSIZE;

   float *VTXARRAY; // 3 components

   float *COLARRAY; // 3 or 4 components
   int COLCOMPS;

   float *NRMARRAY; // 3 components
   int NRMCOMPS;

   float *TEXARRAY; // 1-4 components
   int TEXCOMPS;

   float VTXX,VTXY,VTXZ;
   miniv3d BBOXMIN,BBOXMAX;

   float COLR,COLG,COLB,COLA;
   float COLAMIN,COLAMAX;

   float NRMX,NRMY,NRMZ;
   float TEXX,TEXY,TEXZ,TEXW;

   int COPYVTX;

   float SCALE;

   double MTX[16];
   BOOLINT MTXSET;
   double TEXMTX[16];
   BOOLINT TEXMTXSET;

   float ZSCALE;

   void addvtx();

   void enablevtxshader(int num);
   void disablevtxshader(int num);

   void enablepixshader(int num);
   void disablepixshader(int num);

   static void initsnippets();
   static void freesnippets();

   static void initshader();

   private:

   void init(int colcomps,int nrmcomps,int texcomps,int maxsize=1);

   struct SHADER_STRUCT
      {
      int occupied;

      char *vtxprog;

      int vtxprogid;
      int vtxdirty;

      float vtxshaderpar1[SHADERVTXPRMMAX];
      float vtxshaderpar2[SHADERVTXPRMMAX];
      float vtxshaderpar3[SHADERVTXPRMMAX];
      float vtxshaderpar4[SHADERVTXPRMMAX];

      char *frgprog;

      int frgprogid;
      int frgdirty;

      float pixshaderpar1[SHADERFRGPRMMAX];
      float pixshaderpar2[SHADERFRGPRMMAX];
      float pixshaderpar3[SHADERFRGPRMMAX];
      float pixshaderpar4[SHADERFRGPRMMAX];

      int pixshadertexid[SHADERFRGTEXMAX];
      int pixshadertexw[SHADERFRGTEXMAX];
      int pixshadertexh[SHADERFRGTEXMAX];
      int pixshadertexcl[SHADERFRGTEXMAX];
      int pixshadertexmm[SHADERFRGTEXMAX];
      int pixshadertexnofree[SHADERFRGTEXMAX];
      };

   typedef SHADER_STRUCT SHADER_TYPE;

   struct SNIPPET_STRUCT
      {
      char *snippetname;
      char *snippet;
      };

   typedef SNIPPET_STRUCT SNIPPET_TYPE;

   static int INSTANCES;

   static SHADER_TYPE SHADER[SHADERMAX];

   static SNIPPET_TYPE SNIPPET[SNIPPETMAX];
   static int SNIPPETS;

   static int USEGLOBALSHADER;
   int USESHADER;

   // global shader settings:

   static int global_shader[64];

   static BOOLINT global_texgen;
   static BOOLINT global_shade,global_shade_direct;
   static BOOLINT global_tex,global_tex3;
   static BOOLINT global_fog;

   static float global_fogstart,global_fogend;
   static float global_fogdensity,global_fogcolor[3];

   static float global_lightdir[3];
   static float global_lightbias,global_lightoffset;
   static float global_transbias,global_transoffset;

   static int createshader(BOOLINT texgen,
                           BOOLINT shade,BOOLINT shade_direct,
                           BOOLINT tex,BOOLINT tex3,
                           BOOLINT fog);

   static void enableglobalshader(BOOLINT texgen,
                                  BOOLINT shade,BOOLINT shade_direct,
                                  BOOLINT tex,BOOLINT tex3,
                                  BOOLINT fog);

   static void enableglobalshader();
   };

#endif
