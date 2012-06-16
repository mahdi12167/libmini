// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QtGui/QGroupBox>
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
class MyQGroupBox;

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

   void repoPathChanged(QString);
   void tmpPathChanged(QString);

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

   MyQGroupBox *viewerGroup;
   QBoxLayout *viewerLayout;

   QGroupBox *prefGroup;
   QVBoxLayout *prefLayout;

   QPushButton *clearButton;
   QPushButton *quitButton;

   MyQTableWidget *viewerTable;
   MyQGroupBox *sliderBox;
   QBoxLayout *sliderLayout;
   QBoxLayout *sliderLayout1;
   QBoxLayout *sliderLayout2;

   QGroupBox *fogGroup;
   QHBoxLayout *fogLayout;
   QCheckBox *fogCheck;
   QSlider *fogDensitySlider;

   QGroupBox *contourGroup;
   QHBoxLayout *contourLayout;
   QCheckBox *contourCheck;

   QGroupBox *seaGroup;
   QHBoxLayout *seaLayout;
   QCheckBox *seaLevelCheck;
   QSlider *seaLevelSlider;

   QGroupBox *lightGroup;
   QHBoxLayout *lightLayout;
   QCheckBox *lightCheck;
   QSlider *lightSlider;

   QGroupBox *exaggerGroup;
   QHBoxLayout *exaggerLayout;
   QCheckBox *exaggerCheck;
   QSlider *exaggerSlider;

   QGroupBox *stereoGroup;
   QHBoxLayout *stereoLayout;
   QCheckBox *stereoCheck;

   QGroupBox *wireGroup;
   QHBoxLayout *wireLayout;
   QCheckBox *wireFrameCheck;

   QLineEdit *lineEdit_repoPath;
   QLineEdit *lineEdit_tmpPath;

   QCheckBox *verticalButton;
   QCheckBox *sliderButton;

   void getNameInfo(Object *obj,
                    QString &name, QString &info);

protected:
   ministrings m_Keys;

   ministring repoPath;
   ministring tmpPath;

   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);
};

// subclass group box widget
class MyQGroupBox: public QGroupBox
{
   Q_OBJECT

public:
   MyQGroupBox(QSize size, QWidget *parent = 0)
      : QGroupBox(parent)
   {
      this->size = size;
   }

   QSize sizeHint() const
   {
      return(size);
   }

protected:
   QSize size;
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
