/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <malloc.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "defs.h"
#include "driver.h"
#include "menu.h"
#include "jpeg.h"
#include "OGL.h"

/* modul-lokale Makro-Definitionen */
#define FOG 0.7,0.7,1.0

#define NONE (-1)

#ifndef IMPACT
#define usetexture(tex){}
#else
#define usetexture(tex) glBindTextureEXT(GL_TEXTURE_2D,tex)
#endif

#define begin(mode1,mode2)\
   if (mode1!=drawmode || mode2!=shademode)\
      {\
      if (drawmode!=NONE) glEnd();\
      if (mode2!=shademode)\
         if (mode2==GL_LIGHTING)\
            {\
            if (shademode==GL_TEXTURE_2D) glDisable(GL_TEXTURE_2D);\
            glEnable(shademode=GL_LIGHTING);\
            }\
         else if (mode2==GL_TEXTURE_2D)\
            {\
            if (shademode==GL_LIGHTING) glDisable(GL_LIGHTING);\
            if (tex==0) ERROR();\
            usetexture(tex);\
            glEnable(shademode=GL_TEXTURE_2D);\
            }\
         else\
            {\
            if (shademode==GL_LIGHTING) glDisable(GL_LIGHTING);\
            else glDisable(GL_TEXTURE_2D);\
            shademode=NONE;\
            }\
      if ((drawmode=mode1)!=NONE) glBegin(drawmode);\
      }

#define SELECTBUFFERSIZE (100)

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
extern XtAppContext app;
extern Widget       toplevel,mainwin,manager,Xarea;

GLXContext GLXcontext;

GLint rendermode=GL_RENDER,
      drawmode=NONE,shademode=NONE;

GLuint tex=0;

int rendering=TRUE,popname;

GLuint selectbuffer[SELECTBUFFERSIZE];

#ifdef VERBOSE
   int tricnt,vtxcnt;
#endif

/* modul-lokale Funktions-Deklarationen */
