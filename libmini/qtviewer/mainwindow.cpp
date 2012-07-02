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
   {clear(true);}

void MainWindow::initSettings()
{
   ministring home_path = getenv("HOME");

#ifdef __APPLE__
   home_path += "/Desktop";
#endif

   repoPath = home_path;
   exportPath = home_path;
   tmpPath = grid_resampler::get_tmp_dir();
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
   mainLayout = new QBoxLayout(QBoxLayout::RightToLeft);

   viewerWindow = new ViewerWindow;
   tabWidget = new QTabWidget;

   viewerGroup = new MyQGroupBox(QSize(300, 300));
   viewerLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   prefGroup = new QGroupBox;
   prefLayout = new QVBoxLayout;

   buttonBox = new QDialogButtonBox;

   viewerTable = new MyQTableWidget(viewerWindow);

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

   viewerLayout->addWidget(viewerTable);
   viewerLayout->addWidget(sliderBox);

   viewerGroup->setLayout(viewerLayout);
   viewerGroup->setFlat(true);

   // pref group:

   QGroupBox *lineEditGroup_repoPath = new QGroupBox(tr("Repository Path"));
   QVBoxLayout *lineEditLayout_repoPath = new QVBoxLayout;
   lineEditGroup_repoPath->setLayout(lineEditLayout_repoPath);
   lineEdit_repoPath = new QLineEdit(repoPath.c_str());
   lineEditLayout_repoPath->addWidget(lineEdit_repoPath);
   browseButton_repoPath = new QPushButton(tr("Browse"));
   lineEditLayout_repoPath->addWidget(browseButton_repoPath);
   lineEditLayout_repoPath->setAlignment(browseButton_repoPath, Qt::AlignLeft);
   viewerWindow->setRepo(repoPath);

   connect(lineEdit_repoPath,SIGNAL(textChanged(QString)),this,SLOT(repoPathChanged(QString)));
   connect(browseButton_repoPath, SIGNAL(pressed()), this, SLOT(browseRepoPath()));

   QGroupBox *lineEditGroup_exportPath = new QGroupBox(tr("Export Path"));
   QVBoxLayout *lineEditLayout_exportPath = new QVBoxLayout;
   lineEditGroup_exportPath->setLayout(lineEditLayout_exportPath);
   lineEdit_exportPath = new QLineEdit(exportPath.c_str());
   lineEditLayout_exportPath->addWidget(lineEdit_exportPath);
   browseButton_exportPath = new QPushButton(tr("Browse"));
   lineEditLayout_exportPath->addWidget(browseButton_exportPath);
   lineEditLayout_exportPath->setAlignment(browseButton_exportPath, Qt::AlignLeft);
   viewerWindow->setExport(exportPath);

   connect(lineEdit_exportPath,SIGNAL(textChanged(QString)),this,SLOT(exportPathChanged(QString)));
   connect(browseButton_exportPath, SIGNAL(pressed()), this, SLOT(browseExportPath()));

   QGroupBox *lineEditGroup_tmpPath = new QGroupBox(tr("Temporary Path"));
   QVBoxLayout *lineEditLayout_tmpPath = new QVBoxLayout;
   lineEditGroup_tmpPath->setLayout(lineEditLayout_tmpPath);
   lineEdit_tmpPath = new QLineEdit(tmpPath.c_str());
   lineEditLayout_tmpPath->addWidget(lineEdit_tmpPath);
   browseButton_tmpPath = new QPushButton(tr("Browse"));
   lineEditLayout_tmpPath->addWidget(browseButton_tmpPath);
   lineEditLayout_tmpPath->setAlignment(browseButton_tmpPath, Qt::AlignLeft);
   viewerWindow->setTmp(tmpPath);

   connect(lineEdit_tmpPath,SIGNAL(textChanged(QString)),this,SLOT(tmpPathChanged(QString)));
   connect(browseButton_tmpPath, SIGNAL(pressed()), this, SLOT(browseTmpPath()));

   verticalButton = new QCheckBox(tr("Vertical Layout"));
   verticalButton->setChecked(true);

   connect(verticalButton, SIGNAL(stateChanged(int)), this, SLOT(checkVertical(int)));

   sliderButton = new QCheckBox(tr("Show Controls"));
   sliderButton->setChecked(true);

   connect(sliderButton, SIGNAL(stateChanged(int)), this, SLOT(checkSliders(int)));

   prefLayout->addWidget(lineEditGroup_repoPath);
   prefLayout->addWidget(lineEditGroup_exportPath);
   prefLayout->addWidget(lineEditGroup_tmpPath);

   prefLayout->addWidget(verticalButton);
   prefLayout->addWidget(sliderButton);

   prefLayout->addStretch();

   prefGroup->setLayout(prefLayout);

   // tabs:

   tabWidget->addTab(viewerGroup, "View");
   tabWidget->addTab(prefGroup, "Prefs");

   // button group:

   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));

   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   // main group:

   mainLayout->addWidget(viewerWindow);
   mainLayout->addWidget(tabWidget);
   mainLayout->addWidget(buttonBox);
   buttonBox->hide();

   mainGroup->setLayout(mainLayout);
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
   ministring url=obj->get_name();
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

   if (row>=0 and row<m_Keys.size())
      key = m_Keys[row];

   viewerWindow->runAction(action, key);
}

void MainWindow::click(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

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
      lineEdit_repoPath->setText(dir.c_str());
}

void MainWindow::browseExportPath()
{
   ministring dir = viewerWindow->browseDir("Browse Export Path", exportPath);

   if (dir!="")
      lineEdit_exportPath->setText(dir.c_str());
}

void MainWindow::browseTmpPath()
{
   ministring dir = viewerWindow->browseDir("Browse Temporary Path", tmpPath);

   if (dir!="")
      lineEdit_tmpPath->setText(dir.c_str());
}

void MainWindow::checkVertical(int on)
{
   if (on)
   {
      mainLayout->setDirection(QBoxLayout::RightToLeft);
      viewerWindow->setVertical(TRUE);
      viewerLayout->setDirection(QBoxLayout::TopToBottom);
      sliderLayout->setDirection(QBoxLayout::TopToBottom);
      sliderLayout1->setDirection(QBoxLayout::TopToBottom);
      sliderLayout2->setDirection(QBoxLayout::TopToBottom);
      buttonBox->hide();
   }
   else
   {
      mainLayout->setDirection(QBoxLayout::TopToBottom);
      viewerWindow->setVertical(FALSE);
      viewerLayout->setDirection(QBoxLayout::LeftToRight);
      sliderLayout->setDirection(QBoxLayout::TopToBottom);
      sliderLayout1->setDirection(QBoxLayout::LeftToRight);
      sliderLayout2->setDirection(QBoxLayout::LeftToRight);
      buttonBox->show();
   }
}

void MainWindow::checkSliders(int on)
{
   sliderBox->setVisible(on);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
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
   else if (event->key() == Qt::Key_T)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("select", row);
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

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
   QWidget::keyReleaseEvent(event);
}

void MyQTableWidget::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_T)
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
    QAction *selectAction = new QAction(tr("select"), this);
    myMenu.addAction(selectAction);
    QAction *selectAllAction = new QAction(tr("select all"), this);
    myMenu.addAction(selectAllAction);
    QAction *deselectAllAction = new QAction(tr("deselect all"), this);
    myMenu.addAction(deselectAllAction);
    myMenu.addSeparator();
    QAction *infoAction = new QAction(tr("info"), this);
    myMenu.addAction(infoAction);
    QAction *shadeAction = new QAction(tr("shade"), this);
    myMenu.addAction(shadeAction);
    QAction *resampleAction = new QAction(tr("resample"), this);
    myMenu.addAction(resampleAction);
    QAction *resampleSelAction = new QAction(tr("resample selected"), this);
    myMenu.addAction(resampleSelAction);
    QAction *resampleAllAction = new QAction(tr("resample all"), this);
    myMenu.addAction(resampleAllAction);
    QAction *saveGridAction = new QAction(tr("save to grid file"), this);
    myMenu.addAction(saveGridAction);
    myMenu.addSeparator();
    QAction *deleteAction = new QAction(tr("delete"), this);
    myMenu.addAction(deleteAction);
    QAction *deleteSelAction = new QAction(tr("delete selected"), this);
    myMenu.addAction(deleteSelAction);
    QAction *deleteAllAction = new QAction(tr("delete all"), this);
    myMenu.addAction(deleteAllAction);

    // exec connect menu
    QAction *selectedAction = myMenu.exec(globalPos);

    // process selected action
    if (selectedAction)
       if (selectedAction == selectAction)
          emit(activate("select", row));
       else if (selectedAction == selectAllAction)
          emit(activate("select_all"));
       else if (selectedAction == deselectAllAction)
          emit(activate("deselect_all"));
       else if (selectedAction == infoAction)
          emit(activate("info", row));
       else if (selectedAction == shadeAction)
          emit(activate("shade", row));
       else if (selectedAction == resampleAction)
          emit(activate("resample", row));
       else if (selectedAction == resampleSelAction)
          emit(activate("resample_selected"));
       else if (selectedAction == resampleAllAction)
          emit(activate("resample_all"));
       else if (selectedAction == saveGridAction)
          emit(activate("save_grid"));
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
