#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>

#include <mini/minibase.h>

#include <mini/miniOGL.h>
#include <mini/minishader.h>

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
{
   this->window = window;
   m_bIsInited = false;

   viewer=NULL;
   camera=NULL;

   m_Shift=false;
   m_Control=false;
   m_Meta=false;
}

Renderer::~Renderer()
{
   if (viewer!=NULL)
      delete viewer;

   if (camera!=NULL)
      delete camera;
}

// gl init
void Renderer::init()
{
   if (m_bIsInited) return;

   // viewport size
   viewportwidth=window->width();
   viewportheight=window->height();

   // create the viewer object
   viewer=new viewerbase();

   // initialize VIS bathy map
   initBathyMap();

   // init libMini parameters
   initParameters();

   // load optional features
   viewer->getearth()->loadopts();

   // create the camera object
   camera=new minicam(viewer->getearth());

   // initialize the view
   initView();

   // initialize the camera transition
   initTransition();

   // load textures
   loadTextureFromResource(":/images/crosshair.png", m_CrosshairTextureId);

   m_bIsInited = true;
}

// load map layer from url
bool Renderer::loadMap(const char* url)
{
   if (m_bIsInited)
      if (url!=NULL)
         if (viewer->getearth()->loadLTS(url, TRUE, TRUE, VIEWER_LEVELS))
         {
            viewer->getearth()->defineroi(0.0);
            return(true);
         }
         else
         {
            QString message;
            message.sprintf("Unable to load map data from url=%s\n", url);
            QMessageBox::warning(window, "Error", message, QMessageBox::Ok);
         }

   return(false);
}

// remove map layers
void Renderer::clearMaps()
{
   printf("check1\n"); //!!
   viewer->getearth()->getterrain()->remove(); //!! crash boom bang
   printf("check2\n"); //!!
}

// initialize libMini parameters
void Renderer::initParameters()
{
   // the viewing parameters
   viewerbase::VIEWER_PARAMS viewerParams;
   viewer->get(viewerParams);
   viewerParams.winwidth = viewportwidth;
   viewerParams.winheight = viewportheight;
   viewerParams.fps = VIEWER_FPS;
   viewerParams.fovy = VIEWER_FOVY;
   viewerParams.nearp = VIEWER_NEARP;
   viewerParams.farp = VIEWER_FARP;
   viewerParams.usewireframe = FALSE;
   viewer->set(viewerParams);
   m_pViewerParams = viewer->get();

   // the earth parameters
   miniearth::MINIEARTH_PARAMS earthParams;
   viewer->getearth()->get(earthParams);
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
   earthParams.fogdensity = VIEWER_FOGDENSITY;
   earthParams.voidstart = VIEWER_VOIDSTART;
   earthParams.abyssstart = VIEWER_ABYSSSTART;
   viewer->getearth()->set(earthParams);
   m_pEarthParams = viewer->getearth()->get();

   // the terrain parameters
   miniterrain::MINITERRAIN_PARAMS terrainParams;
   viewer->getearth()->getterrain()->get(terrainParams);
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
   viewer->getearth()->getterrain()->set(terrainParams);
   m_pTerrainParams  =  viewer->getearth()->getterrain()->get();
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
                                   hue1,hue2,
                                   sat1,sat2,
                                   val1,val2,
                                   VIEWER_BATHYMID);
}

// set camera position and direction
void Renderer::setCamera(float latitude, float longitude, float altitude, float heading, float pitch)
{
   if (camera==NULL) return;

   minicoord eye(miniv3d(latitude * 3600.0, longitude * 3600.0, altitude),
                 minicoord::MINICOORD_LLH);

   camera->set_eye(eye,heading,pitch);
}

// initialize the view point
void Renderer::initView()
{
   resizeViewport();

   camera->move_down(-VIEWER_HEIGHT_START);
   camera->move_above(VIEWER_HEIGHT_FLOOR);

   viewer->initeyepoint(camera->get_eye());
}

// initialize the camera transition
void Renderer::initTransition()
{
   m_TransitionTimerId = -1;
   m_IdlingTimerId = -1;

   m_bInCameraTransition = false;

   m_CursorValid = false;
}

// resize window
void Renderer::resizeWindow(int width, int height)
{
   viewportwidth = width;
   viewportheight = height;
   resizeViewport();

   window->updateGL();
}

// initialize the render window
void Renderer::resizeViewport()
{
   int winWidth = viewportwidth;
   int winHeight = viewportheight;

   if (winWidth<1) winWidth=1;
   if (winHeight<1) winHeight=1;

   m_pViewerParams->winwidth=winWidth;
   m_pViewerParams->winheight=winHeight;
   viewer->set(m_pViewerParams);

   glViewport(0, 0, winWidth, winHeight);
}

// draw scene
void Renderer::draw()
{
   minilayer *nst;

   // set reference layer
   nst=viewer->getearth()->getnearest(camera->get_eye());
   viewer->getearth()->setreference(nst);

   // render scene
   setupMatrix();
   renderTerrain();
   renderHUD();

   // wait for static scene
   startIdling();
}

// setup OpenGL modelview and projection matrices
void Renderer::setupMatrix()
{
   glViewport(0.0f, 0.0f, viewportwidth, viewportheight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float fovy = m_pViewerParams->fovy;
   float aspectRatio = (float)viewportwidth/viewportheight;
   double nearpGL = m_pViewerParams->nearp;
   double farpGL = m_pViewerParams->farp;

   gluPerspective(fovy, aspectRatio, nearpGL, farpGL);

   minicoord eyeGL = camera->get_eye_opengl();
   miniv3d dirGL = camera->get_dir_opengl();
   miniv3d upGL = camera->get_up_opengl();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(eyeGL.vec.x, eyeGL.vec.y, eyeGL.vec.z,
             eyeGL.vec.x+dirGL.x, eyeGL.vec.y+dirGL.y, eyeGL.vec.z+dirGL.z,
             upGL.x,upGL.y,upGL.z);
}

// render terrain
void Renderer::renderTerrain()
{
   // start timer
   viewer->starttimer();

   // update scene
   float aspectRatio = (float)viewportwidth/viewportheight;
   viewer->cache(camera->get_eye(), camera->get_dir(), camera->get_up(), aspectRatio);

   // render scene
   viewer->clear();
   viewer->render();

   // get time spent
   double delta=viewer->gettimer();

   // update quality parameters
   viewer->adapt(delta);
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

   float lx = 60.0f/viewportwidth;
   float ly = 60.0f/viewportheight;

   glColor4f(1.0f,1.0f,1.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f,  0.0f); glVertex2f(-lx, ly); // P0
   glTexCoord2f(1.0f,  0.0f); glVertex2f(lx, ly); // P1
   glTexCoord2f(1.0f,  1.0f); glVertex2f(lx, -ly); // P2
   glTexCoord2f(0.0f,  1.0f);  glVertex2f(-lx, -ly); // P3
   glEnd();

   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);

   // render text:

   minicoord cameraPosLLH = camera->get_eye();
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR)
      cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   double cameraAngle=camera->get_angle();

   minicoord cameraHit=camera->get_hit();
   double cameraHitDist=camera->get_hitdist();

   QString str;
   const QColor color(255, 255, 255);
   window->qglColor(color);
   int x = 10;
   int y = window->height() - 20;
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

      double hitElev=camera->get_elev(cameraHit);
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
   int sy = window->height() - y;

   if (!bIsDoublePrint)
   {
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
   else
   {
      window->qglColor(QColor(0, 0, 0, 255));
      window->renderText(sx+2, sy+2, str);
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
}

miniv3d Renderer::unprojectMouse()
{
   double x, y, z;
   GLdouble modelMatrix[16];
   GLdouble projMatrix[16];
   int viewport[4];

   if (!m_CursorValid)
      return(camera->get_dir());

   setupMatrix();

   glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
   glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
   glGetIntegerv(GL_VIEWPORT, viewport);

   // calculate the ray, starting from the opengl camera pos and passing through where mouse pointer is
   miniv3d pointerScreenPos(m_CursorScreenPos.x(), viewportheight-m_CursorScreenPos.y(), 0.0);
   gluUnProject(pointerScreenPos.x, pointerScreenPos.y, 0.0, modelMatrix, projMatrix, viewport, &x, &y, &z);
   miniv3d dir = miniv3d(x,y,z) - camera->get_eye_opengl();
   dir = viewer->rot_o2g(dir, camera->get_eye_opengl());
   dir.normalize();

   return(dir);
}

miniv3d Renderer::targetVector()
{
   miniv3d targetVec(0.0);

   // trace to find the hit point under current cursor
   minicoord target = camera->get_hit(camera->get_eye(), unprojectMouse());
   if (target != camera->get_eye())
      targetVec = target.vec - camera->get_eye().vec;

   return(targetVec);
}

miniv3d Renderer::cursorVector()
{
   miniv3d cursorVec(0.0);

   // trace to find the hit point under current focus
   minicoord hit = camera->get_hit();
   if (hit != camera->get_eye())
   {
      // trace to find the hit point under current cursor
      minicoord target = camera->get_hit(camera->get_eye(), unprojectMouse());
      if (target != camera->get_eye())
      {
         // find out the target vector from focus to cursor
         double elev1 = camera->get_eye().vec.getlength();
         double elev2 = target.vec.getlength();
         double scale = elev1 / elev2;
         cursorVec = scale * (target.vec - hit.vec);
      }
   }

   return(cursorVec);
}

void Renderer::rotateCamera(float dx, float dy)
{
   stopTransition();

   if (m_Shift)
   {
      camera->rotate_right(360 *dx);
      camera->rotate_up(180 * dy);

      camera->rotate_limit(-90.0,90.0);
   }
   else
   {
      double dist = camera->get_hitdist();

      camera->move_forward_plain(dist);

      camera->rotate_right(360 *dx);
      camera->rotate_up(180 * dy);

      camera->rotate_limit(-90.0,0.0);

      camera->move_forward_plain(-dist);
   }

   startIdling();
}

void Renderer::moveCameraForward(float delta)
{
   const double mindist = 1000.0; // minimum travel distance

   stopTransition();

   delta *= 3.0;
   if (delta > 1.0) delta = 1.0;
   else if (delta < -1.0) delta = -1.0;

   double dist = camera->get_hitdist();
   if (dist == 0.0) dist = camera->get_dist();
   if (dist < mindist) dist = mindist;

   camera->move(delta * dist * unprojectMouse());
   camera->move_above(VIEWER_HEIGHT_FLOOR);

   startIdling();
}

void Renderer::moveCameraSideward(float delta)
{
   const double mindist = 1000.0; // minimum travel distance

   stopTransition();

   delta *= 3.0;
   if (delta > 1.0) delta = 1.0;
   else if (delta < -1.0) delta = -1.0;

   double dist = camera->get_hitdist();
   if (dist == 0.0) dist = camera->get_dist();
   if (dist < mindist) dist = mindist;

   camera->move_right(-delta * dist);
   camera->move_above(VIEWER_HEIGHT_FLOOR);

   startIdling();
}

void Renderer::focusOnTarget()
{
   startTransition(camera->get_eye() + cursorVector());
}

void Renderer::processTransition(double dt)
{
   const double maxtime = 0.5; // maximum transition time interval (s)
   const double minspeed = 3000.0; // minimum speed (m/second)

   miniv3d dir = m_TargetCameraPos.vec - camera->get_eye().vec;
   double speed = dir.getlength() / maxtime;

   if (speed < minspeed) speed = minspeed;

   if (dir.getlength() > speed * dt)
   {
      dir.normalize();
      camera->move(dir * speed * dt);
      camera->move_above(VIEWER_HEIGHT_FLOOR);
   }
   else
   {
      camera->move(dir);
      camera->move_above(VIEWER_HEIGHT_FLOOR);

      stopTransition();
   }

   window->updateGL();
}

void Renderer::timerEvent(int timerId)
{
   const double minIdle = 2.0; // minimum idle time interval (s)

   if (timerId == m_IdlingTimerId)
   {
      if (!m_bInCameraTransition)
         window->updateGL();

      bool bPagingFinished = !viewer->getearth()->checkpending();

      if (!bPagingFinished)
         m_IdlingTimer.start();
      else
      {
         int deltaT = m_IdlingTimer.elapsed();
         double dt = deltaT / 1000.0f;

         if (dt>minIdle)
            stopIdling();
      }
   }
   else if (timerId == m_TransitionTimerId)
   {
      int deltaT = m_TransitionTimer.restart();
      double dt = deltaT / 1000.0f;

      processTransition(dt);
   }
}

void Renderer::startIdling()
{
   if (m_IdlingTimerId == -1)
      m_IdlingTimerId = window->startTimer((int)(1000.0/VIEWER_FPS));
}

void Renderer::stopIdling()
{
   if (m_IdlingTimerId != -1)
   {
      window->killTimer(m_IdlingTimerId);
      m_IdlingTimerId=-1;
   }
}

void Renderer::startTransition(minicoord target)
{
   stopTransition();

   m_TransitionTimerId = window->startTimer((int)(1000.0/VIEWER_FPS));
   m_TargetCameraPos = target;
   m_bInCameraTransition = true;
   m_TransitionTimer.start();
}

void Renderer::stopTransition()
{
   if (m_TransitionTimerId != -1)
   {
      window->killTimer(m_TransitionTimerId);
      m_TransitionTimerId = -1;
      m_bInCameraTransition = false;
   }
}

void Renderer::moveCursor(const QPoint& pos)
{
   m_CursorScreenPos = pos;
   m_CursorValid = true;
}

void Renderer::modifierKey(modifierKeys modifier, bool pressed)
{
   if (modifier==ModifierShift)
      m_Shift=pressed;
   else if (modifier==ModifierControl)
      m_Control=pressed;
   else if (modifier==ModifierMeta)
      m_Meta=pressed;
}

void Renderer::toggleWireframe()
{
   m_pViewerParams->usewireframe = !m_pViewerParams->usewireframe;

   startIdling();
}

void Renderer::toggleSeaSurface()
{
   if (m_pTerrainParams->sealevel==-MAXFLOAT) m_pTerrainParams->sealevel=0.0f;
   else m_pTerrainParams->sealevel=-MAXFLOAT;

   viewer->propagate();
   viewer->getearth()->getterrain()->update();

   startIdling();
}
