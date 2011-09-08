// (c) by Stefan Roettger

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <QtGui/QMouseEvent>

#include <mini/minilayer.h>

class Renderer;

class QMimeData;

class ViewerWindow : public QGLWidget
{
   Q_OBJECT;

public:
   ViewerWindow(QWidget* parent = 0);
   virtual ~ViewerWindow();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   void loadMap(QString url);
   void clearMaps();

   void gotoMap(minilayer *layer);

   void checkFog(bool on);
   void setFogDensity(double density);
   void checkContours(bool on);
   void checkSeaLevel(bool on);
   void setSeaLevel(double level);

signals:
   void changed(const QString url, minilayer *layer);

protected:
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();

   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent* event);
   void mouseMoveEvent(QMouseEvent *event);
   void mouseDoubleClickEvent(QMouseEvent *);

   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);

   void wheelEvent(QWheelEvent *event);

   void timerEvent(QTimerEvent *);

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);

private:
   Renderer* renderer;

   QPoint    lastPos;
   QPoint    movedPos;
   bool      bLeftButtonDown;
   bool      bRightButtonDown;

   void      reportModifiers();
};

#endif
