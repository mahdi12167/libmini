#include <QtGui>
#include <QtOpenGL/qgl.h>

#include "landscape.h"
#include "viewerconst.h"
#include "viewerwindow.h"
#include "renderer.h"

const char* baseurl = "Hawaii/";
const char* baseid = "data/HawaiiTileset/";
const char* basepath1 = "tiles";
const char* basepath2 = "landsat";

const float MapScrollSpeedX = 0.25;
const float MapScrollSpeedY = 0.25;

static QString mapDataUrl[] =
{
   baseurl, baseid, basepath1, basepath2
};

static QString dataFilePath;

ViewerWindow::ViewerWindow(QWidget* )
   : renderer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer));

   QStringList dataPathList = QCoreApplication::arguments();

   for (int i = 1; i < 5; i++)
   {
      mapDataUrl[i-1] = dataPathList[i];
   }

   // init renderer
   renderer = new Renderer(this);
   // renderer->setMapURL(baseurl, baseid, basepath1, basepath2);
   renderer->setMapURL(mapDataUrl[0].toAscii().constData(), mapDataUrl[1].toAscii().constData(), mapDataUrl[2].toAscii().constData(), mapDataUrl[3].toAscii().constData());

   // init camera
   renderer->initCamera(-157.974908, 21.344997, 45000, 0, 90, VIEWER_FOVY, VIEWER_NEARP, VIEWER_FARP);
}

ViewerWindow::~ViewerWindow()
{
   if (renderer != NULL)
   {
      delete renderer;
   }
}

void ViewerWindow::initializeGL()
{
   // initialize render here as render need GL context to init
   if (!renderer->isInited())
      renderer->init();

   qglClearColor(Qt::black);
   glShadeModel(GL_SMOOTH);
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
   lastPos = event->pos();
   if (event->buttons() & Qt::LeftButton)
   {
      bLeftButtonDown = true;
   }
   else if (event->buttons() & Qt::RightButton)
   {
      bRightButtonDown = true;
   }
   else
      event->ignore();
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent* event)
{
   // if mouse did not move and we have buttons down, it is a click
   if ((event->pos().x() - lastPos.x() < 3) && (event->pos().y() - lastPos.y() < 3))
   {
      if (bLeftButtonDown && bRightButtonDown)
      {
	 // a left-right button click
	 renderer->resetMapOrientation();
      }
      else if (bLeftButtonDown)
      {
	 // a left button click
      }
      else if (bRightButtonDown)
      {
	 // a right button click
      }
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
   float dx = ((float)(event->x() - lastPos.x())) / width();
   float dy = ((float)(event->y() - lastPos.y())) / height();
   if (event->buttons() & Qt::MiddleButton)
   {
      renderer->rotateCamera(dx, dy);
   }
   else if (event->buttons() & Qt::LeftButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}
   else
   {
      renderer->moveCursor(event->pos());
   }
   lastPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *) {}

void ViewerWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Shift)
   {
      renderer->setCameraFastMoveForward(true);
   }
   else if (event->key() == Qt::Key_Control)
   {}
   else if (event->key() == Qt::Key_0)
   {
      renderer->toggleLinearMode();
   }
   else if (event->key() == Qt::Key_T)
   {
      renderer->toggleTerrainRender();
   }
   else if (event->key() == Qt::Key_Control)
   {}
   else if (event->key() == Qt::Key_P)
   {
      renderer->toggleFreeCamera();
   }
   else if (event->key() == Qt::Key_W)
   {
      renderer->moveCamera(0, MapScrollSpeedY);
   }
   else if (event->key() == Qt::Key_A)
   {
      renderer->moveCamera(-MapScrollSpeedX, 0);
   }
   else if (event->key() == Qt::Key_S)
   {
      renderer->moveCamera(0, -MapScrollSpeedY);
   }
   else if (event->key() == Qt::Key_D)
   {
      renderer->moveCamera(MapScrollSpeedX, 0);
   }
   else if (event->key() == Qt::Key_Space)
   {
      renderer->focusOnTarget();
   }
   else if (event->key() == Qt::Key_R)
   {
      renderer->resetMap();
   }
   else if (event->key() == Qt::Key_Q)
   {
      renderer->moveCameraForward(1.0f);
   }
   else if (event->key() == Qt::Key_E)
   {
      renderer->moveCameraForward(-1.0f);
   }
   else
      QGLWidget::keyPressEvent(event);
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Shift)
   {
      renderer->setCameraFastMoveForward(false);
   }
   else if (event->key() == Qt::Key_Control)
   {}
   else
      QGLWidget::keyReleaseEvent(event);
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   int numDegrees = event->delta() / 8;
   int numSteps = numDegrees / 15;

   float fDelta = (float)numSteps/5.0f;

   if (event->orientation() == Qt::Vertical)
   {
      renderer->moveCameraForward(fDelta);
   }

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   renderer->timerEvent(event->timerId());
}
