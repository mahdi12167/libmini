// (c) by Stefan Roettger

#ifndef PANORNDR_H
#define PANORNDR_H

#include <mini/minipoint.h>
#include <mini/ministrip.h>

// test grid renderer
class minipointrndr_panorndr: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_panorndr();

   //! destructor
   ~minipointrndr_panorndr();

   void init(minipoint *points,
             float ex,float ey,float ez,
             float dx,float dy,float dz,
             float nearp,float farp,float fovy,float aspect,
             double time,minipointopts *global);

   void pre(int pass);
   void render(minipointdata *vpoint,int pass);
   void post(int pass);

   void exit();

   protected:

   ministrip *STRIP;
   int SLOT;

   void create_sphere(float radius,
                      const float r,const float g,const float b,const float a,
                      int stripes);

   private:

   float EX,EY,EZ;
   minipointopts *GLOBAL;
   float SCALEELEV;
   };

#endif
