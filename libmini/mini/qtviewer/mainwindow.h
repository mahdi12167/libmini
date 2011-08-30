#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;
class QLabel;
class ViewerWindow;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

private slots:
   void about();

private:
   void createActions();
   void createMenus();

   QMenu* fileMenu;
   QMenu* helpMenu;

   QAction* aboutAction;
   QAction* quitAction;

   ViewerWindow* viewerWindow;
};

#endif
