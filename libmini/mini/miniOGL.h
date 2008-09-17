// (c) by Stefan Roettger

#ifndef MINIOGL_H
#define MINIOGL_H

// OpenGL dependent functions:

namespace miniOGL {

void initstate();
void exitstate();

void clearwindow(float r,float g,float b,float a=1.0f);

void disableculling();
void enableBFculling();
void enableFFculling();

void disableAtest();
void enableAtest(float alpha=0.0f);

void disableZtest();
void enableZtest();

void disableZwriting();
void enableZwriting();

void enableAwriting();
void enableRGBwriting();
void enableRGBAwriting();
void disableRGBAwriting();

void enableblending();
void enableADDblending();
void enableATTblending();
void enableEMIblending();
void enablePRJblending();
void enableMIPblending();
void disableblending();

void enablefog(float fogstart,float fogend,float r,float g,float b,float a=1.0f);
void disablefog();

int buildLtexmap(unsigned char *image,int *width,int *height,int mipmaps=1,int mipmapped=0);
int buildLAtexmap(unsigned char *image,int *width,int *height,int mipmaps=1,int mipmapped=0);
int buildRGBtexmap(unsigned char *image,int *width,int *height,int mipmaps=1,int s3tc=0,int bytes=0,int mipmapped=0);
int buildRGBAtexmap(unsigned char *image,int *width,int *height,int mipmaps=1,int s3tc=0,int bytes=0,int mipmapped=0);
int buildRGBAtexmap(unsigned char *imageRGB,unsigned char *imageA,int *width,int *height);
int buildtexmap(unsigned char *image,int *width,int *height,int components,int depth,int mipmaps=1,int s3tc=0,int bytes=0,int mipmapped=0);
void bindtexmap(int texid,int width=0,int height=0,int size=0,int mipmaps=1);
int build3Dtexmap(unsigned char *volume,int *width,int *height,int *depth,int components);
void bind3Dtexmap(int texid);
void deletetexmap(int texid);

int compressRGBtexmap(unsigned char *image,int width,int height,
                      unsigned char **data,int *bytes);

void texunit(int unit);
void texclientunit(int unit);

int buildvtxprog(const char *prog);
void bindvtxprog(int progid);
void setvtxprogpar(int n,float p1,float p2,float p3,float p4);
void deletevtxprog(int progid);

int buildfrgprog(const char *prog);
void bindfrgprog(int progid);
void setfrgprogpar(int n,float p1,float p2,float p3,float p4);
void deletefrgprog(int progid);

inline void mtxmodel();
inline void mtxproj();
inline void mtxtex();

inline void mtxpush();
inline void mtxpop();

inline void mtxid();
inline void mtxscale(const float sx,const float sy,const float sz);
inline void mtxtranslate(const float tx,const float ty,const float tz);
inline void mtxrotate(const float angle,const float ax,const float ay,const float az);

inline void mtxmult(const float mtx[16]);
inline void mtxmult(const double mtx[16]);

void mtxgetmodel(float mtx[16]);
void mtxgetmodel(double mtx[16]);

inline void beginfans();
inline void beginfan();
inline void color(const float r,const float g,const float b,const float a=1.0f);
inline void normal(const float dx,const float dy,const float dz);
inline void texcoord(const float s,const float t,const float r=0.0f);
inline void fanvertex(const float x,const float y,const float z);
inline void endfans();

int getfancnt();
int getvtxcnt();

void linewidth(const int w);
void enablelinesmooth();
void disablelinesmooth();

inline void drawline(const float x1,const float y1,const float z1,const float x2,const float y2,const float z2);

void vertexarray(float *array,int comps=3);
void colorarray(float *array,int comps=3);
void normalarray(float *array);
void texcoordarray(float *array,int comps=2);

void rendertriangles(int start,int size);
void renderstrip(int start,int size);
void renderfan(int start,int size);

int getmaxtexsize();
int getmax3Dtexsize();

void getviewport(int *x,int *y,int *width,int *height);

unsigned char *readRGBpixels(int x,int y,int width,int height);
void writeRGBpixels(unsigned char *pixels,int width,int height,int winwidth,int winheight,int x,int y);

float *readZpixels(int x,int y,int width,int height);
void writeZpixels(float *pixels,int width,int height,int winwidth,int winheight,int x,int y);

int copytexrect(int depthcomp=0);
void bindtexrect(int texid);
void painttexrect(int texid,int depthcomp=0);
void deletetexrect(int texid);

// configuring
void configure_generatemm(int generatemm=0); // enable hw-accelerated mipmap generation
void configure_compression(int compression=1); // enable texture compression (may result in texture seams)
int configure_depth(int depth=24); // bit depth of texture maps (may result in quantization artifacts)
void configure_gamma(float gamma=1.0f); // gamma correction of texture maps
void configure_level(float level=0.0f); // level adjustment of texture maps

// graphics info
int get_unsupported_glexts(); // get number of unsupported OpenGL extensions
void print_unsupported_glexts(); // print unsupported OpenGL extensions
void print_graphics_info(); // print useful OpenGL information

}

// OpenGL includes:

#ifndef NOOGL

#ifdef _WIN32
#   ifndef APIENTRY
#      define NOMINMAX
#      include <windows.h>
#   endif
#endif

#ifndef __APPLE__
#   ifndef _WIN32
#      define GL_GLEXT_PROTOTYPES
#   endif
#   include <GL/gl.h>
#   include <GL/glu.h>
#   ifndef __sgi
#      include "GL/glext.h"
#   endif
#else
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <OpenGL/glext.h>
#endif

// OpenGL 1.0 workaround:

#ifndef GL_VERSION_1_1
#   ifndef glGenTextures
#      define glGenTextures glGenTexturesEXT
#   endif
#   ifndef glBindTexture
#      define glBindTexture glBindTextureEXT
#   endif
#   ifndef glDeleteTextures
#      define glDeleteTextures glDeleteTexturesEXT
#   endif
#   ifndef glVertexPointer
#      define glVertexPointer glVertexPointerEXT
#   endif
#   ifndef glEnableClientState
#      define glEnableClientState glEnableClientStateEXT
#   endif
#   ifndef glDisableClientState
#      define glDisableClientState glDisableClientStateEXT
#   endif
#   ifndef glInterleavedArrays
#      define glInterleavedArrays glInterleavedArraysEXT
#   endif
#   ifndef glDrawArrays
#      define glDrawArrays glDrawArraysEXT
#   endif
#endif

// OpenGL 1.1 workaround:

#ifndef GL_VERSION_1_2
#   ifndef glBlendEquation
#      define glBlendEquation glBlendEquationEXT
#   endif
#   ifndef GL_TEXTURE_3D
#      define GL_TEXTURE_3D GL_TEXTURE_3D_EXT
#   endif
#   ifndef GL_MAX_3D_TEXTURE_SIZE
#      define GL_MAX_3D_TEXTURE_SIZE GL_MAX_3D_TEXTURE_SIZE_EXT
#   endif
#   ifndef GL_TEXTURE_WRAP_R
#      define GL_TEXTURE_WRAP_R GL_TEXTURE_WRAP_R_EXT
#   endif
#   ifndef glTexImage3D
#      define glTexImage3D glTexImage3DEXT
#   endif
#else
#   ifndef GL_EXT_texture3D
#      define GL_EXT_texture3D
#   endif
#endif

#endif

// OpenGL dependent inline functions:

namespace miniOGL {

#ifndef NOOGL
extern int fancnt,vtxcnt;
#endif

void mtxmodel()
   {
#ifndef NOOGL
   glMatrixMode(GL_MODELVIEW);
#endif
   }

void mtxproj()
   {
#ifndef NOOGL
   glMatrixMode(GL_PROJECTION);
#endif
   }

void mtxtex()
   {
#ifndef NOOGL
   glMatrixMode(GL_TEXTURE);
#endif
   }

void mtxpush()
   {
#ifndef NOOGL
   glPushMatrix();
#endif
   }

void mtxpop()
   {
#ifndef NOOGL
   glPopMatrix();
#endif
   }

void mtxid()
   {
#ifndef NOOGL
   glLoadIdentity();
#endif
   }

void mtxscale(const float sx,const float sy,const float sz)
   {
#ifndef NOOGL
   glScalef(sx,sy,sz);
#endif
   }

void mtxtranslate(const float tx,const float ty,const float tz)
   {
#ifndef NOOGL
   glTranslatef(tx,ty,tz);
#endif
   }

void mtxrotate(const float angle,const float ax,const float ay,const float az)
   {
#ifndef NOOGL
   glRotatef(angle,ax,ay,az);
#endif
   }

void mtxmult(const float mtx[16])
   {
#ifndef NOOGL
   glMultMatrixf(mtx);
#endif
   }

void mtxmult(const double mtx[16])
   {
#ifndef NOOGL
   glMultMatrixd(mtx);
#endif
   }

void beginfans()
   {
#ifndef NOOGL
   fancnt=vtxcnt=0;
#endif
   }

inline void beginfan()
   {
#ifndef NOOGL
   if (fancnt++>0) glEnd();
   glBegin(GL_TRIANGLE_FAN);
#endif
   }

inline void color(const float r,const float g,const float b,const float a)
   {
#ifndef NOOGL
   glColor4f(r,g,b,a);
#endif
   }

inline void normal(const float dx,const float dy,const float dz)
   {
#ifndef NOOGL
   glNormal3f(dx,dy,dz);
#endif
   }

inline void texcoord(const float s,const float t,const float r)
   {
#ifndef NOOGL
   glTexCoord3f(s,t,r);
#endif
   }

inline void fanvertex(const float x,const float y,const float z)
   {
#ifndef NOOGL
   glVertex3f(x,y,z);
   vtxcnt++;
#endif
   }

void endfans()
   {
#ifndef NOOGL
   if (fancnt>0) glEnd();
#endif
   }

inline void drawline(const float x1,const float y1,const float z1,const float x2,const float y2,const float z2)
   {
#ifndef NOOGL
   glBegin(GL_LINES);
   glVertex3f(x1,y1,z1);
   glVertex3f(x2,y2,z2);
   glEnd();
#endif
   }

}

using namespace miniOGL;

#endif
