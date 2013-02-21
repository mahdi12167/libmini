// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef QTSIMPLE_H
#define QTSIMPLE_H

#define VIEWER_WIDTH 768
#define VIEWER_MINWIDTH 256
#define VIEWER_ASPECT 1.0f

#define VIEWER_FOGSTART 0.1f
#define VIEWER_FOGEND 1.0f
#define VIEWER_FOGDENSITY 0.75f

#define VIEWER_VOIDSTART 100000.0f
#define VIEWER_ABYSSSTART -500.0f

#define VIEWER_EXAGGER 3.0f
#define VIEWER_RES 1.0E4f
#define VIEWER_RELRES 0.25f
#define VIEWER_RANGE 0.01f
#define VIEWER_RELRANGE 0.25f
#define VIEWER_SEALEVEL -MAXFLOAT
#define VIEWER_LEVELS 5
#define VIEWER_CONTOURS 100.0f
#define VIEWER_SEABOTTOM -1000.0f

#define CAMERA_FPS 30.0f

#define CAMERA_FOVY 90.0f
#define CAMERA_NEARP 50.0f
#define CAMERA_FARP 1.0E7f

#define CAMERA_HEIGHT 3.0E6f
#define CAMERA_LAT 0
#define CAMERA_LON 0

#include <QtOpenGL/qgl.h>

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>
#include <mini/mini_sfx.h>

class QTimerEvent;

//! qt viewer window that
//! contains the opengl rendering context
class QtSimpleViewer: public QGLWidget, public miniview
{
public:

   QtSimpleViewer(QWidget *parent = 0)
   {
      m_bIsInited = false;

      setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer));
      setCursor(Qt::CrossCursor);

      viewer_aspect = VIEWER_ASPECT;

      startTimer((int)(1000.0/CAMERA_FPS));
   }

   virtual ~QtSimpleViewer()
   {}

   mininode_root* getRoot()
   {
      return(m_root);
   }

   mininode_cam* getCamera()
   {
      return(m_root->get_camera());
   }

   minilayer* loadMap(ministring url)
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

   void removeMap(minilayer *layer)
   {
      int num=getearth()->getterrain()->getnum(layer);

      getearth()->getterrain()->removeLTS(num);
   }

   void clearMaps()
   {
      getearth()->getterrain()->remove();
   }

   mininode_geometry *pick(const minicoord &o, const miniv3d &d, double mindist=0.0)
   {
      minicoord o0;
      mininode_geometry *obj;

      o0 = o;
      o0.convert2ecef();
      m_root->shoot_ray(o0.vec, d, &obj, mindist);

      return(obj);
   }

   void setAspect(double aspect)
   {
      viewer_aspect = aspect;
   }

   QSize minimumSizeHint() const
   {
      return(QSize(VIEWER_MINWIDTH, VIEWER_MINWIDTH/viewer_aspect));
   }

   QSize sizeHint() const
   {
      return(QSize(VIEWER_WIDTH, VIEWER_WIDTH/viewer_aspect));
   }

protected:

   // scene graph root node
   mininode_rootref m_root;

   // initialization flag
   bool m_bIsInited;

   // parameter pointers
   miniscene::MINISCENE_PARAMS* m_pSceneParams; // the scene parameters
   miniearth::MINIEARTH_PARAMS* m_pEarthParams; // the earth parameters
   miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams; // the terrain parameters

   virtual mininode_group *build_ecef_geometry()=0;

   double viewer_aspect;

   void init()
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

   void resizeViewport()
   {
      int winWidth = width();
      int winHeight = height();

      if (winWidth<1) winWidth = 1;
      if (winHeight<1) winHeight = 1;

      m_pSceneParams->winwidth = winWidth;
      m_pSceneParams->winheight = winHeight;
      set(m_pSceneParams);

      glViewport(0, 0, winWidth, winHeight);
   }

   void draw()
   {
      // tell camera
      set_camera(getCamera());

      // render scene
      render_geometry();
   }

   virtual void check_ecef_geometry(miniv3d &center, double &radius)
   {
      // specify empty bounding sphere
      center=miniv3d(0,0,0);
      radius=0.0;

      // update scene graph
      m_root->clear_dirty();

      // specify bounding sphere
      m_root->get_bsphere(center,radius);
   }

   virtual void render_ecef_geometry(double t)
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

   virtual double shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double mindist=0.0)
   {
      return(m_root->shoot_ray(o,d,NULL,mindist));
   }

   void initParameters()
   {
      // the scene parameters
      miniscene::MINISCENE_PARAMS sceneParams;
      get(sceneParams);
      sceneParams.winwidth = width();
      sceneParams.winheight = height();
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

   void initializeGL()
   {
      if (!m_bIsInited)
      {
         // initialize viewer here as it needs GL context to init
         init();
      }

      qglClearColor(Qt::black);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
   }

   void resizeGL(int, int)
   {
      resizeViewport();
   }

   void paintGL()
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      draw();
   }

   virtual void timerEvent(QTimerEvent *) = 0;

};

#endif
