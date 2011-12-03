// (c) by Stefan Roettger

#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QTime>

#include <mini/minicam.h>
#include <mini/minianim.h>

#include <mini/mininodes.h>

#include "cameraconst.h"

class QGLWidget;

class miniearth;
class minilayer;

enum modifierKeys
{
   ModifierShift,
   ModifierControl,
   ModifierAlt,
   ModifierMeta
};

//! qt viewer camera node
class Camera : public mininode_cam
{
public:
   Camera(QGLWidget *window, miniearth *earth,
          double lat, double lon, double height,
          double mindist=0.0,
          float fovy=0.0f,float aspect=0.0f,
          double nearp=0.0f,double farp=0.0f);

   ~Camera();

   void    moveAbove();

   void    rotateCamera(float dx, float dy);

   void    moveCameraForward(float delta);
   void    moveCameraSideward(float delta);

   void    focusOnTarget(double zoom=0.0); // zoom in factor 0.0=none 1.0=full
   void    focusOnMap(minilayer *layer);

   void    timerEvent(int timerId);
   void    moveCursor(int mx, int my);
   void    modifierKey(modifierKeys modifier, bool pressed);

   void    startIdling();
   void    stopIdling();

   void    startTransition(minianim target, double dangle, double dpitch, double dtime);
   void    stopTransition();

protected:
   void    initView();
   void    initTransition();

   void    processTransition(double w, double dw);

   miniv3d unprojectMouse();

   miniv3d hitVector();
   miniv3d nearVector();
   miniv3d targetVector();
   miniv3d cursorVector(double zoom=0.0);

private:
   // camera idling timer
   int      m_IdlingTimerId;
   QTime    m_IdlingTimer;

   // camera transition animation
   minianim m_TargetCameraAnim;
   double   m_TargetDeltaAngle;
   double   m_TargetDeltaPitch;
   double   m_TargetDeltaTime;
   double   m_TargetCameraFinished;
   bool     m_bInCameraTransition;
   int      m_TransitionTimerId;
   QTime    m_TransitionTimer;
   QTime    m_TransitionStart;

protected:
   // window reference
   QGLWidget *m_window;

   // earth reference
   miniearth *m_earth;

   // cursor position
   float    m_CursorPosX;
   float    m_CursorPosY;
   bool     m_CursorValid;

   // modifier keys
   bool     m_Shift, m_Control, m_Alt, m_Meta;

private:
   double   delta_angle(double a, double b);
};

#endif
