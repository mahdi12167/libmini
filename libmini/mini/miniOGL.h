// (c) by Stefan Roettger

#ifndef MINIOGL_H
#define MINIOGL_H

#include "miniv3d.h"
#include "miniv4d.h"

#include "database.h"

// OpenGL dependent functions:

namespace miniOGL {

void checkOGLerror();

void initstate();
void exitstate();

void clearbuffer(float r,float g,float b,float a=1.0f);
void clearbuffer(const miniv3d &c);
void clearbuffer(const miniv4d &c);
void cleardepthbuffer();

void writeleftbuffer();
void writerightbuffer();
void writebackbuffer();

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
void enableRwriting();
void enableGBwriting();
void enableRGBwriting();
void enableRGBAwriting();
void disableRGBAwriting();

void enableblending();
void enableADDblending();
void enableATTblending();
void enableEMIblending();
void enableRKEblending();
void enablePRJblending();
void enableMIPblending();
void disableblending();

void enablestenciling(int pass=0);
void disablestenciling();

void enablefog(float fogstart,float fogend,float r,float g,float b,float a=1.0f);
void enablefog(float fogstart,float fogend,const miniv3d &c);
void enablefog(float fogstart,float fogend,const miniv4d &c);
void disablefog();

void getfog(float *start,float *end);

void enableplane(int n,float a,float b,float c,float d);
void disableplane(int n);

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

int db2texid(databuf *buf,int *width,int *height,int *mipmaps);

void enabletexgen();
void disabletexgen();

void texunit(int unit=0);
void texclientunit(int unit=0);

int buildvtxprog(const char *prog);
void bindvtxprog(int progid);
void setvtxprogpar(int n,float p1,float p2,float p3,float p4);
void setvtxprogpars(int n,int count,const float *params);
void deletevtxprog(int progid);

int buildfrgprog(const char *prog);
void bindfrgprog(int progid);
void setfrgprogpar(int n,float p1,float p2,float p3,float p4);
void setfrgprogpars(int n,int count,const float *params);
void deletefrgprog(int progid);

inline void mtxmodel();
inline void mtxproj();
inline void mtxtex();

inline void mtxpush();
inline void mtxpop();

inline void mtxid();
inline void mtxscale(const float sx,const float sy,const float sz);
inline void mtxscale(const miniv3d &s);
inline void mtxtranslate(const float tx,const float ty,const float tz);
inline void mtxtranslate(const miniv3d &t);
inline void mtxrotate(const float angle,const float ax,const float ay,const float az);
inline void mtxrotate(const float angle,const miniv3d &a);

inline void mtxmult(const float oglmtx[16]);
inline void mtxmult(const double oglmtx[16]);

inline void mtxperspective(float fovy,float aspect,float nearp,float farp);
inline void mtxlookat(float ex,float ey,float ez,float ax,float ay,float az,float ux,float uy,float uz);
inline void mtxlookat(const miniv3d &eye,const miniv3d &at,const miniv3d &up);

void mtxgetmodel(float oglmtx[16]);
void mtxgetmodel(double oglmtx[16]);

void mtxget(const miniv3d mtx[3],double oglmtx[16]);
void mtxget(const miniv4d mtx[3],double oglmtx[16]);
void mtxget(const double oglmtx[16],miniv3d mtx[3]);
void mtxget(const double oglmtx[16],miniv4d mtx[3]);
void mtxget(const double oglmtx[16],miniv3d &vec);
void mtxget(const double oglmtx[16],miniv4d &vec);

inline void beginfans();
inline void beginfan();
inline void color(const float r,const float g,const float b,const float a=1.0f);
inline void color(const miniv3d &c);
inline void color(const miniv4d &c);
inline void normal(const float dx,const float dy,const float dz);
inline void normal(const miniv3d &n);
inline void texcoord(const float s,const float t,const float r=0.0f);
inline void texcoord(const miniv3d &t);
inline void fanvertex(const float x,const float y,const float z);
inline void fanvertex(const miniv3d &v);
inline void endfans();

void multitexcoord(const unsigned int unit,const float s,const float t,const float r=0.0f);
void multitexcoord(const unsigned int unit,const miniv3d &t);

int getfancnt();
int getvtxcnt();

void linewidth(const float w=1.0f);
void enablelinesmooth();
void disablelinesmooth();

inline void renderline(const float x1,const float y1,const float z1,const float x2,const float y2,const float z2);
inline void renderline(const miniv3d &v1,const miniv3d &v2);

void pointsize(const float s=1.0f);
void enablepointsmooth();
void disablepointsmooth();

inline void renderpoint(const float x,const float y,const float z);
inline void renderpoint(const miniv3d &v);

void vertexarray(float *array,int comps=3);
void colorarray(float *array,int comps=3);
void normalarray(float *array);
void texcoordarray(float *array,int comps=2);
void interleavedNVarray(float *array);

void rendertriangles(int start,int size);
void renderquads(int start,int size);
void renderlines(int start,int size);
void renderstrip(int start,int size);
void renderfan(int start,int size);

int getmaxtexsize();
int getmax3Dtexsize();

int getmaxtexunits();
int getmaxtexcoords();
int getmaxteximageunits();

void getviewport(int *x,int *y,int *width,int *height);

unsigned char *readRGBpixels(int x,int y,int width,int height);
void writeRGBpixels(unsigned char *pixels,int width,int height,int winwidth,int winheight,int x,int y);

float *readZpixels(int x,int y,int width,int height);
void writeZpixels(float *pixels,int width,int height,int winwidth,int winheight,int x,int y);

int copytexrect(int depthcomp=0,int nofrills=0);
void bindtexrect(int texid,int nofrills=0);
void painttexrect(int texid,int depthcomp=0);
void deletetexrect(int texid);

void paintbuffer();

void polygonmode(int wire=0);

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
#      ifndef NOMINMAX
#         define NOMINMAX
#      endif
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
       extern "C"
          {
#      ifndef _WIN32
#         include <GL/glext.h>
#      else
#         include "GL/glext.h"
#      endif
          }
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

void mtxscale(const miniv3d &s)
   {
#ifndef NOOGL
   glScaled(s.x,s.y,s.z);
#endif
   }

void mtxtranslate(const float tx,const float ty,const float tz)
   {
#ifndef NOOGL
   glTranslatef(tx,ty,tz);
#endif
   }

void mtxtranslate(const miniv3d &t)
   {
#ifndef NOOGL
   glTranslated(t.x,t.y,t.z);
#endif
   }

void mtxrotate(const float angle,const float ax,const float ay,const float az)
   {
#ifndef NOOGL
   glRotatef(angle,ax,ay,az);
#endif
   }

void mtxrotate(const float angle,const miniv3d &a)
   {
#ifndef NOOGL
   glRotated(angle,a.x,a.y,a.z);
#endif
   }

void mtxmult(const float oglmtx[16])
   {
#ifndef NOOGL
   glMultMatrixf(oglmtx);
#endif
   }

void mtxmult(const double oglmtx[16])
   {
#ifndef NOOGL
   glMultMatrixd(oglmtx);
#endif
   }

void mtxperspective(float fovy,float aspect,float nearp,float farp)
   {
#ifndef NOOGL
   gluPerspective(fovy, aspect, nearp, farp);
#endif
   }

void mtxlookat(float ex,float ey,float ez,float ax,float ay,float az,float ux,float uy,float uz)
   {
#ifndef NOOGL
   gluLookAt(ex,ey,ez,
             ax,ay,az,
             ux,uy,uz);
#endif
   }

void mtxlookat(const miniv3d &eye,const miniv3d &at,const miniv3d &up)
   {
#ifndef NOOGL
   gluLookAt(eye.x,eye.y,eye.z,
             at.x,at.y,at.z,
             up.x,up.y,up.z);
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

inline void color(const miniv3d &c)
   {
#ifndef NOOGL
   glColor3d(c.x,c.y,c.z);
#endif
   }

inline void color(const miniv4d &c)
   {
#ifndef NOOGL
   glColor4d(c.x,c.y,c.z,c.w);
#endif
   }

inline void normal(const float dx,const float dy,const float dz)
   {
#ifndef NOOGL
   glNormal3f(dx,dy,dz);
#endif
   }

inline void normal(const miniv3d &n)
   {
#ifndef NOOGL
   glNormal3d(n.x,n.y,n.z);
#endif
   }

inline void texcoord(const float s,const float t,const float r)
   {
#ifndef NOOGL
   glTexCoord3f(s,t,r);
#endif
   }

inline void texcoord(const miniv3d &t)
   {
#ifndef NOOGL
   glTexCoord3d(t.x,t.y,t.z);
#endif
   }

inline void fanvertex(const float x,const float y,const float z)
   {
#ifndef NOOGL
   glVertex3f(x,y,z);
   vtxcnt++;
#endif
   }

inline void fanvertex(const miniv3d &v)
   {
#ifndef NOOGL
   glVertex3d(v.x,v.y,v.z);
   vtxcnt++;
#endif
   }

void endfans()
   {
#ifndef NOOGL
   if (fancnt>0) glEnd();
#endif
   }

inline void renderline(const float x1,const float y1,const float z1,const float x2,const float y2,const float z2)
   {
#ifndef NOOGL
   glBegin(GL_LINES);
   glVertex3f(x1,y1,z1);
   glVertex3f(x2,y2,z2);
   glEnd();
#endif
   }

inline void renderline(const miniv3d &v1,const miniv3d &v2)
   {
#ifndef NOOGL
   glBegin(GL_LINES);
   glVertex3d(v1.x,v1.y,v1.z);
   glVertex3d(v2.x,v2.y,v2.z);
   glEnd();
#endif
   }

inline void renderpoint(const float x,const float y,const float z)
   {
#ifndef NOOGL
   glBegin(GL_POINTS);
   glVertex3f(x,y,z);
   glEnd();
#endif
   }

inline void renderpoint(const miniv3d &v)
   {
#ifndef NOOGL
   glBegin(GL_POINTS);
   glVertex3f(v.x,v.y,v.z);
   glEnd();
#endif
   }

}

using namespace miniOGL;

#endif
