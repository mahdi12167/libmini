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

   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));

   mainLayout->addWidget(viewerWindow);
   mainLayout->addWidget(viewerTable);
   mainLayout->addWidget(buttonBox);

   mainGroup->setLayout(mainLayout);
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

void MainWindow::click(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   viewerWindow->gotoMap(row);
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
