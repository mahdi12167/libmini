#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ViewerWindow;

class QMenu;
class QAction;
class QVBoxLayout;
class QTableWidget;
class QDialogButtonBox;
class QPushButton;
class QMimeData;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

public slots:
   void clear();
   void updateTable(const QMimeData *mimeData);

private slots:
   void about();
   void open();

private:
   void createActions();
   void createMenus();

   QMenu* fileMenu;
   QMenu* helpMenu;

   QAction* aboutAction;
   QAction* openAction;
   QAction* quitAction;

   QVBoxLayout *mainLayout;

   ViewerWindow* viewerWindow;
   QTableWidget *viewerTable;
   QDialogButtonBox *buttonBox;

   QPushButton *clearButton;
   QPushButton *quitButton;
};

#endif
