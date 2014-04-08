// (c) by Stefan Roettger, licensed under LGPL 2.1

#define OPENGL_TEST // enable this to perform an OpenGL test
#define MINICRS_TEST // enable this to perform a test of the minicrs class
#define MINICOORD_TEST // enable this to perform a test of the minicoord class
#define MINISTRING_TEST // enable this to perform a test of the ministring class
#define MINISTRIP_TEST // enable this to perform a test of the ministrip class
#define MINITIME_TEST // enable this to perform a test of the minitime module
#define MINIPATH_TEST // enable this to perform a test of the minipath module
#define MINIXML_TEST // enable this to perform a test of the minixml module
#define GLSLMATH_TEST // enable this to perform a test of the glslmath module

#include <mini/minibase.h>
#include <mini/miniOGL.h>
#include <mini/minicrs.h>
#include <mini/minicoord.h>
#include <mini/ministring.h>
#include <mini/ministrip.h>
#include <mini/minitime.h>
#include <mini/minipath.h>
#include <mini/minixml.h>
#include <mini/glslmath.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
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

   if (mousex<0.0f) mousex=0.0f;
   else if (mousex>1.0f) mousex=1.0f;

   if (mousey<0.0f) mousey=0.0f;
   else if (mousey>1.0f) mousey=1.0f;

   if (key=='q' || key==27)
      {
      glutDestroyWindow(winid);
      exit(0);
      }
   }

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: %s\n",argv[0]);
      exit(1);
      }

#ifdef OPENGL_TEST
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
#endif

   printf("running test code\n\n");

   // add test code here:
   // ...

#ifdef OPENGL_TEST
   std::cout << "miniOGL:" << std::endl;
   miniOGL::print_unsupported_glexts();
   miniOGL::print_graphics_info();
   if (get_unsupported_glexts()==0) std::cout << "OPENGL SUCCESS" << std::endl;
   else std::cout << "OPENGL FAILURE" << std::endl;
#endif

#ifdef MINICRS_TEST
   std::cout << "minicrs:" << std::endl;
   int z0=4;
   double lon=minicrs::UTMZ2L(z0);
   std::cout << z0 << " -> " << lon/3600 << std::endl;
   int z=minicrs::LL2UTMZ(0,lon);
   if (z==z0) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINICOORD_TEST
   std::cout << "minicoord:" << std::endl;
   minicoord c1(miniv4d(11*3600,49*3600,0),minicoord::MINICOORD_LLH,0,minicoord::MINICOORD_DATUM_WGS84);
   std::cout << c1 << std::endl;
   minicoord c2=c1;
   c2.convert2(minicoord::MINICOORD_UTM);
   std::cout << " " << c2 << std::endl;
   std::cout << " " << c2.to_string() << std::endl;
   ministring info=c2.to_string();
   minicoord c3;
   c3.from_string(info);
   std::cout << " " << c3 << std::endl;
   c3.convert2llh();
   std::cout << c3 << std::endl;
   c3-=c1;
   if (c3.vec.getlength()<1e-5) c3.vec=miniv3d(0,0,0);
   c3+=c1;
   if (c3==c1) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;

   std::cout << "minicoord:" << std::endl;
   minicoord u(miniv4d(500000,0,0),minicoord::MINICOORD_UTM,4,minicoord::MINICOORD_DATUM_WGS84);
   std::cout << u << std::endl;
   u.convert2(minicoord::MINICOORD_LLH);
   std::cout << " " << u << std::endl;
   u.convert2(minicoord::MINICOORD_UTM);
   std::cout << " " << u << std::endl;
   if (u.vec.x==500000) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINISTRING_TEST
   std::cout << "ministring:" << std::endl;
   ministring test1("this is a simple test string");
   ministring test2("this is a s1mple test string");
   ministring check1=test1.checksum(4);
   ministring check2=test2.checksum(4);
   std::cout << test1 << " -> " << check1 << std::endl;
   std::cout << test2 << " -> " << check2 << std::endl;
   unsigned int count=0;
   for (unsigned int i=0; i<check1.getsize(); i++) if (check1[i]!=check2[i]) count++;
   if (count==1) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINISTRIP_TEST
   ministrip strip1(3,3);
   strip1.setcol(1,0,0);
   strip1.setnrm(0,0,-1);
   strip1.beginstrip();
   strip1.addvtx(0,0,0);
   strip1.addvtx(1,0,0);
   strip1.addvtx(0,1,0);
   strip1.addvtx(1,1,0);
   strip1.beginstrip();
   strip1.addvtx(0,0,0);
   strip1.addvtx(-1,0,0);
   strip1.addvtx(0,1,0);
   strip1.addvtx(-1,1,0);
   int size1=strip1.getsize();
   ministring strip=strip1.to_string();
   unsigned int before=strip.size();
   miniv3d bboxmin1,bboxmax1;
   strip1.getbbox(bboxmin1,bboxmax1);
   std::cout << "ministrip: size1=" << size1 << " before=" << before << std::endl;
   std::cout << "ministrip: bbox1=" << bboxmin1 << "->" << bboxmax1 << std::endl;
   std::cout << "ministrip: " << strip << std::endl;
   ministrip strip2;
   strip2.from_string(strip);
   int size2=strip2.getsize();
   unsigned int after=strip.size();
   miniv3d bboxmin2,bboxmax2;
   strip2.getbbox(bboxmin2,bboxmax2);
   std::cout << "ministrip: size2=" << size2 << " after=" << after << std::endl;
   std::cout << "ministrip: bbox2=" << bboxmin2 << "->" << bboxmax2 << std::endl;
   if (size1==10 && size2==10 && before==453 && after==0 &&
       bboxmin1==bboxmin2 && bboxmax1==bboxmax2)
      std::cout << "SUCCESS" << std::endl;
   else
      std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINITIME_TEST
   double s1971=utc2minitime(1971,7,2,14,34,0,0);
   double s1970=utc2minitime(1970,1,1,0,0,0,0);
   double s1969=utc2minitime(1969,7,1,0,0,0,700);
   double s2014=utc2minitime(2014,3,19,15,18,14,0);
   ministring utc("2014-03-13T10:33:09.876Z");
   std::cout << "minitime: s1971=" << s1971 << std::endl;
   std::cout << "minitime: s1970=" << s1970 << std::endl;
   double sutc=utc2unixtime(2014,3,13,10,33,9,876);
   double utcd=utc2unixtime(utc)-sutc;
   if ((long long)(s1971+0.5)==0 &&
       (long long)(s1970+0.5)==-47313239 &&
       (long long)(s1969+0.5)==-63210838 &&
       (long long)(s2014+0.5)==1346978654 &&
       utcd==0.0)
      std::cout << "SUCCESS" << std::endl;
   else
      std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINIPATH_TEST
   minicoord point1(miniv4d(11*3600,49*3600,0),minicoord::MINICOORD_LLH,0,minicoord::MINICOORD_DATUM_WGS84);
   minicoord point2(miniv4d(500000,0,0),minicoord::MINICOORD_UTM,4,minicoord::MINICOORD_DATUM_WGS84);
   minipath path;
   path.append(point1);
   path.append(point2);
   double length=path.get_length();
   std::cout << "minipath: length=" << length << std::endl;
   if (dabs(length-1.15704e+07)<1E1) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

#ifdef MINIXML_TEST
   ministrings txt;
   txt.from_string("<tag><subtag></subtag></tag> \n <anothertag> < / anothertag >");
   minixml xml;
   xml.from_strings(txt);
   if (txt.empty()) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

#ifdef GLSLMATH_TEST
   std::cout << "glslmath:" << std::endl;
   if (test_glslmath()==0) std::cout << "SUCCESS" << std::endl;
   else std::cout << "FAILURE" << std::endl;
#endif

   // end of test code

   printf("\nfinished test code\n");

#ifdef OPENGLTEST
   glutMainLoop();
#endif

   return(0);
   }
