// (c) by Stefan Roettger

#include <QtOpenGL/qgl.h>

#include "camera.h"
#include "cameraconst.h"

Camera::Camera(QGLWidget *window, miniearth *earth,
               double lat,double lon,double height,
               double mindist)
   : minicam(earth, lat, lon, height, mindist)
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

// set camera field of view
void Camera::setCamera(float fovy)
{
   m_fovy = fovy;
}

// set camera position and direction
void Camera::setCamera(float latitude, float longitude, float altitude, float heading, float pitch)
{
   set_eye(minicoord(miniv3d(latitude * 3600.0, longitude * 3600.0, altitude),
                     minicoord::MINICOORD_LLH), heading, pitch);
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
}

miniv3d Camera::unprojectMouse()
{
   if (!m_CursorValid)
      return(get_dir());

   int viewportwidth = m_window->width();
   int viewportheight = m_window->height();

   float aspect = (float)viewportwidth/viewportheight;

   double mx = (double)m_CursorPosX / (viewportwidth-1) - 0.5;
   double my = 0.5 - (double)m_CursorPosY / (viewportheight-1);

   double wy = tan(m_fovy*PI/360);
   double wx = aspect * wy;

   miniv3d dir = get_dir() +
                 get_right() * 2.0 * wx * mx +
                 get_up() * 2.0 * wy * my;

   dir.normalize();

   return(dir);
}

miniv3d Camera::hitVector()
{
   miniv3d hitVec;

   // trace to find the hit point under current focus
   minicoord hit = get_hit();
   hitVec = hit.vec - get_eye().vec;

   return(hitVec);
}

miniv3d Camera::targetVector()
{
   miniv3d targetVec;

   // trace to find the hit point under current cursor
   minicoord target = get_hit(get_eye(), unprojectMouse());
   targetVec = target.vec - get_eye().vec;

   return(targetVec);
}

miniv3d Camera::cursorVector(double zoom)
{
   miniv3d cursorVec(0.0);

   // trace to find the hit point under current focus
   minicoord hit = get_hit();
   if (hit != get_eye())
   {
      // trace to find the hit point under current cursor
      minicoord target = get_hit(get_eye(), unprojectMouse());
      if (target != get_eye())
      {
         double elev1 = get_eye().vec.getlength();
         double elev2 = target.vec.getlength();
         double scale = elev1 / elev2;

         // find out the target vector from focus to cursor
         cursorVec = (1.0 - zoom) * scale * (target.vec - hit.vec);

         // find out the zoom vector from eye to focus
         cursorVec += zoom * (target.vec - get_eye().vec);
      }
   }

   return(cursorVec);
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
   else
   {
      double dist = get_hitdist();

      move_forward_plain(dist);

      rotate_right(360 *dx);
      rotate_up(180 * dy);

      rotate_limit(-90.0, 0.0);

      move_forward_plain(-dist);
   }

   move_above(CAMERA_HEIGHT_FLOOR);

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
   if (dist == 0.0) dist = sqrt(pow(miniearth::EARTH_radius+get_dist(), 2.0)-pow(miniearth::EARTH_radius, 2.0));
   if (dist < mindist) dist = mindist;

   if (m_Shift)
      move_back(-delta * dist);
   else
      move(delta * dist * unprojectMouse());

   move_above(CAMERA_HEIGHT_FLOOR);

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
   if (dist == 0.0) dist = sqrt(pow(miniearth::EARTH_radius+get_dist(), 2.0)-pow(miniearth::EARTH_radius, 2.0));
   if (dist < mindist) dist = mindist;

   move_right(-delta * dist);
   move_above(CAMERA_HEIGHT_FLOOR);

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
   minianim anim;

   minicoord target = get_eye();

   if (m_Shift)
      target -= cursorVector(zoom);
   else
      target += cursorVector(zoom);

   anim.append_sector(get_eye(), target, 100);
   startTransition(anim, 0.0, 0.0, 0.5, 0.25);
}

void Camera::focusOnMap(minilayer *layer)
{
   minianim anim;

   if (layer==NULL) return;

   minicoord target = layer->getcenter();
   miniv3d normal = layer->getnormal();
   miniv3d extent = layer->getextent();

   double size = (extent.x+extent.y) / 2.0;
   target += size * normal;

   anim.append_sector(get_eye(), target, 100);

   if ((get_eye().vec - target.vec).getlength() < size/4.0)
      startTransition(anim, delta_angle(0.0, get_angle()), delta_angle(-90, get_pitch()), 2.0, 0.0);
   else
      startTransition(anim, 0.0, delta_angle(-90, get_pitch()), 2.0, 0.0);
}

void Camera::processTransition(double t, double dt)
{
   const double minspeed = 3000.0; // minimum speed (m/second)

   t /= m_TargetDeltaTime;
   if (t < 1.0) t = 1.0 - pow(1.0 - t, 2.0);

   miniv3d dir = m_TargetCameraAnim.interpolate(t).vec - get_eye().vec;
   double speed = dir.getlength();

   if (m_TargetCameraFollow>0.0) speed /= m_TargetCameraFollow;
   else speed = MAXFLOAT;

   if (speed < minspeed) speed = minspeed;

   if (dir.getlength() > speed * dt)
   {
      dir.normalize();
      move(dir * speed * dt);
   }
   else
   {
      move(dir);
      if (t >= 1.0) stopTransition();
   }

   move_above(CAMERA_HEIGHT_FLOOR);

   double w = dt / m_TargetDeltaTime;

   rotate_right(w * m_TargetDeltaAngle);
   rotate_up(w * m_TargetDeltaPitch);

   m_window->updateGL();
}

void Camera::timerEvent(int timerId)
{
   const double minIdle = 2.0; // minimum idle time interval (s)

   if (timerId == m_IdlingTimerId)
   {
      if (!m_bInCameraTransition)
         m_window->updateGL();

      bool bPagingFinished = !m_earth->checkpending();

      if (!bPagingFinished)
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
      double t = timeT / 1000.0;

      int deltaT = m_TransitionTimer.restart();
      double dt = deltaT / 1000.0;

      processTransition(t, dt);
   }
}

void Camera::startIdling()
{
   if (m_IdlingTimerId == -1)
   {
      m_IdlingTimer.start();
      m_IdlingTimerId = m_window->startTimer((int)(1000.0/CAMERA_FPS));
   }
}

void Camera::stopIdling()
{
   if (m_IdlingTimerId != -1)
   {
      m_window->killTimer(m_IdlingTimerId);
      m_IdlingTimerId=-1;
   }
}

void Camera::startTransition(minianim target, double dangle, double dpitch, double dtime, double follow)
{
   stopTransition();

   m_TargetCameraAnim = target;
   m_TargetDeltaAngle = dangle;
   m_TargetDeltaPitch = dpitch;
   m_TargetDeltaTime = dtime;
   m_TargetCameraFollow = follow;
   m_bInCameraTransition = true;
   m_TransitionStart = m_TransitionTimer;
   m_TransitionTimer.start();
   m_TransitionStart = m_TransitionTimer;
   m_TransitionTimerId = m_window->startTimer((int)(1000.0/CAMERA_FPS));
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
