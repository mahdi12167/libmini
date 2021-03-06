// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QtOpenGL/qgl.h>

#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

#include <mini/mini_object.h>

#include "object.h"
#include "objects.h"
#include "objectlist.h"

class Renderer;
class WorkerThread;

#define TR(s) (ministring(tr(s).toStdString().c_str()))

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

   void setAspect(double aspect);
   void setRotation(double earth, double left=0.0, double back=0.0);

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   void setRepo(ministring path);
   void setExport(ministring path);
   void setTmp(ministring path);

   void setSplitSettings(double meters=0.0);
   void setResampleSettings(int level=0, int levels=1, int step=2);
   void setExportSettings(double power=2.0, double ambient=0.1, double quality=90.0);
   void setContourSettings(double spacing=100.0, double thickness=1.0, double border=1.0);
   void setContrastSettings(double black=0.0, double white=1.0, double linear=0.5);
   void setGammaSettings(double red=1.2, double green=1.6, double blue=1.0);
   void setTrackSettings(double delta=50*60, double length=500, double accuracy=50);

   ministring loadURL(ministring url);
   ministrings loadURLs(ministrings urls);

   BOOLINT loadMap(ministring url);
   void clearMaps();

   BOOLINT loadImage(ministring url);
   void clearImages();

   BOOLINT loadPath(ministring url);
   void clearPaths();

   int addObject(ministring key, Object *obj, ministring tag);
   int addObject(ministring key, Object *obj, ministrings tags);
   Object *getObject(ministring key);
   ministrings *getTags(ministring key);
   void addTag(ministring key, ministring tag);
   void addTags(ministring key, ministrings tags);
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

   void runAction(const ministring &action="",
                  const ministring &value="");

   void select_object(ministring key, BOOLINT yes=TRUE);
   void hide_object(ministring key, BOOLINT yes=TRUE);
   void create_extent(ministring key, double dh=0.0);
   void show_fullres(ministring key);
   void tack_layer(ministring key, BOOLINT bottom=FALSE);
   void untack_layer(ministring key);
   void shade_elevation(ministring key);
   void contour_elevation(ministring key);
   void colormap_elevation(ministring key, int map=0);
   void blend_imagery(ministrings keys, BOOLINT modulate=FALSE);
   void ndi_layers(ministrings keys, int method=0);
   void dci_layers(ministrings keys);
   void dwi_layers(ministrings keys);
   void mmi_layers(ministrings keys);
   void topo_layers(ministrings keys);
   void merge_layers(ministrings keys);
   void match_layers(ministrings keys);
   void split_layers(ministrings keys);
   void resample_list(ministrings keys, ministring crop_key="", int level=0, int levels=1, int step=2);
   void crop_list(ministrings keys, ministring crop_key, BOOLINT cell_centered);
   void save_grid_list(ministrings keys, ministring crop_key="", ministring filename="", int level=0);
   void save_list(ministrings keys, ministring filename="");
   BOOLINT load_list(ministring filename);

   ministring getRepo();
   ministring getExport();
   ministring getTmp();

   void getSplitSettings(double &meters);
   void getResampleSettings(int &level, int &levels, int &step);
   void getExportSettings(double &power, double &ambient, double &quality);
   void getContourSettings(double &spacing, double &thickness, double &border);
   void getContrastSettings(double &black, double &white, double &linear);
   void getGammaSettings(double &red, double &green, double &blue);
   void getTrackSettings(double &delta, double &length, double &accuracy);

   void notify(ministring text);
   ministrings browse(ministring title, ministring path="", BOOLINT newfile=FALSE);
   ministring browseDir(ministring title, ministring path="");

   ministrings make_grid_list(ministrings keys, int level=0);

signals:
   void signalChange(const ministring &action, const ministring &value="");
   void signalProgress(double percentage, const ministring &job, unsigned int jobs);

private slots:
   void receiveProgress(double percentage, const ministring &job, unsigned int jobs);
   void finishedJob(const ministring &job, const ministrings &args);
   void failedJob(const ministring &job, const ministrings &args, int errorcode);

protected:
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();

   Object_image *get_image(ministring key);
   Object_extents *get_extent(ministring key);

   ministring guess_panchro_layer(const ministrings &keys);
   ministring guess_nir_layer(const ministrings &keys);
   ministring guess_red_layer(const ministrings &keys);
   ministring guess_green_layer(const ministrings &keys);
   ministring guess_blue_layer(const ministrings &keys);

   BOOLINT check_list(ministrings keys);
   BOOLINT check_elev_list(ministrings keys);

   bool event(QEvent *event);

   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mouseDoubleClickEvent(QMouseEvent *event);

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
   double viewer_aspect;

   Renderer *viewer;
   WorkerThread *worker;

   double viewer_rotation_earth;
   double viewer_rotation_left;
   double viewer_rotation_back;

   ministring repository_path;
   ministring export_path;
   ministring tmp_path;

   double split_meters;

   int grid_level;
   int grid_levels;
   int grid_step;

   double shadePower;
   double shadeAmbient;
   double jpegQuality;

   double contourSpacing;
   double contourThickness;
   double contourBorder;

   double blackLevel;
   double whiteLevel;
   double contrastLinearity;

   double redGamma;
   double greenGamma;
   double blueGamma;

   double maxDelta;
   double maxLength;
   double minAccuracy;

   Objects objects;

   QPoint lastPos;
   QPoint movedPos;
   bool bLeftButtonDown;
   bool bMiddleButtonDown;
   bool bRightButtonDown;

   void reportModifiers();
   void reportProgress();
};

#endif
