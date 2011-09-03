#include <QtGui/QApplication>

#include "renderer.h"

#include "viewerconst.h"
#include "viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget* )
   : renderer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

   QStringList dataPathList = QCoreApplication::arguments();

   // init renderer
   renderer = new Renderer(this);

   // init map
   if (dataPathList.size()>1)
      renderer->setMapURL(dataPathList[1].toAscii().constData());
}

ViewerWindow::~ViewerWindow()
{
   if (renderer!=NULL)
      delete renderer;
}

void ViewerWindow::initializeGL()
{
   // initialize renderer here as it needs GL context to init
   renderer->init();

   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
}

void ViewerWindow::resizeGL(int width, int height)
{
   renderer->resizeWindow(width, height);
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

   lastPos = movedPos = event->pos();
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
      {}
      // a left button click
      else if (bLeftButtonDown)
         renderer->focusOnTarget();
      // a right button click
      else if (bRightButtonDown)
      {}
      else
         event->ignore();
   }
   else
      event->ignore();

   bLeftButtonDown = false;
   bRightButtonDown = false;
}

void ViewerWindow::mouseMoveEvent(QMouseEvent *event)
{
   float dx = (float)(event->x()-movedPos.x())/width();
   float dy = (float)(event->y()-movedPos.y())/height();

   if (event->buttons() & Qt::LeftButton)
      renderer->rotateCamera(dx, dy);
   else if (event->buttons() & Qt::MiddleButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}
   else
      renderer->moveCursor(event->pos());

   movedPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *)
{
   renderer->focusOnTarget();
}

void ViewerWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Shift)
      renderer->modifierKey(ModifierShift, true);
   else if (event->key() == Qt::Key_Control)
      renderer->modifierKey(ModifierControl, true);
   else if (event->key() == Qt::Key_Meta)
      renderer->modifierKey(ModifierMeta, true);
   else if (event->key() == Qt::Key_Space)
      renderer->focusOnTarget();
   else if (event->key() == Qt::Key_W)
      renderer->toggleWireframe();
   else if (event->key() == Qt::Key_S)
      renderer->toggleSeaSurface();
   else
      QGLWidget::keyPressEvent(event);
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Shift)
      renderer->modifierKey(ModifierShift, false);
   else if (event->key() == Qt::Key_Control)
      renderer->modifierKey(ModifierControl, false);
   else if (event->key() == Qt::Key_Meta)
      renderer->modifierKey(ModifierMeta, false);
   else
      QGLWidget::keyReleaseEvent(event);
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   double numDegrees = event->delta()/8.0;

   if (event->orientation() == Qt::Vertical)
      renderer->moveCameraForward(numDegrees/360.0);
   else
      renderer->moveCameraSideward(numDegrees/360.0);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   renderer->timerEvent(event->timerId());
}

void ViewerWindow::loadMapURL(const char* url)
{
   renderer->loadMapURL(url);
}
