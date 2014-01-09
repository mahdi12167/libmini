// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_gfx.h>

#include "viewer.h"
#include "viewerconst.h"

Viewer::Viewer(ViewerWindow* window)
{
   m_window = window;
   m_bIsInited = false;
}

Viewer::~Viewer()
{}

// init when gl context is available
void Viewer::init()
{
   mininode_cam *camera;

   if (m_bIsInited) return;

   // init libMini parameters
   initParameters();

   // load optional features
   getearth()->loadopts();

   // create the camera object
   camera = new mininode_cam(getearth(),
                             CAMERA_LAT, CAMERA_LON, CAMERA_HEIGHT);

   // set camera lens
   camera->set_lens(CAMERA_FOVY, 0.0f, CAMERA_NEARP, CAMERA_FARP);

   // add camera as first element of scene graph
   m_root=camera;

   // link ecef geometry to scene graph
   m_root->append_child(build_ecef_geometry());

   // init viewport
   resizeViewport();

   m_bIsInited = true;
}

// resize window
void Viewer::resizeWindow()
{
   resizeViewport();
}

// initialize the render window
void Viewer::resizeViewport()
{
   int winWidth = m_window->width();
   int winHeight = m_window->height();

   if (winWidth<1) winWidth = 1;
   if (winHeight<1) winHeight = 1;

   m_pSceneParams->winwidth = winWidth;
   m_pSceneParams->winheight = winHeight;
   set(m_pSceneParams);

   glViewport(0, 0, winWidth, winHeight);
}

// get root node
mininode_root *Viewer::getRoot()
{
   return(m_root);
}

// get actual camera
mininode_cam *Viewer::getCamera()
{
   return(m_root->get_camera());
}

// load map layer from url
minilayer* Viewer::loadMap(ministring url)
{
   minilayer *layer;

   if (m_bIsInited)
      if (!url.empty())
      {
         layer=getearth()->loadLTS(url.c_str(), TRUE, TRUE, VIEWER_LEVELS);

         if (layer!=NULL)
         {
            getearth()->defineroi(0.0);

            return(layer);
         }
      }

   return(NULL);
}

// remove map layer
void Viewer::removeMap(minilayer *layer)
{
   int num=getearth()->getterrain()->getnum(layer);

   getearth()->getterrain()->removeLTS(num);
}

// remove map layers
void Viewer::clearMaps()
{
   getearth()->getterrain()->remove();
}

// initialize libMini parameters
void Viewer::initParameters()
{
   // the scene parameters
   miniscene::MINISCENE_PARAMS sceneParams;
   get(sceneParams);
   sceneParams.winwidth = m_window->width();
   sceneParams.winheight = m_window->height();
   sceneParams.fps = CAMERA_FPS;
   sceneParams.fovy = CAMERA_FOVY;
   sceneParams.nearp = CAMERA_NEARP;
   sceneParams.farp = CAMERA_FARP;
   sceneParams.usewireframe = FALSE;
   set(sceneParams);
   m_pSceneParams = get();

   // the earth parameters
   miniearth::MINIEARTH_PARAMS earthParams;
   getearth()->get(earthParams);
   earthParams.warpmode = WARPMODE_AFFINE_REF;
   earthParams.nonlin = TRUE;
   earthParams.usefog = TRUE;
   earthParams.useshaders = TRUE;
   earthParams.usediffuse = FALSE;
   earthParams.usevisshader = TRUE;
   earthParams.usebathymap = TRUE;
   earthParams.usecontours = FALSE;
   earthParams.useearth = TRUE;
   earthParams.fogstart = VIEWER_FOGSTART;
   earthParams.fogend = VIEWER_FOGEND;
   earthParams.fogdensity = VIEWER_FOGDENSITY;
   earthParams.voidstart = VIEWER_VOIDSTART;
   earthParams.abyssstart = VIEWER_ABYSSSTART;
   getearth()->set(earthParams);
   m_pEarthParams = getearth()->get();

   // the terrain parameters
   miniterrain::MINITERRAIN_PARAMS terrainParams;
   getearth()->getterrain()->get(terrainParams);
   terrainParams.scale = 1.0f;
   terrainParams.exaggeration = VIEWER_EXAGGER;
   terrainParams.res = VIEWER_RES;
   terrainParams.relres1 = VIEWER_RELRES;
   terrainParams.range = VIEWER_RANGE;
   terrainParams.relrange1 = VIEWER_RELRANGE;
   terrainParams.sealevel = VIEWER_SEALEVEL;
   terrainParams.genmipmaps = TRUE;
   terrainParams.automipmap = TRUE;
   terrainParams.autocompress = FALSE;
   terrainParams.lod0uncompressed = FALSE;
   terrainParams.contours = VIEWER_CONTOURS;
   terrainParams.seabottom = VIEWER_SEABOTTOM;
   getearth()->getterrain()->set(terrainParams);
   m_pTerrainParams = getearth()->getterrain()->get();
}

// draw scene
void Viewer::draw()
{
   // tell camera
   set_camera(getCamera());

   // render scene
   render_geometry();
}

mininode_geometry *Viewer::pick(const minicoord &o, const miniv3d &d, double mindist)
{
   minicoord o0;
   mininode_geometry *obj;

   o0 = o;
   o0.convert2ecef();
   m_root->shoot_ray(o0.vec, d, &obj, mindist);

   return(obj);
}

// check ecef geometry
void Viewer::check_ecef_geometry(miniv3d &center, double &radius)
{
   // specify empty bounding sphere
   center=miniv3d(0,0,0);
   radius=0.0;

   // update scene graph
   m_root->clear_dirty();

   // specify bounding sphere
   m_root->get_bsphere(center,radius);
}

// render ecef geometry
void Viewer::render_ecef_geometry(double t)
{
   // render plain globe for z-values:

   static miniglobe globe;

   static const int gltess = 32;
   static const double glscale = 0.99;
   static const double glzscale = 1.05;

   globe.settess(gltess);
   globe.setscale(1.0);
   globe.setdynscale(glscale);
   globe.setZscale(glzscale);

   disableRGBAwriting();
   globe.render();
   enableRGBAwriting();

   // setup render state
   initstate();

   // render ecef geometry by traversing scene graph
   m_root->traverse();

   // cleanup render state
   exitstate();
}

// shoot a ray at the ecef geometry
double Viewer::shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double mindist)
{
   return(m_root->shoot_ray(o,d,NULL,mindist));
}
