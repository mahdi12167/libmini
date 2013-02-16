// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui>

#include "viewerwindow.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   quitAction = new QAction(tr("Q&uit"), this);
   quitAction->setShortcuts(QKeySequence::Quit);
   quitAction->setStatusTip(tr("Quit the application"));
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

   fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction(quitAction);

   viewerWindow = new ViewerWindow;
   setCentralWidget(viewerWindow);
}

MainWindow::~MainWindow()
{
   delete viewerWindow;
}
