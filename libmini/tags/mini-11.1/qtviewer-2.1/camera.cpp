// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtOpenGL/qgl.h>

#include "objects.h"
#include "viewerwindow.h"
#include "renderer.h"

#include "camera.h"
#include "cameraconst.h"

Camera::Camera(ViewerWindow *window, miniearth *earth,
               double lat, double lon, double height,
               double mindist,
               float fovy,float aspect,
               double nearp,double farp)
   : mininode_cam(earth, lat, lon, height, mindist,
                  fovy, aspect, nearp, farp)
{
   m_window = window;
   m_earth = earth;

   // mouse cursor
   m_CursorValid = false;

   // modifiers
   m_Shift=false;
   m_Control=false;
   m_Alt=false;
   m_Meta=false;

   // initialize the view
   initView();

   // initialize the camera transition
   initTransition();
}

Camera::~Camera()
{}

// move camera above ground
void Camera::moveAbove()
{
   move_above(CAMERA_HEIGHT_FLOOR);
}

// set continuous camera rotation
void Camera::setRotation(double earth, double left, double back)
{
   m_rotationEarth = earth;
   m_rotationLeft = left;
   m_rotationBack = back;
}

// initialize the view point
void Camera::initView()
{
   // just use the default eye point
   set_eye(get_eye(), 0.0, -90.0, CAMERA_HEIGHT_FLOOR);
}

// initialize the camera transition
void Camera::initTransition()
{
   m_TransitionTimerId = -1;
   m_IdlingTimerId = -1;

   m_bInCameraTransition = false;

   m_rotationEarth = 0.0;
   m_rotationLeft = 0.0;
   m_rotationBack = 0.0;
}

miniv3d Camera::unprojectPosition(int x, int y)
{
   return(unproject_viewport(x, y,
                             m_window->width(), m_window->height()));
}

miniv3d Camera::unprojectMouse(float dx, float dy)
{
   if (!m_CursorValid)
      return(get_dir());

   return(unprojectPosition(m_CursorPosX+dx*m_window->width(),
                            m_CursorPosY+dy*m_window->height()));
}

miniv3d Camera::hitVector()
{
   miniv3d hitVec;

   // trace to find the hit point under current focus
   minicoord hit = get_hit();
   hitVec = hit.getpos() - get_eye().getpos();

   return(hitVec);
}

miniv3d Camera::nearVector()
{
   miniv3d nearVec;

   // trace to find the nearest hit point
   minicoord hit = get_hit(get_eye(), -get_eye().getpos());
   nearVec = hit.getpos() - get_eye().getpos();

   return(nearVec);
}

miniv3d Camera::targetVector(float dx, float dy)
{
   miniv3d targetVec;

   // trace to find the hit point under current cursor
   minicoord target = get_hit(get_eye(), unprojectMouse(dx, dy));
   targetVec = target.getpos() - get_eye().getpos();

   return(targetVec);
}

miniv3d Camera::cursorVector(double zoom)
{
   miniv3d cursorVec(0.0);

   // trace to find the hit point under current focus
   minicoord hit = get_hit();

   // trace to nearest hit point as fallback
   if (hit == get_eye())
      hit = get_hit(get_eye(), -get_eye().getpos());

   // trace to find the hit point under current cursor
   if (hit != get_eye())
   {
      minicoord target = get_hit(get_eye(), unprojectMouse());
      if (target != get_eye())
      {
         double elev1 = get_eye().getpos().getlength();
         double elev2 = target.getpos().getlength();
         double scale = elev1 / elev2;

         // find out the target vector from focus to cursor
         cursorVec = (1.0 - zoom) * scale * (target.getpos() - hit.getpos());

         // find out the zoom vector from eye to focus
         cursorVec += zoom * (target.getpos() - get_eye().getpos());
      }
   }

   return(cursorVec);
}

void Camera::rotateEarth(double delta)
{
   minicoord eye;
   miniv3d dir,right,up;
   double angle,pitch;

   eye = get_eye();
   get_local_base(eye,dir,right,up);

   angle = get_angle();
   pitch = get_pitch();

   double radius = sqrt(eye.vec.x*eye.vec.x+eye.vec.y*eye.vec.y);

   move_plain(delta/180.0*PI*radius*right);
   get_local_base(get_eye(),eye_dir,eye_right,eye_up);

   rotate_right(angle);
   rotate_up(pitch);
}

void Camera::rotateCamera(float dx, float dy)
{
   stopTransition();

   if (m_Shift)
   {
      rotate_right(360 *dx);
      rotate_up(180 * dy);

      rotate_limit(-90.0, 90.0);
   }
   else if (m_Control)
   {
      rotate_right(360 *dx);
   }
   else
   {
      double dist = get_hitdist();

      move_forward_plain(dist);

      rotate_right(360 *dx);
      rotate_up(180 * dy);

      rotate_limit(-90.0, 90.0);

      move_forward_plain(-dist);
   }

   moveAbove();

   startIdling();
}

void Camera::moveCameraForward(float delta)
{
   const double mindist = 1000.0; // minimum travel distance

   stopTransition();

   delta *= 3.0;
   if (delta > 1.0) delta = 1.0;
   else if (delta < -1.0) delta = -1.0;

   miniv3d dir = unprojectMouse();
   double dist = get_hitdist();
   double dist0 = get_hitdist(get_eye(), dir);

   if (dist0 < dist) dist = dist0;
   if (dist == 0.0) dist = sqrt(pow(get_orb_radius()+get_dist(), 2.0)-pow(get_orb_radius(), 2.0));
   if (dist < mindist) dist = mindist;

   if (m_Shift || m_Control)
      move_back(-delta * dist);
   else
      move(delta * dist * unprojectMouse());

   moveAbove();

   startIdling();
}

void Camera::moveCameraSideward(float delta)
{
   const double mindist = 1000.0; // minimum travel distance

   stopTransition();

   delta *= 3.0;
   if (delta > 1.0) delta = 1.0;
   else if (delta < -1.0) delta = -1.0;

   miniv3d dir = unprojectMouse();
   double dist = get_hitdist();
   double dist0 = get_hitdist(get_eye(), dir);

   if (dist0 < dist) dist = dist0;
   if (dist == 0.0) dist = sqrt(pow(get_orb_radius()+get_dist(), 2.0)-pow(get_orb_radius(), 2.0));
   if (dist < mindist) dist = mindist;

   move_right(-delta * dist);
   moveAbove();

   startIdling();
}

double Camera::delta_angle(double a, double b)
   {
   double d = a - b;

   if (d > 180.0) d -= 360.0;
   else if (d < -180.0) d += 360.0;

   return(d);
   }

void Camera::focusOnTarget(double zoom)
{
   minicurve curve;

   minicoord target = get_eye();

   if (m_Shift || m_Control)
      target -= cursorVector(zoom);
   else
      target += cursorVector(zoom);

   curve.append_sector(get_eye(), target, 100);
   startTransition(curve, 0.0, 0.0, 0.5);

   MINILOG((ministring)"focusing on target: " + target);
}

void Camera::focusOnMap(minilayer *layer)
{
   minicurve curve;

   if (layer==NULL) return;

   minicoord target = layer->getcenter();
   miniv3d normal = layer->getnormal();
   miniv3d extent = layer->getextent();

   double size = (extent.x+extent.y) / 2.0;
   target += size * normal;

   curve.append_sector(get_eye(), target, 100);

   if ((get_eye().getpos() - target.getpos()).getlength() < size/4.0)
      startTransition(curve, delta_angle(0.0, get_angle()), delta_angle(-90, get_pitch()), 1.0);
   else
      startTransition(curve, 0.0, delta_angle(-90, get_pitch()), 2.0);

   MINILOG((ministring)"focusing on map: " + target);
}

void Camera::focusOnObject(Object *obj)
{
   minicurve curve;

   if (obj==NULL) return;

   minicoord target = obj->get_center();
   miniv3d normal = obj->get_normal();
   double size = 2.0*obj->get_radius();

   target.set_time(0.0);

   target += size * normal;

   curve.append_sector(get_eye(), target, 100);

   if ((get_eye().getpos() - target.getpos()).getlength() < size/4.0)
      startTransition(curve, delta_angle(0.0, get_angle()), delta_angle(-90, get_pitch()), 1.0);
   else
      startTransition(curve, 0.0, delta_angle(-90, get_pitch()), 2.0);

   MINILOG((ministring)"focusing on object: " + target);
}

void Camera::processTransition(double w, double dw)
{
   w = 1.0 - pow(1.0 - w, 2.0);

   move(m_TargetCameraCurve.interpolate_cubic(w).getpos() - get_eye().getpos());

   moveAbove();

   rotate_right(dw * m_TargetDeltaAngle);
   rotate_up(dw * m_TargetDeltaPitch);

   if (w >= 1.0) stopTransition();

   m_window->updateGL();
}

void Camera::timerEvent(int timerId)
{
   const double minIdle = 2.0; // minimum idle time interval (s)

   if (timerId == m_IdlingTimerId)
   {
      if (!m_bInCameraTransition)
         m_window->updateGL();

      if (m_rotationEarth!=0.0 || m_rotationLeft!=0.0 || m_rotationBack!=0.0)
      {
         double dist = get_dist();

         if (m_rotationEarth!=0.0)
            rotateEarth(m_rotationEarth/CAMERA_FPS);

         if (m_rotationLeft!=0.0)
            move_left(m_rotationLeft/CAMERA_FPS);

         if (m_rotationBack!=0.0)
            move_back(m_rotationBack/CAMERA_FPS);

         move_down(get_dist()-dist);
      }

      bool bPagingFinished = !m_earth->checkpending();
      bool bRotationFinished = m_rotationEarth==0.0 && m_rotationLeft==0.0 && m_rotationBack==0.0;

      bool bFinished = bPagingFinished && bRotationFinished;

      if (!bFinished)
         m_IdlingTimer.start();
      else
      {
         int deltaT = m_IdlingTimer.elapsed();
         double dt = deltaT / 1000.0;

         if (dt>minIdle)
            stopIdling();
      }
   }
   else if (timerId == m_TransitionTimerId)
   {
      int timeT = m_TransitionStart.elapsed();
      double t0 = timeT / 1000.0;

      int deltaT = m_TransitionTimer.restart();
      double dt0 = deltaT / 1000.0;

      double t = t0 - dt0, dt, dw;

      while (t<t0 && dt0>0.0)
      {
         dt = 1.0/CAMERA_FPS;
         if (dt0 < dt) dt = dt0;

         dw = dt / m_TargetDeltaTime;

         if (m_TargetCameraFinished + dw > 1.0)
            if (m_TargetCameraFinished < 1.0) dw = 1.0 - m_TargetCameraFinished;
            else dw = 0.0;

         m_TargetCameraFinished += dw;

         processTransition(m_TargetCameraFinished, dw);

         t += dt;
         dt0 -= dt;
      }
   }
}

void Camera::startIdling()
{
   if (m_IdlingTimerId == -1)
      m_IdlingTimerId = m_window->startTimer((int)(1000.0/CAMERA_FPS));

   m_IdlingTimer.start();
}

void Camera::stopIdling()
{
   if (m_IdlingTimerId != -1)
   {
      m_window->killTimer(m_IdlingTimerId);
      m_IdlingTimerId=-1;
   }
}

void Camera::startTransition(minicurve target, double dangle, double dpitch, double dtime)
{
   stopTransition();

   m_TargetCameraCurve = target;
   m_TargetDeltaAngle = dangle;
   m_TargetDeltaPitch = dpitch;
   m_TargetDeltaTime = dtime;
   m_TargetCameraFinished = 0.0;
   m_bInCameraTransition = true;
   m_TransitionStart = m_TransitionTimer;
   m_TransitionTimer.start();
   m_TransitionStart = m_TransitionTimer;
   m_TransitionTimerId = m_window->startTimer((int)(1000.0/CAMERA_FPS));

   setRotation(0.0);
}

void Camera::stopTransition()
{
   if (m_TransitionTimerId != -1)
   {
      m_window->killTimer(m_TransitionTimerId);
      m_TransitionTimerId = -1;
      m_bInCameraTransition = false;
   }
}

void Camera::moveCursor(int mx, int my)
{
   m_CursorPosX = mx;
   m_CursorPosY = my;
   m_CursorValid = true;
}

void Camera::modifierKey(modifierKeys modifier, bool pressed)
{
   if (modifier==ModifierShift)
      m_Shift=pressed;
   else if (modifier==ModifierControl)
      m_Control=pressed;
   else if (modifier==ModifierAlt)
      m_Alt=pressed;
   else if (modifier==ModifierMeta)
      m_Meta=pressed;
}

double Camera::shoot(const minicoord &o,const miniv3d &d,double mindist)
{
   return(m_window->getViewer()->shoot(o,d,mindist));
}
