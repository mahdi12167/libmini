// (c) by Stefan Roettger

#include <QDateTime>

#include <mini/miniOGL.h>
#include <mini/minishader.h>

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
{
   m_window = window;
   m_bIsInited = false;

   m_camera = NULL;

   m_StereoBase = 0.0;
   m_FogDensity = 0.0;
   m_SeaLevel = 0.0;
   m_DayHourDelta = 0.0;
   m_ExaggerOn = false;
   m_ExaggerScale = 1.0;
}

Renderer::~Renderer()
{
   if (m_camera!=NULL)
      delete m_camera;
}

// gl init
void Renderer::init()
{
   if (m_bIsInited) return;

   // initialize VIS bathy map
   initBathyMap();

   // init libMini parameters
   initParameters();

   // load optional features
   getearth()->loadopts();

   // create the camera object
   m_camera=new Camera(m_window, getearth());

   // tell camera lens fovy
   m_camera->setLens(VIEWER_FOVY);

   // tell camera
   set_camera(m_camera);

   // load textures
   loadTextureFromResource(":/images/crosshair.png", m_CrosshairTextureId);

   // init viewport
   resizeViewport();

   m_bIsInited = true;
}

// load map layer from url
minilayer* Renderer::loadMap(const char* url)
{
   minilayer *layer;

   if (m_bIsInited)
      if (url!=NULL)
      {
         layer=getearth()->loadLTS(url, TRUE, TRUE, VIEWER_LEVELS);

         if (layer!=NULL)
         {
            getearth()->defineroi(0.0);

            m_camera->focusOnMap(layer);
            m_camera->startIdling();

            return(layer);
         }
      }

   return(NULL);
}

// remove map layers
void Renderer::clearMaps()
{
   getearth()->getterrain()->remove();

   m_camera->startIdling();
}

// initialize libMini parameters
void Renderer::initParameters()
{
   // the scene parameters
   miniscene::MINISCENE_PARAMS sceneParams;
   get(sceneParams);
   sceneParams.winwidth = m_window->width();
   sceneParams.winheight = m_window->height();
   sceneParams.fps = CAMERA_FPS;
   sceneParams.fovy = VIEWER_FOVY;
   sceneParams.nearp = VIEWER_NEARP;
   sceneParams.farp = VIEWER_FARP;
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
void Renderer::initBathyMap()
{
   static const float hue1=190.0f;
   static const float hue2=240.0f;

   static const float sat1=0.75f;
   static const float sat2=0.5f;

   static const float val1=1.0f;
   static const float val2=0.5f;

   minishader::initbathymap_linear(m_BathyMap,VIEWER_BATHYWIDTH,
                                   hue1, hue2,
                                   sat1, sat2,
                                   val1, val2,
                                   VIEWER_BATHYMID);
}

// resize window
void Renderer::resizeWindow()
{
   resizeViewport();

   m_camera->startIdling();
}

// initialize the render window
void Renderer::resizeViewport()
{
   int winWidth = m_window->width();
   int winHeight = m_window->height();

   if (winWidth<1) winWidth=1;
   if (winHeight<1) winHeight=1;

   m_pSceneParams->winwidth=winWidth;
   m_pSceneParams->winheight=winHeight;
   set(m_pSceneParams);

   glViewport(0, 0, winWidth, winHeight);
}

// draw scene
void Renderer::draw()
{
   render_geometry(m_StereoBase);
   renderHUD();

   m_camera->startIdling();
}

// render head-up display
void Renderer::renderHUD()
{
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

   float lx = 60.0f/m_window->width();
   float ly = 60.0f/m_window->height();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex2f(-lx, ly); // P0
   glTexCoord2f(1.0f, 0.0f); glVertex2f(lx, ly); // P1
   glTexCoord2f(1.0f, 1.0f); glVertex2f(lx, -ly); // P2
   glTexCoord2f(0.0f, 1.0f); glVertex2f(-lx, -ly); // P3
   glEnd();

   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);

   // render text:

   minicoord cameraPosLLH = m_camera->get_eye();
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR)
      cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   double cameraAngle=m_camera->get_angle();

   minicoord cameraHit=m_camera->get_hit();
   double cameraHitDist=m_camera->get_hitdist();

   QString str;
   const QColor color(255, 255, 255);
   m_window->qglColor(color);
   int x = 10;
   int y = m_window->height() - 20;
   int line_space = -16;
   int second_column_offset = 90;

   str.sprintf("Latitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.x/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Longitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.y/3600.0);
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

      double hitElev=m_camera->get_elev(cameraHit);
      if (hitElev==-MAXFLOAT) hitElev=0.0;

      str.sprintf("Elevation:");
      drawText(x, y, str);
      str.sprintf("%-6.2f m", hitElev);
      drawText(x+second_column_offset, y, str);
      y+=line_space;
   }
}

void Renderer::loadTextureFromResource(const char* respath, GLuint& texId)
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

void Renderer::drawText(float x, float y, QString& str, QColor color, bool bIsDoublePrint)
{
   int sx = x;
   int sy = m_window->height() - y;

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

void Renderer::toggleStereo(bool on)
{
   if (on) m_StereoBase=VIEWER_SBASE;
   else m_StereoBase=0.0;

   m_camera->startIdling();
}

void Renderer::toggleWireFrame(bool on)
{
   m_pSceneParams->usewireframe=on;

   m_camera->startIdling();
}

void Renderer::checkFog(bool on)
{
   m_pEarthParams->usefog=on;
   setFogDensity(m_FogDensity);
}

void Renderer::setFogDensity(double density)
{
   m_FogDensity=density;

   density=pow(density,1.0/10);
   m_pEarthParams->fogdensity=VIEWER_FOGDENSITY*(1.0+density);
   m_pEarthParams->fogstart=(1.0-density)*VIEWER_FOGSTART;
   m_pEarthParams->fogend=(1.0-density)*VIEWER_FOGEND+density*VIEWER_NEARP/VIEWER_FARP;
   propagate();

   m_camera->startIdling();
}

void Renderer::checkContours(bool on)
{
   m_pEarthParams->usecontours=on;
   propagate();

   m_camera->startIdling();
}

void Renderer::checkSeaLevel(bool on)
{
   if (on) m_pTerrainParams->sealevel=m_SeaLevel;
   else m_pTerrainParams->sealevel=-MAXFLOAT;

   setSeaLevel(m_SeaLevel);
}

void Renderer::setSeaLevel(double level)
{
   m_SeaLevel=level;

   if (m_pTerrainParams->sealevel!=-MAXFLOAT) m_pTerrainParams->sealevel=level;

   propagate();
   getearth()->getterrain()->update();

   m_camera->startIdling();
}

void Renderer::checkLight(bool on)
{
   m_pEarthParams->usediffuse=on;
   setLight(m_DayHourDelta);
}

void Renderer::setLight(double hour)
{
   m_DayHourDelta=hour;

   QTime time_utc = QDateTime::currentDateTimeUtc().time();
   double hour_utc = time_utc.hour()+time_utc.minute()/60.0+time_utc.second()/3600.0;

   double light=2*PI*(hour_utc+m_DayHourDelta)/24.0;
   miniv3d lightdir(-cos(light),sin(light),0.0);
   m_pEarthParams->lightdir=lightdir;
   propagate();

   m_camera->startIdling();
}

void Renderer::checkExagger(bool on)
{
   m_ExaggerOn=on;
   setExagger(m_ExaggerScale);
}

void Renderer::setExagger(double scale)
{
   m_ExaggerScale=scale;

   getearth()->getterrain()->flatten(m_ExaggerOn?scale:1.0/VIEWER_EXAGGER);

   m_camera->moveAbove();

   getearth()->getterrain()->update();
   propagate();

   m_camera->startIdling();
}
