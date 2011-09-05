#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ViewerWindow;

class QMenu;
class QAction;
class QGroupBox;
class QVBoxLayout;
class QTableWidget;
class QDialogButtonBox;
class QPushButton;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

public slots:
   void updateTable(const QString url);

private slots:
   void about();
   void open();
   void clear();

private:
   void createActions();
   void createMenus();
   void createWidgets();

   QMenu* fileMenu;
   QMenu* helpMenu;

   QAction* aboutAction;
   QAction* openAction;
   QAction* quitAction;

   QGroupBox *mainGroup;
   QVBoxLayout *mainLayout;

   ViewerWindow* viewerWindow;
   QTableWidget *viewerTable;
   QDialogButtonBox *buttonBox;

   QPushButton *clearButton;
   QPushButton *quitButton;
};

#endif
