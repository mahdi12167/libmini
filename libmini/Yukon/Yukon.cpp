// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef YUKON_DATA
#define YUKON_DATA "data"
#endif

#include <mini/minibase.h>
#include <mini/minitime.h>
#include <mini/minitile.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

static int winwidth,winheight,winid;

static minitile *terrain,*layer;

static int rotmode=0;
static int posmode=0;
static int texmode=0;
static int fogmode=1;
static int skymode=1;

void displayfunc()
   {
   double time;

   const float fps=25.0f;
   const float res=1.0E4f;

   const float ex1=0.0f,ey1=250.0f,ez1=750.0f;
   const float ex2=0.0f,ey2=75.0f,ez2=500.0f;
   const float dx=0.0f,dy=-1.0f,dz=-2.0f;
   const float ux=0.0f,uy=1.0f,uz=0.0f;

   const float fovy=60.0f,nearp=10.0f,farp=10000.0f;

   float ex,ey,ez;

   static float ttime=0.0f;
   float sinr,cosr,exr,ezr,dxr,dzr;
   const float rps=0.05f;

   if (posmode==0) {ex=ex1; ey=ey1; ez=ez1;}
   else {ex=ex2; ey=ey2; ez=ez2;}

   time=gettime();

   sinr=sin(2.0f*rps*ttime*PI);
   cosr=cos(2.0f*rps*ttime*PI);

   exr=ex*sinr+ez*cosr;
   ezr=ex*cosr-ez*sinr;
   dxr=dx*sinr+dz*cosr;
   dzr=dx*cosr-dz*sinr;

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fovy,(float)winwidth/winheight,nearp,farp);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(exr,ey,ezr,exr+dxr,ey+dy,ezr+dzr,ux,uy,uz);

   terrain->draw(res,
                 exr,ey,ezr,
                 dxr,dy,dzr,
                 ux,uy,uz,
                 fovy,(float)winwidth/winheight,
                 nearp,farp);

   if (skymode!=0)
      layer->draw(res/8.0f,
                  exr,ey,ezr,
                  dxr,dy,dzr,
                  ux,uy,uz,
                  fovy,(float)winwidth/winheight,
                  nearp,farp);

   glutSwapBuffers();

   while (gettime()-time<1.0f/fps);
   if (rotmode==0) ttime+=gettime()-time;
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
   static int wire=0;

   float mousex,mousey;

   mousex=(float)x/(winwidth-1);
   mousey=(float)y/(winwidth-1);

   if (key=='w')
      {
      if (wire==0) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      wire=1-wire;
      }
   else if (key=='p') posmode=1-posmode;
   else if (key=='r' || key==' ') rotmode=1-rotmode;
   else if (key=='t')
      {
      texmode=1-texmode;
      terrain->settexmode(texmode);
      }
   else if (key=='f')
      {
      fogmode=(fogmode+1)%3;
      terrain->setfogmode(fogmode);
      }
   else if (key=='s')
      {
      skymode=(skymode+1)%3;
      layer->setfogmode(skymode);
      }
   else if (key=='q' || key==27)
      {
      delete terrain;
      delete layer;

      glutDestroyWindow(winid);
      exit(0);
      }
   }

int main(int argc,char *argv[])
   {
   const unsigned char hfield[]=  YUKON_DATA "/Yukon.map.pgm";
   const unsigned char texture[]= YUKON_DATA "/Yukon.texmap.ppm";
   const unsigned char fogmap[]=  YUKON_DATA "/Yukon.fogmap.pgm";

   const unsigned char *hfields=hfield;
   const unsigned char *textures=texture;
   const unsigned char *fogmaps=fogmap;

   const int cols=1;
   const int rows=1;

   const float xdim=1000.0f;
   const float zdim=1000.0f;

   const float scale=0.33f;

   const float lambda=0.15f;
   const float displace=1.0f;

   const float emission=0.01f;
   const float attenuation=0.33f;

   const float fogR=0.75f;
   const float fogG=0.75f;
   const float fogB=1.0f;

   const unsigned char layer1[]= YUKON_DATA "/Yukon.fogmap1.pgm";
   const unsigned char layer2[]= YUKON_DATA "/Yukon.fogmap2.pgm";

   const unsigned char *layers1=layer1;
   const unsigned char *layers2=layer2;

   const float lambda1=0.15f;
   const float lambda2=0.4f;
   const float displace2=150.0f;

   const float emission2=0.0025f;
   const float attenuation2=0.5f;

   const float fogR2=0.7f;
   const float fogG2=0.85f;
   const float fogB2=1.0f;

   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("Yukon Demo");

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

   terrain=new minitile(&hfields,&textures,cols,rows,xdim,zdim,scale,0.0f,0.0f,0.0f,
                        &fogmaps,lambda,displace,emission,
                        attenuation,fogR,fogG,fogB);

   layer=new minitile(&layers1,0,cols,rows,xdim,zdim,lambda1,0.0f,0.0f,0.0f,
                      &layers2,lambda2,displace2,emission2,
                      attenuation2,fogR2,fogG2,fogB2);

   terrain->setfogmode(fogmode);
   layer->setfogmode(skymode);

   glutMainLoop();

   return(0);
   }
