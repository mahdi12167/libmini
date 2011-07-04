#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

class Renderer;

class ViewerWindow : public QGLWidget
{
   Q_OBJECT;

public:
   ViewerWindow(QWidget* parent = 0);
   virtual ~ViewerWindow();

protected:
   void    initializeGL();
   void    resizeGL(int width, int height);
   void    paintGL();
   void    mousePressEvent(QMouseEvent *event);
   void    mouseReleaseEvent(QMouseEvent* event);
   void    mouseMoveEvent(QMouseEvent *event);
   void    mouseDoubleClickEvent(QMouseEvent *);
   void    keyPressEvent(QKeyEvent* event);
   void    keyReleaseEvent(QKeyEvent* event);

   void    wheelEvent(QWheelEvent *event);

   void    timerEvent(QTimerEvent *);

private:
   Renderer*   renderer;

   QPoint  lastPos;
   bool    bLeftButtonDown;
   bool    bRightButtonDown;
};

#endif
