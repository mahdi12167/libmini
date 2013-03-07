// (c) by Stefan Roettger, licensed under GPL 2+

#include <string>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <QtCore/QUrl>

#include <mini/mini_format.h>
#include <grid/grid.h>

#include "renderer.h"
#include "worker.h"

#include "viewerconst.h"
#include "viewerwindow.h"

ViewerWindow::ViewerWindow()
   : viewer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);

   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer));

   setCursor(Qt::CrossCursor);

   viewer_aspect = VIEWER_ASPECT;

   // init viewer
   viewer = new Renderer(this);
   if (viewer == NULL) MEMERROR();

   // init worker
   worker = new WorkerThread;
   if (worker == NULL) MEMERROR();

   // setup queued worker connection
   qRegisterMetaType<ministring>("ministring");
   qRegisterMetaType<ministrings>("ministrings");
   connect(worker, SIGNAL(reportProgress(double, ministring)),
           this, SLOT(reportProgress(double, ministring)),
           Qt::QueuedConnection);
   connect(worker, SIGNAL(finishedJob(ministring, ministrings)),
           this, SLOT(finishedJob(ministring, ministrings)),
           Qt::QueuedConnection);
   connect(worker, SIGNAL(failedJob(ministring, ministrings, int)),
           this, SLOT(failedJob(ministring, ministrings, int)),
           Qt::QueuedConnection);

   // worker settings
   grid_level = 0;
   grid_levels = 1;
   grid_step = 2;

   setExportSettings();
   setDefaultSettings();

   // accept drag and drop
   setAcceptDrops(true);
}

ViewerWindow::~ViewerWindow()
{
   if (worker != NULL)
      delete worker;

   if (viewer != NULL)
      delete viewer;
}

Renderer *ViewerWindow::getViewer()
{
   return(viewer);
}

WorkerThread *ViewerWindow::getWorker()
{
   return(worker);
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

      // load objects url from arguments
      QStringList dataPathList = QCoreApplication::arguments();
      for (int i=1; i<dataPathList.size(); i++)
         runAction("open", dataPathList[i].toStdString().c_str());
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
      if (QApplication::keyboardModifiers() & Qt::ControlModifier ||
          QApplication::keyboardModifiers() & Qt::AltModifier)
         bRightButtonDown = true;
      else
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
      // a left button click
      if (bLeftButtonDown)
      {
         viewer->getCamera()->focusOnTarget();
      }
      // a right button click
      else if (bRightButtonDown)
      {
         // get eye position
         minicoord pos = viewer->getCamera()->get_eye();

         // get target vector
         miniv3d vec = viewer->getCamera()->targetVector();

         // shoot ray in target direction
         mininode_geometry *obj = viewer->pick(pos, vec);
         if (obj != NULL)
            if (!obj->get_name().empty())
               runAction("select", obj->get_name());
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
   static const double border=0.2;

   float dx = (float)(event->x()-movedPos.x())/width();
   float dy = (float)(event->y()-movedPos.y())/height();

   reportModifiers();

   viewer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   // a left button move
   if (bLeftButtonDown)
      viewer->getCamera()->rotateCamera(dx, dy);
   // a right button move
   else if (bRightButtonDown)
   {
      // get eye position
      minicoord pos = viewer->getCamera()->get_eye();

      // get target vector
      miniv3d vec = viewer->getCamera()->targetVector();
      vec.normalize();

      // shoot ray in target direction
      mininode_geometry *obj = viewer->pick(pos, vec);
      if (obj != NULL)
      {
         // get drag vector
         miniv3d drag  = viewer->getCamera()->targetVector(dx, dy);
         drag.normalize();

         // shoot ray for target vector
         double dist0 = viewer->shoot(pos, vec);

         // shoot ray for drag vector
         double dist1 = viewer->shoot(pos, drag);

         if (dist0<MAXFLOAT && dist1<MAXFLOAT)
         {
            // compute target hit point
            minicoord pos0 = pos+dist0*vec;

            // compute drag hit point
            minicoord pos1 = pos+dist1*drag;

            // drag to new position
            if (!obj->get_name().empty())
            {
               Object *object = getObject(obj->get_name());

               if (object != NULL)
               {
                  Object_extents *extent = dynamic_cast<Object_extents *>(object);

                  if (extent != NULL)
                  {
                     double s,t;

                     grid_extent area = extent->get_extent();
                     area.get_norm(pos0, s, t);

                     if (s>border && s<1.0-border && t>border && t<1.0-border)
                        extent->move(pos0, pos1); // grabbed extent body
                     else if ((s<border || s>1.0-border) && (t<border || t>1.0-border))
                        extent->rotate(pos0, pos1); // grabbed extent corners
                     else
                        if (s<border || s>1.0-border)
                           extent->scale_ds(pos0, pos1); // grabbed vertical extent edges
                        else
                           extent->scale_dt(pos0, pos1); // grabbed horizontal extent edges

                     extent->updateGFX();
                  }
               }
            }
         }
      }
   }

   movedPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
   reportModifiers();

   mousePressEvent(event);

   if (bLeftButtonDown)
      viewer->getCamera()->focusOnTarget(0.75);
   else if (bRightButtonDown)
   {
      // get eye position
      minicoord pos = viewer->getCamera()->get_eye();

      // get target vector
      miniv3d vec = viewer->getCamera()->targetVector();

      // shoot ray in target direction
      mininode_geometry *obj = viewer->pick(pos, vec);
      if (obj != NULL)
      {
         ministring name = obj->get_name();

         if (!name.empty())
            if (hasTag(name, "image"))
                runAction("fullres", name);
      }
   }

   bLeftButtonDown = false;
   bRightButtonDown = false;
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

   viewer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   if (event->orientation() == Qt::Vertical)
      viewer->getCamera()->moveCameraForward(numDegrees/360.0);
   else
      viewer->getCamera()->moveCameraSideward(numDegrees/360.0);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   reportModifiers();
   reportProgress();

   viewer->getCamera()->timerEvent(event->timerId());
}

void ViewerWindow::reportModifiers()
{
   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   viewer->getCamera()->modifierKey(ModifierShift, keyMod & Qt::ShiftModifier);
   viewer->getCamera()->modifierKey(ModifierControl, keyMod & Qt::ControlModifier);
   viewer->getCamera()->modifierKey(ModifierAlt, keyMod & Qt::AltModifier);
   viewer->getCamera()->modifierKey(ModifierMeta, keyMod & Qt::MetaModifier);
}

void ViewerWindow::reportProgress()
{
   if (worker->running())
      emit progress(worker->get_progress(), worker->get_job_id()); // synchronous
}

void ViewerWindow::setRepo(ministring path)
{
   repository_path = Object::normalize_path(path);

   objects.set_repo(repository_path);
}

void ViewerWindow::setExport(ministring path)
   {export_path = Object::normalize_path(path);}

void ViewerWindow::setTmp(ministring path)
   {tmp_path = Object::normalize_path(path);}

void ViewerWindow::setResampleSettings(int level, int levels, int step)
{
   grid_level = level;
   grid_levels = levels;
   grid_step = step;
}

void ViewerWindow::setExportSettings(double power, double ambient, double quality)
{
   shadePower = power;
   shadeAmbient = ambient;
   jpegQuality = quality;
}

void ViewerWindow::setDefaultSettings(double c_spacing, double c_thickness, double c_border)
{
   contourSpacing = c_spacing;
   contourThickness = c_thickness;
   contourBorder = c_border;
}

ministring ViewerWindow::loadURL(ministring url)
{
   url = Object::normalize_file(url);

   if (!Object::is_absolute_path(url))
      url = repository_path+url;

   if (getObject(url) != NULL) // already existing?
      gotoObject(url);
   else
   {
      // check imagery extensions
      if (url.endswith(".jpg") ||
          url.endswith(".png") ||
          url.endswith(".tif") ||
          url.endswith(".jpgintif"))
         loadImage(url);
      // check elevation extensions
      else if (url.endswith(".bt"))
         loadImage(url);
      // check tileset extensions
      else if (url.endswith(".ini"))
         loadMap(url);
      // check qtv extension
      else if (url.endswith(".qtv"))
         load_list(url);
      // check for directories
      else if (!url.suffix("/").contains("."))
         loadMap(url);
      else
         loadImage(url); // try unknown format via gdal
   }

   return(url);
}

void ViewerWindow::loadURLs(ministrings urls)
{
   for (unsigned int i=0; i<urls.getsize(); i++)
      loadURL(urls[i]);
}

void ViewerWindow::loadMap(ministring url)
{
   int errorcode;

   url = Object::normalize_file(url);

   if (!Object::is_absolute_path(url))
      url = repository_path+url;

   if (url.endswith(".ini"))
   {
      unsigned int lio, lio0;

      lio = url.getsize();
      if (url.findr("/", lio0)) lio = lio0;
      url.truncate(lio);
   }

   Object_tileset *tileset = new Object_tileset(url, repository_path, viewer);
   if (tileset == NULL) MEMERROR();

   errorcode = addObject(url, tileset, "tileset");

   if (errorcode != OBJECT_SUCCESS)
   {
      delete tileset;

      notify(TR("Unable to load map from url=")+url);
   }
}

void ViewerWindow::clearMaps()
{
   removeObjects(listObjects("tileset"));
}

void ViewerWindow::loadImage(ministring url)
{
   int errorcode;

   url = Object::normalize_file(url);

   if (!Object::is_absolute_path(url))
      url = repository_path+url;

   Object_image *image = new Object_image(url, repository_path, viewer);

   if (image == NULL) MEMERROR();

   errorcode = addObject(url, image, "image");

   if (errorcode == OBJECT_SUCCESS ||
       errorcode == OBJECT_NOT_REFERENCED)
   {
      if (image->is_imagery())
         addTag(url, "imagery");
      else
         addTag(url, "elevation");

      image->focus();

      ThumbJob *job = new ThumbJob("", 3, 30.0);
      if (job == NULL) MEMERROR();

      job->append(image->get_full_name());
      worker->run_job(job);
   }
   else
   {
      delete image;

      notify(TR("Unable to load image from url=")+url+"\n\n"+
             TR("Standard file format ist GeoTiff."));
   }

   if (errorcode == OBJECT_NOT_REFERENCED)
      notify(TR("Unable to geo-reference the image properly")+"\n\n"+
             "Please use the gdalwarp utility from gdal.org to reproject the image into a standard coordinate reference system.\n"
             "Standard coordinate reference systems (CRS) are Lat/Lon, UTM and Mercator.");
   else if (errorcode == OBJECT_TOO_LARGE)
      notify(TR("Unable to handle an image of that size")+"\n\n"+
             "Please use the gdal_translate utility from gdal.org to partition the image into smaller tiles.\n"
             "A handable image has to be smaller than 1GB in size.");
}

void ViewerWindow::clearImages()
{
   removeObjects(listObjects("image"));
}

int ViewerWindow::addObject(ministring key, Object *obj, ministring tag)
{
   int errorcode;

   errorcode = objects.add(key, obj, tag);

   if (errorcode != OBJECT_FAILURE)
      emit changed(key);

   return(errorcode);
}

int ViewerWindow::addObject(ministring key, Object *obj, ministrings tags)
{
   int errorcode;

   errorcode = objects.add(key, obj, tags);

   if (errorcode != OBJECT_FAILURE)
      emit changed(key);

   return(errorcode);
}

Object *ViewerWindow::getObject(ministring key)
{
   return(objects.get(key));
}

ministrings *ViewerWindow::getTags(ministring key)
{
   return(objects.get_tags(key));
}

void ViewerWindow::addTag(ministring key, ministring tag)
{
   objects.add_tag(key, tag);
   emit changed(key);
}

void ViewerWindow::addTags(ministring key, ministrings tags)
{
   objects.add_tags(key, tags);
   emit changed(key);
}

void ViewerWindow::removeTag(ministring key, ministring tag)
{
   objects.remove_tag(key, tag);
   emit changed(key);
}

void ViewerWindow::toggleTag(ministring key, ministring tag)
{
   if (hasTag(key, tag))
      removeTag(key, tag);
   else
      addTag(key, tag);
}

BOOLINT ViewerWindow::hasTag(ministring key, ministring tag)
{
   return(objects.has_tag(key, tag));
}

ministrings ViewerWindow::listObjects()
{
   return(objects.get_items());
}

ministrings ViewerWindow::listObjects(ministring tag)
{
   return(objects.get_items(tag));
}

void ViewerWindow::gotoObject(ministring key)
{
   Object *obj=objects.get(key);

   if (obj != NULL) obj->focus();
}

void ViewerWindow::removeObject(ministring key)
{
   objects.remove(key);
   emit changed(key);
}

void ViewerWindow::removeObjects(ministrings keys)
{
   unsigned int i;

   for (i=0; i<keys.getsize(); i++)
      removeObject(keys[i]);
}

void ViewerWindow::clearObjects()
{
   removeObjects(listObjects());
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
         QUrl qurl = urlList.at(i);
         ministring url=qurl.toString().toStdString().c_str();

         runAction("open", url);
      }
   }
}

void ViewerWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}

void ViewerWindow::runAction(ministring action,
                             ministring value)
{
   if (action == "repo")
   {
      setRepo(value);
   }
   else if (action == "export")
   {
      setExport(value);
   }
   else if (action == "tmp")
   {
      setTmp(value);
   }
   else if (action == "open")
   {
      if (value != "")
         loadURL(value);
      else
         loadURLs(browse("Open File", repository_path));
   }
   else if (action == "select")
   {
      select_object(value, !hasTag(value, "selected"));
   }
   else if (action == "select_all")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         select_object(keys[i], TRUE);
   }
   else if (action == "deselect_all")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         select_object(keys[i], FALSE);
   }
   else if (action == "info")
   {
      Object *obj = getObject(value);
      if (obj) notify(obj->get_info());
   }
   else if (action == "show")
   {
      hide_object(value, FALSE);
   }
   else if (action == "hide")
   {
      hide_object(value, TRUE);
   }
   else if (action == "toggle")
   {
      hide_object(value, !hasTag(value, "hidden"));
   }
   else if (action == "show_elevation")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (hasTag(keys[i], "image"))
            if (hasTag(keys[i], "elevation"))
               hide_object(keys[i], FALSE);
   }
   else if (action == "hide_elevation")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (hasTag(keys[i], "image"))
            if (hasTag(keys[i], "elevation"))
               hide_object(keys[i], TRUE);
   }
   else if (action == "show_imagery")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (hasTag(keys[i], "image"))
            if (hasTag(keys[i], "imagery"))
               hide_object(keys[i], FALSE);
   }
   else if (action == "hide_imagery")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (hasTag(keys[i], "image"))
            if (hasTag(keys[i], "imagery"))
               hide_object(keys[i], TRUE);
   }
   else if (action == "fullres")
   {
      if (value != "")
         show_fullres(value);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "create_extent")
   {
      if (value != "")
         create_extent(value, 1000);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "shade_elevation")
   {
      if (value != "")
         shade_elevation(value);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "shade_selected")
   {
      ministrings keys = listObjects("selected");

      for (unsigned int i=0; i<keys.getsize(); i++)
         if (hasTag(keys[i], "elevation"))
            shade_elevation(keys[i]);
   }
   else if (action == "resample")
   {
      ministrings keys = listObjects("image");
      resample_list(keys, value, grid_level, grid_levels, grid_step);
   }
   else if (action == "resample_selected")
   {
      ministrings keys = listObjects("image");

      ministrings sel_keys;
      for (unsigned int i=0; i<keys.getsize(); i++)
         if (hasTag(keys[i], "selected"))
            sel_keys.append(keys[i]);

      resample_list(sel_keys, "", grid_level, grid_levels, grid_step);
   }
   else if (action == "resample_selected_area")
   {
      ministrings keys = listObjects("image");

      ministrings sel_keys;
      for (unsigned int i=0; i<keys.getsize(); i++)
         if (hasTag(keys[i], "selected"))
            sel_keys.append(keys[i]);

      resample_list(sel_keys, value, grid_level, grid_levels, grid_step);
   }
   else if (action == "resample_all")
   {
      ministrings keys = listObjects("image");
      resample_list(keys, "", grid_level, grid_levels, grid_step);
   }
   else if (action == "crop_elevation")
   {
      ministrings keys = listObjects("image");
      crop_list(keys, value, FALSE);
   }
   else if (action == "crop_imagery")
   {
      ministrings keys = listObjects("image");
      crop_list(keys, value, TRUE);
   }
   else if (action == "crop_selected")
   {
      ministrings keys = listObjects("image");

      ministrings sel_keys;
      for (unsigned int i=0; i<keys.getsize(); i++)
         if (hasTag(keys[i], "selected"))
            sel_keys.append(keys[i]);

      if (hasTag(value, "elevation"))
         crop_list(sel_keys, value, FALSE);
      else if (hasTag(value, "imagery"))
         crop_list(sel_keys, value, TRUE);
      else
         notify(TR("Cannot determine crop type"));
   }
   else if (action == "abort")
   {
      worker->abort_jobs();
   }
   else if (action == "treat_black")
   {
      TreatBlackJob *job = new TreatBlackJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "treat_white")
   {
      TreatWhiteJob *job = new TreatWhiteJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "mask_black")
   {
      MaskBlackJob *job = new MaskBlackJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "mask_white")
   {
      MaskWhiteJob *job = new MaskWhiteJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "remove_bathy")
   {
      RemoveBathyJob *job = new RemoveBathyJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "keep_bathy")
   {
      KeepBathyJob *job = new KeepBathyJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "fill_missing")
   {
      FillMissingJob *job = new FillMissingJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "fill_holes")
   {
      FillHolesJob *job = new FillHolesJob(repository_path,export_path);
      if (job == NULL) MEMERROR();

      job->append(value);
      worker->run_job(job);
   }
   else if (action == "contour")
   {
      if (value != "")
         contour_elevation(value);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "graymap")
   {
      if (value != "")
         graymap_elevation(value);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "colormap")
   {
      if (value != "")
         colormap_elevation(value);
      else
         notify(TR("Operation requires a layer"));
   }
   else if (action == "save_db")
   {
      SaveJob *job = new SaveJob(getObject(value)->get_relative_path(), export_path, TRUE);
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_relative_name());
      worker->run_job(job);
   }
   else if (action == "save_tif")
   {
      SaveJob *job = new SaveJob(getObject(value)->get_relative_path(), export_path);
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_relative_name());
      worker->run_job(job);
   }
   else if (action == "save_jpgintif")
   {
      SaveJob *job = new SaveJob(getObject(value)->get_relative_path(), export_path, FALSE, FALSE, FALSE, TRUE, jpegQuality);
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_relative_name());
      worker->run_job(job);
   }
   else if (action == "save")
   {
      ministrings keys = listObjects();
      save_list(keys, "");
   }
   else if (action == "save_grid")
   {
      ministrings keys = listObjects("image");
      save_grid_list(keys, value, "", grid_level);
   }
   else if (action == "load")
   {
      clearObjects();
      load_list("");
   }
   else if (action == "delete")
   {
      removeObject(value);
   }
   else if (action == "delete_all")
   {
      removeObjects(listObjects());
   }
   else if (action == "delete_selected")
   {
      removeObjects(listObjects("selected"));
   }
   else if (action == "goto")
   {
      gotoObject(value);
   }
}

Object_image *ViewerWindow::get_image(ministring key)
{
   Object *obj = getObject(key);

   if (obj != NULL)
   {
      Object_image *image = dynamic_cast<Object_image *>(obj);
      return(image);
   }

   return(NULL);
}

Object_extents *ViewerWindow::get_extent(ministring key)
{
   Object *obj = getObject(key);

   if (obj != NULL)
   {
      Object_extents *extent = dynamic_cast<Object_extents *>(obj);
      return(extent);
   }

   return(NULL);
}

void ViewerWindow::select_object(ministring key, BOOLINT yes)
{
   if (yes)
      if (!hasTag(key, "selected"))
         addTag(key, "selected");

   if (!yes)
      if (hasTag(key, "selected"))
         removeTag(key, "selected");

   Object_extents *extent = get_extent(key);
   if (extent) extent->mark(yes);
}

void ViewerWindow::hide_object(ministring key, BOOLINT yes)
{
   if (yes)
      if (!hasTag(key, "hidden"))
         addTag(key, "hidden");

  if (!yes)
     if (hasTag(key, "hidden"))
        removeTag(key, "hidden");

  Object *obj = getObject(key);
  if (obj) obj->show(!yes);
}

void ViewerWindow::create_extent(ministring key, double dh)
{
   int errorcode = OBJECT_FAILURE;

   grid_extent ext;

   Object_extents *extent = get_extent(key);
   if (extent) ext = extent->get_extent();

   if (ext.check())
   {
      ext.convert2metric();
      ext.elevate(dh);

      ministring key_ext = "extent_" + Objects::newkey() + "_from_" + key;
      Object_extent *obj_ext = new Object_extent(key_ext, ext, viewer);
      if (obj_ext == NULL) MEMERROR();

      errorcode = addObject(key_ext, obj_ext, "extent");

      if (errorcode != OBJECT_SUCCESS)
         delete obj_ext;
      else
         obj_ext->focus();
   }

   if (errorcode != OBJECT_SUCCESS)
      notify(TR("Unable to create extent from object"));
}

void ViewerWindow::show_fullres(ministring key)
{
   Object_image *image = get_image(key);

   if (image)
   {
      FullResJob *job = new FullResJob("", 30, 4096);
      if (job == NULL) MEMERROR();

      job->append(image->get_full_name());
      worker->run_job(job);
   }
}

void ViewerWindow::shade_elevation(ministring key)
{
   Object_image *image = get_image(key);

   if (image)
      if (!image->is_elevation())
         notify(TR("Shading requires an elevation layer"));
      else
      {
         ShadeJob *job = new ShadeJob("", shadePower, shadeAmbient);
         if (job == NULL) MEMERROR();

         job->append(image->get_full_name());
         worker->run_job(job);
      }
}

void ViewerWindow::contour_elevation(ministring key)
{
   Object_image *image = get_image(key);

   if (image)
      if (!image->is_elevation())
         notify(TR("Contouring requires an elevation layer"));
      else
      {
         ContourJob *job = new ContourJob("", contourSpacing, contourThickness, contourBorder);
         if (job == NULL) MEMERROR();

         job->append(image->get_full_name());
         worker->run_job(job);
      }
}

grid_colormap ViewerWindow::make_graymap()
   {
   static double gamma=0.5;
   static double minheight=-10000.0;
   static double maxheight=10000.0;
   static double step=100.0;

   double height;
   double gray1,gray2;

   grid_colormap graymap;

   // above sea level
   for (height=0.0; height<maxheight+step/2; height+=step)
      {
      gray1=pow(height/maxheight,gamma);
      gray2=pow((height+step)/maxheight,gamma);

      graymap.append(height,miniv4f(gray1,gray1,gray1,1.0),
                     height+step,miniv4f(gray2,gray2,gray2,1.0));
      }

   // below sea level
   for (height=0.0; height>minheight-step/2; height-=step)
      {
      gray1=1.0-pow(-height/maxheight,gamma);
      gray2=1.0-pow(-(height-step)/maxheight,gamma);

      graymap.append(height,miniv4f(0.0,0.25*gray1,0.5*gray1,1.0),
                     height-step,miniv4f(0.0,0.25*gray2,0.5*gray2,1.0));
      }

   return(graymap);
   }

void ViewerWindow::graymap_elevation(ministring key)
{
   Object_image *image = get_image(key);

   if (image)
      if (!image->is_elevation())
         notify(TR("Grayscale-mapping requires an elevation layer"));
      else
      {
         grid_colormap graymap = make_graymap();

         ColorMapJob *job = new ColorMapJob("", &graymap);
         if (job == NULL) MEMERROR();

         job->append(image->get_full_name());
         worker->run_job(job);
      }
}

grid_colormap ViewerWindow::make_colormap()
   {
   grid_colormap colormap;

   // above sea level:

   // green
   colormap.append(0.0,miniv4f(0.0,0.75,0.0,1.0),500.0,miniv4f(0.5,1.0,0.0,1.0));
   // brown
   colormap.append(500.0,miniv4f(0.5,1.0,0.0,1.0),1000.0,miniv4f(0.5,0.5,0.0,1.0));
   // dark
   colormap.append(1000.0,miniv4f(0.5,0.5,0.0,1.0),2000.0,miniv4f(0.25,0.25,0.25,1.0));
   // gray
   colormap.append(2000.0,miniv4f(0.25,0.25,0.25,1.0),3000.0,miniv4f(0.5,0.5,0.5,1.0));
   // bright
   colormap.append(3000.0,miniv4f(0.5,0.5,0.5,1.0),10000.0,miniv4f(0.5,1.0,1.0,1.0));

   // below sea level:
   
   // blue
   colormap.append(0.0,miniv4f(0.25,0.5,1.0,1.0),-100.0,miniv4f(0.0,0.25,0.5,1.0));
   // dark
   colormap.append(-100.0,miniv4f(0.0,0.25,0.5,1.0),-1000.0,miniv4f(0.0,0.0,0.25,1.0));
   // black
   colormap.append(-1000.0,miniv4f(0.0,0.0,0.25,1.0),-10000.0,miniv4f(0.0,0.0,0.0,1.0));

   return(colormap);
   }

void ViewerWindow::colormap_elevation(ministring key)
{
   Object_image *image = get_image(key);

   if (image)
      if (!image->is_elevation())
         notify(TR("Color-mapping requires an elevation layer"));
      else
      {
         grid_colormap colormap = make_colormap();

         ColorMapJob *job = new ColorMapJob("", &colormap,
                                            shadePower, shadeAmbient);

         if (job == NULL) MEMERROR();

         job->append(image->get_full_name());
         worker->run_job(job);
      }
}

BOOLINT ViewerWindow::check_list(ministrings keys)
{
   unsigned int i;

   unsigned int elev,imag;

   elev=imag=0;

   for (i=0; i<keys.size(); i++)
   {
      Object_image *image = get_image(keys[i]);

      if (image)
      {
         if (image->is_elevation()) elev++;
         if (image->is_imagery()) imag++;
      }
   }

   if (elev==0)
   {
      notify(TR("Resampling requires at least one elevation layer"));
      return(FALSE);
   }

   if (elev>1 && imag==0)
   {
      notify(TR("Resampling requires at least one imagery or shaded layer"));
      return(FALSE);
   }

   return(TRUE);
}

BOOLINT ViewerWindow::check_elev_list(ministrings keys)
{
   unsigned int i;

   unsigned int elev,imag;

   elev=imag=0;

   for (i=0; i<keys.size(); i++)
   {
      Object_image *image = get_image(keys[i]);

      if (image)
      {
         if (image->is_elevation()) elev++;
         if (image->is_imagery()) imag++;
      }
   }

   if (elev==0) return(FALSE);
   if (imag>0) return(FALSE);

   return(TRUE);
}

void ViewerWindow::resample_list(ministrings keys,
                                 ministring crop_key,
                                 int level, int levels, int step)
{
   unsigned int i;

   grid_extent crop_ext;
   ministring crop_name;

   if (getObject(crop_key) != NULL)
   {
      if (hasTag(crop_key, "image"))
         crop_name = getObject(crop_key)->get_full_name();
      else
      {
         Object_extents *extent = get_extent(crop_key);
         if (extent) crop_ext = extent->get_extent();
      }
   }

   ResampleJob *job;

   if (check_elev_list(keys))
      job = new ResampleElevJob("", export_path,
                                level, levels, step,
                                5.0,0.0,
                                &crop_ext,crop_name,
                                tmp_path);
   else if (check_list(keys))
      job = new ResampleJob("", export_path,
                            level, levels, step,
                            5.0,0.0,
                            &crop_ext,crop_name,
                            tmp_path);
   else return;

   if (job == NULL) MEMERROR();

   for (i=0; i<keys.size(); i++)
      job->append(getObject(keys[i])->get_full_name());

   worker->run_job(job);
}

void ViewerWindow::crop_list(ministrings keys,
                             ministring crop_key,
                             BOOLINT cell_centered)
{
   unsigned int i;

   grid_extent ext;

   Object *obj = getObject(crop_key);

   if (obj != NULL)
   {
      Object_extents *extent = get_extent(crop_key);

      if (extent)
         ext = extent->get_extent();
      else
         notify(TR("Cropping requires a layer that defines the crop area"));
   }
   else
      notify(TR("Cannot determine crop area"));

   if (ext.check())
   {
      ministring output = CropJob::make_name(repository_path,
                                             obj->get_relative_name(),
                                             export_path);

      CropJob *job = new CropJob(repository_path,
                                 export_path, output,
                                 ext,
                                 cell_centered,
                                 0.0,
                                 tmp_path);

      if (job == NULL) MEMERROR();

      for (i=0; i<keys.size(); i++)
         job->append(getObject(keys[i])->get_relative_name());

      worker->run_job(job);
   }
}

void ViewerWindow::save_grid_list(ministrings keys,
                                  ministring crop_key,
                                  ministring filename,
                                  int level)
{
   unsigned int i;

   ministrings filenames;

   grid_extent crop_ext;
   ministring crop_name;

   if (!check_list(keys)) return;

   for (i=0; i<keys.size(); i++)
      filenames.append(getObject(keys[i])->get_relative_name());

   if (getObject(crop_key) != NULL)
   {
      if (hasTag(crop_key, "image"))
         crop_name = getObject(crop_key)->get_relative_name();
      else
      {
         Object_extents *extent = get_extent(crop_key);
         if (extent) crop_ext = extent->get_extent();
      }
   }

   ministrings grid_list = ResampleJob::make_grid_list(filenames,
                                                       repository_path, export_path,
                                                       level,
                                                       shadePower,shadeAmbient,
                                                       &crop_ext,crop_name);

   if (filename=="")
   {
      ministrings files = browse(TR("Save To Grid File"), export_path, TRUE);
      if (files.size()==0) return;

      filename = files[0];
      if (!filename.endswith(".grid")) filename += ".grid";
   }

   grid_list.save(filename);
}

void ViewerWindow::save_list(ministrings keys, ministring filename)
{
   unsigned int i;

   ministrings qtv;

   qtv.append("qtviewer file format .qtv");
   qtv.append("repo "+repository_path);
   qtv.append("export "+export_path);
   qtv.append(ministring("levels ")+(double)grid_level+"/"+(double)grid_levels+"/"+(double)grid_step);

   for (i=0; i<keys.size(); i++)
   {
      ministring key = keys[i];
      ministring info = key + " = [" + getObject(keys[i])->serialize() + "]";
      info += "[" + getTags(keys[i])->to_string(",") + "]";

      qtv.append(info);
   }

   if (filename=="")
   {
      ministrings files = browse(TR("Save"), export_path, TRUE);
      if (files.size()==0) return;

      filename = files[0];
      if (!filename.endswith(".qtv")) filename += ".qtv";
   }

   qtv.save(filename);
}

BOOLINT ViewerWindow::load_list(ministring filename)
{
   unsigned int i;

   ministrings qtv;

   if (filename=="")
   {
      ministrings files = browse(TR("Load"), export_path, FALSE);
      if (files.size()==0) return(FALSE);
      filename=files[0];
   }

   qtv.load(filename);

   if (!qtv.empty())
      if (qtv[0]!="qtviewer file format .qtv") return(FALSE);
      else qtv.dispose(0);

   if (!qtv.empty())
      if (!qtv[0].startswith("repo ")) return(FALSE);
      else
      {
         setRepo(qtv[0].tail("repo "));
         qtv.dispose(0);
      }

   if (!qtv.empty())
      if (!qtv[0].startswith("export ")) return(FALSE);
      else
      {
         setExport(qtv[0].tail("export "));
         qtv.dispose(0);
      }

   if (!qtv.empty())
      if (!qtv[0].startswith("levels ")) return(FALSE);
      else
      {
         ministrings params;
         params.from_string(qtv[0].tail("levels "), "/");
         if (params.size()!=3) return(FALSE);
         setResampleSettings(params[0].value(),params[1].value(),params[2].value());
         qtv.dispose(0);
      }

   for (i=0; i<qtv.size(); i++)
   {
      ministring key, info, tags;

      if (qtv[i].endswith("]"))
      {
         qtv[i] = qtv[i].head("]");
         tags = qtv[i].suffix("[");
         qtv[i] = qtv[i].head("[");
      }
      else return(FALSE);

      if (qtv[i].endswith("]"))
      {
         qtv[i] = qtv[i].head("]");
         info = qtv[i].suffix(" = [");
         qtv[i] = qtv[i].head(" = [");

         key = qtv[i];
      }
      else return(FALSE);

      ministrings taglist;
      taglist.from_string(tags, ",");

      if (taglist[0] == "tileset" ||
          taglist[0] == "image")
         key = loadURL(key);
      else
      {
         Object *obj = NULL;

         if (taglist[0] == "extent")
            obj = Object_extent::deserialize(key, info, viewer);

         if (obj != NULL)
         {
            int errorcode;

            errorcode = addObject(key, obj ,taglist);

            if (errorcode != OBJECT_SUCCESS)
               return(FALSE);

            gotoObject(key);
         }
         else return(FALSE);
      }

      if (tags.contains("hidden"))
          runAction("hide", key);

      if (tags.contains("selected"))
         runAction("select", key);
   }

   return(TRUE);
}

void ViewerWindow::notify(ministring text)
{
   QMessageBox::information(this, tr("Information"),
                            text.c_str(),
                            QMessageBox::Ok);
}

ministrings ViewerWindow::browse(ministring title,
                                 ministring path,
                                 BOOLINT newfile)
{
   QFileDialog* fd = new QFileDialog(this, title.c_str());
   if (fd == NULL) MEMERROR();

   if (!newfile) fd->setFileMode(QFileDialog::ExistingFiles);
   else fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);
   if (newfile) fd->setAcceptMode(QFileDialog::AcceptSave);
   fd->setFilter(tr("All Files (*.*);;Ini Files (*.ini);;Binary Terrain (*.bt);;Images (*.tif *.tiff *.jpg *.png);; QTV Files (*.qtv);; Grid Files (*.grid)"));

   if (path!="") fd->setDirectory(path.c_str());

   ministrings files;

   if (fd->exec() == QDialog::Accepted)
      for (int i=0; i<fd->selectedFiles().size(); i++)
      {
         QString fileName = fd->selectedFiles().at(i);

         if (!fileName.isNull())
            files += fileName.toStdString().c_str();
      }

   return(files);
}

ministring ViewerWindow::browseDir(ministring title,
                                   ministring path)
{
   QFileDialog *fd = new QFileDialog(this, title.c_str());
   if (fd == NULL) MEMERROR();

   fd->setFileMode(QFileDialog::DirectoryOnly);
   fd->setViewMode(QFileDialog::List);
   if (path!="") fd->setDirectory(path.c_str());

   ministring dir;

   if (fd->exec() == QDialog::Accepted)
   {
      QString fileName = fd->selectedFiles().at(0);

      if (!fileName.isNull())
         dir=fileName.toStdString().c_str();
   }

   return(dir);
}

void ViewerWindow::reportProgress(double percentage, const ministring &job)
{
   emit progress(percentage, job); // asynchronous
}

void ViewerWindow::finishedJob(const ministring &job, const ministrings &args)
{
   if (job=="thumb")
   {
      // autoload thumbs
      for (unsigned int i=0; i<args.size(); i++)
      {
         // autoselect thumb name
         ministring thumb = ThumbJob::make_name(args[i]);

         // load from db format
         databuf buf;
         buf.loaddata(thumb.c_str());
         if (buf.missing()) continue;

         // add thumb to scene graph
         Object_image *image = get_image(args[i]);
         if (image) image->set_thumb(&buf);

         // release thumb data
         buf.release();
      }

      // try to load full-res textures if already present
      finishedJob("fullres", args);
   }
   else if (job=="fullres")
   {
      // autoload full-res textures
      for (unsigned int i=0; i<args.size(); i++)
      {
         // autoselect full-res name
         ministring fullres = FullResJob::make_name(args[i]);

         // load from db format
         databuf buf;
         buf.loaddata(fullres.c_str());
         if (buf.missing()) continue;

         // add full-res texture to scene graph
         Object_image *image = get_image(args[i]);
         if (image) image->set_fullres(&buf);

         // release full-res texture data
         buf.release();
      }
   }
   else if (job=="shader")
   {
      // process shaded keys
      for (unsigned int i=0; i<args.size(); i++)
      {
         // autoselect shaded layer name
         ministring name = ShadeJob::make_name(args[i]);

         // autoload shaded layer
         if (name!="")
         {
            // open shaded layer
            runAction("open", name);

            // hide original layer
            runAction("hide", args[i]);
         }
      }
   }
   else if (job=="resampler")
   {
      // autoselect grid name
      ministring name = ResampleJob::make_grid_name(args, "");

      // autoload resampled tileset
      if (name!="")
      {
         // open resampled tileset
         runAction("open", export_path+name);

         // make resampled layers invisible
         for (unsigned int i=0; i<args.size(); i++)
            runAction("hide", args[i]);
      }
   }
}

void ViewerWindow::failedJob(const ministring &job, const ministrings &/*args*/, int errorcode)
{
   if (job=="shader")
   {
      notify(TR("Shading failed"));
   }
   else if (job=="resampler")
   {
      notify(TR("Resampling failed"));
   }
   else if (job=="cropper")
   {
      if (errorcode == GRID_WORKER_FILE_EXISTS)
         notify(TR("Not cropped: file already exists"));
      else
         notify(TR("Cropping failed"));
   }
   else if (job=="modifier")
   {
      if (errorcode == GRID_WORKER_FILE_EXISTS)
         notify(TR("Not done: file already exists"));
      else if (errorcode == GRID_WORKER_NOT_APPLICABLE)
         notify(TR("Not done: modification not applicable"));
      else
         notify(TR("Modification failed"));
   }
   else if (job=="save")
   {
      if (errorcode == GRID_WORKER_FILE_EXISTS)
         notify(TR("Not saved: file already exists"));
      else
         notify(TR("Saving failed"));
   }
}
