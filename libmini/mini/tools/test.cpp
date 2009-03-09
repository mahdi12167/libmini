// (c) by Stefan Roettger

#define MESHTEST //!!

#undef OPENGLTEST // enable this to perform an OpenGL test

#include <mini/minibase.h>
#include <mini/miniOGL.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#ifdef MESHTEST
#include <mini/minimesh.h>
#include <mini/minibspt.h>
#endif

static int winwidth,winheight,winid;

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

   // add OpenGL test code here:
   // ...

   beginfans();
   color(1,0,0);
   beginfan();
   fanvertex(-5,3,0);
   fanvertex(5,3,0);
   fanvertex(5,0,5);
   fanvertex(-5,0,5);
   endfans();

   // end of test OpenGL code

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

#ifdef MESHTEST

void addhex(miniv3d midpoint,double size,BOOLINT flip,unsigned int slot,unsigned int brickid,minimesh *mesh)
   {
   miniv3d vtx[8];
   miniv3d crd1,crd2;

   crd1=midpoint-size/2.0*miniv3d(1,1,1);
   crd2=midpoint+size/2.0*miniv3d(1,1,1);

   vtx[0]=miniv3d(crd1.x,crd1.y,crd1.z);
   vtx[1]=miniv3d(crd1.x,crd2.y,crd1.z);
   vtx[2]=miniv3d(crd2.x,crd2.y,crd1.z);
   vtx[3]=miniv3d(crd2.x,crd1.y,crd1.z);

   vtx[4]=miniv3d(crd1.x,crd1.y,crd2.z);
   vtx[5]=miniv3d(crd1.x,crd2.y,crd2.z);
   vtx[6]=miniv3d(crd2.x,crd2.y,crd2.z);
   vtx[7]=miniv3d(crd2.x,crd1.y,crd2.z);

   if (!flip)
      {
      mesh->append(minihedron(vtx[0],vtx[1],vtx[3],vtx[4],slot,brickid));
      mesh->append(minihedron(vtx[2],vtx[3],vtx[1],vtx[6],slot,brickid));
      mesh->append(minihedron(vtx[7],vtx[6],vtx[4],vtx[3],slot,brickid));
      mesh->append(minihedron(vtx[5],vtx[4],vtx[6],vtx[1],slot,brickid));
      mesh->append(minihedron(vtx[3],vtx[1],vtx[6],vtx[4],slot,brickid));
      }
   else
      {
      mesh->append(minihedron(vtx[3],vtx[0],vtx[2],vtx[7],slot,brickid));
      mesh->append(minihedron(vtx[1],vtx[2],vtx[0],vtx[5],slot,brickid));
      mesh->append(minihedron(vtx[4],vtx[7],vtx[5],vtx[0],slot,brickid));
      mesh->append(minihedron(vtx[6],vtx[5],vtx[7],vtx[2],slot,brickid));
      mesh->append(minihedron(vtx[0],vtx[5],vtx[2],vtx[7],slot,brickid));
      }
   }

#endif

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

   winwidth=winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("libMini Test");

#ifdef OPENGLTEST
   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(NULL);
#endif

   printf("running test code\n\n");

   // add test code here:
   // ...

#ifndef MESHTEST

   miniOGL::print_unsupported_glexts();
   miniOGL::print_graphics_info();

#else

   minimesh mesh;
   addhex(miniv3d(-1,-1,0),1,TRUE,1,0,&mesh);
   addhex(miniv3d(1,-1,0),1,FALSE,1,0,&mesh);
   addhex(miniv3d(-1,1,0),1,FALSE,1,0,&mesh);
   addhex(miniv3d(1,1,0),1,TRUE,1,0,&mesh);
   std::cout << mesh;

   minibsptree bspt;
   minimesh unsorted;
   bspt.insertbbox(mesh);
   unsorted=bspt.extract();
   std::cout << bspt;
   std::cout << unsorted;

   std::cout << "in=" << mesh.getsize() << " out=" << unsorted.getsize() << std::endl;

#endif

   // end of test code

   printf("\nfinished test code\n");

#ifdef OPENGLTEST
   glutMainLoop();
#endif

   return(0);
   }
