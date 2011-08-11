#include <QtGui>
#include <QtOpenGL/qgl.h>

#include <mini/minibase.h>
#include <mini/miniOGL.h>
#include <mini/miniearth.h>
#include <mini/miniterrain.h>
#include <mini/minirgb.h>

#include <mini/viewerbase.h>

#include "landscape.h"
#include "viewerconst.h"
#include "renderer.h"

static unsigned char VIEWER_BATHYMAP[VIEWER_BATHYWIDTH*4*2];
static unsigned char VIEWER_NPRBATHYMAP[VIEWER_NPRBATHYWIDTH*4*2];

Renderer::Renderer(QGLWidget* window)
{
   this->window = window;
   m_bIsInited = false;

   SAFE_DECLARE_STRING(m_strBaseURL, MAX_BASE_URL_LEN);
   SAFE_DECLARE_STRING(m_strBaseID, MAX_BASE_URL_LEN);
   SAFE_DECLARE_STRING(m_strBasePath1, MAX_BASE_URL_LEN);
   SAFE_DECLARE_STRING(m_strBasePath2, MAX_BASE_URL_LEN);
}

Renderer::~Renderer()
{
   SAFE_DELETE_STRING(m_strBaseURL);
   SAFE_DELETE_STRING(m_strBaseID);
   SAFE_DELETE_STRING(m_strBasePath1);
   SAFE_DELETE_STRING(m_strBasePath2);
}

bool Renderer::setMapURL(const char* baseurl, const char* baseid, const char* basepath1, const char* basepath2)
{
   if (baseurl == NULL || baseid == NULL || basepath1 == NULL || basepath2 == NULL)
      return false;

   strncpy(m_strBaseURL, baseurl, MAX_BASE_URL_LEN);
   strncpy(m_strBaseID, baseid, MAX_BASE_URL_LEN);
   strncpy(m_strBasePath1, basepath1, MAX_BASE_URL_LEN);
   strncpy(m_strBasePath2, basepath2, MAX_BASE_URL_LEN);

   return true;
}

void Renderer::initCamera(float latitude, float longitude, float altitude, float heading, float pitch, float fov, float nearplane, float farplane)
{
   m_Camera.pos.vec.x = latitude * 3600.0f;
   m_Camera.pos.vec.y = longitude * 3600.0f;
   m_Camera.pos.vec.z = altitude;
   m_Camera.pos.type = minicoord::MINICOORD_LLH;
   m_Camera.pos.convert2(minicoord::MINICOORD_ECEF);

   m_Camera.heading = heading;
   m_Camera.pitch = pitch;
   m_Camera.fov = fov;
   m_Camera.nearplane = nearplane;
   m_Camera.farplane = farplane;

   m_Camera.viewportwidth = window->width();
   m_Camera.viewportheight = window->height();

   m_Camera.doupdate = true;
   m_Camera.dooverride = false;
}

void Renderer::init()
{
   if (m_bIsInited) return;

   // set viewport size into camera again since this init will change viewport size
   m_Camera.viewportwidth = window->width();
   m_Camera.viewportheight = window->height();

   // create the viewer object
   viewer=new viewerbase();
   initParameters();

   if (!viewer->getearth()->load(m_strBaseURL, m_strBaseID, m_strBasePath1, m_strBasePath2, TRUE, FALSE))
   {
      QString message;
      message.sprintf("Unable to load map data from url=%s%s%s (resp. %s)\n",
                      m_strBaseURL,m_strBaseID,m_strBasePath1,m_strBasePath2);
      QMessageBox::warning(window, "Error", message, QMessageBox::Ok);
      m_bIsInited = false;
      return;
   }

   // load optional features
   viewer->getearth()->loadopts();

   // initialize VIS bathy map
   initVISbathymap();

   // initialize NPR bathy map
   initNPRbathymap();

   initView();

   m_fMoveCameraForward = 0.0f;
   m_bFastCameraMove = false;

   m_bCameraPanning = false;
   m_fMoveCameraX = 0.0f;
   m_fMoveCameraY = 0.0f;

   m_bCameraRotating = false;
   m_bInCameraTransition = false;
   m_CameraTransitionMode = TRANSITION_NONE;

   m_DisableCursorMoveTimerId = -1;
   m_bDisableCursorMoveEvent = false;

   m_bSetupFocusingOnTarget = false;
   m_bSetupResetMap = false;

   m_CursorGlobalPos = m_Camera.pos;
   m_CursorValid = false;

   // offscreen rendering
   m_FBOId = -1;
   m_DepthBufferId = -1;
   m_TerrainTextureId = -1;

   // load textures
   loadTextureFromResource(":/images/crosshair.png", m_CrosshairTextureId);

   m_bIsInited = true;
}

void Renderer::initParameters()
{
   // the viewing parameters
   viewerbase::VIEWER_PARAMS viewerParams;
   viewer->get(viewerParams);
   viewerParams.winwidth = m_Camera.viewportwidth;
   viewerParams.winheight = m_Camera.viewportheight;
   viewerParams.fps = VIEWER_FPS;
   viewerParams.fovy = m_Camera.fov;
   viewerParams.nearp = m_Camera.nearplane;
   viewerParams.farp = m_Camera.farplane;
   viewer->set(viewerParams);
   m_pViewerParams = viewer->get();

   // the earth parameters
   miniearth::MINIEARTH_PARAMS earthParams;
   viewer->getearth()->get(earthParams);
   earthParams.warpmode = 4;
   earthParams.nonlin = TRUE;
   earthParams.usefog = TRUE;
   earthParams.useshaders = FALSE;
   earthParams.usediffuse = FALSE;
   earthParams.usedetail = FALSE;
   earthParams.usevisshader = TRUE;
   earthParams.usebathymap = FALSE;
   earthParams.usecontours = FALSE;
   earthParams.usenprshader = FALSE;
   earthParams.useskydome = TRUE;
   earthParams.usewaypoints = TRUE;
   earthParams.usebricks = FALSE;
   earthParams.useearth = TRUE;
   earthParams.useflat = FALSE;
   earthParams.fogstart = VIEWER_FOGSTART;
   earthParams.fogdensity = VIEWER_FOGDENSITY;
   earthParams.voidstart = VIEWER_VOIDSTART;
   earthParams.abyssstart = VIEWER_ABYSSSTART;
   viewer->getearth()->set(earthParams);
   m_pEarthParams = viewer->getearth()->get();

   // the terrain parameters
   miniterrain::MINITERRAIN_PARAMS terrainParams;
   viewer->getearth()->getterrain()->get(terrainParams);
   terrainParams.scale = VIEWER_SCALE;
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
   terrainParams.bathymap = VIEWER_BATHYMAP;
   terrainParams.bathywidth = VIEWER_BATHYWIDTH;
   terrainParams.bathyheight = VIEWER_BATHYHEIGHT;
   terrainParams.bathycomps = VIEWER_BATHYCOMPS;
   terrainParams.nprbathystart = VIEWER_NPRBATHYSTART;
   terrainParams.nprbathyend = VIEWER_NPRBATHYEND;
   terrainParams.nprbathymap = VIEWER_NPRBATHYMAP;
   terrainParams.nprbathywidth = VIEWER_NPRBATHYWIDTH;
   terrainParams.nprbathyheight = VIEWER_NPRBATHYHEIGHT;
   terrainParams.nprbathycomps = VIEWER_NPRBATHYCOMPS;
   terrainParams.nprcontours = VIEWER_NPRCONTOURS;
   terrainParams.signpostheight = VIEWER_SIGNPOSTHEIGHT;
   terrainParams.signpostrange = VIEWER_SIGNPOSTRANGE;
   terrainParams.bricksize = VIEWER_BRICKSIZE;
   terrainParams.brickradius = VIEWER_BRICKRADIUS;
   terrainParams.brickpasses = 1;
   terrainParams.brickscroll = VIEWER_BRICKSCROLL;
   viewer->getearth()->getterrain()->set(terrainParams);
   m_pTerrainParams  =  viewer->getearth()->getterrain()->get();
}

// initialize the view point
void Renderer::initView()
{
   resizeViewport();
   initFBO();

   viewer->initeyepoint(m_Camera.pos);

   // setting up mini layers in camera
   m_Camera.refLayer = viewer->getearth()->getreference();
   m_Camera.nearestLayer = viewer->getearth()->getnearest(m_Camera.pos);

   minilayer* nst = m_Camera.nearestLayer;

   // initial camera pos
   minicoord cameraPos = m_Camera.pos;
   cameraPos.convert2(minicoord::MINICOORD_LLH);
   float fAltitude = cameraPos.vec.z;
   cameraPos.vec.z = 20000;
   cameraPos.convert2(minicoord::MINICOORD_ECEF);

   miniv3d vDown(0, 0, -1.0f);
   minicoord cameraPosLocal = nst->map_g2l(m_Camera.pos);
   vDown = nst->rot_l2g(vDown, cameraPosLocal);

   double dist = viewer->shoot(cameraPos, vDown);
   if (dist != MAXFLOAT)
      {
      cameraPos = cameraPos + vDown * dist;
      cameraPos.convert2(minicoord::MINICOORD_LLH);

      // test if the camera pos is underneath the ground, if yes, move it up:
      if (cameraPos.vec.z > fAltitude)
      {
         cameraPos.convert2(minicoord::MINICOORD_ECEF);
         m_Camera.pos = cameraPos;
      }
   }

   m_Camera.distToGroundHit = MAXFLOAT; // clear the dist to trigger recalculation
   viewer->initeyepoint(m_Camera.pos);
}

void Renderer::resize(int width, int height)
{
   m_Camera.viewportwidth = width;
   m_Camera.viewportheight = height;
   resizeViewport();

   window->updateGL();
}

// initialize the render window
void Renderer::resizeViewport()
{
   int winWidth = m_Camera.viewportwidth;
   int winHeight = m_Camera.viewportheight;

   if (winWidth<1) winWidth=1;
   if (winHeight<1) winHeight=1;

   m_pViewerParams->winwidth=winWidth;
   m_pViewerParams->winheight=winWidth;
   viewer->set(m_pViewerParams);

   glViewport(0, 0, winWidth, winWidth);

   resizeTextures(winWidth, winHeight);

   m_Camera.doupdate = true;
}

void Renderer::resizeTextures(int width, int height)
{
   // create terrain texture object
   if (m_TerrainTextureId > 0)
      glDeleteTextures(1, &m_TerrainTextureId);

   glGenTextures(1, &m_TerrainTextureId);
   glBindTexture(GL_TEXTURE_2D, m_TerrainTextureId);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   // create overlay texture object
   if (m_OverlayTextureId > 0)
      glDeleteTextures(1, &m_OverlayTextureId);

   glGenTextures(1, &m_OverlayTextureId);
   glBindTexture(GL_TEXTURE_2D, m_OverlayTextureId);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   // create the depth render buffer
   if (m_DepthBufferId > 0)
      glDeleteRenderbuffersEXT(1, &m_DepthBufferId);

   glGenRenderbuffersEXT(1, &m_DepthBufferId);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_DepthBufferId);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void Renderer::draw()
{
    renderLandscape();
}

void Renderer::updateCamera()
{
   m_Camera.updated = false;

   minilayer* nearestLayer = m_Camera.nearestLayer;

   // manage transition, for focusing on target.
   // we have to put it here since we have to have matrix setting up beforehand so we could use ray cast
   if (m_bSetupFocusingOnTarget || m_bSetupResetMap)
   {
      // calculate the ray, starting from the camera pos and passing through where mouse pointer is
      double x, y, z;
      GLdouble modelMatrix[16];
      GLdouble projMatrix[16];
      int viewport[4];

      glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
      glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
      glGetIntegerv(GL_VIEWPORT, viewport);

      miniv3d PointerScreenPos(m_CursorScreenPos.x(), window->height()-m_CursorScreenPos.y(), 0.0f);
      gluUnProject((GLdouble)PointerScreenPos.x, (GLdouble)PointerScreenPos.y, 0.0f, modelMatrix, projMatrix, viewport, &x, &y, &z);

      miniv3d rayVec = miniv3d(x,y,z) - miniv3d(m_Camera.posGL.vec);
      rayVec.normalize();

      // trace to find the distance to current focus point
      m_Camera.distToGroundHit = viewer->shoot(m_Camera.pos, m_Camera.forward);
      if (m_Camera.distToGroundHit != MAXFLOAT)
      {
         m_Camera.posGroundHit = m_Camera.pos + m_Camera.distToGroundHit * m_Camera.forward;

         miniv3d targetRay = m_Camera.nearestLayer->rot_o2g(rayVec, m_Camera.pos);

         double distToTargetHit = viewer->shoot(m_Camera.pos, targetRay);
         if (distToTargetHit != MAXFLOAT)
         {
            // we have a target hit too, calculate the target camera pos and start the transition
            minicoord targetHitPos = m_Camera.pos + distToTargetHit * targetRay;

            if (m_bSetupFocusingOnTarget)
            {
               // find out the target position of camera transition
               minicoord cameraTargetPos = targetHitPos - m_Camera.distToGroundHit * m_Camera.forward;

               // convert into opengl space and limit the camera height within ceiling
               minicoord cameraTargetPosGL = m_Camera.nearestLayer->map_g2o(cameraTargetPos);
               double cameraHeightCeiling = m_Camera.nearestLayer->len_g2o(CAMERA_HEIGHT_CEILING);

               // camera is over ceiling, fix it
               if (cameraTargetPosGL.vec.y > cameraHeightCeiling)
               {
                  double t = (cameraTargetPosGL.vec.y - cameraHeightCeiling) / rayVec.z;
                  cameraTargetPosGL -= t * rayVec;
               }

               m_TargetCameraPos = m_Camera.nearestLayer->map_o2g(cameraTargetPosGL);
               m_TargetCameraPos.type = minicoord::MINICOORD_ECEF;

               startTransition(TRANSITION_FOCUS_ON_TARGET);
            }
            else if (m_bSetupResetMap)
            {
               // straight looking down at the target pos
               float targetCameraHeight = m_Camera.distToGroundHit > CAMERA_HEIGHT_CEILING ? CAMERA_HEIGHT_CEILING : m_Camera.distToGroundHit;
               targetHitPos.convert2(minicoord::MINICOORD_LLH);
               targetHitPos.vec.z = targetCameraHeight;
               targetHitPos.convert2(minicoord::MINICOORD_ECEF);
               m_TargetCameraPos = targetHitPos;
               m_TargetCameraPos.type = minicoord::MINICOORD_ECEF;
               startTransition(TRANSITION_RESET_MAP);
            }
         }
      }

      m_bSetupFocusingOnTarget = false;
      m_bSetupResetMap = false;
   }

   // update camera
   if (m_Camera.doupdate || m_fMoveCameraForward != 0.0f || m_bCameraPanning)
   {
      minicoord cameraPos = m_Camera.pos;
      minicoord cameraPosLocal = nearestLayer->map_g2l(cameraPos);

      float sindir = sin(2.0*PI/360.0*m_Camera.heading);
      float cosdir = cos(2.0*PI/360.0*m_Camera.heading);

      float sinpitch = sin(2.0*PI/360.0*m_Camera.pitch);
      float cospitch = cos(2.0*PI/360.0*m_Camera.pitch);

      // update eye coordinate system:
      miniv3d dirLocal;
      dirLocal.x = sindir * cospitch;
      dirLocal.y = cosdir * cospitch;
      dirLocal.z = -sinpitch;

      miniv3d upLocal;
      upLocal.x = sindir * sinpitch;
      upLocal.y = cosdir * sinpitch;
      upLocal.z = cospitch;

      miniv3d rightLocal;
      rightLocal.x = cosdir;
      rightLocal.y = -sindir;
      rightLocal.z = 0.0f;

      // update camera movement
      if (!m_Camera.dooverride)
      {
         if (m_bCameraPanning)
         {
            double speed = 20.0f;
            if (m_bFastCameraMove)
               speed *= 10.0f;

            m_bCameraPanning = false;
            cameraPosLocal.vec.x += rightLocal.x * m_fMoveCameraX * speed;
            cameraPosLocal.vec.y += rightLocal.y * m_fMoveCameraX * speed;
            cameraPosLocal.vec.z += rightLocal.z * m_fMoveCameraX * speed;

            miniv3d flatUp = upLocal;
            flatUp.z = 0.0f;
            flatUp.normalize();

            cameraPosLocal.vec.x += flatUp.x * m_fMoveCameraY * speed;
            cameraPosLocal.vec.y += flatUp.y * m_fMoveCameraY * speed;
            cameraPosLocal.vec.z += flatUp.z * m_fMoveCameraY * speed;

            m_fMoveCameraX = 0.0f;
            m_fMoveCameraY = 0.0f;

            // clear the dist so it will be detected again next time do rotating
            m_Camera.distToGroundHit = MAXFLOAT;
         }
         else
         {
            double cameraHeightCeiling = nearestLayer->len_g2l(CAMERA_HEIGHT_CEILING);

            if (m_bCameraRotating)
            {
               m_bCameraRotating = false;
               if (m_Camera.distToGroundHit == MAXFLOAT)
               {
                  m_Camera.distToGroundHit = viewer->shoot(m_Camera.pos, m_Camera.forward);
                  if (m_Camera.distToGroundHit != MAXFLOAT)
                  {
                     m_Camera.posGroundHit = m_Camera.pos + m_Camera.distToGroundHit * m_Camera.forward;
                  }
                  else
                  {
                     m_Camera.posGroundHit = m_Camera.pos;
                  }
               }
               if (m_Camera.distToGroundHit != MAXFLOAT)
               {
                  minicoord posLookAtLocal = nearestLayer->map_g2l(m_Camera.posGroundHit);
                  double distToLookAtLocal = nearestLayer->len_g2l(m_Camera.distToGroundHit);

                  cameraPosLocal = posLookAtLocal - distToLookAtLocal * dirLocal;

                  if (cameraPosLocal.vec.z > cameraHeightCeiling)
                  {
                     double newDist = -(cameraHeightCeiling - posLookAtLocal.vec.z)/dirLocal.z;
                     cameraPosLocal = posLookAtLocal - newDist * dirLocal;
                  }

                  // if we process camera rotation, we stop process mouse wheels since we use middle button for rotation
                  m_fMoveCameraForward = 0.0f;
               }
            }

            if (m_fMoveCameraForward != 0.0f)
            {
               double speed = 2.0;
               if (m_bFastCameraMove)
                  speed *= 10.0f;

               double delta = speed * m_fMoveCameraForward;
               cameraPosLocal.vec.x += dirLocal.x * delta;
               cameraPosLocal.vec.y += dirLocal.y * delta;
               cameraPosLocal.vec.z += dirLocal.z * delta;

               double cameraHeightFloor = m_Camera.nearestLayer->len_g2o(CAMERA_HEIGHT_FLOOR);

               if (cameraPosLocal.vec.z > cameraHeightCeiling)
               {
                  double overHeight = cameraPosLocal.vec.z - cameraHeightCeiling;
                  double adjustRatio = overHeight / (dirLocal.z * delta);
                  cameraPosLocal.vec.x -= dirLocal.x * delta * adjustRatio;
                  cameraPosLocal.vec.y -= dirLocal.y * delta * adjustRatio;
                  cameraPosLocal.vec.z -= dirLocal.z * delta * adjustRatio;
               }

               if (cameraPosLocal.vec.z < cameraHeightFloor)
               {
                  double overHeight =  cameraPosLocal.vec.z - cameraHeightFloor;
                  double adjustRatio = overHeight / (dirLocal.z * delta);
                  cameraPosLocal.vec.x -= dirLocal.x * delta * adjustRatio;
                  cameraPosLocal.vec.y -= dirLocal.y * delta * adjustRatio;
                  cameraPosLocal.vec.z -= dirLocal.z * delta * adjustRatio;
               }

               m_fMoveCameraForward = 0.0f;

               // clear the dist so it will be detected again next time do rotating
               m_Camera.distToGroundHit = MAXFLOAT;
            }
         }

         miniv3d dir = nearestLayer->rot_l2g(dirLocal, cameraPosLocal);
         miniv3d up = nearestLayer->rot_l2g(upLocal, cameraPosLocal);
         miniv3d right = nearestLayer->rot_l2g(rightLocal, cameraPosLocal);

         m_Camera.forward = dir;
         m_Camera.side = right;
         m_Camera.up = up;

         // remap eye coordinates
         cameraPos = nearestLayer->map_l2g(cameraPosLocal);
         m_Camera.pos = cameraPos;

         m_Camera.posGL = nearestLayer->map_g2o(m_Camera.pos);
         m_Camera.forwardGL = nearestLayer->rot_g2o(m_Camera.forward, m_Camera.pos);
         m_Camera.upGL = nearestLayer->rot_g2o(m_Camera.up, m_Camera.pos);
         m_Camera.sideGL = nearestLayer->rot_g2o(m_Camera.side, m_Camera.pos);
      }
      else
      {
         // camera movement has been overridden
         if (m_CameraTransitionMode == TRANSITION_FOCUS_ON_TARGET)
         {
            miniv3d cameraForwardLocal = nearestLayer->rot_g2l(m_Camera.forward, m_Camera.pos);
            miniv3d cameraSideLocal = nearestLayer->rot_g2l(m_Camera.side, m_Camera.pos);
            miniv3d cameraUpLocal = nearestLayer->rot_g2l(m_Camera.up, m_Camera.pos);

            m_Camera.pos = m_TransitingCameraPos;

            minicoord cameraPosLocal = nearestLayer->map_g2l(m_Camera.pos);

            m_Camera.forward = nearestLayer->rot_l2g(cameraForwardLocal, cameraPosLocal);
            m_Camera.side = nearestLayer->rot_l2g(cameraSideLocal, cameraPosLocal);
            m_Camera.up = nearestLayer->rot_l2g(cameraUpLocal, cameraPosLocal);

            m_Camera.posGL = nearestLayer->map_g2o(m_Camera.pos);
            m_Camera.forwardGL = nearestLayer->rot_g2o(m_Camera.forward, m_Camera.pos);
            m_Camera.upGL = nearestLayer->rot_g2o(m_Camera.up, m_Camera.pos);
            m_Camera.sideGL = nearestLayer->rot_g2o(m_Camera.side, m_Camera.pos);
         }
         else if (m_CameraTransitionMode == TRANSITION_RESET_MAP)
         {
            miniv3d cameraForwardLocal(0.0f, 0.0f, -1.0f);
            miniv3d cameraSideLocal(1.0f, 0.0f, 0.0f);
            miniv3d cameraUpLocal(0.0f, 1.0f, 0.0f);

            m_Camera.heading = 0.0f;
            m_Camera.pitch = 90.0f;

            m_Camera.pos = m_TargetCameraPos;

            minicoord cameraPosLocal = nearestLayer->map_g2l(m_Camera.pos);

            m_Camera.forward = nearestLayer->rot_l2g(cameraForwardLocal, cameraPosLocal);
            m_Camera.side = nearestLayer->rot_l2g(cameraSideLocal, cameraPosLocal);
            m_Camera.up = nearestLayer->rot_l2g(cameraUpLocal, cameraPosLocal);

            m_Camera.posGL = nearestLayer->map_g2o(m_Camera.pos);
            m_Camera.forwardGL = nearestLayer->rot_g2o(m_Camera.forward, m_Camera.pos);
            m_Camera.upGL = nearestLayer->rot_g2o(m_Camera.up, m_Camera.pos);
            m_Camera.sideGL = nearestLayer->rot_g2o(m_Camera.side, m_Camera.pos);
         }

         m_Camera.dooverride = false;
      }

      m_Camera.nearestLayer = viewer->getearth()->getnearest(m_Camera.pos);

      updateFrustum();

      m_Camera.updated = true;
   }

   // clear the do update flag
   m_Camera.doupdate = false;
}

void Renderer::updateFrustum()
{
   // build frustum in local (camera) coordinate
   float fAspectRatio = ((float)m_Camera.viewportwidth)/((float) m_Camera.viewportheight);
   double nearpGL = m_Camera.refLayer->len_g2o(m_Camera.nearplane);
   double farpGL = m_Camera.refLayer->len_g2o(m_Camera.farplane);

   double neartopGL = tan((m_Camera.fov / 2.0) * (PI/180.0f)) * nearpGL;
   double nearrightGL = fAspectRatio * neartopGL;
   double nearbottomGL = -neartopGL;
   double nearleftGL = -nearrightGL;

   double fartopGL = tan((m_Camera.fov / 2.0) * (PI/180.0f)) * farpGL;
   double farrightGL = fAspectRatio * fartopGL;
   double farbottomGL = -fartopGL;
   double farleftGL = -farrightGL;

   miniv3d frustumPoints[8] =
   {
      miniv3d(nearleftGL, nearbottomGL, -nearpGL),
      miniv3d(nearleftGL, neartopGL, -nearpGL),
      miniv3d(nearrightGL, neartopGL, -nearpGL),
      miniv3d(nearrightGL, nearbottomGL, -nearpGL),
      miniv3d(farleftGL, farbottomGL, -farpGL),
      miniv3d(farleftGL, fartopGL, -farpGL),
      miniv3d(farrightGL, fartopGL, -farpGL),
      miniv3d(farrightGL, farbottomGL, -farpGL)
   };

   // transform into global (opengl) coordinate
   miniv3d vx = m_Camera.sideGL;
   miniv3d vy = m_Camera.upGL;
   miniv3d vz = -m_Camera.forwardGL;
   miniv4d vt = m_Camera.posGL.vec;

   miniv4d cameraToWorldMtx[3] =
   {
      miniv4d(vx.x, vy.x, vz.x, vt.x),
      miniv4d(vx.y, vy.y, vz.y, vt.y),
      miniv4d(vx.z, vy.z, vz.z, vt.z)
   };

   for (int i = 0; i < 8; i++)
   {
      miniv4d p(frustumPoints[i].x, frustumPoints[i].y, frustumPoints[i].z, 1.0f);
      m_Camera.frustumPointsGL[i].x = cameraToWorldMtx[0] * p;
      m_Camera.frustumPointsGL[i].y = cameraToWorldMtx[1] * p;
      m_Camera.frustumPointsGL[i].z = cameraToWorldMtx[2] * p;
   }

   // construct frustum planes
   CalculateFrustumPlanes(m_Camera.frustumPointsGL, m_Camera.frustumPlanesGL);
}

void Renderer::setupMatrix()
{
   glViewport(0.0f, 0.0f, m_Camera.viewportwidth, m_Camera.viewportheight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float fAspectRatio = ((float)m_Camera.viewportwidth)/((float) m_Camera.viewportheight);
   double nearpGL = m_Camera.refLayer->len_g2o(m_pViewerParams->nearp);
   double farpGL = m_Camera.refLayer->len_g2o(m_pViewerParams->farp);

   gluPerspective(m_pViewerParams->fovy, fAspectRatio, nearpGL, farpGL);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(m_Camera.posGL.vec.x, m_Camera.posGL.vec.y, m_Camera.posGL.vec.z,
             m_Camera.posGL.vec.x+m_Camera.forwardGL.x, m_Camera.posGL.vec.y+m_Camera.forwardGL.y, m_Camera.posGL.vec.z+m_Camera.forwardGL.z,
             m_Camera.upGL.x,m_Camera.upGL.y,m_Camera.upGL.z);
}

void Renderer::renderTerrain()
{
   minilayer* nearestLayer = m_Camera.nearestLayer;

   // render scene
   if (m_Camera.updated)
   {
      // start timer
      viewer->starttimer();

      viewer->getearth()->getterrain()->setreference(viewer->getearth()->getterrain()->getnum(nearestLayer));

      // update scene
      float fAspectRatio = ((float)m_Camera.viewportwidth)/((float) m_Camera.viewportheight);
      viewer->cache(m_Camera.pos, m_Camera.forward, m_Camera.up, fAspectRatio);

      attachTexture(m_TerrainTextureId, m_DepthBufferId);
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      viewer->clear();
      viewer->render();

      // get time spent
      double delta=viewer->gettimer();

      // update quality parameters
      viewer->adapt(delta);
   }
}

void Renderer::renderComposition()
{
   glViewport(0.0f, 0.0f, m_Camera.viewportwidth, m_Camera.viewportheight);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   // draw terrain
   glBindTexture(GL_TEXTURE_2D, m_TerrainTextureId);
   drawFullscreenTexQuad();

   // draw overlay
   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_GREATER, 0);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
   glBindTexture(GL_TEXTURE_2D, m_OverlayTextureId);
   drawFullscreenTexQuad();
   glDisable(GL_BLEND);
}

void Renderer::renderOverlay()
{
   attachTexture(m_OverlayTextureId, m_DepthBufferId);
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);
   glDepthMask(GL_FALSE);

   glDepthMask(GL_TRUE);
}

void Renderer::renderLandscape()
{
   setupMatrix();
   updateCamera();

   // bind FBO to render into texture
   bindFBO();
   setupMatrix();
   renderTerrain();
   renderOverlay();
   unbindFBO();

   renderComposition();
   renderHUD();
}

void Renderer::initFBO()
{
   // create a framebuffer object
   glGenFramebuffersEXT(1, &m_FBOId);
   bindFBO();

   attachTexture(m_TerrainTextureId, m_DepthBufferId);
   attachTexture(m_OverlayTextureId, m_DepthBufferId);

   unbindFBO();
}

void Renderer::bindFBO()
{
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FBOId);
}

void Renderer::unbindFBO()
{
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Renderer::attachTexture(int textureId, int depthId)
{
   if (textureId<0) return;

   // attach a texture to FBO color attachment point
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0);
   // attach a renderbuffer to depth attachment point
   if (depthId>=0)
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthId);

   // check FBO status
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
      printf("Binding texture error. Error Code: %d\n", (int)status);
}

void Renderer::loadTextureFromResource(const char* respath, GLuint& texId)
{
   QImage tex, buf;
   bool bLoaded = buf.load(respath);

   if (bLoaded)
      tex = QGLWidget::convertToGLFormat( buf );

   glGenTextures( 1, &texId );
   glBindTexture( GL_TEXTURE_2D, texId );
   glTexImage2D( GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void Renderer::drawRefFrame(miniv3d pos)
{
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);

   glScalef(20.0,20.0f,20.0f);
   glDisable(GL_DEPTH_TEST);
   glLineWidth(1);

   glColor3f(1.0f,0.0f,0.0f);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(1.0f, 0.0f, 0.0f);
   glEnd();

   glColor3f(0.0f,1.0f,0.0f);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f,0.0f, -1.0f);
   glEnd();

   glColor3f(0.0f,0.0f,1.0f);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(0.0f,1.0f, 0.0f);
   glEnd();

   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
}

void Renderer::drawLine(miniv3d pos1, miniv3d pos2, int width)
{
   miniv3d delta = pos2 - pos1;

   glPushMatrix();
   glTranslatef(pos1.x, pos1.y, pos1.z);
   glDisable(GL_DEPTH_TEST);
   glLineWidth(width);

   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(delta.x, delta.y, delta.z);
   glEnd();

   glLineWidth(1);
   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
}

void Renderer::drawFullscreenTexQuad()
{
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
   glBegin(GL_QUADS);
      glColor4f(1, 1, 1, 1);
      glTexCoord2f(1, 1);  glVertex3f(1,1,1);
      glTexCoord2f(0, 1);  glVertex3f(-1,1,1);
      glTexCoord2f(0, 0);  glVertex3f(-1,-1,1);
      glTexCoord2f(1, 0);  glVertex3f(1,-1,1);
   glEnd();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
}

void Renderer::initVISbathymap()
{
   float rgba[4];
   static const float hue1=0.0f;
   static const float hue2=240.0f;

   for (int i=0; i<VIEWER_BATHYWIDTH; i++)
   {
      float t=(float)i/(VIEWER_BATHYWIDTH-1);

      hsv2rgb(hue1+(hue2-hue1)*t,1.0f,1.0f,rgba);

      rgba[3]=fpow(1.0f-fabs(2.0f*(t-0.5f)),1.0f/3);

      VIEWER_BATHYMAP[4*i]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)]=ftrc(255.0f*rgba[0]+0.5f);
      VIEWER_BATHYMAP[4*i+1]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+1]=ftrc(255.0f*rgba[1]+0.5f);
      VIEWER_BATHYMAP[4*i+2]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+2]=ftrc(255.0f*rgba[2]+0.5f);
      VIEWER_BATHYMAP[4*i+3]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+3]=ftrc(255.0f*rgba[3]+0.5f);
   }
}

void Renderer::initNPRbathymap()
{
   float rgba[4];

   static const float hue1=120.0f;
   static const float hue2=60.0f;

   static const float hue_ctr=60.0f;
   static const float sat_ctr=0.75f;
   static const float val_ctr=0.25f;

   for (int i=0; i<VIEWER_NPRBATHYWIDTH; i++)
   {
      float t=(float)i/(VIEWER_NPRBATHYWIDTH-1);

      float alpha=t*fabs(VIEWER_NPRBATHYEND-VIEWER_NPRBATHYSTART)/VIEWER_CONTOURS;
      alpha=alpha-ftrc(alpha);

      if (t<0.5f) hsv2rgb(hue1+(hue2-hue1)*t,t,1.0f,rgba);
      else hsv2rgb(hue1+(hue2-hue1)*t,1.0f-t,1.0f,rgba);

      rgba[3]=0.5f;

      if (alpha>0.9f) hsv2rgb(hue_ctr,sat_ctr,val_ctr,rgba);

      VIEWER_NPRBATHYMAP[4*i]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)]=ftrc(255.0f*rgba[0]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+1]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+1]=ftrc(255.0f*rgba[1]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+2]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+2]=ftrc(255.0f*rgba[2]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+3]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+3]=ftrc(255.0f*rgba[3]+0.5f);
   }
}

void Renderer::CalculateFrustumPlanes(miniv3d* points, miniv4d* planes)
{
   // all planes pointing inward
   // three points in clockwise order when normal pointing to you
   planes[0] = points2plane(points[4], points[5], points[6]); // far plane
   planes[1] = points2plane(points[2], points[6], points[5]); // top
   planes[2] = points2plane(points[0], points[4], points[7]); // bottom
   planes[3] = points2plane(points[1], points[5], points[4]); // left
   planes[4] = points2plane(points[7], points[6], points[2]); // right
   planes[5] = points2plane(points[2], points[1], points[0]); // near plane
}

void Renderer::FindMinMax(const miniv4d& pos, float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ)
{
   if (pos.x < minX) minX = pos.x;
   if (pos.x > maxX) maxX = pos.x;
   if (pos.y < minY) minY = pos.y;
   if (pos.y > maxY) maxY = pos.y;
   if (pos.z < minZ) minZ = pos.z;
   if (pos.z > maxZ) maxZ = pos.z;
}

miniv4d Renderer::points2plane(const miniv3d& v0, const miniv3d& v1, const miniv3d& v2)
{
   miniv3d vec0, vec1, vecNormal;

   vec0 = v2 - v0;
   vec1 = v1 - v0;
   vecNormal = vec0/vec1;
   vecNormal.normalize();
   double w = vecNormal * v0;

   return miniv4d(vecNormal.x, vecNormal.y, vecNormal.z, w);
}

minicoord Renderer::trace2ground(minicoord point, double& dist)
{
   // down vector
   minilayer *nst = viewer->getearth()->getnearest(point);
   miniv3d vDown(0, 0, -1.0f);
   minicoord posLocal = nst->map_g2l(point);
   vDown = nst->rot_l2g(vDown, posLocal);

   // ray casting to ground
   point.convert2(minicoord::MINICOORD_LLH);
   minicoord rayCastSrc(point.vec.x, point.vec.y, 20000, minicoord::MINICOORD_LLH);
   rayCastSrc.convert2(minicoord::MINICOORD_ECEF);
   dist=viewer->shoot(rayCastSrc, vDown);

   // if we have a hit, we have a proj position on ground, or we use pos
   minicoord posProj = point;
   if (dist != MAXFLOAT)
   {
      posProj = rayCastSrc + dist*vDown;
   }

   return posProj;
}

using namespace std;

bool Renderer::isBoundingBoxVisible(const BoundingBox& bb)
{
   miniv3d points[8] =
   {
      bb.minPoint,
      miniv3d(bb.maxPoint.x, bb.minPoint.y, bb.minPoint.z),
      miniv3d(bb.maxPoint.x, bb.minPoint.y, bb.maxPoint.z),
      miniv3d(bb.minPoint.x, bb.minPoint.y, bb.maxPoint.z),
      miniv3d(bb.minPoint.x, bb.maxPoint.y, bb.minPoint.z),
      miniv3d(bb.maxPoint.x, bb.maxPoint.y, bb.minPoint.z),
      bb.maxPoint,
      miniv3d(bb.minPoint.x, bb.maxPoint.y, bb.maxPoint.z)
   };

   bool bVisible = true;
   for (int i = 0; i < 6; i++)
   {
      bool bPointInside = false;
      for (int j = 0; j < 8; j++)
      {
         miniv4d p(points[j].x, points[j].y, points[j].z, -1.0f);
         if (p * m_Camera.frustumPlanesGL[i] > 0)
         {
            bPointInside = true;
            break;
         }
      }
      if (!bPointInside)
      {
         bVisible = false;
         break;
      }
   }

   return bVisible;
}

bool Renderer::RayBoundingBoxIntersect(const BoundingBox& bb, const Ray& r)
{
   miniv3d parameters[2];
   parameters[0] = bb.minPoint;
   parameters[1] = bb.maxPoint;

   float t0 = 0.0f, t1 = MAXFLOAT;
   float tmin, tmax, tymin, tymax, tzmin, tzmax;

   tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
   tmax = (parameters[1-r.sign[0]].x - r.origin.x) * r.inv_direction.x;
   tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
   tymax = (parameters[1-r.sign[1]].y - r.origin.y) * r.inv_direction.y;

   if ( (tmin > tymax) || (tymin > tmax) )
      return false;

   if (tymin > tmin)
      tmin = tymin;
   if (tymax < tmax)
      tmax = tymax;

   tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
   tzmax = (parameters[1-r.sign[2]].z - r.origin.z) * r.inv_direction.z;

   if ( (tmin > tzmax) || (tzmin > tmax) )
      return false;

   if (tzmin > tmin)
      tmin = tzmin;
   if (tzmax < tmax)
      tmax = tzmax;

   return ( (tmin < t1) && (tmax > t0) );
}

void Renderer::renderHUD()
{
   minilayer *nst=viewer->getearth()->getnearest(m_Camera.pos);

   double cameraElev=viewer->getearth()->getterrain()->getheight(m_Camera.pos);
   if (cameraElev==-MAXFLOAT) cameraElev=0.0f;

   minicoord cameraPosLLH = nst->map_g2t(m_Camera.pos);
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR) cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   // draw crosshair
   {
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

      float lx = 60.0f/(window->width());
      float ly = 60.0f/(window->height());

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
   }

   QString str;
   const QColor colorRed(255, 255, 255);
   window->qglColor(colorRed);
   int x = 10;
   int y = window->height() - 20;
   int line_space = -16;
   int second_column_offset = 90;

   str.sprintf("Camera Data");
   drawText(x, y, str);
   y+=line_space;

   str.sprintf("=============");
   drawText(x, y, str);
   y+=line_space;

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
   str.sprintf("%-6.2f m", cameraPosLLH.vec.z);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Heading:");
   drawText(x, y, str);
   str.sprintf("%03d", (int)m_Camera.heading);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Pitch:");
   drawText(x, y, str);
   str.sprintf("%-6.0f", m_Camera.pitch);
   drawText(x+second_column_offset, y, str);
   y+=line_space;
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

// user input handlers
void Renderer::rotateCamera(float dx, float dy)
{
   m_Camera.heading += 180 * dx;
   if (m_Camera.heading >= 360)
      m_Camera.heading -= 360;
   if (m_Camera.heading < 0)
      m_Camera.heading += 360;

   m_Camera.pitch += 180 * dy;
   if (m_Camera.pitch < 5) m_Camera.pitch = 5;
   if (m_Camera.pitch > 90) m_Camera.pitch = 90;

   m_bCameraRotating = true;
   m_Camera.doupdate = true;

   window->updateGL();
}

void Renderer::moveCamera(float dx, float dy)
{
   minicoord tempCamPos = m_Camera.pos;
   tempCamPos.convert2(minicoord::MINICOORD_LLH);

   float speedAdj = fmax(1.0f, (tempCamPos.vec.z / 45000.0) * 10.0f);

   m_bCameraPanning = true;
   m_fMoveCameraX += dx * speedAdj;
   m_fMoveCameraY += dy * speedAdj;
   m_Camera.doupdate = true;

   window->updateGL();
}

void Renderer::setCameraFastMoveForward(bool bEnable)
{
   m_bFastCameraMove = bEnable;
}

void Renderer::moveCameraForward(float delta)
{
   m_fMoveCameraForward += delta;
   m_Camera.doupdate = true;

   // we alreday have the timer running
   if (m_DisableCursorMoveTimerId != -1)
   {
      // kill timer and restart it
      window->killTimer(m_DisableCursorMoveTimerId);
      //restart timer
      m_DisableCursorMoveTimerId = window->startTimer(500); // half a second
   }
   else
   {
      // start timer
      m_DisableCursorMoveTimerId = window->startTimer(500); // half a second
      m_bDisableCursorMoveEvent = true;
   }

   window->updateGL();
}

void Renderer::resetMap()
{
   if (m_CameraTransitionMode == TRANSITION_NONE)
   {
      m_bSetupResetMap = true;
      window->updateGL();
   }
}

bool Renderer::processResetMap(int )
{
   if (m_Camera.pos == m_TargetCameraPos)
   {
      // clear the dist so it will be detected again next time do rotating
      m_Camera.distToGroundHit = MAXFLOAT;
      return true;
   }

   m_Camera.doupdate = true;
   m_Camera.dooverride = true;
   window->updateGL();

   return false;
}

void Renderer::resetMapOrientation()
{
   if (m_CameraTransitionMode == TRANSITION_NONE)
   {
      m_HeadingRotateDirection = (m_Camera.heading > 180)? 1 : -1;
      startTransition(TRANSITION_RESET_HEADING);
   }
}

bool Renderer::processResetMapOrientation(int deltaT)
{
   const float speed = 360.0f / 0.5f; // speed is 0.5s for a whole round
   float deltaAngle = ((float)deltaT) / 1000.0f * speed;

   float newheading = m_Camera.heading + deltaAngle * m_HeadingRotateDirection;

   bool bFinished = false;
   if (m_HeadingRotateDirection > 0)
   {
      if (newheading > 360)
      {
         newheading = 0;
         bFinished = true;
      }
   }
   else
   {
      if (newheading < 0)
      {
         newheading  = 0;
         bFinished = true;
      }
   }

   m_Camera.heading = newheading;
   m_bCameraRotating = true;
   m_Camera.doupdate = true;

   window->updateGL();

   return bFinished;
}

void Renderer::focusOnTarget()
{
   if (m_CameraTransitionMode == TRANSITION_NONE)
   {
      m_bSetupFocusingOnTarget = true;
      window->updateGL();
   }
}

bool Renderer::processFocusOnTarget(int deltaT)
{
   const double speed = 100000.0f; // speed is 100km/second
   double dt = ((float)deltaT) / 1000.0f;

   // we delay one update in order to make sure the last step is moved properly
   if (m_TransitingCameraPos == m_TargetCameraPos)
   {
      // clear the dist so it will be detected again next time do rotating
      m_Camera.distToGroundHit = MAXFLOAT;
      return true;
   }

   miniv3d dirVec = m_TargetCameraPos.vec - m_Camera.pos.vec;
   if (dirVec.getlength() > speed * dt)
   {
      dirVec.normalize();
      m_TransitingCameraPos = m_Camera.pos + dirVec * speed * dt;
   }
   else
   {
      m_TransitingCameraPos = m_TargetCameraPos;
   }

   m_Camera.doupdate = true;
   m_Camera.dooverride = true;

   window->updateGL();

   return false;
}

void Renderer::timerEvent(int timerId)
{
   if (m_MapTransitionTimerId == timerId && m_bInCameraTransition)
   {
      int deltaT = m_Timer.restart();

      bool bTransitionFinished = false;
      switch(m_CameraTransitionMode)
      {
      case TRANSITION_RESET_HEADING:
         bTransitionFinished = processResetMapOrientation(deltaT);
         break;
      case TRANSITION_RESET_MAP:
         bTransitionFinished = processResetMap(deltaT);
         break;
      case TRANSITION_FOCUS_ON_TARGET:
         bTransitionFinished = processFocusOnTarget(deltaT);
         break;
      default:
         break;
      }

      if (bTransitionFinished)
      {
         stopTransition();
      }
   }
   else if (m_DisableCursorMoveTimerId == timerId)
   {
      // kill timer
      window->killTimer(m_DisableCursorMoveTimerId);
      m_DisableCursorMoveTimerId = -1;
      m_bDisableCursorMoveEvent = false;
      window->updateGL();
   }
}

void Renderer::startTransition(CameraTransitionMode mode)
{
   m_bInCameraTransition = true;
   m_CameraTransitionMode = mode;
   m_MapTransitionTimerId = window->startTimer(0);
   m_Timer.start();
}

void Renderer::stopTransition()
{
   if (m_CameraTransitionMode != TRANSITION_NONE)
   {
      window->killTimer(m_MapTransitionTimerId);
      m_CameraTransitionMode = TRANSITION_NONE;
      m_bInCameraTransition = false;
   }
}

void Renderer::moveCursor(const QPoint& pos)
{
   m_CursorScreenPos = pos;

   if (!m_bDisableCursorMoveEvent)
      window->updateGL();
}
