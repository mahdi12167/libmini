// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>
#include <mini/minitime.h>
#include <mini/miniOGL.h>

#include <mini/database.h>
#include <mini/miniimg.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

int winsize=512;
float fps=25.0f;

unsigned int frame=0;
BOOLINT pausing=TRUE;

databuf buf;

unsigned char *volume;
unsigned int width,height,depth;

unsigned char *image;
unsigned int imgwidth,imgheight;

static int winwidth,winheight,winid;

void displayfunc()
   {
   unsigned int i;

   GLuint texid;

   clearbuffer(0,0,0);

   for (i=0; i<height; i++)
      memcpy(&image[i*3*imgwidth],&volume[frame*3*width*height+i*3*width],3*width);

   glGenTextures(1,&texid);
   glBindTexture(GL_TEXTURE_2D,texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,imgwidth,imgheight,0,GL_RGB,GL_UNSIGNED_BYTE,image);

   glDepthMask(GL_FALSE);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_DITHER);
   glDisable(GL_CULL_FACE);

   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glLoadIdentity();
   glTranslatef(0.5f/imgwidth,0.5f/imgheight,0.0f);
   glScalef((float)(width-1)/imgwidth,(float)(height-1)/imgheight,0.0f);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0.0f,winwidth-1,0.0f,winheight-1,-1.0f,1.0f);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   if (width*winheight<winwidth*height)
      {
      glTranslatef((winwidth-width*winheight/height)/2.0f,0.0f,0.0f);
      glScalef(width*winheight/height,winheight,0.0f);
      }
   else
      {
      glTranslatef(0.0f,(winheight-height*winwidth/width)/2.0f,0.0f);
      glScalef(winwidth,height*winwidth/width,0.0f);
      }

   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

   glEnable(GL_TEXTURE_2D);

   glColor3f(1.0f,1.0f,1.0f);
   glBegin(GL_TRIANGLE_FAN);
   glTexCoord2f(0.0f,1.0f);
   glVertex3f(0.0f,0.0f,0.0f);
   glTexCoord2f(1.0f,1.0f);
   glVertex3f(1.0f,0.0f,0.0f);
   glTexCoord2f(1.0f,0.0f);
   glVertex3f(1.0f,1.0f,0.0f);
   glTexCoord2f(0.0f,0.0f);
   glVertex3f(0.0f,1.0f,0.0f);
   glEnd();

   glDisable(GL_TEXTURE_2D);
   glDeleteTextures(1,&texid);

   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_DITHER);
   glEnable(GL_CULL_FACE);

   glMatrixMode(GL_TEXTURE);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glutSwapBuffers();
   waitfor(1.0/fps);

   if (!pausing)
      if (frame<depth-1) frame++;
      else frame=0;

   if (frame==depth-1) pausing=TRUE;
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

   switch (key)
      {
      case ' ':
         pausing=!pausing;
         break;
      case 'b':
         pausing=TRUE;
         frame=0;
         break;
      case 'e':
         pausing=TRUE;
         frame=depth-1;
         break;
      case '-':
      case '<':
      case '1':
         pausing=TRUE;
         if (frame>0) frame--;
         break;
      case '+':
      case '=':
      case '>':
      case '2':
         pausing=TRUE;
         if (frame<depth-1) frame++;
         break;
      case '\033':
      case 'q':
         free(image);
         buf.release();
         glutDestroyWindow(winid);
         exit(0);
      }
   }

int main(int argc,char *argv[])
   {
   if (argc!=2)
      {
      printf("usage: %s <image.ext>\n",argv[0]);
      printf(" dump images with ext = db/pnm/pgm/ppm/pvm/jpg/png\n");
      exit(1);
      }

   if (miniimg::loadimg(buf,argv[1])!=0)
      {
      buf.drop_mipmap();
      buf.autodecompress();

      if (buf.type==databuf::DATABUF_TYPE_SHORT || buf.type==databuf::DATABUF_TYPE_FLOAT)
         buf.convertdata(databuf::DATABUF_TYPE_BYTE);

      if (buf.type==databuf::DATABUF_TYPE_BYTE)
         buf.convertdata(databuf::DATABUF_TYPE_RGB);

      if (buf.type==databuf::DATABUF_TYPE_RGBA)
         buf.convertdata(databuf::DATABUF_TYPE_RGB);

      volume=(unsigned char *)buf.data;
      width=buf.xsize;
      height=buf.ysize;
      depth=buf.zsize;

      for (imgwidth=2; imgwidth<width; imgwidth*=2);
      for (imgheight=2; imgheight<height; imgheight*=2);

      if ((image=(unsigned char *)malloc(3*imgwidth*imgheight))==NULL) exit(1);
      memset(image,0,3*imgwidth*imgheight);

      winwidth=winheight=winsize;

      glutInit(&argc,argv);
      glutInitWindowSize(winwidth,winheight);
      glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
      winid=glutCreateWindow("dbdump");

      glutDisplayFunc(displayfunc);
      glutReshapeFunc(reshapefunc);
      glutMouseFunc(NULL);
      glutMotionFunc(NULL);
      glutKeyboardFunc(keyboardfunc);
      glutSpecialFunc(NULL);
      glutIdleFunc(displayfunc);

      glutMainLoop();
      }

   return(0);
   }
