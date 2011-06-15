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

protected:
    //void closeEvent(QCloseEvent *);

private slots:
    void open();
    void about();

private:
    void createActions();
    void createMenus();
    void createStatusBar();


    QLabel* locationLabel;
    QLabel* messageLabel;
    QMenu*  fileMenu;
    QMenu*  helpMenu;

    QAction*    openAction;
    QAction*    aboutAction;

    ViewerWindow*   viewerWindow;
};

#endif // MAINWINDOW_H
