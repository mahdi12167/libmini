// (c) by Stefan Roettger, licensed under GPL 2+

#include <grid/grid.h>

#include <QtGui>

#include "viewerwindow.h"

#include "mainconst.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   createActions();
   createMenus();
   createWidgets();

   setCentralWidget(mainGroup);
   setWindowTitle(tr(VIEWER_NAME));
}

MainWindow::~MainWindow()
   {clear(true);}

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

   viewerTable = new MyQTableWidget;

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
   lineEdit_repoPath = new QLineEdit;
   lineEditLayout_repoPath->addWidget(lineEdit_repoPath);
   browseButton_repoPath = new QPushButton(tr("Browse"));
   lineEditLayout_repoPath->addWidget(browseButton_repoPath);
   lineEditLayout_repoPath->setAlignment(browseButton_repoPath, Qt::AlignLeft);

   connect(lineEdit_repoPath,SIGNAL(textChanged(QString)),this,SLOT(repoPathChanged(QString)));
   connect(browseButton_repoPath, SIGNAL(pressed()), this, SLOT(browseRepoPath()));

   QGroupBox *lineEditGroup_tmpPath = new QGroupBox(tr("Temporary Path"));
   QVBoxLayout *lineEditLayout_tmpPath = new QVBoxLayout;
   lineEditGroup_tmpPath->setLayout(lineEditLayout_tmpPath);
   lineEdit_tmpPath = new QLineEdit;
   lineEditLayout_tmpPath->addWidget(lineEdit_tmpPath);
   browseButton_tmpPath = new QPushButton(tr("Browse"));
   lineEditLayout_tmpPath->addWidget(browseButton_tmpPath);
   lineEditLayout_tmpPath->setAlignment(browseButton_tmpPath, Qt::AlignLeft);

   connect(lineEdit_tmpPath,SIGNAL(textChanged(QString)),this,SLOT(tmpPathChanged(QString)));
   connect(browseButton_tmpPath, SIGNAL(pressed()), this, SLOT(browseTmpPath()));

   verticalButton = new QCheckBox(tr("Vertical Layout"));
   verticalButton->setChecked(true);

   connect(verticalButton, SIGNAL(stateChanged(int)), this, SLOT(checkVertical(int)));

   sliderButton = new QCheckBox(tr("Show Controls"));
   sliderButton->setChecked(true);

   connect(sliderButton, SIGNAL(stateChanged(int)), this, SLOT(checkSliders(int)));

   prefLayout->addWidget(lineEditGroup_repoPath);
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
   QFileDialog* fd = new QFileDialog(this, "Open File");
   fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);
   fd->setFilter("Ini Files (*.ini);;Images (*.tif *.tiff *.jpg *.png)");

   if (fd->exec() == QDialog::Accepted)
      for (int i=0; i<fd->selectedFiles().size(); i++)
      {
         QString fileName = fd->selectedFiles().at(i);

         if (!fileName.isNull())
            viewerWindow->loadURL(fileName.toStdString().c_str());
      }
}

void MainWindow::clear(bool all)
{
   int row = viewerTable->currentRow();

   if (row==-1 || all)
      viewerWindow->clearObjects();
   else
      viewerWindow->removeObject(m_Keys[row]);
}

ministrings MainWindow::browse(ministring title)
{
   QFileDialog* fd = new QFileDialog(this, title.c_str());
   fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);

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

void MainWindow::getNameInfo(Object *obj,
                             QString &name, QString &info)
{
   ministring url=obj->filename;
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

   viewerWindow->gotoObject(m_Keys[row]);
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
   viewerWindow->setRepo(repoPath);
}

void MainWindow::tmpPathChanged(QString tmp)
{
   tmpPath = tmp.toStdString().c_str();
   grid_resampler::set_tmp_dir(tmpPath);
}

void MainWindow::browseRepoPath()
{
   ministrings files = browse("Browse Repository Path");

   if (files.size()>0)
      lineEdit_repoPath->setText(files[0].c_str());
}

void MainWindow::browseTmpPath()
{
   ministrings files = browse("Browse Temporary Path");

   if (files.size()>0)
      lineEdit_tmpPath->setText(files[0].c_str());
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

    // create contex menu
    QMenu myMenu;
    QAction *selectAction = new QAction(tr("select"), this);
    myMenu.addAction(selectAction);
    QAction *selectAllAction = new QAction(tr("select all"), this);
    myMenu.addAction(selectAllAction);
    QAction *deselectAllAction = new QAction(tr("deselect all"), this);
    myMenu.addAction(deselectAllAction);

    // exec connect menu
    QAction *selectedAction = myMenu.exec(globalPos);

    // process selected action
    if (selectedAction)
       if (selectedAction == selectAction)
       {
          QTableWidgetItem *item = itemAt(pos);

          if (item)
             emit(activate("select", item->row()));
       }
       else if (selectedAction == selectAllAction)
       {
          emit(activate("select_all", -1));
       }
       else if (selectedAction == deselectAllAction)
       {
          emit(activate("deselect_all", -1));
       }
}
