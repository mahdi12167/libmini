// (c) by Stefan Roettger

#include <string>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <QtCore/QUrl>

#include "renderer.h"

#include "viewerconst.h"
#include "viewerwindow.h"

ViewerWindow::ViewerWindow()
   : viewer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);

   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

   // init viewer
   viewer = new Renderer(this);

   // accept drag and drop
   setAcceptDrops(true);
}

ViewerWindow::~ViewerWindow()
{
   if (viewer!=NULL)
      delete viewer;
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
   if (!viewer->isInited())
   {
      // initialize viewer here as it needs GL context to init
      viewer->init();

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
   viewer->resizeWindow();
}

void ViewerWindow::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   viewer->draw();
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

void ViewerWindow::mouseReleaseEvent(QMouseEvent *event)
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
         viewer->getCamera()->focusOnTarget();
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
      viewer->getCamera()->rotateCamera(dx, dy);
   else if (event->buttons() & Qt::MiddleButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}
   else
      viewer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   movedPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *)
{
   reportModifiers();

   viewer->getCamera()->focusOnTarget(0.75);
}

void ViewerWindow::reportModifiers()
{
   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   viewer->getCamera()->modifierKey(ModifierShift, keyMod & Qt::ShiftModifier);
   viewer->getCamera()->modifierKey(ModifierControl, keyMod & Qt::ControlModifier);
   viewer->getCamera()->modifierKey(ModifierAlt, keyMod & Qt::AltModifier);
   viewer->getCamera()->modifierKey(ModifierMeta, keyMod & Qt::MetaModifier);
}

void ViewerWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Space)
      viewer->getCamera()->focusOnTarget();

   QGLWidget::keyPressEvent(event);

   reportModifiers();
}

void ViewerWindow::keyReleaseEvent(QKeyEvent *event)
{
   QGLWidget::keyReleaseEvent(event);

   reportModifiers();
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   double numDegrees = event->delta()/8.0;

   reportModifiers();

   if (event->orientation() == Qt::Vertical)
      viewer->getCamera()->moveCameraForward(numDegrees/360.0);
   else
      viewer->getCamera()->moveCameraSideward(numDegrees/360.0);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   reportModifiers();

   viewer->getCamera()->timerEvent(event->timerId());
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

   layer=viewer->loadMap(url.toStdString().c_str());

   if (layer!=NULL)
      emit changed(url, layer);
   else
      QMessageBox::warning(this, "Error",
                           "Unable to load map data from url="+url,
                           QMessageBox::Ok);
}

void ViewerWindow::clearMaps()
{
   viewer->clearMaps();
}

void ViewerWindow::gotoMap(minilayer *layer)
{
   viewer->getCamera()->focusOnMap(layer);
}

void ViewerWindow::toggleStereo(bool on)
{
   viewer->toggleStereo(on);
}

void ViewerWindow::toggleWireFrame(bool on)
{
   viewer->toggleWireFrame(on);
}

void ViewerWindow::checkFog(bool on)
{
   viewer->checkFog(on);
}

void ViewerWindow::setFogDensity(double density)
{
   viewer->setFogDensity(density);
}

void ViewerWindow::checkContours(bool on)
{
   viewer->checkContours(on);
}

void ViewerWindow::checkSeaLevel(bool on)
{
   viewer->checkSeaLevel(on);
}

void ViewerWindow::setSeaLevel(double level)
{
   viewer->setSeaLevel(level);
}

void ViewerWindow::checkLight(bool on)
{
   viewer->checkLight(on);
}

void ViewerWindow::setLight(double hour)
{
   viewer->setLight(hour);
}

void ViewerWindow::checkExagger(bool on)
{
   viewer->checkExagger(on);
}

void ViewerWindow::setExagger(double scale)
{
   viewer->setExagger(scale);
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
         QUrl url = urlList.at(i);
         QString path = url.toString();

         if (path.startsWith("file:"))
            path = url.toLocalFile();

         loadMap(path);
      }
   }
}

void ViewerWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}
