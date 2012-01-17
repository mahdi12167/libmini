// (c) by Stefan Roettger

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <mini/mini_generic.h>
#include <mini/mini_object.h>

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

//! qt viewer main window that
//! contains the viewer window with the opengl rendering context and
//! contains the qt gui elements that interact with the renderer
class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

public slots:
   void updateTable(const QString url, minilayer *layer);

private slots:
   void about();
   void open();
   void clear();

   void toggleStereo(int);
   void toggleWireFrame(int);

   void click(int, int);
   void checkFog(int);
   void setFogDensity(int);
   void checkContours(int);
   void checkSeaLevel(int);
   void setSeaLevel(int);
   void checkLight(int);
   void setLight(int);
   void checkExagger(int);
   void setExagger(int);

private:
   void createActions();
   void createMenus();
   void createWidgets();

   QSlider *createSlider(int minimum, int maximum, int value);

   QMenu* fileMenu;
   QMenu* helpMenu;

   QAction* aboutAction;
   QAction* openAction;
   QAction* clearAction;
   QAction* quitAction;

   QGroupBox *mainGroup;
   QVBoxLayout *mainLayout;

   ViewerWindow* viewerWindow;
   QHBoxLayout *viewerLayout;
   QTableWidget *viewerTable;
   QGroupBox *sliderBox;
   QVBoxLayout *sliderLayout;
   QHBoxLayout *sliderLayout1;
   QHBoxLayout *sliderLayout2;
   QDialogButtonBox *buttonBox;

   QCheckBox *fogCheck;
   QSlider *fogDensitySlider;

   QCheckBox *contourCheck;

   QCheckBox *seaLevelCheck;
   QSlider *seaLevelSlider;

   QCheckBox *lightCheck;
   QSlider *lightSlider;

   QCheckBox *exaggerCheck;
   QSlider *exaggerSlider;

   QCheckBox *stereoCheck;
   QCheckBox *wireFrameCheck;

   QPushButton *clearButton;
   QPushButton *quitButton;

   protected:

   minidyna<minilayer *> m_Layer;

   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);
};

#endif
