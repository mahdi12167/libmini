// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtCore/QDateTime>

#include <mini/mini_gfx.h>

#include "viewer.h"

Viewer::Viewer(ViewerWindow* window)
{
   m_window = window;
   m_bIsInited = false;

   m_StereoBase = 0.0;
   m_FogDensity = 0.0;
   m_SeaLevel = 0.0;
   m_DayHourDelta = 0.0;
   m_ExaggerOn = false;
   m_ExaggerScale = 1.0;
}

Viewer::~Viewer()
{}

// init when gl context is available
void Viewer::init()
{
   Camera *camera;

   if (m_bIsInited) return;

   // initialize VIS bathy map
   initBathyMap();

   // init libMini parameters
   initParameters();

   // load optional features
   getearth()->loadopts();

   // create the camera object
   camera = new Camera(m_window, getearth(),
                       CAMERA_LAT, CAMERA_LON, CAMERA_HEIGHT);

   // set camera lens
   camera->set_lens(CAMERA_FOVY, 0.0f, CAMERA_NEARP, CAMERA_FARP);

   // add camera as first element of scene graph
   m_root=camera;

   // link ecef geometry to scene graph
   m_root->append_child(build_ecef_geometry());

   // load textures
#if VIEWER_INTERNAL_CROSSHAIR
   loadCrossHair(m_CrosshairTextureId);
#else
   loadTextureFromResource(":/images/crosshair.png", m_CrosshairTextureId);
#endif

   // init viewport
   resizeViewport();

   m_bIsInited = true;
}

// resize window
void Viewer::resizeWindow()
{
   resizeViewport();

   getCamera()->startIdling();
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
Camera *Viewer::getCamera()
{
   return((Camera *)m_root->get_camera());
}

// load map layer from url
minilayer* Viewer::loadMap(ministring url)
{
   minilayer *layer;

   Camera *camera = getCamera();

   if (m_bIsInited)
      if (!url.empty())
      {
         layer=getearth()->loadLTS(url.c_str(), TRUE, TRUE, VIEWER_LEVELS);

         if (layer!=NULL)
         {
            getearth()->defineroi(0.0);

            camera->focusOnMap(layer);
            camera->startIdling();

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

   getCamera()->startIdling();
}

// remove map layers
void Viewer::clearMaps()
{
   getearth()->getterrain()->remove();

   getCamera()->startIdling();
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
   terrainParams.bathystart = VIEWER_BATHYSTART;
   terrainParams.bathyend = VIEWER_BATHYEND;
   terrainParams.bathymap = m_BathyMap;
   terrainParams.bathywidth = VIEWER_BATHYWIDTH;
   terrainParams.bathyheight = 2;
   terrainParams.bathycomps = 4;
   getearth()->getterrain()->set(terrainParams);
   m_pTerrainParams = getearth()->getterrain()->get();
}

// initialize bathymetry map
void Viewer::initBathyMap()
{
   static const float hue1 = 190.0f;
   static const float hue2 = 240.0f;

   static const float sat1 = 0.75f;
   static const float sat2 = 0.5f;

   static const float val1 = 1.0f;
   static const float val2 = 0.5f;

   minishader::initbathymap_linear(m_BathyMap, VIEWER_BATHYWIDTH,
                                   hue1, hue2,
                                   sat1, sat2,
                                   val1, val2,
                                   VIEWER_BATHYMID);
}

// draw scene
void Viewer::draw()
{
   // tell camera
   set_camera(getCamera());

   // tell lighting
   if (m_pEarthParams->usediffuse) setLight(m_DayHourDelta);

   // render scene
   render_geometry(m_StereoBase);

   // render head-up display
   renderHUD();
}

// render head-up display
void Viewer::renderHUD()
{
   Camera *camera = getCamera();

   // draw crosshair:

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glBindTexture(GL_TEXTURE_2D, m_CrosshairTextureId);
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_CULL_FACE);

   float lx = 50.0f/(m_window->width()+1);
   float ly = 50.0f/(m_window->height()+1);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex2f(-lx, ly);
   glTexCoord2f(1.0f, 0.0f); glVertex2f(lx, ly);
   glTexCoord2f(1.0f, 1.0f); glVertex2f(lx, -ly);
   glTexCoord2f(0.0f, 1.0f); glVertex2f(-lx, -ly);
   glEnd();

   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);

   if (m_window->width()<32 || m_window->height()<32) return;

   // render text:

   minicoord cameraPosLLH = camera->get_eye();
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR)
      cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   double cameraAngle = camera->get_angle();

   minicoord cameraHit = camera->get_hit();
   double cameraHitDist = camera->get_hitdist();

   QString str;
   const QColor color(255, 255, 255);
   m_window->qglColor(color);
   int x = 10;
   int y = m_window->height()-20;
   int line_space = -16;
   int second_column_offset = 90;

   str.sprintf("Latitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.y/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Longitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.x/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Altitude:");
   drawText(x, y, str);
   if (cameraPosLLH.vec.z<-1000.0 || cameraPosLLH.vec.z>1000.0)
      str.sprintf("%-6.2f km", cameraPosLLH.vec.z/1000.0);
   else
      str.sprintf("%-6.2f m", cameraPosLLH.vec.z);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Heading:");
   drawText(x, y, str);
   if (cameraAngle<=45.0 || cameraAngle>315.0)
      str.sprintf("%-6.0f North", cameraAngle);
   else if (cameraAngle>45.0 && cameraAngle<=135.0)
      str.sprintf("%-6.0f West", cameraAngle);
   else if (cameraAngle>135.0 && cameraAngle<=225.0)
      str.sprintf("%-6.0f South", cameraAngle);
   else if (cameraAngle>225.0 && cameraAngle<=315.0)
      str.sprintf("%-6.0f East", cameraAngle);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   if (cameraHitDist>0.0)
   {
      str.sprintf("Distance:");
      drawText(x, y, str);
      if (cameraHitDist>1000.0)
         str.sprintf("%-6.2f km", cameraHitDist/1000.0);
      else
         str.sprintf("%-6.2f m", cameraHitDist);
      drawText(x+second_column_offset, y, str);
      y+=line_space;

      double hitElev=camera->get_elev(cameraHit);
      if (hitElev==-MAXFLOAT) hitElev=0.0;

      str.sprintf("Elevation:");
      drawText(x, y, str);
      str.sprintf("%-6.2f m", hitElev);
      drawText(x+second_column_offset, y, str);
      y+=line_space;
   }
}

void Viewer::loadTextureFromResource(const char* respath, GLuint& texId)
{
   QImage tex, buf;
   bool bLoaded = buf.load(respath);

   if (bLoaded)
      tex = QGLWidget::convertToGLFormat(buf);

   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Viewer::loadCrossHair(GLuint& texId)
{
   int i;

   static const int size=33;

   QImage tex;

   QImage crosshair(size, size, QImage::Format_ARGB32);

   crosshair.fill(0);

   QRgb black = qRgb(0,0,0);

   setPixel(crosshair, size/2,size/2, black);
   for (i=3; i<size/2-1; i++)
   {
      setPixel(crosshair, size/2-i,size/2, black);
      setPixel(crosshair, size/2+i,size/2, black);
      setPixel(crosshair, size/2,size/2-i, black);
      setPixel(crosshair, size/2,size/2+i, black);
   }

   tex = QGLWidget::convertToGLFormat(crosshair);

   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Viewer::setPixel(QImage &image, int x, int y, QRgb color)
   {
   QRgb white = qRgba(255,255,255,31);

   image.setPixel(x, y, color);

   if (QColor::fromRgba(image.pixel(x-1, y)).alpha()==0) image.setPixel(x-1, y, white);
   if (QColor::fromRgba(image.pixel(x+1, y)).alpha()==0) image.setPixel(x+1, y, white);
   if (QColor::fromRgba(image.pixel(x, y-1)).alpha()==0) image.setPixel(x, y-1, white);
   if (QColor::fromRgba(image.pixel(x, y+1)).alpha()==0) image.setPixel(x, y+1, white);

   if (QColor::fromRgba(image.pixel(x-1, y-1)).alpha()==0) image.setPixel(x-1, y-1, white);
   if (QColor::fromRgba(image.pixel(x+1, y-1)).alpha()==0) image.setPixel(x+1, y-1, white);
   if (QColor::fromRgba(image.pixel(x-1, y+1)).alpha()==0) image.setPixel(x-1, y+1, white);
   if (QColor::fromRgba(image.pixel(x+1, y+1)).alpha()==0) image.setPixel(x+1, y+1, white);
   }

void Viewer::drawText(float x, float y, QString& str, QColor color, bool bIsDoublePrint)
{
   int sx = x;
   int sy = m_window->height()-y;

   if (!bIsDoublePrint)
   {
      m_window->qglColor(color);
      m_window->renderText(sx, sy, str);
   }
   else
   {
      m_window->qglColor(QColor(0, 0, 0, 255));
      m_window->renderText(sx+2, sy+2, str);
      m_window->qglColor(color);
      m_window->renderText(sx, sy, str);
   }
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

void Viewer::toggleStereo(bool on)
{
   if (on) m_StereoBase = CAMERA_SBASE;
   else m_StereoBase = 0.0;

   getCamera()->startIdling();
}

void Viewer::toggleWireFrame(bool on)
{
   m_pSceneParams->usewireframe = on;

   getCamera()->startIdling();
}

void Viewer::checkFog(bool on)
{
   m_pEarthParams->usefog = on;
   setFogDensity(m_FogDensity);
}

void Viewer::setFogDensity(double density)
{
   m_FogDensity = density;

   density=pow(density, 1.0/10);
   m_pEarthParams->fogdensity = VIEWER_FOGDENSITY*(1.0+density);
   m_pEarthParams->fogstart = (1.0-density)*VIEWER_FOGSTART;
   m_pEarthParams->fogend = (1.0-density)*VIEWER_FOGEND+density*CAMERA_NEARP/CAMERA_FARP;
   propagate();

   getCamera()->startIdling();
}

void Viewer::checkContours(bool on)
{
   m_pEarthParams->usecontours = on;
   propagate();

   getCamera()->startIdling();
}

void Viewer::checkSeaLevel(bool on)
{
   if (on) m_pTerrainParams->sealevel = m_SeaLevel;
   else m_pTerrainParams->sealevel = -MAXFLOAT;

   setSeaLevel(m_SeaLevel);
}

void Viewer::setSeaLevel(double level)
{
   m_SeaLevel=level;

   if (m_pTerrainParams->sealevel!=-MAXFLOAT) m_pTerrainParams->sealevel = level;

   propagate();
   getearth()->getterrain()->update();

   getCamera()->startIdling();
}

void Viewer::checkLight(bool on)
{
   m_pEarthParams->usediffuse = on;
   setLight(m_DayHourDelta);
}

void Viewer::setLight(double hour)
{
   m_DayHourDelta = hour;

   QTime time_utc = QDateTime::currentDateTimeUtc().time();
   double hour_utc = time_utc.hour()+time_utc.minute()/60.0+time_utc.second()/3600.0;

   double light = 2*PI*(hour_utc+m_DayHourDelta)/24.0;
   miniv3d lightdir(-cos(light), sin(light), 0.0);
   m_pEarthParams->lightdir = lightdir;
   propagate();

   getCamera()->startIdling();
}

void Viewer::checkExagger(bool on)
{
   m_ExaggerOn = on;
   setExagger(m_ExaggerScale);
}

void Viewer::setExagger(double scale)
{
   Camera *camera = getCamera();

   m_ExaggerScale = scale;

   getearth()->getterrain()->flatten(m_ExaggerOn?scale:1.0/VIEWER_EXAGGER);

   camera->moveAbove();

   getearth()->getterrain()->update();
   propagate();

   camera->startIdling();
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

   // setup shaders:

   ministrip::setglobal_texgen(TRUE);
   ministrip::setglobal_shade(TRUE);
   ministrip::setglobal_fog(TRUE);
   mininode_coord::set_lightdir(miniv3d(0,0,0));

   if (m_pEarthParams->usediffuse)
   {
      ministrip::setglobal_shade_direct(TRUE);

      miniv3d l = getearth()->get()->lightdir;
      mininode_coord::set_lightdir(l);

      double oglmtx[16];
      mtxgetmodel(oglmtx);
      miniv3d mtx[3];
      mtxget(oglmtx,mtx);
      miniv3d invtra[3];
      inv_mtx(invtra,mtx);
      tra_mtx(invtra,invtra);

      miniv3d lgl;
      lgl = mlt_vec(invtra, l);
      float lightdir[3] = {(float)lgl.x, (float)lgl.y, (float)lgl.z};
      mininode_geometry::setglobalshadedirectparams(lightdir, 0.5f, 0.5f);
   }

   // setup fogging params
   float fogstart = getearth()->get()->fogstart/2.0f*len_g2o(getearth()->get()->farp);
   float fogend = getearth()->get()->fogend*len_g2o(getearth()->get()->farp);
   if (!getearth()->get()->usefog) fogend = 0.0f;
   mininode_geometry::setglobalfogparams(fogstart, fogend, getearth()->get()->fogdensity, getearth()->get()->fogcolor);

   // set animation time
   static BOOLINT firstcall=TRUE;
   if (firstcall)
      {
      mininode_animation::set_time_start(t);
      firstcall=FALSE;
      }
   mininode_animation::set_time(t);

   // setup render state
   initstate();

   // render ecef geometry by traversing scene graph
   if (m_root->traverse())
      getCamera()->startIdling();

   // cleanup render state
   exitstate();
}

// shoot a ray at the ecef geometry
double Viewer::shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double mindist)
{
   return(m_root->shoot_ray(o,d,NULL,mindist));
}

// save scene graph
void Viewer::saveGraph(ministring filename) const
{
   m_root->get_child()->save(filename);
}

// load scene graph
void Viewer::loadGraph(ministring filename)
{
   m_root->append_child(m_root->load(filename));
}
