// (c) by Stefan Roettger

#include <mini/minibase.h>

// add include files here:
// ...

#include <mini/miniOGL.h>
#include <mini/database.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

static int winwidth,winheight,winid;

static int texid;

void init3Dtex()
   {
   databuf buf;

   int max3Dtexsize;
   int width,height,depth;

   buf.loadPVMdata("test.pvm",0,0,0,1,1,1);

   max3Dtexsize=getmax3Dtexsize();
   buf.resampledata(max3Dtexsize,max3Dtexsize,max3Dtexsize);

   width=buf.xsize;
   height=buf.ysize;
   depth=buf.zsize;

   texid=build3Dtexmap((unsigned char *)buf.data,&width,&height,&depth,1);
   buf.release();
   }

void exit3Dtex()
   {deletetexmap(texid);}

void displayfunc()
   {
   float ex=0.0f,ey=10.0f,ez=30.0f;
   float dx=0.0f,dy=-0.25f,dz=-1.0f;
   float ux=0.0f,uy=1.0f,uz=0.0f;
   float fovy=60.0f;
   float aspect=1.0f;
   float nearp=1.0f;
   float farp=100.0f;

   aspect=(float)winwidth/winheight;

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fovy,aspect,nearp,farp);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(ex,ey,ez,ex+dx,ey+dy,ez+dz,ux,uy,uz);

   bind3Dtexmap(texid);

   beginfans();
   color(1,0,0);
   beginfan();
   texcoord(0,0,0);
   fanvertex(-5,3,0);
   texcoord(1,1,0);
   fanvertex(5,3,0);
   texcoord(1,1,1);
   fanvertex(5,1,5);
   texcoord(0,0,1);
   fanvertex(-5,1,5);
   endfans();

   bind3Dtexmap(0);

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
      exit3Dtex();
      glutDestroyWindow(winid);
      exit(0);
      }
   }

// end of include files

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

   printf("running test code\n\n");

   // add test code here:
   // ...

   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("libMini Test");

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(NULL);

   init3Dtex();

   glutMainLoop();

   // end of test code

   printf("\nfinished test code\n");

   return(0);
   }
