// (c) by Stefan Roettger

#include <mini/minibase.h>

#include <mini/minitime.h>
#include <mini/miniOGL.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

static float winr,wing,winb;
static int winwidth,winheight,winid;
static void (*renderfunc)(double time);

double mygettime()
   {
   static double time0=gettime();
   return(gettime()-time0);
   }

void displayfunc()
   {
   float aspect;
   float x1,y1,x2,y2,dx,dy;

   aspect=(float)winwidth/winheight;

   if (aspect>1.0f)
      {
      x1=0.0f;
      x2=aspect;
      y1=0.0f;
      y2=1.0f;

      dx=(1.0f-aspect)/2.0f;
      dy=0.0f;
      }
   else
      {
      x1=0.0f;
      x2=1.0;
      y1=0.0f;
      y2=1.0f/aspect;

      dx=0.0f;
      dy=(1.0f-1.0f/aspect)/2.0f;
      }

   clearbuffer(winr,wing,winb);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(x1+dx,x2+dx,y1+dy,y2+dy);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   enablelinesmooth();

   color(1.0f,1.0f,1.0f);

   renderfunc(mygettime());

   disablelinesmooth();

   glutSwapBuffers();
   }

void reshapefunc(int width,int height)
   {
   winwidth=width;
   winheight=height;

   glViewport(0,0,width,height);

   displayfunc();
   }

void keyboardfunc(unsigned char key,int x,int y)
   {
   float mousex,mousey;

   mousex=(float)x/(winwidth-1);
   mousey=(float)y/(winwidth-1);

   if (key=='q' || key==27)
      {
      glutDestroyWindow(winid);
      exit(0);
      }
   }

void plot_openwindow(int *argc,char *argv[],
                     int width,int height,
                     float r,float g,float b,
                     void (*render)(double time))
   {
   winwidth=width;
   winheight=height;

   winr=r;
   wing=g;
   winb=b;

   renderfunc=render;

   glutInit(argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("Plotter");

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

   glutMainLoop();
   }

void plot_color(const float r,const float g,const float b)
   {color(r,g,b);}

void plot_linewidth(const int w)
   {linewidth(w);}

void plot_line(const float x1,const float y1,const float x2,const float y2)
   {renderline(x1,y1,0.0,x2,y2,0.0);}
