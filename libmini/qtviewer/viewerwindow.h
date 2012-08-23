// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <QtGui/QMouseEvent>

#include <mini/mini_object.h>

#include "object.h"
#include "objects.h"
#include "objectlist.h"

class Renderer;
class WorkerThread;

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
   WorkerThread *getWorker();

   void setVertical(BOOLINT on);

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   void setRepo(ministring path);
   void setExport(ministring path);
   void setTmp(ministring path);

   void setResampleSettings(int level=0, int levels=1, int step=2);
   void setExportSettings(double power=2, double quality=90);

   void loadURL(ministring url);
   void loadURLs(ministrings urls);

   void loadMap(ministring url);
   void clearMaps();

   void loadImage(ministring url);
   void clearImages();

   BOOLINT addObject(ministring key, Object *obj, ministring tag);
   Object *getObject(ministring key);
   ministrings *getTags(ministring key);
   void addTag(ministring key, ministring tag);
   void removeTag(ministring key, ministring tag);
   void toggleTag(ministring key, ministring tag);
   BOOLINT hasTag(ministring key, ministring tag);
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

   void runAction(ministring action="",
                  ministring value="");

   void shade(ministring key);
   BOOLINT check_list(ministrings keys);
   void resample_list(ministrings keys, int level=0, int levels=1, int step=2);
   void save_list(ministrings keys, ministring filename="", int level=0);

   void notify(ministring text);
   ministrings browse(ministring title, ministring path="", BOOLINT newfile=FALSE);
   ministring browseDir(ministring title, ministring path="");

   ministrings make_grid_list(ministrings keys, int level=0);

signals:
   void changed(ministring key);
   void progress(double percentage, const ministring &job);

private slots:
   void reportProgress(double percentage, const ministring &job);
   void finishedJob(const ministring &job, const ministrings &args);
   void failedJob(const ministring &job, const ministrings &args);

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
   void dragLeaveEvent(QDragLeaveEvent *event);

public:
   void dropEvent(QDropEvent *event);

private:
   BOOLINT vertical;

   Renderer *viewer;
   WorkerThread *worker;

   ministring repository_path;
   ministring export_path;
   ministring tmp_path;

   int grid_level;
   int grid_levels;
   int grid_step;

   double shadePower;
   double jpegQuality;

   Objects objects;

   QPoint lastPos;
   QPoint movedPos;
   bool bLeftButtonDown;
   bool bRightButtonDown;

   void reportModifiers();
   void reportProgress();
};

#endif
