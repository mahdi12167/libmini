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
class QLabel;
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
   void runAction(ministring action, int row = -1);
   void reportProgress(double percentage);

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
   void exportPathChanged(QString);
   void tmpPathChanged(QString);

   void browseRepoPath();
   void browseExportPath();
   void browseTmpPath();

   void gridLevelChanged(QString);
   void gridLevelsChanged(QString);
   void gridStepChanged(QString);

   void shadePowerChanged(QString);
   void jpegQualityChanged(QString);

   void checkVertical(int);
   void checkSliders(int);

private:
   void createActions();
   void createMenus();
   void createWidgets();

   QSlider *createSlider(int minimum, int maximum, int value);

   QGroupBox *createEdit(ministring name, ministring value,
                         QLineEdit **lineEdit);

   QGroupBox *createPathEdit(ministring name, ministring value,
                             QLineEdit **lineEdit, QPushButton **browseButton);

   QMenu *fileMenu;
   QMenu *helpMenu;

   QAction *aboutAction;
   QAction *openAction;
   QAction *clearAction;
   QAction *quitAction;

   QGroupBox *mainGroup;
   QBoxLayout *mainLayout;

   ViewerWindow *viewerWindow;
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

   QLabel *workerActivity;

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
   QPushButton *browseButton_repoPath;

   QLineEdit *lineEdit_exportPath;
   QPushButton *browseButton_exportPath;

   QLineEdit *lineEdit_tmpPath;
   QPushButton *browseButton_tmpPath;

   QLineEdit *lineEdit_gridLevel;
   QLineEdit *lineEdit_gridLevels;
   QLineEdit *lineEdit_gridStep;

   QLineEdit *lineEdit_shadePower;
   QLineEdit *lineEdit_jpegQuality;

   QCheckBox *verticalButton;
   QCheckBox *sliderButton;

   void getNameInfo(Object *obj,
                    QString &name, QString &info);

protected:
   ministrings m_Keys;

   ministring repoPath;
   ministring exportPath;
   ministring tmpPath;

   int grid_level;
   int grid_levels;
   int grid_step;

   double shadePower;
   double jpegQuality;

   void initSettings();
   void saveSettings();

   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
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
   MyQTableWidget(ViewerWindow *viewer, QWidget *parent = 0)
      : QTableWidget(parent)
   {
      viewerWindow = viewer;
   }

protected:
   void keyPressEvent(QKeyEvent *event);

private slots:
   void showContextMenu(const QPoint &pos);

signals:
   void activate(ministring action, int row = -1);

protected:
   ViewerWindow *viewerWindow;

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);
};

#endif
