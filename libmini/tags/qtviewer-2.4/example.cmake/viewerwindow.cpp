// (c) by Stefan Roettger, licensed under GPL 2+

#include "renderer.h"

#include "viewerwindow.h"
#include "viewerconst.h"

ViewerWindow::ViewerWindow()
   : viewer(NULL)
{
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer));

   setCursor(Qt::CrossCursor);

   viewer_aspect = VIEWER_ASPECT;

   // init viewer
   viewer = new Renderer(this);
   if (viewer == NULL) MEMERROR();
}

ViewerWindow::~ViewerWindow()
{
   if (viewer != NULL)
      delete viewer;
}

Renderer *ViewerWindow::getViewer()
{
   return(viewer);
}

void ViewerWindow::setAspect(double aspect)
{
   viewer_aspect = aspect;
}

QSize ViewerWindow::minimumSizeHint() const
{
   return(QSize(VIEWER_MINWIDTH, VIEWER_MINWIDTH/viewer_aspect));
}

QSize ViewerWindow::sizeHint() const
{
   return(QSize(VIEWER_WIDTH, VIEWER_WIDTH/viewer_aspect));
}

void ViewerWindow::initializeGL()
{
   if (!viewer->isInited())
   {
      // initialize viewer here as it needs GL context to init
      viewer->init();
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
