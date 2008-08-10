#include "mini/minibase.h"
#include "mini/minitime.h"
#include "mini/minitile.h"
#include "mini/pnmbase.h"

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

static int winwidth,winheight,winid;

static minitile *terrain;

static FILE *flyfile;

static int freeze=0;

void displayfunc()
   {
   double time;

   static float t,res,
                ex,ey,ez,
                dx,dy,dz,
                ux,uy,uz;

   static int getonce=0;

   const float fovy=60.0f,nearp=10.0f,farp=10000.0f;

   const float fps=15.0f;

   time=minigettime();

   if (freeze==0 || getonce++==0)
      if (fscanf(flyfile,"%gs: res=%g ex=%g ey=%g ez=%g dx=%g dy=%g dz=%g ux=%g uy=%g uz=%g\n",
                 &t,&res,&ex,&ey,&ez,&dx,&dy,&dz,&ux,&uy,&uz)!=11)
         if ((flyfile=fopen("data/Stuttgart.fly","rb"))==NULL) exit(1);

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fovy,(float)winwidth/winheight,nearp,farp);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(ex,ey,ez,ex+dx,ey+dy,ez+dz,ux,uy,uz);

   terrain->draw(res/4.0f,
                 ex,ey,ez,
                 dx,dy,dz,
                 ux,uy,uz,
                 fovy,(float)winwidth/winheight,
                 nearp,farp);

   glutSwapBuffers();

   while (minigettime()-time<1.0f/fps);
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
   else if (key=='f' || key==' ') freeze=1-freeze;
   else if (key=='q' || key==27)
      {
      delete terrain;

      fclose(flyfile);
      glutDestroyWindow(winid);
      exit(0);
      }
   }

int main(int argc,char *argv[])
   {
   int i;

   FILE *file;
   unsigned char *data,*ptr;

   int width,height,components;

   const unsigned char **hfields,**textures;

   const int incoming=17164776;

   const int cols=8;
   const int rows=8;

   const float xdim=256*0.75f;
   const float zdim=256*0.75f;

   const float scale=0.3f;

   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("Stuttgart Demo");

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

   if ((hfields=(const unsigned char **)malloc(cols*rows*sizeof(unsigned char *)))==NULL) exit(1);
   if ((textures=(const unsigned char **)malloc(cols*rows*sizeof(unsigned char *)))==NULL) exit(1);

   if ((file=fopen("data/Stuttgart.raw","rb"))==NULL) exit(1);
   if ((data=(unsigned char *)malloc(incoming))==NULL) exit(1);
   if (fread(data,incoming,1,file)!=1) exit(1);
   fclose(file);

   ptr=data;
   for (i=0; i<cols*rows; i++)
      {
      hfields[i]=ptr;
      readPNMimage(ptr,&width,&height,&components,1,&ptr);
      }
   for (i=0; i<cols*rows; i++)
      {
      textures[i]=ptr;
      readPNMimage(ptr,&width,&height,&components,1,&ptr);
      }

   terrain=new minitile(hfields,textures,cols,rows,xdim,zdim,scale,
                        0.0f,0.0f,0.0f,0,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1);

   free(hfields);
   free(textures);
   free(data);

   if ((flyfile=fopen("data/Stuttgart.fly","rb"))==NULL) exit(1);

   glutMainLoop();

   return(0);
   }
