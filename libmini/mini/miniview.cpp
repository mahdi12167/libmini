// (c) by Stefan Roettger

#ifndef NOVIEWER

#include "miniOGL.h"
#include "miniglobe.h"
#include "miniearth.h"
#include "minicam.h"

#include "miniview.h"

miniview::miniview() : miniscene()
   {m_camera = NULL;}

miniview::~miniview()
   {}

// tell actual camera
void miniview::set_camera(minicam *cam)
   {m_camera = cam;}

// render earth and terrain geometry
void miniview::render_geometry()
   {
   minilayer *nst;

   if (m_camera == NULL) return;

   // set reference layer
   nst=getearth()->getnearest(m_camera->get_eye());
   getearth()->setreference(nst);

   // render scene
   setup_matrix();
   render_terrain_geometry();
   }

// setup OpenGL modelview and projection matrices
void miniview::setup_matrix()
   {
   mtxproj();
   mtxid();

   float fovy = get()->fovy;
   float aspect = (float)get()->winwidth/get()->winheight;
   double nearp = get()->nearp;
   double farp = get()->farp;

   mtxperspective(fovy, aspect, nearp, farp);

   minicoord eye = m_camera->get_eye_opengl();
   miniv3d dir = m_camera->get_dir_opengl();
   miniv3d up = m_camera->get_up_opengl();

   mtxmodel();
   mtxid();
   mtxlookat(miniv3d(eye.vec), miniv3d(eye.vec) + dir, up);
   }

// render terrain geometry
void miniview::render_terrain_geometry()
   {
   // start timer
   starttimer();

   // update scene
   float aspect = (float)get()->winwidth/get()->winheight;
   cache(m_camera->get_eye(), m_camera->get_dir(), m_camera->get_up(), aspect);

   // render scene
   clear();
   render();

   // get time spent
   double delta=gettimer();

   // update quality parameters
   adapt(delta);
   }

// render ecef geometry
void miniview::render_ecef_geometry()
   {
   // render plain globe for z-values:

   static miniglobe globe;

   static const int gltess=32;
   static const double glfactor=0.999;

   globe.settess(gltess);
   globe.setscale(glfactor);

   disableRGBAwriting();
   globe.render();
   enableRGBAwriting();

   // render ecef z-axis:

   linewidth(2);
   enablelinesmooth();

   static const miniv3d zacolor(0.25,0.25,0.5);

   color(zacolor);
   renderline(miniv3d(0.0,0.0,-1.1*miniearth::EARTH_radius),
              miniv3d(0.0,0.0,-miniearth::EARTH_radius));
   renderline(miniv3d(0.0,0.0,miniearth::EARTH_radius),
              miniv3d(0.0,0.0,1.1*miniearth::EARTH_radius));

   // render equator:

   linewidth(1);
   disableZwriting();

   mtxproj();
   mtxpush();
   mtxscale(0.95,0.95,0.95); // prevent Z-fighting

   static const int eqlines=100;
   static const miniv3d eqcolor(0.25,0.25,0.25);

   color(eqcolor);
   for (int i=0; i<=eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600,0.0,0.0),minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);

      static minicoord c0;

      if (i>0) renderline(c0.vec,c.vec);
      c0=c;
      }

   mtxpop();
   mtxmodel();

   enableZwriting();

   disablelinesmooth();
   }

#endif
