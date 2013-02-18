// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

class QTimerEvent;
class Renderer;

//! qt viewer window
class ViewerWindow: public QGLWidget
{
   Q_OBJECT;

public:
   ViewerWindow();
   virtual ~ViewerWindow();

   Renderer *getViewer();

   void setAspect(double aspect);

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

protected:
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();

   void timerEvent(QTimerEvent *);

private:
   double viewer_aspect;

   Renderer *viewer;
};

#endif
