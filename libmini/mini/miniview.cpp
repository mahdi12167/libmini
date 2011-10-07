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
void miniview::render_geometry(float sbase,BOOLINT anaglyph)
   {
   minilayer *nst;

   if (m_camera==NULL) return;

   // start timer
   starttimer();

   // set reference layer
   nst=getearth()->getnearest(m_camera->get_eye());
   getearth()->setreference(nst);

   // update scene
   float aspect = (float)get()->winwidth/get()->winheight;
   cache(m_camera->get_eye(), m_camera->get_dir(), m_camera->get_up(), aspect);

   // clear scene
   clear();

   // render scene
   if (sbase==0.0f)
      {
      setup_matrix();
      render();
      }
   else
      {
      // left stereo channel
      setup_matrix(-sbase);
      if (anaglyph) enableRwriting();
      else writeleftbuffer();
      render();

      // right stereo channel
      setup_matrix(sbase);
      cleardepthbuffer();
      if (anaglyph) enableGBwriting();
      else writerightbuffer();
      render();
      if (anaglyph) enableRGBwriting();
      else writebackbuffer();
      }

   // get time spent
   double delta=gettimer();

   // update quality parameters
   adapt(delta);
   }

// setup OpenGL modelview and projection matrices
void miniview::setup_matrix(float sbase)
   {
   mtxproj();
   mtxid();

   float fovy = get()->fovy;
   float aspect = (float)get()->winwidth/get()->winheight;
   double nearp = get()->nearp;
   double farp = get()->farp;

   mtxperspective(fovy, aspect, len_g2o(nearp), len_g2o(farp));

   minicoord egl = m_camera->get_eye_opengl();
   miniv3d dgl = m_camera->get_dir_opengl();
   miniv3d ugl = m_camera->get_up_opengl();
   miniv3d rgl = m_camera->get_right_opengl()*len_g2o(sbase);

   mtxmodel();
   mtxid();
   mtxlookat(miniv3d(egl.vec)+rgl, miniv3d(egl.vec)+rgl+dgl, ugl);
   }

// render ecef geometry
void miniview::render_ecef_geometry()
   {
   // render plain globe for z-values:

   static miniglobe globe;

   static const int gltess=32;
   static const double glscale=0.999;
   static const double glzscale=1.01;

   globe.settess(gltess);
   globe.setscale(1.0);
   globe.setdynscale(glscale);
   globe.setZscale(glzscale);

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

   enableZwriting();
   disablelinesmooth();
   }

#endif
