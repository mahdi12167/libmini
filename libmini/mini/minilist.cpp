// (c) by Stefan Roettger

#include "miniOGL.h"

#include "minilist.h"

// default constructor
minilist::minilist() {}

// destructor
minilist::~minilist()
   {
   unsigned int i;

   for (i=0; i<LIST.getsize(); i++)
      if (LIST[i]!=0) glDeleteLists(LIST[i],1);
   }

// set number of display lists
void minilist::setnum(unsigned int num)
   {
   unsigned int i;

   for (i=num; i<LIST.getsize(); i++)
      if (LIST[i]!=0) glDeleteLists(LIST[i],1);

   LIST.growsize(num,0);
   }

// start compiling a display list
void minilist::start(unsigned int n)
   {
   if (LIST[n]==0) LIST[n]=glGenLists(1);
   glNewList(LIST[n],GL_COMPILE);
   }

// add triangle fans to a display list
void minilist::addtrianglefans(float *array,int num,int stride)
   {
   int i,j,k;

   float v1x,v1y,v1z;
   float v2x,v2y,v2z;

   glBegin(GL_TRIANGLE_STRIP);

   for (i=0; i<num; i++)
      {
      k=ftrc(*array+0.5f);

      array+=3;

      v1x=*array++;
      v1y=*array++;
      v1z=*array++;

      array+=stride;

      if (i>0) glVertex3f(v2x,v2y,v2z);

      v2x=*array++;
      v2y=*array++;
      v2z=*array++;

      array+=stride;

      if (i>0) glVertex3f(v2x,v2y,v2z);

      for (j=2; j<k; j++)
         {
         if ((j%2)==0)
            {
            glVertex3f(v2x,v2y,v2z);
            glVertex3f(v1x,v1y,v1z);
            }

         v2x=*array++;
         v2y=*array++;
         v2z=*array++;

         array+=stride;

         glVertex3f(v2x,v2y,v2z);
         }
      }

   glEnd();
   }

// stop compiling a display list
void minilist::stop()
   {glEndList();}

// render a display list
void minilist::render(unsigned int n)
   {if (LIST[n]!=0) glCallList(LIST[n]);}
