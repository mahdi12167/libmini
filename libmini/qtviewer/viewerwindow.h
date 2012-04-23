// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <QtGui/QMouseEvent>

#include <mini/mini_object.h>

#include "objects.h"

class Renderer;

class QMimeData;

//! qt viewer window that
//! forwards mouse events to the camera and
//! forwards user events to the renderer
class ViewerWindow: public QGLWidget
{
   Q_OBJECT;

public:
   ViewerWindow();
   virtual ~ViewerWindow();

   Renderer *getViewer();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   void loadMap(ministring url);
   void clearMaps();

   void loadImage(ministring url);
   void clearImages();

   BOOLINT addObject(ministring key, Object *obj, ministring tag);
   Object *getObject(ministring key);
   ministrings *getTags(ministring key);
   ministrings listObjects();
   ministrings listObjects(ministring tag);
   void gotoObject(ministring key);
   void removeObject(ministring key);
   void removeObjects(ministrings keys);
   void clearObjects();

   void toggleStereo(bool on);
   void toggleWireFrame(bool on);

   void checkFog(bool on);
   void setFogDensity(double density);
   void checkContours(bool on);
   void checkSeaLevel(bool on);
   void setSeaLevel(double level);
   void checkLight(bool on);
   void setLight(double hour);
   void checkExagger(bool on);
   void setExagger(double scale);

signals:
   void changed(ministring key);

protected:
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();

   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mouseDoubleClickEvent(QMouseEvent *);

   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);

   void wheelEvent(QWheelEvent *event);

   void timerEvent(QTimerEvent *);

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);

private:
   Renderer *viewer;
   Objects objects;

   QPoint lastPos;
   QPoint movedPos;
   bool bLeftButtonDown;
   bool bRightButtonDown;

   void reportModifiers();
};

#endif
