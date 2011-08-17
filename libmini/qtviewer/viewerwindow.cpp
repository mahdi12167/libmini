#include <QtGui>
#include <QtOpenGL/qgl.h>

#include "viewerconst.h"
#include "viewerwindow.h"
#include "renderer.h"

const float MapScrollSpeedX = 0.25;
const float MapScrollSpeedY = 0.25;

ViewerWindow::ViewerWindow(QWidget* )
   : renderer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

   QStringList dataPathList = QCoreApplication::arguments();

   // init renderer
   renderer = new Renderer(this);

   // init maps
   for (int i=1; i<dataPathList.size(); i++)
      renderer->setMapURL(dataPathList[i].toAscii().constData());

   // init camera
   renderer->initCamera(VIEWER_FOVY, VIEWER_NEARP, VIEWER_FARP);
}

ViewerWindow::~ViewerWindow()
{
   if (renderer!=NULL)
      delete renderer;
}

void ViewerWindow::initializeGL()
{
   // initialize renderer here as it needs GL context to init
   if (!renderer->isInited())
      renderer->init();

   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
}

void ViewerWindow::resizeGL(int width, int height)
{
   renderer->resize(width, height);
}

void ViewerWindow::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   renderer->draw();
}

void ViewerWindow::mousePressEvent(QMouseEvent *event)
{
   if (event->buttons() & Qt::LeftButton)
      bLeftButtonDown = true;
   else if (event->buttons() & Qt::RightButton)
      bRightButtonDown = true;
   else
      event->ignore();

   lastPos = event->pos();
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent* event)
{
   int dx = event->x()-lastPos.x();
   int dy = event->y()-lastPos.y();

   // if mouse did not move and we have buttons down, it is a click
   if (abs(dx)<3 && abs(dy)<3)
   {
      // a left-right button click
      if (bLeftButtonDown && bRightButtonDown)
         renderer->resetMapOrientation();
      // a left button click
      else if (bLeftButtonDown)
      {}
      // a right button click
      else if (bRightButtonDown)
      {} else
         event->ignore();
   }
   else
      event->ignore();

   bLeftButtonDown = false;
   bRightButtonDown = false;
}

void ViewerWindow::mouseMoveEvent(QMouseEvent *event)
{
   float dx = ((float)(event->x()-lastPos.x()))/width();
   float dy = ((float)(event->y()-lastPos.y()))/height();

   if (event->buttons() & Qt::MiddleButton)
      renderer->rotateCamera(dx, dy);
   else if (event->buttons() & Qt::LeftButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}
   else
      renderer->moveCursor(event->pos());

   lastPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *) {}

void ViewerWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_W)
      renderer->moveCamera(0, MapScrollSpeedY);
   else if (event->key() == Qt::Key_A)
      renderer->moveCamera(-MapScrollSpeedX, 0);
   else if (event->key() == Qt::Key_S)
      renderer->moveCamera(0, -MapScrollSpeedY);
   else if (event->key() == Qt::Key_D)
      renderer->moveCamera(MapScrollSpeedX, 0);
   else if (event->key() == Qt::Key_Space)
      renderer->focusOnTarget();
   else if (event->key() == Qt::Key_R)
      renderer->resetMap();
   else if (event->key() == Qt::Key_Q)
      renderer->moveCameraForward(1.0f);
   else if (event->key() == Qt::Key_E)
      renderer->moveCameraForward(-1.0f);
   else
      QGLWidget::keyPressEvent(event);
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* event)
{
   QGLWidget::keyReleaseEvent(event);
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   int numDegrees = event->delta() / 8;
   int numSteps = numDegrees / 15;

   float fDelta = (float)numSteps/5.0f;

   if (event->orientation() == Qt::Vertical)
      renderer->moveCameraForward(fDelta);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   renderer->timerEvent(event->timerId());
}
