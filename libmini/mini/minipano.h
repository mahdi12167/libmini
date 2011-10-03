// (c) by Stefan Roettger

#ifndef MINIPANO_H
#define MINIPANO_H

#include <mini/minipoint.h>
#include <mini/ministrip.h>

// panoramic addon for the minipoint renderer
class minipointrndr_pano: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_pano();

   //! destructor
   ~minipointrndr_pano();

   BOOLINT init(minipoint *points,
                float ex,float ey,float ez,
                float dx,float dy,float dz,
                float nearp,float farp,float fovy,float aspect,
                double time,minipointopts *global,
                BOOLINT usewarp);

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

   minipoint *POINTS;

   float EX,EY,EZ;
   minipointopts *GLOBAL;
   float SCALEELEV;
   };

#endif
