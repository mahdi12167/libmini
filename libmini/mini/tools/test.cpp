// (c) by Stefan Roettger

#undef MESHTEST
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
#include <mini/minicoord.h>
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

void addhex(miniv3d crd1,miniv3d crd2,
            minicoord crd,BOOLINT flip,
            unsigned int slot,unsigned int brickid,
            minimesh *mesh)
   {
   int i;

   minicoord vtx[8];

   for (i=0; i<8; i++) vtx[i]=crd;

   vtx[0].vec+=miniv4d(crd1.x,crd1.y,crd1.z);
   vtx[1].vec+=miniv4d(crd1.x,crd2.y,crd1.z);
   vtx[2].vec+=miniv4d(crd2.x,crd2.y,crd1.z);
   vtx[3].vec+=miniv4d(crd2.x,crd1.y,crd1.z);

   vtx[4].vec+=miniv4d(crd1.x,crd1.y,crd2.z);
   vtx[5].vec+=miniv4d(crd1.x,crd2.y,crd2.z);
   vtx[6].vec+=miniv4d(crd2.x,crd2.y,crd2.z);
   vtx[7].vec+=miniv4d(crd2.x,crd1.y,crd2.z);

#if 1
   for (i=0; i<8; i++) vtx[i].convert2(minicoord::MINICOORD_ECEF);
#endif

#if 0
   for (i=0; i<8; i++) vtx[i].vec-=miniv3d(-5.50079e+06,-2.2524e+06,2.30771e+06);
#endif

   if (!flip)
      {
      mesh->append(minihedron(vtx[0].vec,vtx[1].vec,vtx[3].vec,vtx[4].vec,slot,brickid));
      mesh->append(minihedron(vtx[2].vec,vtx[3].vec,vtx[1].vec,vtx[6].vec,slot,brickid));
      mesh->append(minihedron(vtx[7].vec,vtx[6].vec,vtx[4].vec,vtx[3].vec,slot,brickid));
      mesh->append(minihedron(vtx[5].vec,vtx[4].vec,vtx[6].vec,vtx[1].vec,slot,brickid));
      mesh->append(minihedron(vtx[3].vec,vtx[1].vec,vtx[6].vec,vtx[4].vec,slot,brickid));
      }
   else
      {
      mesh->append(minihedron(vtx[3].vec,vtx[0].vec,vtx[2].vec,vtx[7].vec,slot,brickid));
      mesh->append(minihedron(vtx[1].vec,vtx[2].vec,vtx[0].vec,vtx[5].vec,slot,brickid));
      mesh->append(minihedron(vtx[4].vec,vtx[7].vec,vtx[5].vec,vtx[0].vec,slot,brickid));
      mesh->append(minihedron(vtx[6].vec,vtx[5].vec,vtx[7].vec,vtx[2].vec,slot,brickid));
      mesh->append(minihedron(vtx[0].vec,vtx[5].vec,vtx[2].vec,vtx[7].vec,slot,brickid));
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
   double gfnx=631433.0;
   double gfny=2361214.0;
   double gfnh=1000.0;
   int gfnz=4;
   double brick=1.0;
   minicoord crd(gfnx,gfny,gfnh,minicoord::MINICOORD_UTM,gfnz,minicoord::MINICOORD_DATUM_WGS84);
   addhex(miniv3d(-1.0,-1.0,-0.5)*brick,miniv3d(0.0,0.0,0.5)*brick,crd,TRUE,1,0,&mesh);
   addhex(miniv3d(0.0,-1.0,-0.5)*brick,miniv3d(1.0,0.0,0.5)*brick,crd,FALSE,1,1,&mesh);
   addhex(miniv3d(-1.0,0.0,-0.5)*brick,miniv3d(0.0,1.0,0.5)*brick,crd,FALSE,1,2,&mesh);
   addhex(miniv3d(0.0,0.0,-0.5)*brick,miniv3d(1.0,1.0,0.5)*brick,crd,TRUE,1,3,&mesh);

   minibsptree bspt;
   minimesh unsorted;
   bspt.insert(mesh);
   unsorted=bspt.extract();

   std::cout << bspt << std::endl;
   std::cout << unsorted << std::endl;

   std::cout << "in=" << mesh.getsize() << " out=" << unsorted.getsize() << std::endl;
   std::cout << "vol_in=" << mesh.getvolume() << " vol_out=" << unsorted.getvolume() << std::endl;

#endif

   // end of test code

   printf("\nfinished test code\n");

#ifdef OPENGLTEST
   glutMainLoop();
#endif

   return(0);
   }
