// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <mini/mini_generic.h>

class ViewerWindow;

//! qt viewer main window that
//! contains the viewer window with the opengl rendering context
class MainWindow: public QMainWindow
{
   Q_OBJECT;

public:
   MainWindow(QWidget *parent = 0);
   virtual ~MainWindow();

private:
   QAction *quitAction;

   QMenu *fileMenu;

   ViewerWindow *viewerWindow;
};

#endif
