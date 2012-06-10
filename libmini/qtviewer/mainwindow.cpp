// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui>

#include "viewerwindow.h"

#include "mainconst.h"
#include "mainwindow.h"

class MyQTableWidget: public QTableWidget
{
public:

   MyQTableWidget(QWidget *parent = 0)
      : QTableWidget(parent)
   {}

protected:

   void keyPressEvent(QKeyEvent *event)
   {
      if (event->key() == Qt::Key_T)
         QWidget::keyPressEvent(event);
      else
         QTableWidget::keyPressEvent(event);
   }
};

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   createActions();
   createMenus();
   createWidgets();

   setCentralWidget(mainGroup);
   setWindowTitle(tr(VIEWER_NAME));
}

MainWindow::~MainWindow() {}

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
   mainLayout = new QVBoxLayout;

   viewerWindow = new ViewerWindow;
   viewerLayout = new QHBoxLayout;
   viewerTable = new MyQTableWidget;
   sliderBox = new QGroupBox;
   sliderLayout = new QVBoxLayout;
   sliderLayout1 = new QHBoxLayout;
   sliderLayout2 = new QHBoxLayout;
   buttonBox = new QDialogButtonBox;

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

   // fog check:

   fogCheck = new QCheckBox(tr("Fog Density"));
   fogCheck->setChecked(true);

   connect(fogCheck, SIGNAL(stateChanged(int)), this, SLOT(checkFog(int)));

   fogDensitySlider = createSlider(0, 100, 0);

   connect(fogDensitySlider, SIGNAL(valueChanged(int)), this, SLOT(setFogDensity(int)));

   QGroupBox *fogGroup = new QGroupBox;
   QHBoxLayout *fogLayout = new QHBoxLayout;

   fogLayout->addWidget(fogCheck);
   fogLayout->addWidget(fogDensitySlider);
   fogGroup->setLayout(fogLayout);

   // contour check:

   contourCheck = new QCheckBox(tr("Contours"));
   contourCheck->setChecked(false);

   connect(contourCheck, SIGNAL(stateChanged(int)), this, SLOT(checkContours(int)));

   QGroupBox *contourGroup = new QGroupBox;
   QHBoxLayout *contourLayout = new QHBoxLayout;

   contourLayout->addWidget(contourCheck);
   contourGroup->setLayout(contourLayout);

   // sea level check:

   seaLevelCheck = new QCheckBox(tr("Ocean Sea Level"));
   seaLevelCheck->setChecked(false);

   connect(seaLevelCheck, SIGNAL(stateChanged(int)), this, SLOT(checkSeaLevel(int)));

   seaLevelSlider = createSlider(-100, 100, 0);

   connect(seaLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setSeaLevel(int)));

   QGroupBox *seaGroup = new QGroupBox;
   QHBoxLayout *seaLayout = new QHBoxLayout;

   seaLayout->addWidget(seaLevelCheck);
   seaLayout->addWidget(seaLevelSlider);
   seaGroup->setLayout(seaLayout);

   // light check:

   lightCheck = new QCheckBox(tr("UTC Night"));
   lightCheck->setChecked(false);

   connect(lightCheck, SIGNAL(stateChanged(int)), this, SLOT(checkLight(int)));

   lightSlider = createSlider(-12, 12, 0);

   connect(lightSlider, SIGNAL(valueChanged(int)), this, SLOT(setLight(int)));

   QGroupBox *lightGroup = new QGroupBox;
   QHBoxLayout *lightLayout = new QHBoxLayout;

   lightLayout->addWidget(lightCheck);
   lightLayout->addWidget(lightSlider);
   lightGroup->setLayout(lightLayout);

   // exaggeration check:

   exaggerCheck = new QCheckBox(tr("Exaggeration"));
   exaggerCheck->setChecked(false);

   connect(exaggerCheck, SIGNAL(stateChanged(int)), this, SLOT(checkExagger(int)));

   exaggerSlider = createSlider(0, 100, 100);

   connect(exaggerSlider, SIGNAL(valueChanged(int)), this, SLOT(setExagger(int)));

   QGroupBox *exaggerGroup = new QGroupBox;
   QHBoxLayout *exaggerLayout = new QHBoxLayout;

   exaggerLayout->addWidget(exaggerCheck);
   exaggerLayout->addWidget(exaggerSlider);
   exaggerGroup->setLayout(exaggerLayout);

   // stereo check:

   stereoCheck = new QCheckBox(tr("Stereo"));
   stereoCheck->setChecked(false);

   connect(stereoCheck, SIGNAL(stateChanged(int)), this, SLOT(toggleStereo(int)));

   QGroupBox *stereoGroup = new QGroupBox;
   QHBoxLayout *stereoLayout = new QHBoxLayout;

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

   // button group:

   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));

   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   // viewer group:

   viewerLayout->addWidget(viewerTable);
   viewerLayout->addWidget(sliderBox);

   // main group:

   mainLayout->addWidget(viewerWindow);
   mainLayout->addLayout(viewerLayout);
   mainLayout->addWidget(buttonBox);

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
   QFileDialog* fd = new QFileDialog(this, "Open Location");
   fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);
   fd->setFilter("Ini Files (*.ini);;Images (*.tif *.jpg *.png)");

   QString fileName;
   if (fd->exec() == QDialog::Accepted)
      fileName = fd->selectedFiles().at(0);

   if (!fileName.isNull())
      viewerWindow->loadURL(fileName.toStdString().c_str());
}

void MainWindow::clear()
{
   int row = viewerTable->currentRow();

   if (row==-1)
      viewerWindow->clearObjects();
   else
      viewerWindow->removeObject(m_Keys[row]);
}

void MainWindow::getNameInfo(Object *obj,
                             QString &name,QString &info)
{
   ministring url=obj->filename;
   name=url.c_str();

   if (name.endsWith("/")) name.truncate(name.size()-1);
   if (name.endsWith("\\")) name.truncate(name.size()-1);

   if (name.lastIndexOf("/")>=0) name.remove(0,name.lastIndexOf("/")+1);
   if (name.lastIndexOf("\\")>=0) name.remove(0,name.lastIndexOf("\\")+1);

   ministrings *tags=viewerWindow->getTags(url);
   ministring type=tags?tags->get(0):"object";
   BOOLINT elevation=tags?tags->has("elevation"):FALSE;

   if (elevation) info="elevation";
   else info=type.c_str();
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
         if (!viewerWindow->hasTag(m_Keys[row], "selected"))
            viewerWindow->addTag(m_Keys[row], "selected");
         else
            viewerWindow->removeTag(m_Keys[row], "selected");
   }
   else
      QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
   QWidget::keyReleaseEvent(event);
}
