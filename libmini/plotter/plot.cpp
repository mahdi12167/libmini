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
static float winfps;

static void (*renderfunc)(double time);
static BOOLINT (*keyfunc)(unsigned char key,float x,float y);

static float px=0.0f,py=0.0f;

double mygettime()
   {
   static double time0=gettime();
   return(gettime()-time0);
   }

void displayfunc()
   {
   float aspect;
   float x1,y1,x2,y2,dx,dy;

   double t;

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

   t=gettime();

   clearbuffer(winr,wing,winb);

   initstate();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(x1+dx,x2+dx,y1+dy,y2+dy);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   enablelinesmooth();

   color(0.0f,0.0f,0.0f);

   renderfunc(mygettime());

   disablelinesmooth();

   exitstate();

   glutSwapBuffers();

   t=gettime()-t;

   waitfor(1.0/winfps-t);
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

   if (tolower(key)=='q' || key==27)
      {
      glutDestroyWindow(winid);
      exit(0);
      }

   if (keyfunc!=NULL)
      if (keyfunc(key,mousex,mousey)) displayfunc();
   }

// open a window for 2D plotting
void plot_openwindow(int *argc,char *argv[],
                     int width,int height,
                     float r,float g,float b,
                     void (*render)(double time),
                     BOOLINT (*keypress)(unsigned char key,float x,float y),
                     BOOLINT continuous,
                     float fps)
   {
   winwidth=width;
   winheight=height;

   winr=r;
   wing=g;
   winb=b;

   winfps=fps;

   renderfunc=render;
   keyfunc=keypress;

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
   glutIdleFunc(continuous?displayfunc:NULL);

   glutMainLoop();
   }

// get window width
int get_winwidth()
   {return(winwidth);}

// get window height
int get_winheight()
   {return(winheight);}

// set plot line color
void plot_color(const float r,const float g,const float b)
   {color(r,g,b);}

// set plot line width
void plot_linewidth(const int w)
   {linewidth(w);}

// plot line from (x1,y1) to (x2,y2)
void plot_line(const float x1,const float y1,const float x2,const float y2)
   {
   renderline(x1,y1,0.0f,x2,y2,0.0f);

   px=x2;
   py=y2;
   }

// plot line from actual position (x,y)
void plot_from(const float x,const float y)
   {
   px=x;
   py=y;
   }

// plot line from last position to (x,y)
void plot_to(const float x,const float y)
   {plot_line(px,py,x,y);}

// plot line to last position plus delta
void plot_delta(const float dx,const float dy)
   {plot_line(px,py,px+dx,py+dy);}

// plot point
void plot_point(const float x,const float y)
   {renderpoint(x,y,0.0f);}

// plot circle
void plot_circle(float x,float y,float r)
   {
   int i;

   static const int segments=30;

   plot_from(x,y+r);

   for (i=1; i<=30; i++)
      plot_to(x+r*sin(i*2*PI/segments),
              y+r*cos(i*2*PI/segments));
   }
