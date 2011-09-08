#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ViewerWindow;

class QMenu;
class QAction;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QCheckBox;
class QSlider;
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

   void click(int, int);
   void checkFog(int);
   void setFogDensity(int);
   void checkContours(int);
   void checkSeaLevel(int);
   void setSeaLevel(int);

private:
   void createActions();
   void createMenus();
   void createWidgets();

   QSlider *createSlider(int minimum, int maximum, int value);

   QMenu* fileMenu;
   QMenu* helpMenu;

   QAction* aboutAction;
   QAction* openAction;
   QAction* quitAction;

   QGroupBox *mainGroup;
   QVBoxLayout *mainLayout;

   ViewerWindow* viewerWindow;
   QTableWidget *viewerTable;
   QGroupBox *sliderBox;
   QHBoxLayout *sliderLayout;
   QDialogButtonBox *buttonBox;

   QCheckBox *fogCheck;
   QSlider *fogDensitySlider;

   QCheckBox *contourCheck;

   QCheckBox *seaLevelCheck;
   QSlider *seaLevelSlider;

   QPushButton *clearButton;
   QPushButton *quitButton;
};

#endif
