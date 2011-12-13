// (c) by Stefan Roettger

#include "miniOGLP.h"

namespace miniOGL {

// configuring
void configure_generatemm(int generatemm) {CONFIGURE_GENERATEMM=generatemm;}
void configure_compression(int compression) {CONFIGURE_COMPRESSION=compression;}
int configure_depth(int depth) {int prev=CONFIGURE_DEPTH; CONFIGURE_DEPTH=depth; return(prev);}
void configure_gamma(float gamma) {CONFIGURE_GAMMA=gamma;}
void configure_level(float level) {CONFIGURE_LEVEL=level;}

#ifndef NOOGL

static void initglexts()
   {
   char *GL_RNDR;
   char *GL_EXTs;

   static BOOLINT done=FALSE;

   if (!done)
      {
      // figure out OpenGL renderer:

      glrndr_sgi=FALSE;
      glrndr_nvidia=FALSE;
      glrndr_ati=FALSE;
      glrndr_intel=FALSE;

      if ((GL_RNDR=(char *)glGetString(GL_VENDOR))==NULL) ERRORMSG();

      if (strstr(GL_RNDR,"SGI")!=NULL) glrndr_sgi=TRUE;
      else if (strstr(GL_RNDR,"NVIDIA")!=NULL) glrndr_nvidia=TRUE;
      else if (strstr(GL_RNDR,"ATI")!=NULL) glrndr_ati=TRUE;
      else if (strstr(GL_RNDR,"Intel")!=NULL) glrndr_intel=TRUE;

      // figure out its OpenGL extensions:

      glext_mm=FALSE;
      glext_tec=FALSE;
      glext_tfa=FALSE;
      glext_t3D=TRUE;
      glext_tc=FALSE;
      glext_ts3=FALSE;
      glext_tgm=FALSE;
      glext_np2=FALSE;
      glext_dt=FALSE;
      glext_tr=FALSE;
      glext_mt=FALSE;
      glext_vp=FALSE;
      glext_fp=FALSE;

      if ((GL_EXTs=(char *)glGetString(GL_EXTENSIONS))==NULL) ERRORMSG();

      if (strstr(GL_EXTs,"GL_EXT_blend_minmax")!=NULL) glext_mm=TRUE;
      if (strstr(GL_EXTs,"GL_SGIS_texture_edge_clamp")!=NULL) glext_tec=TRUE;
      if (strstr(GL_EXTs,"GL_EXT_texture_edge_clamp")!=NULL) glext_tec=TRUE;
      if (strstr(GL_EXTs,"GL_EXT_texture_filter_anisotropic")!=NULL) glext_tfa=TRUE;
#ifndef GL_VERSION_1_2
      if (strstr(GL_EXTs,"GL_EXT_texture3D")==NULL) glext_t3D=FALSE;
#endif
      if (strstr(GL_EXTs,"GL_ARB_texture_compression")!=NULL) glext_tc=TRUE;
      if (strstr(GL_EXTs,"GL_EXT_texture_compression_s3tc")!=NULL) glext_ts3=TRUE;
      if (strstr(GL_EXTs,"GL_SGIS_generate_mipmap")!=NULL) glext_tgm=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_texture_non_power_of_two")!=NULL) glext_np2=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_depth_texture")!=NULL) glext_dt=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_texture_rectangle")!=NULL) glext_tr=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_multitexture")!=NULL) glext_mt=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_vertex_program")!=NULL) glext_vp=TRUE;
      if (strstr(GL_EXTs,"GL_ARB_fragment_program")!=NULL) glext_fp=TRUE;

      done=TRUE;
      }
   }

#ifdef _WIN32

static void initwglprocs()
   {
   static BOOLINT done=FALSE;

   if (!done)
      {
#ifdef GL_EXT_blend_minmax
      if (glext_mm)
         if ((glBlendEquation=(PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT"))==NULL)
            {
            WARNMSG();
            glext_mm=FALSE;
            }
#endif

#ifdef GL_EXT_texture3D
      if (glext_t3D)
         if ((glTexImage3D=(PFNGLTEXIMAGE3DEXTPROC)wglGetProcAddress("glTexImage3DEXT"))==NULL)
            {
            WARNMSG();
            glext_t3D=FALSE;
            }
#endif

#ifdef GL_ARB_texture_compression
      if (glext_tc)
         if ((glCompressedTexImage2DARB=(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB"))==NULL ||
             (glGetCompressedTexImageARB=(PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)wglGetProcAddress("glGetCompressedTexImageARB"))==NULL)
            {
            WARNMSG();
            glext_tc=FALSE;
            }
#endif

#ifdef GL_ARB_multitexture
      if (glext_mt)
         if ((glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB"))==NULL ||
             (glClientActiveTextureARB=(PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB"))==NULL ||
             (glMultiTexCoord3fARB=(PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB"))==NULL)
            {
            WARNMSG();
            glext_mt=FALSE;
            }
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
      if (glext_vp && glext_fp)
         if ((glGenProgramsARB=(PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB"))==NULL ||
             (glBindProgramARB=(PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB"))==NULL ||
             (glProgramStringARB=(PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB"))==NULL ||
             (glProgramEnvParameter4fARB=(PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB"))==NULL ||
             (glDeleteProgramsARB=(PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB"))==NULL)
            {
            WARNMSG();
            glext_vp=glext_fp=FALSE;
            }
#endif

      done=TRUE;
      }
   }

#endif

#endif

int get_unsupported_glexts()
   {
   int num=0;

#ifndef NOOGL

   initglexts();

   if (!glext_mm) num++;
   if (!glext_tec) num++;
   if (!glext_tfa) num++;
   if (!glext_t3D) num++;
   if (!glext_tc) num++;
   if (!glext_ts3) num++;
   if (!glext_tgm) num++;
   if (!glext_np2) num++;
   if (!glext_dt) num++;
   if (!glext_tr) num++;
   if (!glext_mt) num++;
   if (!glext_vp) num++;
   if (!glext_fp) num++;

#endif

   return(num);
   }

void print_unsupported_glexts()
   {
#ifndef NOOGL

   initglexts();

   if (get_unsupported_glexts()>0)
      {
      printf("unsupported OpenGL extensions:");

      if (!glext_mm) printf(" EXT_blend_minmax");
      if (!glext_tec) printf(" SGIS_texture_edge_clamp");
      if (!glext_tfa) printf(" EXT_texture_filter_anisotropic");
      if (!glext_t3D) printf(" EXT_texture3D");
      if (!glext_tc) printf(" ARB_texture_compression");
      if (!glext_ts3) printf(" EXT_texture_compression_s3tc");
      if (!glext_tgm) printf(" SGIS_generate_mipmap");
      if (!glext_np2) printf(" ARB_texture_non_power_of_two");
      if (!glext_dt) printf(" ARB_depth_texture");
      if (!glext_tr) printf(" ARB_texture_rectangle");
      if (!glext_mt) printf(" ARB_multitexture");
      if (!glext_vp) printf(" ARB_vertex_program");
      if (!glext_fp) printf(" ARB_fragment_program");

      printf("\n");
      }

#endif
   }

void print_graphics_info()
   {
#ifndef NOOGL

   initglexts();

   printf("renderer: ");
   if (glrndr_sgi) printf("SGI\n");
   else if (glrndr_nvidia) printf("NVIDIA\n");
   else if (glrndr_ati) printf("ATI\n");
   else if (glrndr_intel) printf("Intel\n");
   else printf("unknown\n");

   printf("maxtexsize=%d\n",getmaxtexsize());
   printf("max3Dtexsize=%d\n",getmax3Dtexsize());
   printf("maxtexunits=%d\n",getmaxtexunits());
   printf("maxtexcoords=%d\n",getmaxtexcoords());
   printf("maxteximageunits=%d\n",getmaxteximageunits());

#endif
   }

void checkOGLerror()
   {
#ifndef NOOGL
   GLenum error;

   if ((error=glGetError()))
      if (error==GL_INVALID_ENUM) fprintf(stderr,"invalid GL enum\n");
      else if (error==GL_INVALID_VALUE) fprintf(stderr,"invalid GL value\n");
      else if (error==GL_INVALID_OPERATION) fprintf(stderr,"invalid GL operation\n");
      else WARNMSG();
#endif
   }

void initstate()
   {
#ifndef NOOGL

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

   glGetIntegerv(GL_DEPTH_FUNC,&depth_func);
   glGetBooleanv(GL_DEPTH_TEST,&depth_test);

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   glGetIntegerv(GL_FRONT_FACE,&front_face);
   glGetIntegerv(GL_CULL_FACE_MODE,&cull_face_mode);
   glGetBooleanv(GL_CULL_FACE,&cull_face);

   glFrontFace(GL_CCW);
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);

   glGetBooleanv(GL_COLOR_WRITEMASK,color_writemask);
   glGetBooleanv(GL_DEPTH_WRITEMASK,&depth_writemask);

   glGetBooleanv(GL_BLEND,&blending);
   glGetIntegerv(GL_BLEND_SRC,&blend_src);
   glGetIntegerv(GL_BLEND_DST,&blend_dst);
   glDisable(GL_BLEND);

   glColor3f(1.0f,1.0f,1.0f);

   maxblendeq=FALSE;

   fancnt=vtxcnt=0;

#endif
   }

void exitstate()
   {
#ifndef NOOGL

   if (depth_func!=GL_LEQUAL) glDepthFunc(depth_func);
   if (!depth_test) glDisable(GL_DEPTH_TEST);

   if (front_face!=GL_CCW) glFrontFace(front_face);
   if (cull_face_mode!=GL_BACK) glCullFace(cull_face_mode);
   if (!cull_face) glDisable(GL_CULL_FACE);

   glColorMask(color_writemask[0],color_writemask[1],color_writemask[2],color_writemask[3]);
   glDepthMask(depth_writemask);

   if (blending)
      {
      glBlendFunc(blend_src,blend_dst);
      glEnable(GL_BLEND);
      }

   checkOGLerror();

#endif
   }

void clearbuffer(float r,float g,float b,float a)
   {
#ifndef NOOGL
   GLboolean mask[4];
   glGetBooleanv(GL_COLOR_WRITEMASK,mask);
   glColorMask(TRUE,TRUE,TRUE,TRUE);
   glClearColor(r,g,b,a);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   glColorMask(mask[0],mask[1],mask[2],mask[3]);
#endif
   }

void clearbuffer(const miniv3d &c)
   {clearbuffer(c.x,c.y,c.z,1.0);}

void clearbuffer(const miniv4d &c)
   {clearbuffer(c.x,c.y,c.z,c.w);}

void cleardepthbuffer()
   {
#ifndef NOOGL
   glClear(GL_DEPTH_BUFFER_BIT);
#endif
   }

void writeleftbuffer()
   {
#ifndef NOOGL
   glDrawBuffer(GL_BACK_LEFT);
#endif
   }

void writerightbuffer()
   {
#ifndef NOOGL
   glDrawBuffer(GL_BACK_RIGHT);
#endif
   }

void writebackbuffer()
   {
#ifndef NOOGL
   glDrawBuffer(GL_BACK);
#endif
   }

void disableculling()
   {
#ifndef NOOGL
   glDisable(GL_CULL_FACE);
#endif
   }

void enableBFculling()
   {
#ifndef NOOGL
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);
#endif
   }

void enableFFculling()
   {
#ifndef NOOGL
   glCullFace(GL_FRONT);
   glEnable(GL_CULL_FACE);
#endif
   }

void disableAtest()
   {
#ifndef NOOGL
   glDisable(GL_ALPHA_TEST);
#endif
   }

void enableAtest(float alpha)
   {
#ifndef NOOGL
   glAlphaFunc(GL_GREATER,alpha);
   glEnable(GL_ALPHA_TEST);
#endif
   }

void disableZtest()
   {
#ifndef NOOGL
   glDisable(GL_DEPTH_TEST);
#endif
   }

void enableZtest()
   {
#ifndef NOOGL
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
#endif
   }

void disableZwriting()
   {
#ifndef NOOGL
   glDepthMask(GL_FALSE);
#endif
   }

void enableZwriting()
   {
#ifndef NOOGL
   glDepthMask(GL_TRUE);
#endif
   }

void enableAwriting()
   {
#ifndef NOOGL
   glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
#endif
   }

void enableRwriting()
   {
#ifndef NOOGL
   glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE);
#endif
   }

void enableGBwriting()
   {
#ifndef NOOGL
   glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_FALSE);
#endif
   }

void enableRGBwriting()
   {
#ifndef NOOGL
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
#endif
   }

void enableRGBAwriting()
   {
#ifndef NOOGL
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
#endif
   }

void disableRGBAwriting()
   {
#ifndef NOOGL
   glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
#endif
   }

void enableblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
#endif
   }

void enableADDblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_ONE,GL_ONE);
   glEnable(GL_BLEND);
#endif
   }

void enableATTblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
#endif
   }

void enableEMIblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
#endif
   }

void enableRKEblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
#endif
   }

void enablePRJblending()
   {
#ifndef NOOGL
   glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
#endif
   }

void enableMIPblending()
   {
#ifndef NOOGL

#ifdef GL_EXT_blend_minmax
   glBlendFunc(GL_ONE,GL_ONE);
   if (glext_mm) glBlendEquation(GL_MAX_EXT);
   glEnable(GL_BLEND);
   maxblendeq=TRUE;
#endif

#endif
   }

void disableblending()
   {
#ifndef NOOGL

#ifdef GL_EXT_blend_minmax
   if (maxblendeq)
      {
      if (glext_mm) glBlendEquation(GL_FUNC_ADD_EXT);
      maxblendeq=FALSE;
      }
#endif

   glDisable(GL_BLEND);

#endif
   }

void enablestenciling(int pass)
   {
#ifndef NOOGL

   if (pass==0)
      {
      glStencilFunc(GL_EQUAL,0,1);

      glStencilOp(GL_KEEP, // fail
                  GL_KEEP, // zfail
                  GL_INCR); // zpass
      }
   else if (pass==1)
      {
      glStencilFunc(GL_ALWAYS,1,1);

      glStencilOp(GL_KEEP, // fail
                  GL_KEEP, // zfail
                  GL_REPLACE); // zpass
      }
   else if (pass==2)
      {
      glStencilFunc(GL_EQUAL,1,1);

      glStencilOp(GL_KEEP, // fail
                  GL_KEEP, // zfail
                  GL_KEEP); // zpass
      }
   else if (pass==3)
      {
      glStencilFunc(GL_NOTEQUAL,1,1);

      glStencilOp(GL_KEEP, // fail
                  GL_KEEP, // zfail
                  GL_KEEP); // zpass
      }

   glEnable(GL_STENCIL_TEST);

#endif
   }

void disablestenciling()
   {
#ifndef NOOGL
   glDisable(GL_STENCIL_TEST);
#endif
   }

void enablefog(float fogstart,float fogend,float r,float g,float b,float a)
   {
#ifndef NOOGL

   GLfloat color[4];

   color[0]=r;
   color[1]=g;
   color[2]=b;
   color[3]=a;

   glFogfv(GL_FOG_COLOR,color);

   glFogi(GL_FOG_MODE,GL_LINEAR);
   glFogf(GL_FOG_START,fogstart);
   glFogf(GL_FOG_END,fogend);

   glEnable(GL_FOG);

#endif
   }

void enablefog(float fogstart,float fogend,const miniv3d &c)
   {enablefog(fogstart,fogend,c.x,c.y,c.z,1.0);}

void enablefog(float fogstart,float fogend,const miniv4d &c)
   {enablefog(fogstart,fogend,c.x,c.y,c.z,c.w);}

void disablefog()
   {
#ifndef NOOGL
   glDisable(GL_FOG);
#endif
   }

void getfog(float *start,float *end)
   {
#ifndef NOOGL
   glGetFloatv(GL_FOG_START,start);
   glGetFloatv(GL_FOG_END,end);
#endif
   }

void enableplane(int n,float a,float b,float c,float d)
   {
#ifndef NOOGL

   GLdouble equ[4];

   equ[0]=a;
   equ[1]=b;
   equ[2]=c;
   equ[3]=d;

   glClipPlane(GL_CLIP_PLANE0+n,equ);

   glEnable(GL_CLIP_PLANE0+n);

#endif
   }

void disableplane(int n)
   {
#ifndef NOOGL
   glDisable(GL_CLIP_PLANE0+n);
#endif
   }

int buildLtexmap(unsigned char *image,int *width,int *height,int mipmaps,int mipmapped)
   {return(buildtexmap(image,width,height,1,0,mipmaps,mipmapped));}

int buildLAtexmap(unsigned char *image,int *width,int *height,int mipmaps,int mipmapped)
   {return(buildtexmap(image,width,height,2,(CONFIGURE_DEPTH==24)?32:CONFIGURE_DEPTH,mipmaps,mipmapped));}

int buildRGBtexmap(unsigned char *image,int *width,int *height,int mipmaps,int s3tc,int bytes,int mipmapped)
   {return(buildtexmap(image,width,height,3,CONFIGURE_DEPTH,mipmaps,s3tc,bytes,mipmapped));}

int buildRGBAtexmap(unsigned char *image,int *width,int *height,int mipmaps,int s3tc,int bytes,int mipmapped)
   {return(buildtexmap(image,width,height,4,(CONFIGURE_DEPTH==24)?32:CONFIGURE_DEPTH,mipmaps,s3tc,bytes,mipmapped));}

int buildRGBAtexmap(unsigned char *imageRGB,unsigned char *imageA,int *width,int *height)
   {
   int i,c;

   int texid;

   unsigned char *image;

   if (width==NULL || height==NULL) ERRORMSG();
   if (*width<2 || *height<2) ERRORMSG();

   c=(*width)*(*height);

   if ((image=(unsigned char *)malloc(4*c))==NULL) MEMERROR();

   for (i=0; i<c; i++)
      {
      image[4*i]=imageRGB[3*i];
      image[4*i+1]=imageRGB[3*i+1];
      image[4*i+2]=imageRGB[3*i+2];
      image[4*i+3]=255-imageA[i];
      }

   texid=buildRGBAtexmap(image,width,height);
   free(image);

   return(texid);
   }

int buildtexmap(unsigned char *image,int *width,int *height,int components,int depth,int mipmaps,int s3tc,int bytes,int mipmapped)
   {
   if (image==NULL) ERRORMSG();

   if (width==NULL || height==NULL) ERRORMSG();
   if (*width<2 || *height<2) ERRORMSG();
   if (components<1) ERRORMSG();

#ifndef NOOGL

   int i,c;

   GLuint texid;
   GLint texsource,texformat;

   unsigned char *image2,*image3;
   unsigned char table[256];

   float intensity;
   int lumi,red,green,blue,alpha;

   GLint width2,height2;

   unsigned char *mipmap;
   int bytes2;
   int level;

   int maxtexsize;

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#ifdef GL_ARB_texture_compression
   if (glext_tc && CONFIGURE_COMPRESSION!=0)
      if (CONFIGURE_COMPRESSION==1) glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_FASTEST);
      else glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_NICEST);
#endif

   c=(*width)*(*height);

   if (mipmapped!=0)
      {
      width2=(*width)/2;
      height2=(*height)/2;

      while (width2>0 && height2>0)
         {
         c+=width2*height2;
         width2/=2;
         height2/=2;
         }
      }

   texsource=texformat=GL_RGB;

   switch (components)
      {
      case 1:
         texsource=GL_LUMINANCE;
         break;
      case 2:
         texsource=GL_LUMINANCE_ALPHA;
         break;
      case 3:
         if (s3tc==0) texsource=GL_RGB;
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
         else texsource=GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#endif
         break;
      case 4:
         if (s3tc==0) texsource=GL_RGBA;
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
         else texsource=GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
#endif
         break;
      default: ERRORMSG();
      }

   image2=image;

   if (s3tc==0)
      if (CONFIGURE_GAMMA!=1.0f ||
          CONFIGURE_LEVEL!=0.0f)
         {
         for (i=0; i<256; i++)
            {
            intensity=i/255.0f;

            if (CONFIGURE_GAMMA>0.0f) intensity=fpow(intensity,1.0f/CONFIGURE_GAMMA);

            if (CONFIGURE_LEVEL<0.0f && CONFIGURE_LEVEL>-1.0f) intensity=(intensity+CONFIGURE_LEVEL)/(1.0f+CONFIGURE_LEVEL);
            if (CONFIGURE_LEVEL>0.0f && CONFIGURE_LEVEL<1.0f) intensity/=(1.0f-CONFIGURE_LEVEL);

            intensity=fmin(fmax(intensity,0.0f),1.0f);

            table[i]=ftrc(255.0f*intensity+0.5f);
            }

         if ((image2=(unsigned char *)malloc(components*c))==NULL) MEMERROR();

         if (components==1)
            for (i=0; i<c; i++) image2[i]=table[image[i]];
         else if (components==2)
            for (i=0; i<c; i++)
               {
               image2[2*i]=table[image[2*i]];
               image2[2*i+1]=image[2*i+1];
               }
         else if (components==3)
            for (i=0; i<3*c; i++) image2[i]=table[image[i]];
         else if (components==4)
            for (i=0; i<c; i++)
               {
               image2[4*i]=table[image[4*i]];
               image2[4*i+1]=table[image[4*i+1]];
               image2[4*i+2]=table[image[4*i+2]];
               image2[4*i+3]=image[4*i+3];
               }
         }

   image3=image2;

   switch (depth)
      {
      case 0:
         texformat=GL_LUMINANCE;
#ifdef GL_ARB_texture_compression
         if (glext_tc && CONFIGURE_COMPRESSION!=0) texformat=GL_COMPRESSED_LUMINANCE_ARB;
#endif
         break;
      case 8:
         texformat=GL_R3_G3_B2;
         break;
      case 16:
         texformat=GL_RGB5_A1;
         break;
      case 24:
      default:
         texformat=GL_RGB;
#ifdef GL_ARB_texture_compression
         if (glext_tc && (s3tc!=0 || CONFIGURE_COMPRESSION!=0)) texformat=GL_COMPRESSED_RGB_ARB;
#endif
         break;
      case 32:
         texformat=GL_RGBA;
#ifdef GL_ARB_texture_compression
         if (glext_tc && (s3tc!=0 || CONFIGURE_COMPRESSION!=0)) texformat=GL_COMPRESSED_RGBA_ARB;
#endif

         if (s3tc==0)
            if (components==1)
               {
               texsource=GL_LUMINANCE_ALPHA;

               if ((image3=(unsigned char *)malloc(2*c))==NULL) MEMERROR();

               for (i=0; i<c; i++)
                  {
                  lumi=image2[i];

                  if (lumi==0) alpha=0;
                  else alpha=255;

                  image3[2*i]=lumi;
                  image3[2*i+1]=alpha;
                  }
               }
            else if (components==3)
               {
               texsource=GL_RGBA;

               if ((image3=(unsigned char *)malloc(4*c))==NULL) MEMERROR();

               for (i=0; i<c; i++)
                  {
                  red=image2[3*i];
                  green=image2[3*i+1];
                  blue=image2[3*i+2];

                  if (red==0 && green==0 && blue==0) alpha=0;
                  else alpha=255;

                  image3[4*i]=red;
                  image3[4*i+1]=green;
                  image3[4*i+2]=blue;
                  image3[4*i+3]=alpha;
                  }
               }
         break;
      }

   if (s3tc!=0 && !glext_ts3)
      {
      WARNMSG();
      return(0);
      }

   glGenTextures(1,&texid);
   glBindTexture(GL_TEXTURE_2D,texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);

#ifdef GL_ARB_texture_compression
   if (s3tc!=0)
      if (mipmapped==0)
         {
         if (!glext_np2)
            {
            if ((((*width)-1)&(*width))!=0) WARNMSG();
            if ((((*height)-1)&(*height))!=0) WARNMSG();
            }

         maxtexsize=getmaxtexsize();
         if (*width>maxtexsize || *height>maxtexsize) WARNMSG();

         glCompressedTexImage2DARB(GL_TEXTURE_2D,0,texsource,*width,*height,0,bytes,image3);
         }
      else
         {
         width2=*width;
         height2=*height;

         mipmap=image3;
         bytes2=8*((width2+3)/4)*((height2+3)/4); // 8 bytes per 4x4 s3tc/dxt1 rgba block
         level=0;

         while (width2>0 && height2>0)
            {
            if (mipmap>=image3+bytes) ERRORMSG();

            if (!glext_np2)
               {
               if (((width2-1)&width2)!=0) WARNMSG();
               if (((height2-1)&height2)!=0) WARNMSG();
               }

            maxtexsize=getmaxtexsize();
            if (width2>maxtexsize || height2>maxtexsize) WARNMSG();

            glCompressedTexImage2DARB(GL_TEXTURE_2D,level,texsource,width2,height2,0,bytes2,mipmap);

            width2/=2;
            height2/=2;

            if (width2>0 || height2>0)
               {
               if (width2==0) width2=1;
               if (height2==0) height2=1;

               mipmap+=bytes2;
               bytes2=8*((width2+3)/4)*((height2+3)/4); // 8 bytes per 4x4 s3tc/dxt1 rgba block
               level++;
               }
            }
         }
#endif
   else if (mipmapped==0)
      if (mipmaps!=0)
         {
#ifndef GL_SGIS_generate_mipmap
         gluBuild2DMipmaps(GL_TEXTURE_2D,texformat,*width,*height,texsource,GL_UNSIGNED_BYTE,image3);
#else
         if (!glext_tgm || !CONFIGURE_GENERATEMM) gluBuild2DMipmaps(GL_TEXTURE_2D,texformat,*width,*height,texsource,GL_UNSIGNED_BYTE,image3);
         else
            {
            glTexParameteri(GL_TEXTURE_2D,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
            glTexImage2D(GL_TEXTURE_2D,0,texformat,*width,*height,0,texsource,GL_UNSIGNED_BYTE,image3);
            }
#endif
         }
      else
         {
         if (!glext_np2)
            {
            if ((((*width)-1)&(*width))!=0) WARNMSG();
            if ((((*height)-1)&(*height))!=0) WARNMSG();
            }

         maxtexsize=getmaxtexsize();
         if (*width>maxtexsize || *height>maxtexsize) WARNMSG();

         glTexImage2D(GL_TEXTURE_2D,0,texformat,*width,*height,0,texsource,GL_UNSIGNED_BYTE,image3);
         }
   else
      {
      width2=*width;
      height2=*height;

      mipmap=image3;
      bytes=width2*height2*components;
      level=0;

      while (width2>0 && height2>0)
         {
         if (!glext_np2)
            {
            if (((width2-1)&width2)!=0) WARNMSG();
            if (((height2-1)&height2)!=0) WARNMSG();
            }

         maxtexsize=getmaxtexsize();
         if (width2>maxtexsize || height2>maxtexsize) WARNMSG();

         glTexImage2D(GL_TEXTURE_2D,level,texformat,width2,height2,0,texsource,GL_UNSIGNED_BYTE,mipmap);

         width2/=2;
         height2/=2;

         if (width2>0 || height2>0)
            {
            if (width2==0) width2=1;
            if (height2==0) height2=1;

            mipmap+=bytes;
            bytes=width2*height2*components;
            level++;
            }
         }
      }

   // by default, enable mip-map filtering
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

   // by default, set texture wrap to clamp
#if !defined(GL_CLAMP_TO_EDGE_SGIS) && !defined(GL_CLAMP_TO_EDGE_EXT)
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
#else
   if (glext_tec)
      {
#ifdef GL_CLAMP_TO_EDGE_SGIS
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE_SGIS);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE_SGIS);
#endif
#ifdef GL_CLAMP_TO_EDGE_EXT
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE_EXT);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE_EXT);
#endif
      }
   else
      {
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
      }
#endif

   // by default, enable anisotropic texture filtering
#ifdef GL_EXT_texture_filter_anisotropic
   if (glext_tfa)
      {
      GLfloat maxaniso=1.0f;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);
      glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);
      }
#endif

   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width2);
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height2);
   glBindTexture(GL_TEXTURE_2D,0);

   if (image3!=image2) free(image3);
   if (image2!=image) free(image2);

   *width=width2;
   *height=height2;

   return(texid);

#else
   return(0);
#endif
   }

void bindtexmap(int texid,int width,int height,int size,int mipmaps)
   {
#ifndef NOOGL

   GLfloat v1[]={1.0f,0.0f,0.0f,0.0f},
           v2[]={0.0f,0.0f,1.0f,1.0f};

   initglexts();

   if (texid>0)
      {
      glBindTexture(GL_TEXTURE_2D,texid);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

      // optionally, disable mip-map filtering
      if (mipmaps==0) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

      // optionally, change texture wrap from clamp to repeat
      if (size==0 && width==0 && height==0)
         {
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
         }

      // optionally, enable texture coordinate generation
      if (size>1 && width>1 && height>1)
         {
         v1[0]=1.0f/(size-1)*(width-1)/width;
         v2[2]=-1.0f/(size-1)*(height-1)/height;

         v1[3]=0.5f/width;
         v2[3]=1.0f-0.5f/height;

         glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
         glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);

         glTexGenfv(GL_S,GL_OBJECT_PLANE,v1);
         glTexGenfv(GL_T,GL_OBJECT_PLANE,v2);

         glEnable(GL_TEXTURE_GEN_S);
         glEnable(GL_TEXTURE_GEN_T);
         }

      glEnable(GL_TEXTURE_2D);
      }
   else
      {
      glBindTexture(GL_TEXTURE_2D,0);
      glDisable(GL_TEXTURE_2D);

      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      }

#endif
   }

int build3Dtexmap(unsigned char *volume,
                  int *width,int *height,int *depth,
                  int components)
   {
   if (volume==NULL) ERRORMSG();

   if (width==NULL || height==NULL || depth==NULL) ERRORMSG();
   if (*width<2 || *height<2 || *depth<2) ERRORMSG();
   if (components<1) ERRORMSG();

#ifndef NOOGL

#ifdef GL_EXT_texture3D

   GLuint texid;

   GLint width2,height2,depth2;

   int max3Dtexsize;

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

   if (!glext_t3D) return(0);

   max3Dtexsize=getmax3Dtexsize();
   if (*width>max3Dtexsize || *height>max3Dtexsize || *depth>max3Dtexsize) WARNMSG();

   glGenTextures(1,&texid);
   glBindTexture(GL_TEXTURE_3D,texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   if (components==1)
      glTexImage3D(GL_TEXTURE_3D,0,GL_LUMINANCE,*width,*height,*depth,0,
                   GL_LUMINANCE,GL_UNSIGNED_BYTE,volume);
   else if (components==3)
      glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,*width,*height,*depth,0,
                   GL_RGB,GL_UNSIGNED_BYTE,volume);
   else if (components==4)
      glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,*width,*height,*depth,0,
                   GL_RGBA,GL_UNSIGNED_BYTE,volume);
   else ERRORMSG();

   // by default, set texture wrap to repeat
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_REPEAT);

   // by default, disable mip-map filtering
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_WIDTH,&width2);
   glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_HEIGHT,&height2);
   glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_DEPTH,&depth2);

   glBindTexture(GL_TEXTURE_3D,0);

   *width=width2;
   *height=height2;
   *depth=depth2;

   return(texid);

#else
   return(0);
#endif

#else
   return(0);
#endif
   }

void bind3Dtexmap(int texid)
   {
   if (texid<0) ERRORMSG();

#ifndef NOOGL

#ifdef GL_EXT_texture3D

   initglexts();

   if (!glext_t3D) return;

   if (texid>0)
      {
      glBindTexture(GL_TEXTURE_3D,texid);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      glEnable(GL_TEXTURE_3D);
      }
   else glDisable(GL_TEXTURE_3D);

#endif

#endif
   }

void deletetexmap(int texid)
   {
#ifndef NOOGL
   GLuint GLtexid=texid;
   if (texid>0) glDeleteTextures(1,&GLtexid);
#endif
   }

int compressRGBtexmap(unsigned char *image,int width,int height,
                      unsigned char **data,int *bytes)
   {
#ifndef NOOGL

#if defined(GL_ARB_texture_compression) && defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)

   GLint success,format,size;
   unsigned char *texture;

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

   // upload texture
   glTexImage2D(GL_TEXTURE_2D,0,GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
                width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);

   // check whether or not compression succeeded
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPRESSED_ARB,&success);
   if (success==0) return(0);

   // check whether or not the internal format is ok
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,&format);
   if (format!=GL_COMPRESSED_RGB_S3TC_DXT1_EXT) return(0);

   // get size of compressed data chunk
   glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB,&size);

   // allocate new texture buffer
   if ((texture=(unsigned char *)malloc(size))==NULL) MEMERROR();

   // get compressed image into buffer
   glGetCompressedTexImageARB(GL_TEXTURE_2D,0,texture);

   *data=texture;
   *bytes=size;

   return(1);

#endif

#endif

   return(0);
   }

// convert databuffer to texture id
int db2texid(databuf *buf,int *width,int *height,int *mipmaps)
   {
   int texid;

   if (buf->missing()) ERRORMSG();

   if (buf->xsize<2 || buf->ysize<2 ||
       buf->zsize>1 || buf->tsteps>1) ERRORMSG();

   texid=0;

   *width=buf->xsize;
   *height=buf->ysize;

   if (buf->type==databuf::DATABUF_TYPE_BYTE) texid=buildLtexmap((unsigned char *)buf->data,width,height,*mipmaps);
   else if (buf->type==databuf::DATABUF_TYPE_RGB) texid=buildRGBtexmap((unsigned char *)buf->data,width,height,*mipmaps);
   else if (buf->type==databuf::DATABUF_TYPE_RGBA) texid=buildRGBAtexmap((unsigned char *)buf->data,width,height,*mipmaps);
   else if (buf->type==databuf::DATABUF_TYPE_RGB_S3TC) texid=buildRGBtexmap((unsigned char *)buf->data,width,height,*mipmaps=0,1,buf->bytes);
   else if (buf->type==databuf::DATABUF_TYPE_RGBA_S3TC) texid=buildRGBAtexmap((unsigned char *)buf->data,width,height,*mipmaps=0,1,buf->bytes);
   else if (buf->type==databuf::DATABUF_TYPE_RGB_MM) texid=buildRGBtexmap((unsigned char *)buf->data,width,height,*mipmaps=1,0,0,1);
   else if (buf->type==databuf::DATABUF_TYPE_RGBA_MM) texid=buildRGBAtexmap((unsigned char *)buf->data,width,height,*mipmaps=1,0,0,1);
   else if (buf->type==databuf::DATABUF_TYPE_RGB_MM_S3TC) texid=buildRGBtexmap((unsigned char *)buf->data,width,height,*mipmaps=1,1,buf->bytes,1);
   else if (buf->type==databuf::DATABUF_TYPE_RGBA_MM_S3TC) texid=buildRGBAtexmap((unsigned char *)buf->data,width,height,*mipmaps=1,1,buf->bytes,1);

   return(texid);
   }

void enabletexgen()
   {
   GLfloat v1[]={1.0f,0.0f,0.0f,0.0f},
           v2[]={0.0f,1.0f,0.0f,0.0f},
           v3[]={0.0f,0.0f,1.0f,0.0f};

   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);

   glTexGenfv(GL_S,GL_OBJECT_PLANE,v1);
   glTexGenfv(GL_T,GL_OBJECT_PLANE,v2);
   glTexGenfv(GL_R,GL_OBJECT_PLANE,v3);

   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glEnable(GL_TEXTURE_GEN_R);
   }

void disabletexgen()
   {
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_GEN_R);
   }

void texunit(int unit)
   {
#ifndef NOOGL
   initglexts();
#ifdef GL_ARB_multitexture
   if (glext_mt!=0) glActiveTextureARB(GL_TEXTURE0_ARB+unit);
#endif
#endif
   }

void texclientunit(int unit)
   {
#ifndef NOOGL
   initglexts();
#ifdef GL_ARB_multitexture
   if (glext_mt!=0) glClientActiveTextureARB(GL_TEXTURE0_ARB+unit);
#endif
#endif
   }

inline int buildprog(const char *prog,BOOLINT vtxorfrg)
   {
#ifndef NOOGL

   GLuint progid=0;

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   if (glext_vp && glext_fp)
      {
      glGenProgramsARB(1,&progid);

      if (vtxorfrg)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,progid);
         glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(prog),prog);
         }
      else
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,progid);
         glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(prog),prog);
         }
      }
#endif

   return(progid);
#else
   return(0);
#endif
   }

inline void bindprog(int progid,BOOLINT vtxorfrg)
   {
#ifndef NOOGL

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   if (glext_vp && glext_fp)
      if (progid!=0)
         if (vtxorfrg)
            {
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB,progid);
            glEnable(GL_VERTEX_PROGRAM_ARB);
            }
         else
            {
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,progid);
            glEnable(GL_FRAGMENT_PROGRAM_ARB);
            }
      else
         if (vtxorfrg)
            {
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
            glDisable(GL_VERTEX_PROGRAM_ARB);
            }
         else
            {
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
            glDisable(GL_FRAGMENT_PROGRAM_ARB);
            }
#endif

#endif
   }

inline void setprogpar(int n,float p1,float p2,float p3,float p4,BOOLINT vtxorfrg)
   {
#ifndef NOOGL

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   if (glext_vp && glext_fp)
      if (vtxorfrg) glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,n,p1,p2,p3,p4);
      else glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,n,p1,p2,p3,p4);
#endif

#endif
   }

inline void setprogpars(int n,int count,const float *params,BOOLINT vtxorfrg)
   {
#ifndef NOOGL

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   if (glext_vp && glext_fp)
      if (vtxorfrg)
         for (int i=0; i<count; i++)
            glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,n+i,params[4*i],params[4*i+1],params[4*i+2],params[4*i+3]);
      else
         for (int i=0; i<count; i++)
            glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,n+i,params[4*i],params[4*i+1],params[4*i+2],params[4*i+3]);
#endif

#endif
   }

inline void deleteprog(int progid)
   {
#ifndef NOOGL

   GLuint id;

   initglexts();

#ifdef _WIN32
   initwglprocs();
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   if (glext_vp && glext_fp)
      if (progid!=0)
         {
         id=progid;
         glDeleteProgramsARB(1,&id);
         }
#endif

#endif
   }

int buildvtxprog(const char *prog) {return(buildprog(prog,TRUE));}
void bindvtxprog(int progid) {bindprog(progid,TRUE);}
void setvtxprogpar(int n,float p1,float p2,float p3,float p4) {setprogpar(n,p1,p2,p3,p4,TRUE);}
void setvtxprogpars(int n,int count,const float *params) {setprogpars(n,count,params,TRUE);}
void deletevtxprog(int progid) {deleteprog(progid);}

int buildfrgprog(const char *prog) {return(buildprog(prog,FALSE));}
void bindfrgprog(int progid) {bindprog(progid,FALSE);}
void setfrgprogpar(int n,float p1,float p2,float p3,float p4) {setprogpar(n,p1,p2,p3,p4,FALSE);}
void setfrgprogpars(int n,int count,const float *params) {setprogpars(n,count,params,FALSE);}
void deletefrgprog(int progid) {deleteprog(progid);}

void mtxgetmodel(float oglmtx[16])
   {
#ifndef NOOGL
   glGetFloatv(GL_MODELVIEW_MATRIX,oglmtx);
#endif
   }

void mtxgetmodel(double oglmtx[16])
   {
#ifndef NOOGL
   glGetDoublev(GL_MODELVIEW_MATRIX,oglmtx);
#endif
   }

void mtxget(const miniv3d mtx[3],double oglmtx[16])
   {
   oglmtx[0]=mtx[0].x;
   oglmtx[1]=mtx[1].x;
   oglmtx[2]=mtx[2].x;
   oglmtx[3]=0.0;

   oglmtx[4]=mtx[0].y;
   oglmtx[5]=mtx[1].y;
   oglmtx[6]=mtx[2].y;
   oglmtx[7]=0.0;

   oglmtx[8]=mtx[0].z;
   oglmtx[9]=mtx[1].z;
   oglmtx[10]=mtx[2].z;
   oglmtx[11]=0.0;

   oglmtx[12]=0.0;
   oglmtx[13]=0.0;
   oglmtx[14]=0.0;
   oglmtx[15]=1.0;
   }

void mtxget(const miniv4d mtx[3],double oglmtx[16])
   {
   oglmtx[0]=mtx[0].x;
   oglmtx[1]=mtx[1].x;
   oglmtx[2]=mtx[2].x;
   oglmtx[3]=0.0;

   oglmtx[4]=mtx[0].y;
   oglmtx[5]=mtx[1].y;
   oglmtx[6]=mtx[2].y;
   oglmtx[7]=0.0;

   oglmtx[8]=mtx[0].z;
   oglmtx[9]=mtx[1].z;
   oglmtx[10]=mtx[2].z;
   oglmtx[11]=0.0;

   oglmtx[12]=mtx[0].w;
   oglmtx[13]=mtx[1].w;
   oglmtx[14]=mtx[2].w;
   oglmtx[15]=1.0;
   }

void mtxget(const double oglmtx[16],miniv3d mtx[3])
   {
   mtx[0]=miniv3d(oglmtx[0],oglmtx[4],oglmtx[8]);
   mtx[1]=miniv3d(oglmtx[1],oglmtx[5],oglmtx[9]);
   mtx[2]=miniv3d(oglmtx[2],oglmtx[6],oglmtx[10]);
   }

void mtxget(const double oglmtx[16],miniv4d mtx[3])
   {
   mtx[0]=miniv4d(oglmtx[0],oglmtx[4],oglmtx[8],oglmtx[12]);
   mtx[1]=miniv4d(oglmtx[1],oglmtx[5],oglmtx[9],oglmtx[13]);
   mtx[2]=miniv4d(oglmtx[2],oglmtx[6],oglmtx[10],oglmtx[14]);
   }

void mtxget(const double oglmtx[16],miniv3d &vec)
   {
   vec.x=oglmtx[12];
   vec.y=oglmtx[13];
   vec.z=oglmtx[14];
   }

void mtxget(const double oglmtx[16],miniv4d &vec)
   {
   vec.x=oglmtx[12];
   vec.y=oglmtx[13];
   vec.z=oglmtx[14];
   vec.w=oglmtx[15];
   }

void multitexcoord(const unsigned int unit,const float s,const float t,const float r)
   {
#ifndef NOOGL
   initglexts();
#ifdef _WIN32
   initwglprocs();
#endif
#ifdef GL_ARB_multitexture
   if (glext_mt!=0)
      if (unit<GL_MAX_TEXTURE_UNITS_ARB)
         glMultiTexCoord3fARB(GL_TEXTURE0_ARB+unit,s,t,r);
#endif
#endif
   }

void multitexcoord(const unsigned int unit,const miniv3d &t)
   {multitexcoord(unit,t.x,t.y,t.z);}

int getfancnt()
   {
#ifndef NOOGL
   return(fancnt);
#else
   return(0);
#endif
   }

int getvtxcnt()
   {
#ifndef NOOGL
   return(vtxcnt);
#else
   return(0);
#endif
   }

void linewidth(const float w)
   {
#ifndef NOOGL
   glLineWidth(w);
#endif
   }

void enablelinesmooth()
   {
#ifndef NOOGL
   glEnable(GL_LINE_SMOOTH);
#endif
   }

void disablelinesmooth()
   {
#ifndef NOOGL
   glDisable(GL_LINE_SMOOTH);
#endif
   }

void pointsize(const float s)
   {
#ifndef NOOGL
   glPointSize(s);
#endif
   }

void enablepointsmooth()
   {
#ifndef NOOGL
   glEnable(GL_POINT_SMOOTH);
#endif
   }

void disablepointsmooth()
   {
#ifndef NOOGL
   glDisable(GL_POINT_SMOOTH);
#endif
   }

void vertexarray(float *array,int comps)
   {
#ifndef NOOGL
   if (array!=NULL)
      {
      glVertexPointer(comps,GL_FLOAT,0,array);
      glEnableClientState(GL_VERTEX_ARRAY);
      }
   else glDisableClientState(GL_VERTEX_ARRAY);
#endif
   }

void colorarray(float *array,int comps)
   {
#ifndef NOOGL
   if (array!=NULL)
      {
      glColorPointer(comps,GL_FLOAT,0,array);
      glEnableClientState(GL_COLOR_ARRAY);
      }
   else glDisableClientState(GL_COLOR_ARRAY);
#endif
   }

void normalarray(float *array)
   {
#ifndef NOOGL
   if (array!=NULL)
      {
      glNormalPointer(GL_FLOAT,0,array);
      glEnableClientState(GL_NORMAL_ARRAY);
      }
   else glDisableClientState(GL_NORMAL_ARRAY);
#endif
   }

void texcoordarray(float *array,int comps)
   {
#ifndef NOOGL
   if (array!=NULL)
      {
      glTexCoordPointer(comps,GL_FLOAT,0,array);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      }
   else glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
   }

void interleavedNVarray(float *array)
   {
#ifndef NOOGL
   if (array!=NULL)
      {
      glInterleavedArrays(GL_N3F_V3F,0,array);

      glEnableClientState(GL_NORMAL_ARRAY);
      glEnableClientState(GL_VERTEX_ARRAY);
      }
   else
      {
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);
      }
#endif
   }

void rendertriangles(int start,int size)
   {
#ifndef NOOGL
   glDrawArrays(GL_TRIANGLES,start,size);
#endif
   }

void renderquads(int start,int size)
   {
#ifndef NOOGL
   glDrawArrays(GL_QUADS,start,size);
#endif
   }

void renderlines(int start,int size)
   {
#ifndef NOOGL
   glDrawArrays(GL_LINES,start,size);
#endif
   }

void renderstrip(int start,int size)
   {
#ifndef NOOGL
   glDrawArrays(GL_TRIANGLE_STRIP,start,size);
#endif
   }

void renderfan(int start,int size)
   {
#ifndef NOOGL
   glDrawArrays(GL_TRIANGLE_FAN,start,size);
#endif
   }

int getmaxtexsize()
   {
#ifndef NOOGL
   GLint param;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&param);
   return(param);
#else
   return(0);
#endif
   }

int getmax3Dtexsize()
   {
#ifndef NOOGL
   GLint param=0;
   initglexts();
#ifdef GL_EXT_texture3D
   if (glext_t3D) glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,&param);
#endif
   return(param);
#else
   return(0);
#endif
   }

int getmaxtexunits()
   {
#ifndef NOOGL
   GLint param=1;
#ifdef GL_ARB_multitexture
   if (glext_mt) glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&param);
#endif
   return(param);
#else
   return(0);
#endif
   }

int getmaxtexcoords()
   {
#ifndef NOOGL
   GLint param=1;
#ifdef GL_ARB_multitexture
   if (glext_mt) glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&param);
#endif
   return(param);
#else
   return(0);
#endif
   }

int getmaxteximageunits()
   {
#ifndef NOOGL
   GLint param=1;
#ifdef GL_ARB_multitexture
   if (glext_mt) glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&param);
#endif
   return(param);
#else
   return(0);
#endif
   }

void getviewport(int *x,int *y,int *width,int *height)
   {
#ifndef NOOGL

   GLint viewport[4];

   glGetIntegerv(GL_VIEWPORT,viewport);

   *x=viewport[0];
   *y=viewport[1];

   *width=viewport[2];
   *height=viewport[3];

#endif
   }

unsigned char *readRGBpixels(int x,int y,int width,int height)
   {
   unsigned char *pixels=NULL;

#ifndef NOOGL

   glFinish();

   if ((pixels=(unsigned char *)malloc(3*width*height))==NULL) MEMERROR();

   glReadBuffer(GL_BACK);
   glReadPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels);

#endif

   return(pixels);
   }

void writeRGBpixels(unsigned char *pixels,int width,int height,int winwidth,int winheight,int x,int y)
   {
#ifndef NOOGL

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0f,winwidth-1,0.0f,winheight-1);
   glMatrixMode(GL_MODELVIEW);

   glRasterPos2i(x,y);
   glDrawBuffer(GL_BACK);

   glColor3f(1.0f,1.0f,1.0f);

   glDepthMask(GL_FALSE);

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels);

   glDepthMask(GL_TRUE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

#endif
   }

float *readZpixels(int x,int y,int width,int height)
   {
   float *pixels=NULL;

#ifndef NOOGL

   initglexts();

   glFinish();

   if ((pixels=(float *)malloc(sizeof(float)*width*height))==NULL) MEMERROR();

   glReadBuffer(GL_BACK);
#ifdef GL_ARB_depth_texture
   if (glext_dt) glReadPixels(x,y,width,height,GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
#endif

#endif

   return(pixels);
   }

void writeZpixels(float *pixels,int width,int height,int winwidth,int winheight,int x,int y)
   {
#ifndef NOOGL

   initglexts();

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0f,winwidth-1,0.0f,winheight-1);
   glMatrixMode(GL_MODELVIEW);

   glRasterPos2i(x,y);
   glDrawBuffer(GL_BACK);

   glColor3f(1.0f,1.0f,1.0f);

   glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
#ifdef GL_ARB_depth_texture
   if (glext_dt) glDrawPixels(width,height,GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
#endif

   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

#endif
   }

int copytexrect(int depthcomp,int nofrills)
   {
#ifndef NOOGL

   GLuint texid=0;

   GLint viewport[4];

   int startx,starty;
   int width,height;

   initglexts();

   glFinish();

   // get viewport dimensions
   glGetIntegerv(GL_VIEWPORT,viewport);

   startx=viewport[0];
   starty=viewport[1];

   width=viewport[2];
   height=viewport[3];

#ifdef GL_ARB_texture_rectangle
   if (glext_tr)
      {
      glGenTextures(1,&texid);
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB,texid);

      if (nofrills==0) glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_WRAP_S,GL_CLAMP);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_WRAP_T,GL_CLAMP);

      if (depthcomp!=0) glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_DEPTH_TEXTURE_MODE,GL_LUMINANCE);

      // copy color or depth component of viewport
      glReadBuffer(GL_BACK);
      if (depthcomp==0) glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,GL_RGB,startx,starty,width,height,0);
#ifdef GL_ARB_depth_texture
      else if (glext_dt) glCopyTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,GL_DEPTH_COMPONENT,startx,starty,width,height,0);
#endif
      }
#endif

   return(texid);

#else
   return(0);
#endif
   }

void bindtexrect(int texid,int nofrills)
   {
#ifndef NOOGL

   initglexts();

#ifdef GL_ARB_texture_rectangle
   if (glext_tr)
      if (texid>0)
         {
         glBindTexture(GL_TEXTURE_RECTANGLE_ARB,texid);
         if (nofrills==0) glEnable(GL_TEXTURE_RECTANGLE_ARB);
         }
      else
         {
         glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);
         if (nofrills==0) glDisable(GL_TEXTURE_RECTANGLE_ARB);
         }
#endif

#endif
   }

void painttexrect(int texid,int depthcomp)
   {
#ifndef NOOGL

   int startx,starty;
   int width,height;

   getviewport(&startx,&starty,&width,&height);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glTranslatef(0.5f,0.5f,0.0f);
   glScalef(width-1,height-1,0.0f);
   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glLoadIdentity();
   glTranslatef(0.5f,0.5f,0.0f);
   glScalef(width,height,0.0f);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(startx,startx+width-1,starty,starty+height-1);
   glMatrixMode(GL_MODELVIEW);

   if (depthcomp==0) glDepthMask(GL_FALSE);
   else glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

   color(1.0f,1.0f,1.0f);
   bindtexrect(texid);

   beginfans();
   beginfan();
   texcoord(0.0f,0.0f);
   fanvertex(0.0f,0.0f,0.0f);
   texcoord(1.0f,0.0f);
   fanvertex(1.0f,0.0f,0.0f);
   texcoord(1.0f,1.0f);
   fanvertex(1.0f,1.0f,0.0f);
   texcoord(0.0f,1.0f);
   fanvertex(0.0f,1.0f,0.0f);
   endfans();

   bindtexrect(0);

   if (depthcomp==0) glDepthMask(GL_TRUE);
   else glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glMatrixMode(GL_TEXTURE);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

#endif
   }

void deletetexrect(int texid)
   {deletetexmap(texid);}

void paintbuffer()
   {
#ifndef NOOGL

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0f,1.0f,0.0f,1.0f);
   glMatrixMode(GL_MODELVIEW);

   beginfans();
   beginfan();
   fanvertex(0.0f,0.0f,0.0f);
   fanvertex(1.0f,0.0f,0.0f);
   fanvertex(1.0f,1.0f,0.0f);
   fanvertex(0.0f,1.0f,0.0f);
   endfans();

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

#endif
   }

void polygonmode(int wire)
   {
#ifndef NOOGL
   if (wire==0) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   else glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
#endif
   }

}
