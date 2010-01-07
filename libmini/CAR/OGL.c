#include "OGLP.h"

/* open a window */
void OGLopenwindow(int width,int height,char *title)
   {
   GLint mode[]={GLX_RGBA,
                 GLX_RED_SIZE,1,
                 GLX_GREEN_SIZE,1,
                 GLX_BLUE_SIZE,1,
                 GLX_DEPTH_SIZE,1,
                 GLX_DOUBLEBUFFER,
                 None};

   Display     *display;
   XVisualInfo *vi;
   Colormap    colormap;
   XEvent      XWinevent;
   int         dummy=1;

   /* create all widgets */
   toplevel=XtVaAppInitialize(&app,"XMotif",NULL,0,&dummy,&title,NULL,NULL);
   display=XtDisplay(toplevel);
   vi=glXChooseVisual(display,DefaultScreen(display),mode);
   if (vi==NULL) error("appropriate graphix hardware missing");
   GLXcontext=glXCreateContext(display,vi,None,GL_TRUE);
   colormap=XCreateColormap(display,RootWindow(display,vi->screen),vi->visual,AllocNone);
   XtVaSetValues(toplevel,XtNvisual,vi->visual,XtNdepth,vi->depth,XtNcolormap,colormap,NULL);
   mainwin=XmCreateMainWindow(toplevel,"MainWindow",NULL,0);
   XtVaSetValues(mainwin,XmNtraversalOn,FALSE,XmNshowSeparator,TRUE,NULL);
   manager=XmCreateForm(mainwin,"Manager",NULL,0);
   Xarea=XmCreateDrawingArea(manager,"X-Area",NULL,0);
   XtVaSetValues(Xarea,XmNwidth,width,XmNheight,height,
                       XmNleftAttachment,XmATTACH_FORM,
                       XmNrightAttachment,XmATTACH_FORM,
                       XmNbottomAttachment,XmATTACH_FORM,
                       XmNtopAttachment,XmATTACH_FORM,NULL);

   XtManageChild(Xarea);
   XtManageChild(manager);
   XtManageChild(mainwin);

   /* draw window and wait for acknowledgement */
   XtRealizeWidget(toplevel);
   glXMakeCurrent(display,XtWindow(Xarea),GLXcontext);
   while (!XCheckMaskEvent(display,ExposureMask,&XWinevent));

   /* initialize OpenGL */
   glClearColor(FOG,1.0);
   glLineWidth(LINE_WIDTH);
   glShadeModel(GL_SMOOTH);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
   glFrontFace(GL_CCW);
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);
   glDisable(GL_NORMALIZE);
   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);
   usetexture(0);

#ifdef VERBOSE
   glPolygonMode(GL_FRONT,GL_LINE);
#endif
   }

/* clear back buffer */
void OGLclearwindow()
   {
   begin(NONE,NONE);
   glViewport(0,0,viewportwidth,viewportheight);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

#ifdef VERBOSE
   tricnt=vtxcnt=0;
#endif
   }

/* swap front and back buffer */
void OGLswapbuffers()
   {
   begin(NONE,NONE);
   if (rendermode==GL_RENDER) glXSwapBuffers(XtDisplay(Xarea),XtWindow(Xarea));

#ifdef VERBOSE
   if (tricnt>0) printf("triangles per frame: %d\n",tricnt);
   if (vtxcnt>0) printf("fan vertices per frame: %d\n",vtxcnt);
#endif
   }

/* free memory and close window */
void OGLclosewindow()
   {
   Display *display=XtDisplay(Xarea);

   begin(NONE,NONE);

   /* last but not least */
   menDestroy();
   glXDestroyContext(display,GLXcontext);
   XCloseDisplay(display);
   }

/* initialize 3D-graphix */
void OGLperspective(float fovy,float aspect,float near,float far)
   {
   static GLfloat fogcolor[4]={FOG,1.0};

   begin(NONE,NONE);

   /* initialize projection/modelview matrix */
   glMatrixMode(GL_PROJECTION);
   if (rendermode==GL_RENDER) glLoadIdentity();
   gluPerspective(fovy,aspect,near,far);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   /* set viewport */
   glViewport(viewportwidth*(1.0-VIEWPORT_SIZE)/2.0,
              viewportheight*(1.0-VIEWPORT_SIZE)/2.0,
              viewportwidth*VIEWPORT_SIZE,
              viewportheight*VIEWPORT_SIZE);

   /* use fog */
   glFogfv(GL_FOG_COLOR,fogcolor);
   glFogi(GL_FOG_MODE,GL_EXP);
#ifdef LOWFOG
   glFogf(GL_FOG_DENSITY,5E-4);
#else
#ifdef IMPACT
   glFogf(GL_FOG_DENSITY,5E-3);
#else
   glFogf(GL_FOG_DENSITY,1E-3);
#endif
#endif
   glEnable(GL_FOG);
   }

/* set lookat parameters */
void OGLlookat(float eyex,float eyey,float eyez,
               float centerx,float centery,float centerz,
               float upx,float upy,float upz)
   {
   begin(NONE,NONE);
   gluLookAt(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz);
   }

/* rotate around the x-axis */
void OGLrotatex(float spin)
   {
   begin(NONE,NONE);
   glRotatef(spin,1.0,0.0,0.0);
   }

/* rotate around the y-axis */
void OGLrotatey(float spin)
   {
   begin(NONE,NONE);
   glRotatef(spin,0.0,1.0,0.0);
   }

/* rotate around the z-axis */
void OGLrotatez(float spin)
   {
   begin(NONE,NONE);
   glRotatef(spin,0.0,0.0,1.0);
   }

/* translate by (dx,dy,dz) */
void OGLtranslate(float dx,float dy,float dz)
   {
   begin(NONE,NONE);
   glTranslatef(dx,dy,dz);
   }

/* scale by sx/sy/sz */
void OGLscale(float sx,float sy,float sz)
   {
   begin(NONE,NONE);
   glScalef(sx,sy,sz);
   }

/* rotate around the x-axis (multiply with M) */
void OGLrotatexM(matrix *M,float spin,int where)
   {
   begin(NONE,NONE);
   glPushMatrix();
   if (where==M_RIGHT)
      {
      glLoadMatrixf((GLfloat *)M);
      glRotatef(spin,1.0,0.0,0.0);
      }
   else
      {
      glLoadIdentity();
      glRotatef(spin,1.0,0.0,0.0);
      glMultMatrixf((GLfloat *)M);
      }
   glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)M);
   glPopMatrix();
   }

/* rotate around the y-axis (multiply with M) */
void OGLrotateyM(matrix *M,float spin,int where)
   {
   begin(NONE,NONE);
   glPushMatrix();
   if (where==M_RIGHT)
      {
      glLoadMatrixf((GLfloat *)M);
      glRotatef(spin,0.0,1.0,0.0);
      }
   else
      {
      glLoadIdentity();
      glRotatef(spin,0.0,1.0,0.0);
      glMultMatrixf((GLfloat *)M);
      }
   glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)M);
   glPopMatrix();
   }

/* rotate around the z-axis (multiply with M) */
void OGLrotatezM(matrix *M,float spin,int where)
   {
   begin(NONE,NONE);
   glPushMatrix();
   if (where==M_RIGHT)
      {
      glLoadMatrixf((GLfloat *)M);
      glRotatef(spin,0.0,0.0,1.0);
      }
   else
      {
      glLoadIdentity();
      glRotatef(spin,0.0,0.0,1.0);
      glMultMatrixf((GLfloat *)M);
      }
   glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)M);
   glPopMatrix();
   }

/* translate by (dx,dy,dz) (multiply with M) */
void OGLtranslateM(matrix *M,float dx,float dy,float dz,int where)
   {
   begin(NONE,NONE);
   glPushMatrix();
   if (where==M_RIGHT)
      {
      glLoadMatrixf((GLfloat *)M);
      glTranslatef(dx,dy,dz);
      }
   else
      {
      glLoadIdentity();
      glTranslatef(dx,dy,dz);
      glMultMatrixf((GLfloat *)M);
      }
   glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)M);
   glPopMatrix();
   }

/* scale by sx/sy/sz (multiply with M) */
void OGLscaleM(matrix *M,float sx,float sy,float sz,int where)
   {
   begin(NONE,NONE);
   glPushMatrix();
   if (where==M_RIGHT)
      {
      glLoadMatrixf((GLfloat *)M);
      glScalef(sx,sy,sz);
      }
   else
      {
      glLoadIdentity();
      glScalef(sx,sy,sz);
      glMultMatrixf((GLfloat *)M);
      }
   glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat *)M);
   glPopMatrix();
   }

/* multiply by M */
void OGLmultiply(matrix *M)
   {
   begin(NONE,NONE);
   glMultMatrixf((GLfloat *)M);
   }

/* push modelview matrix */
void OGLpush()
   {
   begin(NONE,NONE);
   glPushMatrix();
   }

/* pop modelview matrix */
void OGLpop()
   {
   begin(NONE,NONE);
   glPopMatrix();
   }

/* begin display list */
void OGLbeginlist(int id,int pickable)
   {
   if (id<=PICK_NOID) ERROR();
   rendering=FALSE;

   begin(NONE,NONE);
   glNewList(id,GL_COMPILE);
   if (popname=pickable) glPushName(id);
   }

/* end display list */
void OGLendlist()
   {
   begin(NONE,NONE);
   if (popname) glPopName();
   glEndList();

   rendering=TRUE;
   }

/* use display list */
void OGLuselist(int id)
   {
   if (id<=PICK_NOID) ERROR();

   begin(NONE,NONE);
   if (rendering) glEnable(GL_NORMALIZE);
   glCallList(id);
   if (rendering) glDisable(GL_NORMALIZE);
   begin(NONE,NONE);
   }

/* enter pick mode */
void OGLbeginpicking(int x,int y)
   {
   GLint viewport[4];

   begin(NONE,NONE);

   glSelectBuffer(SELECTBUFFERSIZE,selectbuffer);
   glRenderMode(rendermode=GL_SELECT);
   glInitNames();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glGetIntegerv(GL_VIEWPORT,viewport);
   gluPickMatrix(x,y,PICK_SIZE,PICK_SIZE,viewport);
   glMatrixMode(GL_MODELVIEW);
   }

/* leave pick mode */
int OGLendpicking()
   {
   GLuint id=PICK_NOID,hits,*ptr,i,cnt,z=UINT_MAX,j;

   begin(NONE,NONE);

   hits=glRenderMode(rendermode=GL_RENDER);

   ptr=selectbuffer;
   for (i=0; i<hits; i++)
      {
      cnt=*ptr++;
      if (*ptr<z)
         {
         z=*ptr++;
         ptr++;
         for (j=0; j<cnt; j++) id=*ptr++;
         }
      else
         for (j=0; j<cnt+2; j++) ptr++;
      }

   return(id);
   }

/* set ambient&diffuse lighting parameters */
void OGLlight(float dx,float dy,float dz)
   {
   static GLfloat black[4]={BLACK,1.0};
   static GLfloat ambientfraction[4]={0.3,0.3,0.3,1.0};
   static GLfloat diffusefraction[4]={0.7,0.7,0.7,1.0};

   GLfloat direction[4],length;

   if ((length=fsqrt(dx*dx+dy*dy+dz*dz))==0.0) ERROR();
   direction[0]=dx/length;
   direction[1]=dy/length;
   direction[2]=dz/length;
   direction[3]=0.0;

   begin(NONE,NONE);

   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,black);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
   glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambientfraction);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffusefraction);
   glLightfv(GL_LIGHT0,GL_POSITION,direction);

   glEnable(GL_LIGHT0);
   glEnable(GL_COLOR_MATERIAL);
   }

/* set current color */
void OGLsetcolor(float red,float green,float blue)
   {glColor3f(red,green,blue);}

/* load texture map */
void OGLloadtexmap(char *filename,int width,int height,float scale,float offset)
   {
   char *extensions;

   unsigned char *image;

   GLfloat p1[]={0.0,0.0,0.0,0.0};
   GLfloat p2[]={0.0,0.0,0.0,0.0};

   begin(NONE,NONE);

   if ((extensions=(char *)glGetString(GL_EXTENSIONS))==NULL) ERROR();
   if (strstr(extensions,"EXT_texture_object")==NULL) return;

#ifdef IMPACT
   glGenTexturesEXT(1,&tex);
   glBindTextureEXT(GL_TEXTURE_2D,tex);

   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

   image=read_JPEG_file(filename,width,height);
   gluBuild2DMipmaps(GL_TEXTURE_2D,3,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
   free(image);

   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);

   p1[0]=p2[2]=scale;
   p1[3]=p2[3]=offset;
   glTexGenfv(GL_S,GL_OBJECT_PLANE,p1);
   glTexGenfv(GL_T,GL_OBJECT_PLANE,p2);

   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
#endif
   }

/* draw 3D-line */
void OGLdrawline(float x1,float y1,float z1,
                 float x2,float y2,float z2)
   {
   begin(GL_LINES,NONE);
      glVertex3f(x1,y1,z1);
      glVertex3f(x2,y2,z2);
   }

/* draw smooth-shaded 3D-triangle */
void OGLfilltriangle(float x1,float y1,float z1,
                     float nx1,float ny1,float nz1,
                     float r1,float g1,float b1,
                     float x2,float y2,float z2,
                     float nx2,float ny2,float nz2,
                     float r2,float g2,float b2,
                     float x3,float y3,float z3,
                     float nx3,float ny3,float nz3,
                     float r3,float g3,float b3)
   {
   begin(GL_TRIANGLES,GL_LIGHTING);
      glColor3f(r1,g1,b1);
      glNormal3f(nx1,ny1,nz1);
      glVertex3f(x1,y1,z1);
      glColor3f(r2,g2,b2);
      glNormal3f(nx2,ny2,nz2);
      glVertex3f(x2,y2,z2);
      glColor3f(r3,g3,b3);
      glNormal3f(nx3,ny3,nz3);
      glVertex3f(x3,y3,z3);

#ifdef VERBOSE
   tricnt++;
#endif
   }

/* draw textured 3D-triangle */
void OGLtexturetriangle(float x1,float y1,float z1,
                        float x2,float y2,float z2,
                        float x3,float y3,float z3)
   {
   begin(GL_TRIANGLES,GL_TEXTURE_2D);
      glVertex3f(x1,y1,z1);
      glVertex3f(x2,y2,z2);
      glVertex3f(x3,y3,z3);

#ifdef VERBOSE
   tricnt++;
#endif
   }

/* draw flat-shaded 3D-rectangle */
void OGLfillrectangle(float x,float y,float z,
                      float dx1,float dy1,float dz1,
                      float dx2,float dy2,float dz2)
   {
   GLfloat nx,ny,nz,length;

   nx=dy2*dz1-dy1*dz2;
   ny=dz2*dx1-dz1*dx2;
   nz=dx2*dy1-dx1*dy2;
   if ((length=fsqrt(nx*nx+ny*ny+nz*nz))==0.0) ERROR();

   begin(GL_QUADS,GL_LIGHTING);
      glNormal3f(nx/length,ny/length,nz/length);
      glVertex3f(x,y,z);
      glVertex3f(x+dx1,y+dy1,z+dz1);
      glVertex3f(x+dx1+dx2,y+dy1+dy2,z+dz1+dz2);
      glVertex3f(x+dx2,y+dy2,z+dz2);
   }

/* draw textured 3D-rectangle */
void OGLtexturerectangle(float x1,float y1,float z1,
                         float x2,float y2,float z2,
                         float x3,float y3,float z3,
                         float x4,float y4,float z4)
   {
   begin(GL_QUADS,GL_TEXTURE_2D);
      glVertex3f(x1,y1,z1);
      glVertex3f(x2,y2,z2);
      glVertex3f(x3,y3,z3);
      glVertex3f(x4,y4,z4);
   }

/* begin triangle fan */
void OGLbeginfan()
   {
   begin(NONE,GL_TEXTURE_2D);
   begin(GL_TRIANGLE_FAN,GL_TEXTURE_2D);
   }

/* specify a triangle fan vertex */
void OGLfanvertex(float x,float y,float z)
   {
   glVertex3f(x,y,z);

#ifdef VERBOSE
   vtxcnt++;
#endif
   }
