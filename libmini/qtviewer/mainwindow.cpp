#include <string>

#include <QtGui/QMenuBar>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "viewerwindow.h"

#include "mainconst.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   viewerWindow = new ViewerWindow();
   setCentralWidget(viewerWindow);

   createActions();
   createMenus();

   setWindowTitle(tr(VIEWER_NAME));
   resize(VIEWER_WIDTH, VIEWER_HEIGHT);
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
   {
      if (fileName.endsWith(".ini", Qt::CaseInsensitive))
         fileName.truncate(fileName.lastIndexOf("/"));

      viewerWindow->loadMapURL(fileName.toStdString().c_str());
   }
}
