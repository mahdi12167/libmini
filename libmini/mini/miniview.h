// (c) by Stefan Roettger

#ifndef MINIVIEW_H
#define MINIVIEW_H

#include "miniscene.h"

class minicam;

//! base class for viewing tilesets plus ecef geometry
class miniview : public miniscene
   {
   public:

   miniview();
   virtual ~miniview();

   void set_camera(minicam *cam);
   void propagate();

   void render_geometry(float sbase=0.0f, BOOLINT anaglyph=TRUE);

   double shoot(const minicoord &o,const miniv3d &d,double hitdist=0.0);

   protected:

   void setup_matrix(float sbase=0.0f);

   virtual void check_ecef_geometry(miniv3d &center, double &radius);
   virtual void render_ecef_geometry(double t);
   virtual double shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double hitdist=0.0);

   minicam *m_cam;
   };

#endif
