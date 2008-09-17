// (c) by Stefan Roettger

#ifndef MINISKY_H
#define MINISKY_H

#include "ministrip.h"

class minisky
   {
   public:

   //! default constructor
   minisky();

   //! destructor
   ~minisky();

   void loadskydome(const char *skyfile,
                    float mx=0.0f,float my=0.0f,float mz=0.0f,
                    float diameter=1.0f,
                    float scaley=1.0f);

   void setpos(float mx,float my,float mz,
               float diameter,
               float scaley=1.0f);

   void drawskydome();

   protected:

   private:

   ministrip *STRIP;
   int TEXID;

   int LOADED;

   int ALPHA_STEPS;
   int BETA_STEPS;

   float CENTERX,CENTERY,CENTERZ;
   float RADIUS,SCALEY;

   inline float mapalpha(float alpha,int texsize);
   inline float mapbeta(float beta,int texsize);

   inline void addvertex(float alpha,float beta,int width,int height);

   void createskydome(int width,int height);
   };

#endif
