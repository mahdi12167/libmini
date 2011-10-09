// (c) by Stefan Roettger

#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QTime>

#include <mini/mininode.h>
#include <mini/minicam.h>
#include <mini/minianim.h>

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

class Camera : public miniode, minicam
{
public:
   Camera(QGLWidget *window, miniearth *earth,
          double lat=21.39,double lon=-157.72,double height=7E6,
          double mindist=0.0);

   ~Camera();

   void    setLens(float fovy);
   void    setPosition(float latitude, float longitude, float altitude, float heading, float pitch);

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

   // field of view
   float    m_fovy;

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
