// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QTime>

#include <mini/mini_math.h>
#include <mini/mini_object.h>

#include "cameraconst.h"

class Object;
class ViewerWindow;

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
class Camera: public mininode_cam
{
public:
   Camera(ViewerWindow *window, miniearth *earth,
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
   void    focusOnObject(Object *obj);

   void    timerEvent(int timerId);
   void    moveCursor(int mx, int my);
   void    modifierKey(modifierKeys modifier, bool pressed);

   void    startIdling();
   void    stopIdling();

   void    startTransition(minicurve target, double dangle, double dpitch, double dtime);
   void    stopTransition();

protected:
   void    initView();
   void    initTransition();

   void    processTransition(double w, double dw);

   miniv3d unprojectPosition(int x, int y);
   miniv3d unprojectMouse(float dx=0.0f, float dy=0.0f);

public:
   miniv3d hitVector();
   miniv3d nearVector();
   miniv3d targetVector(float dx=0.0f, float dy=0.0f);
   miniv3d cursorVector(double zoom=0.0);

private:
   // camera idling timer
   int       m_IdlingTimerId;
   QTime     m_IdlingTimer;

   // camera transition animation
   minicurve m_TargetCameraCurve;
   double    m_TargetDeltaAngle;
   double    m_TargetDeltaPitch;
   double    m_TargetDeltaTime;
   double    m_TargetCameraFinished;
   bool      m_bInCameraTransition;
   int       m_TransitionTimerId;
   QTime     m_TransitionTimer;
   QTime     m_TransitionStart;

protected:
   // window reference
   ViewerWindow *m_window;

   // earth reference
   miniearth *m_earth;

   // cursor position
   float    m_CursorPosX;
   float    m_CursorPosY;
   bool     m_CursorValid;

   // modifier keys
   bool     m_Shift, m_Control, m_Alt, m_Meta;

   // ray shooting
   virtual double shoot(const minicoord &o,const miniv3d &d,double mindist=0.0);

private:
   double   delta_angle(double a, double b);
};

#endif
