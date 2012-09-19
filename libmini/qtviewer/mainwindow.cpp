// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui>

#include <grid/grid.h>

#include "viewerwindow.h"

#include "mainconst.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   initSettings();

   createActions();
   createMenus();
   createWidgets();

   setCentralWidget(mainGroup);
   setWindowTitle(tr(VIEWER_NAME));
}

MainWindow::~MainWindow()
   {
   saveSettings();

   clear(true);
   }

void MainWindow::initSettings()
{
   // define default settings:

   ministring home_path = Object::normalize_path(QDir::homePath().toStdString().c_str());
   ministring temp_path = Object::normalize_path(QDir::tempPath().toStdString().c_str());

#ifdef __APPLE__
   home_path += "Desktop";
#endif

   repoPath = home_path;
   exportPath = home_path;
   tmpPath = temp_path;

   grid_level = 0;
   grid_levels = 1;
   grid_step = 2;

   shadePower = 2.0;
   shadeAmbient = 0.1;
   jpegQuality = 90;

   // override with persistent settings:

   QSettings settings("www.open-terrain.org", "qtviewer");

   if (settings.contains("repoPath"))
      repoPath = settings.value("repoPath").toString().toStdString().c_str();

   if (settings.contains("exportPath"))
      exportPath = settings.value("exportPath").toString().toStdString().c_str();

   if (settings.contains("tmpPath"))
      tmpPath = settings.value("tmpPath").toString().toStdString().c_str();

   if (settings.contains("gridLevel"))
      grid_level = settings.value("gridLevel").toDouble();

   if (settings.contains("gridLevels"))
      grid_levels = settings.value("gridLevels").toDouble();

   if (settings.contains("gridStep"))
      grid_step = settings.value("gridStep").toDouble();

   if (settings.contains("shadePower"))
      shadePower = settings.value("shadePower").toDouble();

   if (settings.contains("shadeAmbient"))
      shadePower = settings.value("shadeAmbient").toDouble();

   if (settings.contains("jpegQuality"))
      jpegQuality = settings.value("jpegQuality").toDouble();
}

void MainWindow::saveSettings()
{
   QSettings settings("www.open-terrain.org", "qtviewer");

   settings.setValue("repoPath", QString(repoPath.c_str()));
   settings.setValue("exportPath", QString(exportPath.c_str()));
   settings.setValue("tmpPath", QString(tmpPath.c_str()));

   settings.setValue("gridLevel", grid_level);
   settings.setValue("gridLevels", grid_levels);
   settings.setValue("gridStep", grid_step);

   settings.setValue("shadePower", shadePower);
   settings.setValue("shadeAmbient", shadeAmbient);
   settings.setValue("jpegQuality", jpegQuality);
}

void MainWindow::createActions()
{
   aboutAction = new QAction(tr("&About"), this);
   aboutAction->setIcon(QIcon(":/images/about.png"));
   aboutAction->setShortcut(tr("Ctrl+A"));
   aboutAction->setStatusTip(tr("About this program"));
   connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

   openAction = new QAction(tr("O&pen"), this);
   openAction->setIcon(QIcon(":/images/open.png"));
   openAction->setShortcuts(QKeySequence::Open);
   openAction->setStatusTip(tr("Open location"));
   connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

   clearAction = new QAction(tr("C&lear"), this);
   clearAction->setIcon(QIcon(":/images/close.png"));
   clearAction->setShortcuts(QKeySequence::Close);
   clearAction->setStatusTip(tr("Clear locations"));
   connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

   quitAction = new QAction(tr("Q&uit"), this);
   quitAction->setShortcuts(QKeySequence::Quit);
   quitAction->setStatusTip(tr("Quit the application"));
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
   fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction(openAction);
   fileMenu->addAction(clearAction);
   fileMenu->addAction(quitAction);

   menuBar()->addSeparator();

   helpMenu = menuBar()->addMenu(tr("&Help"));
   helpMenu->addAction(aboutAction);
}

void MainWindow::createWidgets()
{
   mainGroup = new QGroupBox;
   mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
   mainSplitter = new QSplitter;

   viewerWindow = new ViewerWindow;
   tabWidget = new QTabWidget;

   viewerGroup = new MyQGroupBox(QSize(300, 300));
   viewerLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   prefGroup = new QGroupBox;
   prefLayout = new QVBoxLayout;

   buttonBox = new QDialogButtonBox;

   viewerTable = new MyQTableWidget(viewerWindow);

   tableBox = new QBoxLayout(QBoxLayout::LeftToRight);
   tableFilter = new QButtonGroup(viewerWindow);

   sliderBox = new MyQGroupBox(QSize(300, 300));
   sliderLayout = new QBoxLayout(QBoxLayout::TopToBottom);
   sliderLayout1 = new QBoxLayout(QBoxLayout::TopToBottom);
   sliderLayout2 = new QBoxLayout(QBoxLayout::TopToBottom);

   // drag and drop:

   connect(viewerWindow, SIGNAL(changed(ministring)),
           this, SLOT(updateTable(ministring)));

   // layer table:

   QStringList labels;
   labels << tr("Object") << tr("Name");

   viewerTable->setColumnCount(2);
   viewerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   viewerTable->setSelectionMode(QAbstractItemView::SingleSelection);
   viewerTable->setHorizontalHeaderLabels(labels);
   viewerTable->horizontalHeader()->setStretchLastSection(true);
   viewerTable->setAcceptDrops(true);

   connect(viewerTable, SIGNAL(cellClicked(int, int)), this, SLOT(click(int, int)));
   connect(viewerTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(doubleclick(int, int)));

   // context menu:

   viewerTable->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(viewerTable, SIGNAL(customContextMenuRequested(const QPoint&)),
           viewerTable, SLOT(showContextMenu(const QPoint&)));

   connect(viewerTable, SIGNAL(activate(ministring,int)),
           this, SLOT(runAction(ministring,int)));

   // layer table filter:

   QRadioButton *filter1 = new QRadioButton("DEM");
   QRadioButton *filter2 = new QRadioButton("IMG");
   QRadioButton *filter3 = new QRadioButton("ALL");
   filter3->setChecked(true);

   tableFilter->addButton(filter1);
   tableFilter->addButton(filter2);
   tableFilter->addButton(filter3);

   tableBox->addWidget(filter1);
   tableBox->addWidget(filter2);
   tableBox->addWidget(filter3);

   connect(filter1, SIGNAL(toggled(bool)), this, SLOT(filterDEMToggled(bool)));
   connect(filter2, SIGNAL(toggled(bool)), this, SLOT(filterIMGToggled(bool)));
   connect(filter3, SIGNAL(toggled(bool)), this, SLOT(filterALLToggled(bool)));

   // worker activity:

   workerActivity = new QLabel(this);
   workerActivity->setText("");

   // fog check:

   fogCheck = new QCheckBox(tr("Fog Density"));
   fogCheck->setChecked(true);

   connect(fogCheck, SIGNAL(stateChanged(int)), this, SLOT(checkFog(int)));

   fogDensitySlider = createSlider(0, 100, 0);

   connect(fogDensitySlider, SIGNAL(valueChanged(int)), this, SLOT(setFogDensity(int)));

   fogGroup = new QGroupBox;
   fogLayout = new QHBoxLayout;

   fogLayout->addWidget(fogCheck);
   fogLayout->addWidget(fogDensitySlider);
   fogGroup->setLayout(fogLayout);

   // contour check:

   contourCheck = new QCheckBox(tr("Contours"));
   contourCheck->setChecked(false);

   connect(contourCheck, SIGNAL(stateChanged(int)), this, SLOT(checkContours(int)));

   contourGroup = new QGroupBox;
   contourLayout = new QHBoxLayout;

   contourLayout->addWidget(contourCheck);
   contourGroup->setLayout(contourLayout);

   // sea level check:

   seaLevelCheck = new QCheckBox(tr("Ocean Sea Level"));
   seaLevelCheck->setChecked(false);

   connect(seaLevelCheck, SIGNAL(stateChanged(int)), this, SLOT(checkSeaLevel(int)));

   seaLevelSlider = createSlider(-100, 100, 0);

   connect(seaLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setSeaLevel(int)));

   seaGroup = new QGroupBox;
   seaLayout = new QHBoxLayout;

   seaLayout->addWidget(seaLevelCheck);
   seaLayout->addWidget(seaLevelSlider);
   seaGroup->setLayout(seaLayout);

   // light check:

   lightCheck = new QCheckBox(tr("UTC Night"));
   lightCheck->setChecked(false);

   connect(lightCheck, SIGNAL(stateChanged(int)), this, SLOT(checkLight(int)));

   lightSlider = createSlider(-12, 12, 0);

   connect(lightSlider, SIGNAL(valueChanged(int)), this, SLOT(setLight(int)));

   lightGroup = new QGroupBox;
   lightLayout = new QHBoxLayout;

   lightLayout->addWidget(lightCheck);
   lightLayout->addWidget(lightSlider);
   lightGroup->setLayout(lightLayout);

   // exaggeration check:

   exaggerCheck = new QCheckBox(tr("Exaggeration"));
   exaggerCheck->setChecked(false);

   connect(exaggerCheck, SIGNAL(stateChanged(int)), this, SLOT(checkExagger(int)));

   exaggerSlider = createSlider(0, 100, 100);

   connect(exaggerSlider, SIGNAL(valueChanged(int)), this, SLOT(setExagger(int)));

   exaggerGroup = new QGroupBox;
   exaggerLayout = new QHBoxLayout;

   exaggerLayout->addWidget(exaggerCheck);
   exaggerLayout->addWidget(exaggerSlider);
   exaggerGroup->setLayout(exaggerLayout);

   // stereo check:

   stereoCheck = new QCheckBox(tr("Stereo"));
   stereoCheck->setChecked(false);

   connect(stereoCheck, SIGNAL(stateChanged(int)), this, SLOT(toggleStereo(int)));

   stereoGroup = new QGroupBox;
   stereoLayout = new QHBoxLayout;

   stereoLayout->addWidget(stereoCheck);
   stereoGroup->setLayout(stereoLayout);

   // wire frame check:

   wireFrameCheck = new QCheckBox(tr("Wire Frame"));
   wireFrameCheck->setChecked(false);

   connect(wireFrameCheck, SIGNAL(stateChanged(int)), this, SLOT(toggleWireFrame(int)));

   QGroupBox *wireGroup = new QGroupBox;
   QHBoxLayout *wireLayout = new QHBoxLayout;

   wireLayout->addWidget(wireFrameCheck);
   wireGroup->setLayout(wireLayout);

   // slider group:

   sliderLayout1->addWidget(fogGroup);
   sliderLayout1->addWidget(contourGroup);
   sliderLayout1->addWidget(seaGroup);
   sliderLayout2->addWidget(lightGroup);
   sliderLayout2->addWidget(exaggerGroup);
   sliderLayout2->addWidget(stereoGroup);
   sliderLayout2->addWidget(wireGroup);
   sliderLayout->addLayout(sliderLayout1);
   sliderLayout->addLayout(sliderLayout2);
   sliderBox->setLayout(sliderLayout);

   // viewer group:

   viewerLayout->addLayout(tableBox);
   viewerLayout->addWidget(viewerTable);
   viewerLayout->addWidget(workerActivity);
   viewerLayout->addWidget(sliderBox);

   viewerGroup->setLayout(viewerLayout);
   viewerGroup->setFlat(true);

   // pref group:

   QGroupBox *lineEditGroup_repoPath = createPathEdit("Repository Path", repoPath,
                                                      &lineEdit_repoPath, &browseButton_repoPath);

   connect(lineEdit_repoPath,SIGNAL(textChanged(QString)),this,SLOT(repoPathChanged(QString)));
   connect(browseButton_repoPath, SIGNAL(pressed()), this, SLOT(browseRepoPath()));

   QGroupBox *lineEditGroup_exportPath = createPathEdit("Export Path", exportPath,
                                                        &lineEdit_exportPath, &browseButton_exportPath);

   connect(lineEdit_exportPath,SIGNAL(textChanged(QString)),this,SLOT(exportPathChanged(QString)));
   connect(browseButton_exportPath, SIGNAL(pressed()), this, SLOT(browseExportPath()));

   QGroupBox *lineEditGroup_tmpPath = createPathEdit("Temporary Path", tmpPath,
                                                     &lineEdit_tmpPath, &browseButton_tmpPath);

   connect(lineEdit_tmpPath,SIGNAL(textChanged(QString)),this,SLOT(tmpPathChanged(QString)));
   connect(browseButton_tmpPath, SIGNAL(pressed()), this, SLOT(browseTmpPath()));

   QGroupBox *lineEditGroup_gridLevel = createEdit("Grid Level", grid_level, &lineEdit_gridLevel);
   connect(lineEdit_gridLevel,SIGNAL(textChanged(QString)),this,SLOT(gridLevelChanged(QString)));

   QGroupBox *lineEditGroup_gridLevels = createEdit("Grid Levels", grid_levels, &lineEdit_gridLevels);
   connect(lineEdit_gridLevels,SIGNAL(textChanged(QString)),this,SLOT(gridLevelsChanged(QString)));

   QGroupBox *lineEditGroup_gridStep = createEdit("Grid Step", grid_step, &lineEdit_gridStep);
   connect(lineEdit_gridStep,SIGNAL(textChanged(QString)),this,SLOT(gridStepChanged(QString)));

   QGroupBox *lineEditGroup_shadePower = createEdit("Shading Power", shadePower, &lineEdit_shadePower);
   connect(lineEdit_shadePower,SIGNAL(textChanged(QString)),this,SLOT(shadePowerChanged(QString)));

   QGroupBox *lineEditGroup_shadeAmbient = createEdit("Shading Ambience", shadeAmbient, &lineEdit_shadeAmbient);
   connect(lineEdit_shadeAmbient,SIGNAL(textChanged(QString)),this,SLOT(shadeAmbientChanged(QString)));

   QGroupBox *lineEditGroup_jpegQuality = createEdit("JPEG Quality", jpegQuality, &lineEdit_jpegQuality);
   connect(lineEdit_jpegQuality,SIGNAL(textChanged(QString)),this,SLOT(jpegQualityChanged(QString)));

   sliderButton = new QCheckBox(tr("Show Controls"));
   sliderButton->setChecked(true);

   connect(sliderButton, SIGNAL(stateChanged(int)), this, SLOT(checkSliders(int)));

   prefLayout->addWidget(lineEditGroup_repoPath);
   prefLayout->addWidget(lineEditGroup_exportPath);
   prefLayout->addWidget(lineEditGroup_tmpPath);

   prefLayout->addWidget(lineEditGroup_gridLevel);
   prefLayout->addWidget(lineEditGroup_gridLevels);
   prefLayout->addWidget(lineEditGroup_gridStep);

   prefLayout->addWidget(lineEditGroup_shadePower);
   prefLayout->addWidget(lineEditGroup_shadeAmbient);
   prefLayout->addWidget(lineEditGroup_jpegQuality);

   prefLayout->addWidget(sliderButton);

   prefLayout->addStretch();

   prefGroup->setLayout(prefLayout);

   // tabs:

   QScrollArea *viewerGroupScrollArea = new QScrollArea;
   viewerGroupScrollArea->setWidgetResizable(true);
   viewerGroupScrollArea->setWidget(viewerGroup);

   QScrollArea *prefGroupScrollArea = new QScrollArea;
   prefGroupScrollArea->setWidgetResizable(true);
   prefGroupScrollArea->setWidget(prefGroup);

   tabWidget->addTab(viewerGroupScrollArea, "View");
   tabWidget->addTab(prefGroupScrollArea, "Prefs");

   // button group:

   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));

   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   // main group:

   mainLayout->addWidget(mainSplitter);
   mainLayout->addWidget(buttonBox);

   mainSplitter->addWidget(tabWidget);
   mainSplitter->addWidget(viewerWindow);

   mainGroup->setLayout(mainLayout);

   // worker directories:

   repoPathChanged(repoPath.c_str());
   exportPathChanged(exportPath.c_str());
   tmpPathChanged(tmpPath.c_str());

   // worker settings:

   viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
   viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);

   // progress:

   connect(viewerWindow, SIGNAL(progress(double, ministring)),
           this, SLOT(reportProgress(double, ministring)));
}

QSlider *MainWindow::createSlider(int minimum, int maximum, int value)
{
   QSlider *slider = new QSlider(Qt::Horizontal);
   slider->setRange(minimum * 16, maximum * 16);
   slider->setSingleStep(16);
   slider->setPageStep((maximum - minimum) / 10 * 16);
   slider->setTickInterval((maximum - minimum) / 10 * 16);
   slider->setTickPosition(QSlider::TicksBelow);
   slider->setValue(value * 16);
   return(slider);
}

QGroupBox *MainWindow::createEdit(ministring name, ministring value,
                                  QLineEdit **lineEdit)
{
   QGroupBox *lineEditGroup = new QGroupBox(tr(name.c_str()));
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value.c_str());
   lineEditLayout->addWidget(*lineEdit);
   return(lineEditGroup);
}

QGroupBox *MainWindow::createPathEdit(ministring name, ministring value,
                                      QLineEdit **lineEdit, QPushButton **browseButton)
{
   QGroupBox *lineEditGroup = new QGroupBox(tr(name.c_str()));
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value.c_str());
   lineEditLayout->addWidget(*lineEdit);
   *browseButton = new QPushButton(tr("Browse"));
   lineEditLayout->addWidget(*browseButton);
   lineEditLayout->setAlignment(*browseButton, Qt::AlignLeft);
   return(lineEditGroup);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About this program"),
                      tr(VIEWER_NAME" "VIEWER_VERSION));
}

void MainWindow::open()
{
   runAction("open");
}

void MainWindow::clear(bool all)
{
   int row = viewerTable->currentRow();

   if (row==-1 || all)
      runAction("delete_all");
   else
      runAction("delete", row);
}

void MainWindow::getNameInfo(Object *obj,
                             QString &name, QString &info)
{
   ministring url=obj->get_full_name();
   name=url.c_str();

   if (name.endsWith("/")) name.truncate(name.size()-1);
   if (name.endsWith("\\")) name.truncate(name.size()-1);

   if (name.lastIndexOf("/")>=0) name.remove(0,name.lastIndexOf("/")+1);
   if (name.lastIndexOf("\\")>=0) name.remove(0,name.lastIndexOf("\\")+1);

   ministrings *tags=viewerWindow->getTags(url);
   ministring tag=tags?tags->get(0):"object";
   BOOLINT elevation=tags?tags->has("elevation"):FALSE;
   info=elevation?"elevation":tag.c_str();
}

void MainWindow::updateTable(ministring key)
{
   unsigned int row;
   int rows = viewerTable->rowCount();

   Object *obj=viewerWindow->getObject(key);

   BOOLINT present=FALSE;

   for (row=0; row<m_Keys.getsize(); row++)
      if (m_Keys[row]==key)
      {
         present=TRUE;
         break;
      }

   // add object
   if (!present && obj!=NULL)
   {
      QString name, info;

      getNameInfo(obj, name, info);

      viewerTable->insertRow(rows);
      viewerTable->setItem(rows, 0, new QTableWidgetItem(info));
      viewerTable->setItem(rows, 1, new QTableWidgetItem(name));

      m_Keys.growsize(rows+1);
      m_Keys[rows]=key;
   }
   // update object
   else if (present && obj!=NULL)
   {
      QString name, info;

      getNameInfo(obj, name, info);

      viewerTable->setItem(row, 0, new QTableWidgetItem(info));
      viewerTable->setItem(row, 1, new QTableWidgetItem(name));

      if (viewerWindow->hasTag(m_Keys[row], "selected"))
         viewerTable->item(row, 0)->setBackground(QBrush(QColor("blue")));
      else
         viewerTable->item(row, 0)->setBackground(QBrush(QColor("white")));

      if (viewerWindow->hasTag(m_Keys[row], "hidden"))
      {
         viewerTable->item(row, 0)->setForeground(QBrush(QColor("grey")));
         viewerTable->item(row, 1)->setForeground(QBrush(QColor("grey")));
      }
      else
      {
         viewerTable->item(row, 0)->setForeground(QBrush(QColor("black")));
         viewerTable->item(row, 1)->setForeground(QBrush(QColor("black")));
      }
   }
   // remove object
   else if (present && obj==NULL)
   {
      viewerTable->removeRow(row);
      m_Keys.dispose(row);
   }
}

void MainWindow::runAction(ministring action, int row)
{
   ministring key;

   if (row>=0 && (unsigned int)row<m_Keys.size())
      key = m_Keys[row];

   viewerWindow->runAction(action, key);
}

void MainWindow::reportProgress(double percentage, const ministring &job)
{
   ministring progress;

   if (percentage<100)
   {
      if (job == "thumb")
         progress = "creating thumbnail";
      else
         if (percentage==0.0)
            progress = job+" progress: working";
         else
            progress = job+" progress: "+(int)percentage+"%";
   }

   workerActivity->setText(progress.c_str());
}

void MainWindow::click(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   if (keyMod & Qt::ShiftModifier)
      runAction("select", row);
   else if (keyMod & Qt::AltModifier)
      runAction("toggle", row);
   else
      viewerTable->setCurrentCell(row, 1);
}

void MainWindow::doubleclick(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   runAction("goto", row);
}

void MainWindow::toggleStereo(int on)
{
   viewerWindow->toggleStereo(on);
}

void MainWindow::toggleWireFrame(int on)
{
   viewerWindow->toggleWireFrame(on);
}

void MainWindow::checkFog(int on)
{
   viewerWindow->checkFog(on);
}

void MainWindow::setFogDensity(int tick)
{
   double density = tick / 16.0 / 100.0;
   viewerWindow->setFogDensity(density);
}

void MainWindow::checkContours(int on)
{
   viewerWindow->checkContours(on);
}

void MainWindow::checkSeaLevel(int on)
{
   viewerWindow->checkSeaLevel(on);
}

void MainWindow::setSeaLevel(int tick)
{
   double level = tick / 16.0;
   viewerWindow->setSeaLevel(level);
}

void MainWindow::checkLight(int on)
{
   viewerWindow->checkLight(on);
}

void MainWindow::setLight(int tick)
{
   double hour = tick / 16.0;
   viewerWindow->setLight(hour);
}

void MainWindow::checkExagger(int on)
{
   viewerWindow->checkExagger(on);
}

void MainWindow::setExagger(int tick)
{
   double scale = tick / 16.0 / 100.0;
   viewerWindow->setExagger(scale);
}

void MainWindow::filterDEMToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_elevation");
      viewerWindow->runAction("hide_imagery");
   }
}

void MainWindow::filterIMGToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_imagery");
      viewerWindow->runAction("hide_elevation");
   }
}

void MainWindow::filterALLToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_elevation");
      viewerWindow->runAction("show_imagery");
   }
}

void MainWindow::repoPathChanged(QString repo)
{
   repoPath = repo.toStdString().c_str();
   viewerWindow->runAction("repo", repoPath);
}

void MainWindow::exportPathChanged(QString expo)
{
   exportPath = expo.toStdString().c_str();
   viewerWindow->runAction("export", exportPath);
}

void MainWindow::tmpPathChanged(QString tmp)
{
   tmpPath = tmp.toStdString().c_str();
   viewerWindow->runAction("tmp", tmpPath);
}

void MainWindow::browseRepoPath()
{
   ministring dir = viewerWindow->browseDir("Browse Repository Path", repoPath);

   if (dir!="")
   {
      lineEdit_repoPath->setText(dir.c_str());
      repoPathChanged(dir.c_str());
   }
}

void MainWindow::browseExportPath()
{
   ministring dir = viewerWindow->browseDir("Browse Export Path", exportPath);

   if (dir!="")
   {
      lineEdit_exportPath->setText(dir.c_str());
      exportPathChanged(dir.c_str());
   }
}

void MainWindow::browseTmpPath()
{
   ministring dir = viewerWindow->browseDir("Browse Temporary Path", tmpPath);

   if (dir!="")
   {
      lineEdit_tmpPath->setText(dir.c_str());
      tmpPathChanged(dir.c_str());
   }
}

void MainWindow::gridLevelChanged(QString level)
{
   bool valid;
   double grid_level = level.toDouble(&valid);

   if (valid)
      {
      this->grid_level = grid_level;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::gridLevelsChanged(QString levels)
{
   bool valid;
   double grid_levels = levels.toDouble(&valid);

   if (valid)
      {
      this->grid_levels = grid_levels;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::gridStepChanged(QString step)
{
   bool valid;
   double grid_step = step.toDouble(&valid);

   if (valid)
      {
      this->grid_step = grid_step;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::shadePowerChanged(QString power)
{
   bool valid;
   double shadePower = power.toDouble(&valid);

   if (valid)
      {
      this->shadePower = shadePower;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::shadeAmbientChanged(QString ambient)
{
   bool valid;
   double shadeAmbient = ambient.toDouble(&valid);

   if (valid)
      {
      this->shadeAmbient = shadeAmbient;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::jpegQualityChanged(QString quality)
{
   bool valid;
   double jpegQuality = quality.toDouble(&valid);

   if (valid)
      {
      this->jpegQuality = jpegQuality;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::checkSliders(int on)
{
   sliderBox->setVisible(on);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_F)
      fogCheck->setChecked(!fogCheck->isChecked());
   else if (event->key() == Qt::Key_C)
      contourCheck->setChecked(!contourCheck->isChecked());
   else if (event->key() == Qt::Key_O)
      seaLevelCheck->setChecked(!seaLevelCheck->isChecked());
   else if (event->key() == Qt::Key_N)
      lightCheck->setChecked(!lightCheck->isChecked());
   else if (event->key() == Qt::Key_E)
      exaggerCheck->setChecked(!exaggerCheck->isChecked());
   else if (event->key() == Qt::Key_S)
      stereoCheck->setChecked(!stereoCheck->isChecked());
   else if (event->key() == Qt::Key_W)
      wireFrameCheck->setChecked(!wireFrameCheck->isChecked());
   else if (event->key() == Qt::Key_Return)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("select", row);
   }
   else if (event->key() == Qt::Key_T)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("toggle", row);
   }
   else if (event->key() == Qt::Key_Backspace)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("delete", row);
   }
   else
      QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
   QWidget::keyReleaseEvent(event);
}

void MyQTableWidget::keyPressEvent(QKeyEvent *event)
{
   char c = event->text().toStdString()[0];

   if (isalpha(c))
      QWidget::keyPressEvent(event); // pass to parent
   else
      QTableWidget::keyPressEvent(event); // pass to widget
}

void MyQTableWidget::showContextMenu(const QPoint &pos)
{
    // map widget to global coordinates
    QPoint globalPos = mapToGlobal(pos);

    // get item
    int row = -1;
    QTableWidgetItem *item = itemAt(pos);

    // highlight item
    if (item)
    {
       row = item->row();
       setCurrentCell(row, 1);
    }

    // create contex menu
    QMenu myMenu;
    QAction *openAction = new QAction(tr("open"), this);
    myMenu.addAction(openAction);
    myMenu.addSeparator();
    QAction *selectAction = new QAction(tr("select layer"), this);
    myMenu.addAction(selectAction);
    QAction *selectAllAction = new QAction(tr("select all"), this);
    myMenu.addAction(selectAllAction);
    QAction *deselectAllAction = new QAction(tr("deselect all"), this);
    myMenu.addAction(deselectAllAction);
    myMenu.addSeparator();
    QAction *infoAction = new QAction(tr("info"), this);
    myMenu.addAction(infoAction);
    QAction *showAction = new QAction(tr("show layer"), this);
    myMenu.addAction(showAction);
    QAction *hideAction = new QAction(tr("hide layer"), this);
    myMenu.addAction(hideAction);
    myMenu.addSeparator();
    QAction *shadeAction = new QAction(tr("shade layer"), this);
    myMenu.addAction(shadeAction);
    QAction *resampleAction = new QAction(tr("resample area to tileset"), this);
    myMenu.addAction(resampleAction);
    QAction *resampleSelAction = new QAction(tr("resample selection to tileset"), this);
    myMenu.addAction(resampleSelAction);
    QAction *resampleAllAction = new QAction(tr("resample all"), this);
    myMenu.addAction(resampleAllAction);
    QAction *cropElevAction = new QAction(tr("crop elevation to layer"), this);
    myMenu.addAction(cropElevAction);
    QAction *cropImagAction = new QAction(tr("crop imagery to layer"), this);
    myMenu.addAction(cropImagAction);
    QAction *cropSelAction = new QAction(tr("crop selection to layer"), this);
    myMenu.addAction(cropSelAction);
    QAction *abortAction = new QAction(tr("abort"), this);
    myMenu.addAction(abortAction);
    myMenu.addSeparator();
    QAction *saveDBAction = new QAction(tr("save layer to DB file"), this);
    myMenu.addAction(saveDBAction);
    QAction *saveGeoTiffAction = new QAction(tr("save layer to GeoTiff file"), this);
    myMenu.addAction(saveGeoTiffAction);
    QAction *saveJpgInTifAction = new QAction(tr("save layer to JpgInTif file"), this);
    myMenu.addAction(saveJpgInTifAction);
    myMenu.addSeparator();
    QAction *loadAction = new QAction(tr("load layer list"), this);
    myMenu.addAction(loadAction);
    QAction *saveAction = new QAction(tr("save layer list"), this);
    myMenu.addAction(saveAction);
    QAction *saveGridAction = new QAction(tr("save to grid file"), this);
    myMenu.addAction(saveGridAction);
    myMenu.addSeparator();
    QAction *deleteAction = new QAction(tr("delete layer"), this);
    myMenu.addAction(deleteAction);
    QAction *deleteSelAction = new QAction(tr("delete selection"), this);
    myMenu.addAction(deleteSelAction);
    QAction *deleteAllAction = new QAction(tr("delete all"), this);
    myMenu.addAction(deleteAllAction);

    // exec connect menu
    QAction *selectedAction = myMenu.exec(globalPos);

    // process selected action
    if (selectedAction)
       if (selectedAction == openAction)
          emit(activate("open"));
       else if (selectedAction == selectAction)
          emit(activate("select", row));
       else if (selectedAction == selectAllAction)
          emit(activate("select_all"));
       else if (selectedAction == deselectAllAction)
          emit(activate("deselect_all"));
       else if (selectedAction == infoAction)
          emit(activate("info", row));
       else if (selectedAction == showAction)
          emit(activate("show", row));
       else if (selectedAction == hideAction)
          emit(activate("hide", row));
       else if (selectedAction == shadeAction)
          emit(activate("shade", row));
       else if (selectedAction == resampleAction)
          emit(activate("resample", row));
       else if (selectedAction == resampleSelAction)
          emit(activate("resample_selected"));
       else if (selectedAction == resampleAllAction)
          emit(activate("resample_all"));
       else if (selectedAction == cropElevAction)
          emit(activate("crop_elevation", row));
       else if (selectedAction == cropImagAction)
          emit(activate("crop_imagery", row));
       else if (selectedAction == cropSelAction)
          emit(activate("crop_selected", row));
       else if (selectedAction == abortAction)
          emit(activate("abort"));
       else if (selectedAction == saveDBAction)
          emit(activate("save_db", row));
       else if (selectedAction == saveGeoTiffAction)
          emit(activate("save_tif", row));
       else if (selectedAction == saveJpgInTifAction)
          emit(activate("save_jpgintif", row));
       else if (selectedAction == saveAction)
          emit(activate("save"));
       else if (selectedAction == saveGridAction)
          emit(activate("save_grid"));
       else if (selectedAction == loadAction)
          emit(activate("load"));
       else if (selectedAction == deleteAction)
          emit(activate("delete", row));
       else if (selectedAction == deleteSelAction)
          emit(activate("delete_selected"));
       else if (selectedAction == deleteAllAction)
          emit(activate("delete_all"));
}

void MyQTableWidget::dragEnterEvent(QDragEnterEvent *event) {event->acceptProposedAction();}
void MyQTableWidget::dragMoveEvent(QDragMoveEvent *event) {event->acceptProposedAction();}
void MyQTableWidget::dropEvent(QDropEvent *event) {viewerWindow->dropEvent(event);}
void MyQTableWidget::dragLeaveEvent(QDragLeaveEvent *event) {event->accept();}
