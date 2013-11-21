// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniOGL.h"
#include "miniglobe.h"
#include "miniearth.h"
#include "minicam.h"

#include "miniview.h"

miniview::miniview() : miniscene()
   {m_cam = NULL;}

miniview::~miniview()
   {}

// tell actual camera
void miniview::set_camera(minicam *cam)
   {m_cam = cam;}

// propagate view parameters
void miniview::propagate()
   {
   // get view parameters
   float fovy = PARAMS.fovy;
   float aspect = (float)get()->winwidth/get()->winheight;
   double nearp = PARAMS.nearp;
   double farp = PARAMS.farp;

   if (m_cam!=NULL) m_cam->set_lens(fovy, aspect, nearp, farp);

   miniscene::propagate();
   }

// render earth and terrain geometry
void miniview::render_geometry(float sbase, BOOLINT anaglyph)
   {
   minilayer *nst;

   if (m_cam==NULL) return;

   // start timer
   starttimer();

   // get camera lens
   float fovy = m_cam->get_fovy();
   float aspect = (float)get()->winwidth/get()->winheight;
   double nearp = m_cam->get_nearp();
   double farp = m_cam->get_farp();

   // update camera lens
   m_cam->set_lens(fovy, aspect, nearp, farp);

   // propagate camera lens
   PARAMS.fovy = fovy;
   PARAMS.nearp = nearp;
   PARAMS.farp = farp;
   miniscene::propagate();

   // set reference layer
   nst = getearth()->getnearest(m_cam->get_eye());
   getearth()->setreference(nst);

   // update scene
   cache(m_cam->get_eye(), m_cam->get_dir(), m_cam->get_up(), m_cam->get_aspect());

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
   double delta = gettimer();

   // update quality parameters
   adapt(delta);
   }

// setup OpenGL modelview and projection matrices
void miniview::setup_matrix(float sbase)
   {
   float fovy = m_cam->get_fovy();
   float aspect = m_cam->get_aspect();
   double nearp = m_cam->get_nearp();
   double farp = m_cam->get_farp();

   // check bounding sphere of ecef geometry
   if (farp>=getorbradius())
      if ((EARTH->get()->warpmode==WARPMODE_AFFINE ||
           EARTH->get()->warpmode==WARPMODE_AFFINE_REF) &&
          EARTH->get()->nonlin)
         {
         miniv3d center;
         double radius;

         check_ecef_geometry(center, radius);

         if (radius>0.0)
            {
            miniv3d eye, dir;
            double farp_ecef;

            eye = m_cam->get_eye().vec;
            dir = m_cam->get_dir();

            farp_ecef = dir*(center-eye)+radius;
            farp = dmax(farp, farp_ecef);
            }
         }

   // opengl perspective
   mtxproj();
   mtxid();
   mtxperspective(fovy, aspect, len_g2o(nearp), len_g2o(farp));

   minicoord egl = m_cam->get_eye_opengl();
   miniv3d dgl = m_cam->get_dir_opengl();
   miniv3d ugl = m_cam->get_up_opengl();
   miniv3d rgl = m_cam->get_right_opengl()*len_g2o(sbase);

   // opengl lookat
   mtxmodel();
   mtxid();
   mtxlookat(miniv3d(egl.vec)+rgl, miniv3d(egl.vec)+rgl+dgl, ugl);
   }

// shoot a ray at the scene
double miniview::shoot(const minicoord &o,const miniv3d &d,double mindist)
   {
   minicoord o0=o;
   o0.convert2ecef();

   double dist1=miniscene::shoot(o0,d,mindist);
   double dist2=shoot_ecef_geometry(o0.vec,d,mindist);

   return(dist1<dist2?dist1:dist2);
   }

// check ecef geometry
void miniview::check_ecef_geometry(miniv3d &center, double &radius)
   {
   // specify bounding sphere
   center = miniv3d(0,0,0);
   radius = 1.1*getorbradius();
   }

// render ecef geometry
void miniview::render_ecef_geometry(double)
   {
   // render plain globe for z-values:

   static miniglobe globe;

   static const int gltess = 32;
   static const double glscale = 0.999;
   static const double glzscale = 1.05;

   globe.settess(gltess);
   globe.setscale(1.0);
   globe.setdynscale(glscale);
   globe.setZscale(glzscale);

   disableRGBAwriting();
   globe.render();
   enableRGBAwriting();

   initstate();

   // render ecef z-axis:

   linewidth(2);
   enablelinesmooth();

   static const miniv3d zacolor(0.25, 0.25, 0.5);

   color(zacolor);
   renderline(miniv3d(0.0, 0.0, -1.1*getorbradius()),
              miniv3d(0.0, 0.0, -getorbradius()));
   renderline(miniv3d(0.0, 0.0, getorbradius()),
              miniv3d(0.0, 0.0, 1.1*getorbradius()));

   // render equator:

   linewidth(1);
   disableZwriting();

   static const int eqlines = 100;
   static const miniv3d eqcolor(0.25, 0.25, 0.25);

   color(eqcolor);
   for (int i=0; i<=eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600, 0.0, 0.0),minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);

      static minicoord c0;

      if (i>0) renderline(c0.vec, c.vec);
      c0 = c;
      }

   enableZwriting();
   disablelinesmooth();

   exitstate();
   }

// shoot a ray at the ecef geometry
double miniview::shoot_ecef_geometry(const miniv3d &,const miniv3d &,double)
   {return(MAXFLOAT);}
