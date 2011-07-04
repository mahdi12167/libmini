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

// user input handlers
void Renderer::rotateCamera(float dx, float dy)
{
   if (m_bFreeCamera)
   {
      m_CameraSave = m_Camera;
      m_Camera = m_DebugCamera;
   }

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

   if (m_bFreeCamera)
   {
      m_DebugCamera = m_Camera;
      m_Camera = m_CameraSave;
   }

   window->updateGL();
}

void Renderer::moveCamera(float dx, float dy)
{
   if (m_bFreeCamera)
   {
      m_CameraSave = m_Camera;
      m_Camera = m_DebugCamera;
   }

   minicoord tempCamPos = m_Camera.pos;
   tempCamPos.convert2(minicoord::MINICOORD_LLH);

   float speedAdj = fmax(1.0f, (tempCamPos.vec.z / 45000.0) * 10.0f);

   m_bCameraPanning = true;
   m_fMoveCameraX += dx * speedAdj;
   m_fMoveCameraY += dy * speedAdj;
   m_Camera.doupdate = true;

   if (m_bFreeCamera)
   {
      m_DebugCamera = m_Camera;
      m_Camera = m_CameraSave;
   }

   window->updateGL();
}

void Renderer::setCameraFastMoveForward(bool bEnable)
{
   m_bFastCameraMove = bEnable;
}

void Renderer::moveCameraForward(float delta)
{
   if (m_bFreeCamera)
   {
      m_CameraSave = m_Camera;
      m_Camera = m_DebugCamera;
   }

   m_fMoveCameraForward += delta;
   m_Camera.doupdate = true;

   if (m_bFreeCamera)
   {
      m_DebugCamera = m_Camera;
      m_Camera = m_CameraSave;
   }

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

void Renderer::toggleLinearMode()
{
   if (!m_pEarthParams->nonlin) m_pEarthParams->nonlin=TRUE;
   else m_pEarthParams->nonlin=FALSE;
   viewer->propagate();
   window->updateGL();
}

void Renderer::toggleTerrainRender()
{
   m_bRenderTerrain = !m_bRenderTerrain;
   window->updateGL();
}

void Renderer::toggleFreeCamera()
{
   m_bFreeCamera = !m_bFreeCamera;
   if (m_bFreeCamera)
   {
      m_DebugCamera = m_Camera;
      m_DebugCamera.farplane *= 5;
   }
}
