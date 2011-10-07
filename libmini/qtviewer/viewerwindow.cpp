// (c) by Stefan Roettger

#include <string>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <QtCore/QUrl>

#include "renderer.h"

#include "viewerconst.h"
#include "viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget* )
   : renderer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);

   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

   // init renderer
   renderer = new Renderer(this);

   // accept drag and drop
   setAcceptDrops(true);
}

ViewerWindow::~ViewerWindow()
{
   if (renderer!=NULL)
      delete renderer;
}

QSize ViewerWindow::minimumSizeHint() const
{
   return(QSize(VIEWER_MINWIDTH, VIEWER_MINWIDTH/VIEWER_ASPECT));
}

QSize ViewerWindow::sizeHint() const
{
   return(QSize(VIEWER_WIDTH, VIEWER_WIDTH/VIEWER_ASPECT));
}

void ViewerWindow::initializeGL()
{
   if (!renderer->isInited())
   {
      // initialize renderer here as it needs GL context to init
      renderer->init();

      // init map from arguments
      QStringList dataPathList = QCoreApplication::arguments();
      for (int i=1; i<dataPathList.size(); i++)
         loadMap(dataPathList[i]);
   }

   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
}

void ViewerWindow::resizeGL(int, int)
{
   renderer->resizeWindow();
}

void ViewerWindow::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   renderer->draw();
}

void ViewerWindow::mousePressEvent(QMouseEvent *event)
{
   reportModifiers();

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

   reportModifiers();

   // if mouse did not move and we have buttons down, it is a click
   if (abs(dx)<3 && abs(dy)<3)
   {
      // a left-right button click
      if (bLeftButtonDown && bRightButtonDown)
      {}
      // a left button click
      else if (bLeftButtonDown)
         renderer->getCamera()->focusOnTarget();
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

   reportModifiers();

   if (event->buttons() & Qt::LeftButton)
      renderer->getCamera()->rotateCamera(dx, dy);
   else if (event->buttons() & Qt::MiddleButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}
   else
      renderer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   movedPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *)
{
   reportModifiers();

   renderer->getCamera()->focusOnTarget(0.75);
}

void ViewerWindow::reportModifiers()
{
   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   renderer->getCamera()->modifierKey(ModifierShift, keyMod & Qt::ShiftModifier);
   renderer->getCamera()->modifierKey(ModifierControl, keyMod & Qt::ControlModifier);
   renderer->getCamera()->modifierKey(ModifierAlt, keyMod & Qt::AltModifier);
   renderer->getCamera()->modifierKey(ModifierMeta, keyMod & Qt::MetaModifier);
}

void ViewerWindow::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Space)
      renderer->getCamera()->focusOnTarget();

   QGLWidget::keyPressEvent(event);

   reportModifiers();
}

void ViewerWindow::keyReleaseEvent(QKeyEvent* event)
{
   QGLWidget::keyReleaseEvent(event);

   reportModifiers();
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   double numDegrees = event->delta()/8.0;

   reportModifiers();

   if (event->orientation() == Qt::Vertical)
      renderer->getCamera()->moveCameraForward(numDegrees/360.0);
   else
      renderer->getCamera()->moveCameraSideward(numDegrees/360.0);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   reportModifiers();

   renderer->getCamera()->timerEvent(event->timerId());
}

void ViewerWindow::loadMap(QString url)
{
   minilayer *layer;

   if (url.endsWith(".ini", Qt::CaseInsensitive))
   {
      int lio1=url.lastIndexOf("/");
      int lio2=url.lastIndexOf("\\");

      if (lio1>0 && lio2>0)
         url.truncate((lio1>lio2)?lio1:lio2);
      else if (lio1>0)
         url.truncate(lio1);
      else if (lio2>0)
         url.truncate(lio2);
   }

   layer=renderer->loadMap(url.toStdString().c_str());

   if (layer!=NULL)
      emit changed(url, layer);
   else
      QMessageBox::warning(this, "Error",
                           "Unable to load map data from url="+url,
                           QMessageBox::Ok);
}

void ViewerWindow::clearMaps()
{
   renderer->clearMaps();
}

void ViewerWindow::gotoMap(minilayer *layer)
{
   renderer->getCamera()->focusOnMap(layer);
}

void ViewerWindow::toggleStereo(bool on)
{
   renderer->toggleStereo(on);
}

void ViewerWindow::toggleWireFrame(bool on)
{
   renderer->toggleWireFrame(on);
}

void ViewerWindow::checkFog(bool on)
{
   renderer->checkFog(on);
}

void ViewerWindow::setFogDensity(double density)
{
   renderer->setFogDensity(density);
}

void ViewerWindow::checkContours(bool on)
{
   renderer->checkContours(on);
}

void ViewerWindow::checkSeaLevel(bool on)
{
   renderer->checkSeaLevel(on);
}

void ViewerWindow::setSeaLevel(double level)
{
   renderer->setSeaLevel(level);
}

void ViewerWindow::checkLight(bool on)
{
   renderer->checkLight(on);
}

void ViewerWindow::setLight(double hour)
{
   renderer->setLight(hour);
}

void ViewerWindow::checkExagger(bool on)
{
   renderer->checkExagger(on);
}

void ViewerWindow::setExagger(double scale)
{
   renderer->setExagger(scale);
}

void ViewerWindow::dragEnterEvent(QDragEnterEvent *event)
{
   event->acceptProposedAction();
}

void ViewerWindow::dragMoveEvent(QDragMoveEvent *event)
{
   event->acceptProposedAction();
}

void ViewerWindow::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls())
   {
      event->acceptProposedAction();

      QList<QUrl> urlList = mimeData->urls();

      for (int i=0; i<urlList.size(); i++)
      {
         QString url = urlList.at(i).path();
         loadMap(url);
      }
   }
}

void ViewerWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}
