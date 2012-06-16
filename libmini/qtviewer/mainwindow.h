// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTableWidget>

#include <mini/mini_generic.h>
#include <mini/mini_object.h>

class Object;
class ViewerWindow;

class QMenu;
class QAction;
class QTabWidget;
class QGroupBox;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QCheckBox;
class QSlider;
class QDialogButtonBox;
class QPushButton;
class QLineEdit;

class MyQTableWidget;

//! qt viewer main window that
//! contains the viewer window with the opengl rendering context and
//! contains the qt gui elements that interact with the renderer
class MainWindow: public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

public slots:
   void updateTable(ministring key);
   void runAction(ministring action, int row);

private slots:
   void about();
   void open();
   void clear(bool all = false);

   void toggleStereo(int);
   void toggleWireFrame(int);

   void click(int, int);
   void doubleclick(int, int);
   void checkFog(int);
   void setFogDensity(int);
   void checkContours(int);
   void checkSeaLevel(int);
   void setSeaLevel(int);
   void checkLight(int);
   void setLight(int);
   void checkExagger(int);
   void setExagger(int);

   void repoPath(QString);
   void tmpPath(QString);

   void checkVertical(int);
   void checkSliders(int);

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
   QBoxLayout *mainLayout;

   ViewerWindow* viewerWindow;
   QTabWidget *tabWidget;
   QDialogButtonBox *buttonBox;

   QGroupBox *viewerGroup;
   QHBoxLayout *viewerLayout;

   QGroupBox *prefGroup;
   QVBoxLayout *prefLayout;

   QPushButton *clearButton;
   QPushButton *quitButton;

   MyQTableWidget *viewerTable;
   QGroupBox *sliderBox;
   QVBoxLayout *sliderLayout;
   QHBoxLayout *sliderLayout1;
   QHBoxLayout *sliderLayout2;

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

   QLineEdit *lineEdit_repoPath;
   QLineEdit *lineEdit_tmpPath;

   QCheckBox *verticalButton;
   QCheckBox *sliderButton;

   void getNameInfo(Object *obj,
                    QString &name, QString &info);

protected:
   ministrings m_Keys;

   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);
};

// subclass table widget
class MyQTableWidget: public QTableWidget
{
   Q_OBJECT

public:
   MyQTableWidget(QWidget *parent = 0)
      : QTableWidget(parent)
   {}

protected:
   void keyPressEvent(QKeyEvent *event);

private slots:
   void showContextMenu(const QPoint &pos);

signals:
   void activate(ministring action, int row);
};

#endif
