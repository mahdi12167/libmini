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

   quitAction = new QAction(tr("Q&uit"), this);
   quitAction->setShortcuts(QKeySequence::Quit);
   quitAction->setStatusTip(tr("Quit the application"));
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
   fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction(openAction);
   fileMenu->addAction(quitAction);

   menuBar()->addSeparator();

   helpMenu = menuBar()->addMenu(tr("&Help"));
   helpMenu->addAction(aboutAction);
}

void MainWindow::createWidgets()
{
   mainGroup = new QGroupBox;
   mainLayout = new QVBoxLayout;

   viewerWindow = new ViewerWindow();
   viewerTable = new QTableWidget;
   sliderBox = new QGroupBox;
   sliderLayout = new QHBoxLayout;
   buttonBox = new QDialogButtonBox;

   connect(viewerWindow, SIGNAL(changed(const QString)),
           this, SLOT(updateTable(const QString)));

   QStringList labels;
   labels << tr("URL");

   viewerTable->setColumnCount(1);
   viewerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   viewerTable->setHorizontalHeaderLabels(labels);
   viewerTable->horizontalHeader()->setStretchLastSection(true);

   connect(viewerTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(click(int, int)));

   fogCheck = new QCheckBox(tr("Fog Density"));
   fogCheck->setChecked(false);

   connect(fogCheck, SIGNAL(stateChanged(int)), this, SLOT(checkFog(int)));

   fogDensitySlider = createSlider(0, 100, 0);

   connect(fogDensitySlider, SIGNAL(valueChanged(int)), this, SLOT(setFogDensity(int)));

   contourCheck = new QCheckBox(tr("Contours"));
   contourCheck->setChecked(false);

   connect(contourCheck, SIGNAL(stateChanged(int)), this, SLOT(checkContours(int)));

   seaLevelCheck = new QCheckBox(tr("Sea Level"));
   seaLevelCheck->setChecked(false);

   connect(seaLevelCheck, SIGNAL(stateChanged(int)), this, SLOT(checkSeaLevel(int)));

   seaLevelSlider = createSlider(-100, 100, 0);

   connect(seaLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setSeaLevel(int)));

   sliderLayout->addWidget(fogCheck);
   sliderLayout->addWidget(fogDensitySlider);
   sliderLayout->addWidget(contourCheck);
   sliderLayout->addWidget(seaLevelCheck);
   sliderLayout->addWidget(seaLevelSlider);
   sliderBox->setLayout(sliderLayout);

   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));

   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   mainLayout->addWidget(viewerWindow);
   mainLayout->addWidget(viewerTable);
   mainLayout->addWidget(sliderBox);
   mainLayout->addWidget(buttonBox);

   mainGroup->setLayout(mainLayout);
}

QSlider *MainWindow::createSlider(int minimum, int maximum, int value)
{
   QSlider *slider = new QSlider(Qt::Vertical);
   slider->setRange(minimum * 16, maximum * 16);
   slider->setSingleStep(16);
   slider->setPageStep((maximum - minimum) / 10 * 16);
   slider->setTickInterval((maximum - minimum) / 10 * 16);
   slider->setTickPosition(QSlider::TicksRight);
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
   fd->setFilter("Ini Files (*.ini)");

   QString fileName;
   if (fd->exec() == QDialog::Accepted)
      fileName = fd->selectedFiles().at(0);

   if (!fileName.isNull())
      viewerWindow->loadMap(fileName);
}

void MainWindow::clear()
{
   viewerWindow->clearMaps();
   viewerTable->setRowCount(0);
}

void MainWindow::updateTable(const QString url)
{
   int rows = viewerTable->rowCount();

   viewerTable->insertRow(rows);
   viewerTable->setItem(rows, 0, new QTableWidgetItem(url));
}

void MainWindow::click(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   viewerWindow->gotoMap(row);
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
