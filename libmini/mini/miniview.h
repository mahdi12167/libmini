// (c) by Stefan Roettger

#ifndef MINIVIEW_H
#define MINIVIEW_H

#include "miniscene.h"

class minicam;

class miniview : public miniscene
   {
   public:

   miniview();
   virtual ~miniview();

   void set_camera(minicam *cam);
   void render_geometry(float sbase=0.0f,BOOLINT anaglyph=TRUE);

   protected:

   void setup_matrix(float sbase=0.0f);

   virtual void render_ecef_geometry(double t);

   minicam *m_cam;
   };

#endif
