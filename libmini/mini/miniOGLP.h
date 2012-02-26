// (c) by Stefan Roettger

#include "minibase.h"

#include "miniOGL.h"

namespace miniOGL {

int CONFIGURE_GENERATEMM=0;
int CONFIGURE_COMPRESSION=1;
int CONFIGURE_DEPTH=24;
float CONFIGURE_GAMMA=1.0f;
float CONFIGURE_LEVEL=0.0f;

#ifndef NOOGL

GLboolean depth_test,cull_face,color_writemask[4],depth_writemask,blending;
GLint depth_func,front_face,cull_face_mode,blend_src,blend_dst;

BOOLINT texgen,maxblendeq;

BOOLINT glrndr_sgi;
BOOLINT glrndr_nvidia;
BOOLINT glrndr_ati;
BOOLINT glrndr_intel;

BOOLINT glext_mm;
BOOLINT glext_tec;
BOOLINT glext_tfa;
BOOLINT glext_t3D;
BOOLINT glext_tc;
BOOLINT glext_ts3;
BOOLINT glext_tgm;
BOOLINT glext_np2;
BOOLINT glext_dt;
BOOLINT glext_tr;
BOOLINT glext_mt;
BOOLINT glext_vp;
BOOLINT glext_fp;

#ifdef _WIN32

#ifdef GL_EXT_blend_minmax
PFNGLBLENDEQUATIONEXTPROC glBlendEquation=NULL;
#endif

#ifdef GL_EXT_texture3D
PFNGLTEXIMAGE3DEXTPROC glTexImage3D=NULL;
#endif

#ifdef GL_ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB=NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB=NULL;
#endif

#ifdef GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB=NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB=NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB=NULL;
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
PFNGLGENPROGRAMSARBPROC glGenProgramsARB=NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB=NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB=NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB=NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB=NULL;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB=NULL;
#endif

#endif

int fancnt,vtxcnt;

#endif

}
