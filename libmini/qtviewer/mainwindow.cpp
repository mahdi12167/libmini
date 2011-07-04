#include <QtGui>

#include "mainwindow.h"
#include "viewerwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   viewerWindow = new ViewerWindow();
   setCentralWidget(viewerWindow);

   createActions();
   createMenus();
   createStatusBar();
}

MainWindow::~MainWindow() {}

void MainWindow::createActions()
{
   openAction = new QAction(tr("&Open"), this);
   openAction->setIcon(QIcon(":/images/open.png"));
   openAction->setShortcut(tr("Ctrl+O"));
   openAction->setStatusTip(tr("Open an existing file"));
   connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

   aboutAction = new QAction(tr("&About"), this);
   aboutAction->setIcon(QIcon(":/images/about.png"));
   aboutAction->setShortcut(tr("Ctrl+A"));
   aboutAction->setStatusTip(tr("About information"));
   connect(openAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
   fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction(openAction);

   menuBar()->addSeparator();

   helpMenu = menuBar()->addMenu(tr("&Help"));
   helpMenu->addAction(aboutAction);
}

void MainWindow::createStatusBar()
{
   locationLabel = new QLabel("");
   locationLabel->setAlignment(Qt::AlignHCenter);
   locationLabel->setMinimumSize(locationLabel->sizeHint());

   messageLabel = new QLabel();
   messageLabel->setIndent(3);

   statusBar()->addWidget(locationLabel);
   statusBar()->addWidget(messageLabel, 1);
}

void MainWindow::open()
{
   QMessageBox::information(this, tr("Open file"), tr("Open existing file"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
}

void MainWindow::about() {}
