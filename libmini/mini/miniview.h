// (c) by Stefan Roettger

#ifndef MINIVIEW_H
#define MINIVIEW_H

#ifndef NOVIEWER

#include "miniscene.h"

class minicam;

class miniview : public miniscene
   {
   public:

   miniview();
   virtual ~miniview();

   void set_camera(minicam *cam);
   void render_geometry(double sbase=0.0,BOOLINT anaglyph=TRUE);

   protected:

   void setup_matrix(double sbase=0.0);

   virtual void render_ecef_geometry();

   minicam *m_camera;
   };

#endif

#endif
